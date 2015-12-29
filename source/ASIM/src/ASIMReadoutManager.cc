/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMReadoutManager.cc
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMReadoutManager class handles the automated extraction
//       of Geant4 simulated detector data for "readouts" that are
//       registered by the user. Each "readout" is attached to an
//       individual G4SensitiveDetector object as the hook into
//       extracting Geant4 data. Once extracted, the data is handed to
//       its sister class ASIMStorageManager for persistent storage to
//       disk in a standardized ROOT file known as an ASIM File.
//
/////////////////////////////////////////////////////////////////////////////////

// Geant4
#include "G4SDManager.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "Randomize.hh"

// C++
#include <sstream>

// ASIM
#include "ASIMScintillatorSD.hh"
#include "ASIMPhotodetectorSD.hh"
#include "ASIMReadoutManager.hh"
#include "ASIMReadoutMessenger.hh"
#include "MPIManager.hh"


ASIMReadoutManager *ASIMReadoutManager::ASIMReadoutMgr = NULL;


ASIMReadoutManager *ASIMReadoutManager::GetInstance()
{ return ASIMReadoutMgr; }


ASIMReadoutManager::ASIMReadoutManager()
  : parallelProcessing(false), MPI_Rank(0), MPI_Size(1), 
    ActiveReadout(0), NumReadouts(0),
    ASIMFileOpen(false), ASIMFileName("ASIMDefault.asim.root"),
    ASIMStorageMgr(new ASIMStorageManager), ASIMRunSummary(new ASIMRun)
{
  if(ASIMReadoutMgr != NULL)
    G4Exception("ASIMReadoutManager::ASIMReadoutManager()", 
		"ASIMReadoutManager-Exception00", 
		FatalException, 
		"\nThe Meyer's singletone ASIMReadoutManager was constructed twice!\n");
  else 
    ASIMReadoutMgr = this;
  
  // Initialize ASIM readout classes
  
  theMessenger = new ASIMReadoutMessenger(this);
}


ASIMReadoutManager::~ASIMReadoutManager()
{
  delete theMessenger;

  vector<ASIMEvent *>::iterator It;
  for(It=ASIMEvents.begin(); It!=ASIMEvents.end(); It++)
    delete (*It);
}


void ASIMReadoutManager::InitializeASIMFile()
{
  if(ASIMFileOpen){
    G4cout << "\nASIMReadoutManager::InitializeASIMFile():\n"
	   <<   "  An ASIM file is presently open for data output! A new file cannot be opened\n"
	   <<   "  until the existing ASIM file is written to disk via the /ASIM/write command.\n"
	   << G4endl;
    
    return;
  }
  
  // Create a new manager to handle persistent storage of readout data
  // into an ASIM file. This ensures that all ROOT objects necessary
  // for storage in an ASIM file are created within the TFile
  // directory that is used for this specified ASIM file
  ASIMStorageMgr = new ASIMStorageManager;
  
  // Clear the pointers to previous file's ASIMEvents
  ASIMEvents.clear();
  
  // Iterate over the register readouts and create new event trees
  for(size_t r=0; r<ASIMReadoutID.size(); r++)
    ASIMEvents.push_back(ASIMStorageMgr->CreateEventTree(ASIMReadoutID.at(r),
							 ASIMReadoutName.at(r),
							 ASIMReadoutDesc.at(r)));

  // Create new architecture-specific ASIM files to receive data
  
  if(parallelProcessing){
#ifdef MPI_ENABLED
    MPIManager *MPIMgr = MPIManager::GetInstance();
    MPI_Rank = MPIMgr->GetRank();
    MPI_Size = MPIMgr->GetSize();
#endif    
    ASIMStorageMgr->CreateParallelFile(ASIMFileName, MPI_Rank, MPI_Size);
  }
  else
    ASIMStorageMgr->CreateSequentialFile(ASIMFileName);

  // Flag that ASIM file(s) is(are) now open and waiting for data!
  ASIMFileOpen = true;
}

  
void ASIMReadoutManager::WriteASIMFile(G4bool EmergencyWrite)
{
  if(!ASIMFileOpen){
    G4cout << "\nASIMReadoutManager::WriteASIMFile():\n"
	   <<   "  There is no valid ASIM file presently open for writing! A new ASIM file should\n"
           <<   "  first be created via the /ASIM/init command.\n"
	   << G4endl;
    
    return;
  }
  
  if(EmergencyWrite)
    G4cout << "\nASIMReadoutManager::WriteASIMFile():\n"
	   <<   "  The ASIM file that presently exists with data is being written to disk in\n"
           <<   "  emergency fashion to avoid losing critical data before the simulation\n"
	   <<   "  terminates. Please issue the /ASIM/write command before exiting next time!\n"
	   << G4endl;
  
  // Perform the final write-to-disk for the ASIM files
  
  if(parallelProcessing)
    ASIMStorageMgr->WriteParallelFile();
  else
    ASIMStorageMgr->WriteSequentialFile();

  // Delete the ASIM file specific readout manager
  delete ASIMStorageMgr;

  // Flag that no ASIM file(s) is(are) open
  ASIMFileOpen = false;
}


void ASIMReadoutManager::RegisterNewReadout(G4String ReadoutDesc,
					    G4VPhysicalVolume *Scintillator,
					    G4VPhysicalVolume *Photodetector)
{
  // Increment total number of registered readouts
  NumReadouts++;

  // Assign a unique readout ID based on registration order
  G4int ReadoutID = NumReadouts;

  // Use the associated G4SD name as the readout name
  G4String ReadoutName = Scintillator->GetLogicalVolume()->GetSensitiveDetector()->GetName();
  
  // Store ASIM readout variables to use later for access
  ASIMReadoutID.push_back(ReadoutID);
  ASIMReadoutName.push_back(ReadoutName);
  ASIMReadoutDesc.push_back(ReadoutDesc);
  ASIMReadoutNameMap[ReadoutName] = ReadoutID;

  // Create new ASIMEvents to hold event-level data. Note that these
  // pointer will be overridden upon the creation of an ASIM file in
  // order to ensure that a new ASIMStorageManager (which holds the
  // ASIMEvent pointer addresses) can be attached to each ASIM file.
  ASIMEvents.push_back(ASIMStorageMgr->CreateEventTree(ReadoutID,
						       ReadoutName,
						       ReadoutDesc));
  
  // Store the readout name associated with a ScintillatorSD for later
  // use during event-level readout ...
  string ScintillatorSDName = ReadoutName + "Collection";
  ScintillatorSDNames.push_back(ScintillatorSDName);

  // ... and also store the readout name of the associated
  // PhotodetectorSD as well if it has been specified
  if(Photodetector != NULL){
    G4String PReadoutName = Photodetector->GetLogicalVolume()->GetSensitiveDetector()->GetName();
    PhotodetectorSDNames.push_back(PReadoutName + "Collection");
  }
  else
    PhotodetectorSDNames.push_back("Photodetector does not exist for this readout!");

  // Increment the event-, run-, and readout setting vectors 

  // Run-level aggregators
  ReadoutEnabled.push_back(0);
  Incidents.push_back(0);
  Hits.push_back(0);
  RunEDep.push_back(0.);
  PhotonsCreated.push_back(0);
  PhotonsDetected.push_back(0);

  // Event-level variables
  EventEDep.push_back(0.);
  EventActivated.push_back(false);

  // Readout settings
  ActiveReadout = 0;
  EnergyBroadening.push_back(false);
  EnergyResolution.push_back(0.);
  EnergyEvaluation.push_back(0.);
  UseEnergyThresholds.push_back(true);
  LowerEnergyThreshold.push_back(0.);
  UpperEnergyThreshold.push_back(1.*TeV);
  UsePhotonThresholds.push_back(false);
  LowerPhotonThreshold.push_back(0);
  UpperPhotonThreshold.push_back(1000000000);
  WaveformStorage.push_back(false);
}


void ASIMReadoutManager::InitializeForRun()
{
  // Set all run-level aggregators to zero to prepare for a new run
  for(G4int r=0; r<NumReadouts; r++){
    Incidents[r] = 0;
    Hits[r] = 0;
    RunEDep[r] = 0;
    PhotonsCreated[r] = 0;
    PhotonsDetected[r] = 0;
  }
}


void ASIMReadoutManager::ReadoutEvent(const G4Event *currentEvent)
{
  G4int TheCollectionID = -1;
  G4SDManager *TheSDManager = G4SDManager::GetSDMpointer();
  
  for(G4int r=0; r<NumReadouts; r++){
    
    G4HCofThisEvent *HCE = currentEvent->GetHCofThisEvent();

    ASIMEvents[r]->Initialize();
    ASIMEvents[r]->SetEventID(currentEvent->GetEventID());
    ASIMEvents[r]->SetRunID(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
    
    G4int HCEntries = TheSDManager->GetHCtable()->entries();
    for(G4int hc=0; hc<HCEntries; hc++){
      
      G4String CollectionName = TheSDManager->GetHCtable()->GetHCname(hc);
      TheCollectionID = TheSDManager->GetCollectionID(CollectionName);

      if(CollectionName == ScintillatorSDNames[r]){
	
	//////////////////////////
	// The scintillator SDs //
	//////////////////////////
	
	ASIMScintillatorSDHitCollection const *ScintillatorHC = 
	  dynamic_cast<ASIMScintillatorSDHitCollection *>(HCE->GetHC(TheCollectionID));
	
	// Iterate through entries in the hit collection (HC). 
	// - If the particle is an optical photon then increment the counter
	// - If the particle is not an optical photon then sum the energy
	//   deposited  during this hit into event-level sum EDep

	EventEDep[r] = 0.;
	EventActivated[r] = false;

	if(ScintillatorHC->entries() > 0)
	  Incidents[r]++;

	for(G4int i=0; i<ScintillatorHC->entries(); i++){
	  if( (*ScintillatorHC)[i]->GetIsOpticalPhoton() ){
	    ASIMEvents[r]->IncrementPhotonsCreated();
	    if(WaveformStorage[r])
	      ASIMEvents[r]->AddPhotonCreationTime( (*ScintillatorHC)[i]->GetCreationTime()/ns );
	  }
	  else
	    EventEDep[r] += (*ScintillatorHC)[i]->GetEnergyDep();
	}

	// Enable artificial gaussian energy broadening
	if(EnergyBroadening[r]){
	  
	  // For simplicity, convert energy deposition to eV to ensure we
	  // can use a simple algorithm with integers
	  
	  // Compute the energy scale based on the desired resolution and
	  // evaluation energy
	  G4double energyScale = EnergyResolution[r]/100 * sqrt(EnergyEvaluation[r]/eV) / 2.35;
	  
	  // Compute the necessary sigma 
	  G4double energySigma = energyScale * std::sqrt(EventEDep[r]/eV);
	  
	  // Compute the assigned energy value
	  EventEDep[r] = G4int(G4RandGauss::shoot(EventEDep[r]/eV, energySigma)) * eV;
	}
	ASIMEvents[r]->SetEnergyDep(EventEDep[r]/MeV);
      }
      

      ///////////////////////////////////
      // The photodetector readout SDs //
      ///////////////////////////////////

      else if(CollectionName == PhotodetectorSDNames[r]){
	
	ASIMPhotodetectorSDHitCollection const *PhotodetectorHC =
	  dynamic_cast<ASIMPhotodetectorSDHitCollection *>(HCE->GetHC(TheCollectionID));
	
	for(G4int i=0; i<PhotodetectorHC->entries(); i++){
	  ASIMEvents[r]->IncrementPhotonsDetected();
	  if(WaveformStorage[r])
	    ASIMEvents[r]->AddPhotonDetectionTime( (*PhotodetectorHC)[i]->GetDetectionTime()/ns );
	}
      }
    }
    
    ///////////////
    // Data readout
    
    if(UseEnergyThresholds[r]){
      
      // Use the lower/upper energy threshold to determine whether or
      // not to count the event as a "hit" on the detectors, which
      // will be used in runAction to calculate intrinsic efficiency
      if(EventEDep[r] > LowerEnergyThreshold[r] and
	 EventEDep[r] < UpperEnergyThreshold[r]){
	
	// Activate the detector for this event
	EventActivated[r] = true;

	// Fill detector trees if output to ASIM has been activated
	if(ASIMFileOpen)
	  ASIMStorageMgr->GetEventTree(ASIMReadoutID[r])->Fill();
      }
    }
    
    else if(UsePhotonThresholds[r]){
      if(ASIMEvents[r]->GetPhotonsDetected() > LowerPhotonThreshold[r] and
	 ASIMEvents[r]->GetPhotonsDetected() < UpperPhotonThreshold[r]){
	
	// Activate the detector for this event
	EventActivated[r] = true;
	
	// Fill detector trees if output to ASIM has been activated
	if(ASIMFileOpen)
	  ASIMStorageMgr->GetEventTree(ASIMReadoutID[r])->Fill();
      }
    }
  }
  
  // Handle incrementing run-level information
  IncrementRunLevelData(EventActivated);
}


void ASIMReadoutManager::IncrementRunLevelData(vector<G4bool> &EventActivated)
{
  G4int EventSum = 0;
  
  for(G4int r=0; r<NumReadouts; r++){
    
    EventSum += EventActivated[r];
    
    if(EventActivated[r]){
      Hits[r]++;
      RunEDep[r] += EventEDep[r];
      PhotonsCreated[r] += ASIMEvents[r]->GetPhotonsCreated();
      PhotonsDetected[r] += ASIMEvents[r]->GetPhotonsDetected();
    }
  }
}


void ASIMReadoutManager::FillRunSummary(const G4Run *currentRun)
{
  if(parallelProcessing)
    ReduceSlaveValuesToMaster();

  // In sequential or in parallel on the master node, add a class with
  // information from this run...
  
  if(ASIMFileOpen and MPI_Rank == 0){
    
    ASIMRun *ASIMRunSummary = new ASIMRun;

    // Fill class with run-level data

    ASIMRunSummary->SetRunID( currentRun->GetRunID() );

#ifdef MPI_ENABLED
    if(parallelProcessing)
      ASIMRunSummary->SetTotalEvents( MPIManager::GetInstance()->GetTotalEvents() );
    else
#endif
      ASIMRunSummary->SetTotalEvents( currentRun->GetNumberOfEvent() );

    /*
    ASIMRunSummary->SetParticlesIncident(Incidents);
    ASIMRunSummary->SetParticlesBetweenThresholds(Hits);
    ASIMRunSummary->SetDetectorLowerThresholdInMeV(LowerEnergyThreshold/MeV);
    ASIMRunSummary->SetDetectorUpperThresholdInMeV(UpperEnergyThreshold/MeV);
    ASIMRunSummary->SetPhotonsCreated(PhotonsCreated);
    ASIMRunSummary->SetPhotonsDetected(detectorPhotonsDetected);
    */
    
    // Add the run class to the list for later readout
    ASIMStorageMgr->AddRun(ASIMRunSummary);
  }
}


void ASIMReadoutManager::HandleOpticalPhotonCreation(const G4Track *CurrentTrack)
{
  ///////////////////////////////////////////
  // Handle the readout of created photons //
  ///////////////////////////////////////////

  // Ensure the current track is an optical photon
  if(CurrentTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
    
    G4VPhysicalVolume *CurrentVolume = CurrentTrack->GetVolume();
    
    // Handle the primary case: scintillation/cerenkov photons are
    // created within a scintillator/cerenkov-radiator volume that has
    // an ASIMScintillatorSD object registered to it
    
    ASIMScintillatorSD *VolumeScintillatorSD = 
      dynamic_cast<ASIMScintillatorSD *>(CurrentVolume->GetLogicalVolume()->GetSensitiveDetector());
    
    if(VolumeScintillatorSD and CurrentTrack->GetParentID() > 0)
      VolumeScintillatorSD->ManualTrigger(CurrentTrack);
  }
}


void ASIMReadoutManager::HandleOpticalPhotonDetection(const G4Step *CurrentStep)
{
  ////////////////////////////////////////////////////
  // Handle the readout of detected optical photons //
  ////////////////////////////////////////////////////

  // Iterate through the available physics processes to find the
  // optical boundary process and store it for later use. Note that
  // static methods enable this code to be successfully executed once.

  static G4OpBoundaryProcess *OpBoundaryProc = NULL;
  
  if(!OpBoundaryProc){
    
    G4ProcessManager *ProcessMgr = CurrentStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int NumProcesses = ProcessMgr->GetProcessListLength();
    G4ProcessVector *ProcessVec = ProcessMgr->GetProcessList();
    
    for(G4int p=0; p<NumProcesses; p++){
      if((*ProcessVec)[p]->GetProcessName()=="OpBoundary"){
	OpBoundaryProc = (G4OpBoundaryProcess*)(*ProcessVec)[p];
	break;
      }
    }
  }

  // Check to see if the currently tracking particle is an optical photon
  G4ParticleDefinition *ParticleType = CurrentStep->GetTrack()->GetDefinition();
  if(ParticleType == G4OpticalPhoton::OpticalPhotonDefinition()){

    // Get the status of the optical photon at the boundary
    G4OpBoundaryProcessStatus OpBoundaryStatus = Undefined;  
    OpBoundaryStatus = OpBoundaryProc->GetStatus();
    
    // Check to ensure that the optical photon is actually at a
    // geometrically defined boundary
    if(CurrentStep->GetPostStepPoint()->GetStepStatus()==fGeomBoundary){
      
      switch(OpBoundaryStatus){
	
	// "Absorption" means the optical photon was killed at the
	// optical boundary but did *not* create a photoelectron
      case Absorption:
	;
	break;

	// "Detection" means the optical photon was killed at the
	// boundary and created a photoelectron 
      case Detection:{

	G4VPhysicalVolume *PostVolume = CurrentStep->GetPostStepPoint()->GetPhysicalVolume();
	
	// Handle the primary case: scintillation/cerenkov photons
	// have been detected at an optical readout device volume that
	// has an ASIMPhotodetectorSD object registered to it
	
	ASIMPhotodetectorSD *PostVolumeSD =
	  dynamic_cast<ASIMPhotodetectorSD *>(PostVolume->GetLogicalVolume()->GetSensitiveDetector());
	
	if(PostVolumeSD)
	  PostVolumeSD->ManualTrigger(CurrentStep);
	break;
      }
	
      default:
	break;
      }
    }
  }
}


// Method used in parallel to aggregate run-level data on each slave
// into single values on the master after the run has concluded
void ASIMReadoutManager::ReduceSlaveValuesToMaster()
{
#ifdef MPI_ENABLED
  
  G4cout << "\nASIMReadoutManager : Beginning the MPI reduction of data to the master!"
	 << G4endl;
  
  MPIManager *theMPImanager = MPIManager::GetInstance();

  for(G4int r=0; r<NumReadouts; r++){
    Incidents[r] = theMPImanager->SumIntsToMaster(Incidents[r]);
    Hits[r] = theMPImanager->SumIntsToMaster(Hits[r]);
    RunEDep[r] = theMPImanager->SumDoublesToMaster(RunEDep[r]);
    PhotonsCreated[r] = theMPImanager->SumDoublesToMaster(PhotonsCreated[r]);
    PhotonsDetected[r] = theMPImanager->SumDoublesToMaster(PhotonsDetected[r]);
  }
  
  G4cout << "\nASIMReadoutManager : Finished the MPI reduction of values to the master!\n"
	 << G4endl;
#endif
}


void ASIMReadoutManager::SetActiveReadout(G4int R)
{
  if(R<NumReadouts)
    ActiveReadout = R;
  else
    G4cout << "\nASIMReadoutManager::SetActiveReadout():\n"
	   <<   "  The specified readout does not exist and therefore cannot be activated!\n"
	   << G4endl;
}


//////////////////////////////////////////
// Set/Get methods for readout settings //
//////////////////////////////////////////

G4int ASIMReadoutManager::GetActiveReadout()
{ return ActiveReadout;}

void ASIMReadoutManager::SetReadoutEnabled(G4bool RE)
{ ReadoutEnabled.at(ActiveReadout) = RE; }

G4bool ASIMReadoutManager::GetReadoutEnabled(G4int R)
{ return ReadoutEnabled.at(R); }

void ASIMReadoutManager::SetEnergyBroadening(G4bool B)
{ EnergyBroadening.at(ActiveReadout) = B; }

G4bool ASIMReadoutManager::GetEnergyBroadening(G4int R)
{ return EnergyBroadening.at(R); }

void ASIMReadoutManager::SetEnergyResolution(G4double E)
{ EnergyResolution.at(ActiveReadout) = E; }

G4double ASIMReadoutManager::GetEnergyResolution(G4int R)
{ return EnergyResolution.at(R); }

void ASIMReadoutManager::SetEnergyEvaluation(G4double E)
{ EnergyEvaluation.at(ActiveReadout) = E; }

G4double ASIMReadoutManager::GetEnergyEvaluation(G4int R)
{ return EnergyEvaluation.at(R); }

void ASIMReadoutManager::EnableEnergyThresholds()
{
  UseEnergyThresholds.at(ActiveReadout) = true;
  UsePhotonThresholds.at(ActiveReadout) = false;
}

G4bool ASIMReadoutManager::GetUseEnergyThresholds(G4int R)
{ return UseEnergyThresholds.at(R); }

void ASIMReadoutManager::SetLowerEnergyThreshold(G4double LET)
{ LowerEnergyThreshold.at(ActiveReadout) = LET; }

G4double ASIMReadoutManager::GetLowerEnergyThreshold(G4int R)
{ return LowerEnergyThreshold.at(R); }

void ASIMReadoutManager::SetUpperEnergyThreshold(G4double UET)
{ UpperEnergyThreshold.at(ActiveReadout) = UET; }

G4double ASIMReadoutManager::GetUpperEnergyThreshold(G4int R)
{ return UpperEnergyThreshold.at(R); }

void ASIMReadoutManager::EnablePhotonThresholds()
{
  UseEnergyThresholds.at(ActiveReadout) = false;
  UsePhotonThresholds.at(ActiveReadout) = true;
}

G4bool ASIMReadoutManager::GetUsePhotonThresholds(G4int R)
{ return UsePhotonThresholds.at(R); }

void ASIMReadoutManager::SetLowerPhotonThreshold(G4int LPT)
{ LowerPhotonThreshold.at(ActiveReadout) = LPT; }

G4int ASIMReadoutManager::GetLowerPhotonThreshold(G4int R)
{ return LowerPhotonThreshold.at(R); }

void ASIMReadoutManager::SetUpperPhotonThreshold(G4int UPT)
{ UpperPhotonThreshold.at(ActiveReadout) = UPT; }

G4int ASIMReadoutManager::GetUpperPhotonThreshold(G4int R)
{ return UpperPhotonThreshold.at(R); }

void ASIMReadoutManager::SetWaveformStorage(G4bool WS)
{ WaveformStorage.at(ActiveReadout) = WS; }

G4bool ASIMReadoutManager::GetWaveformStorage(G4int R)
{ return WaveformStorage.at(R); }


//////////////////////////////////////////
// Set/Get methods for event-level data //
//////////////////////////////////////////

void ASIMReadoutManager::SetEventActivated(G4bool EA)
{ EventActivated.at(ActiveReadout) = EA; }

G4bool ASIMReadoutManager::GetEventActivated(G4int R)
{ return EventActivated.at(R); }


////////////////////////////////////////
// Set/Get methods for run-level data //
////////////////////////////////////////

void ASIMReadoutManager::SetIncidents(G4int I)
{ Incidents.at(ActiveReadout) = I; }

G4int ASIMReadoutManager::GetIncidents(G4int R)
{ return Incidents.at(R); }

void ASIMReadoutManager::SetHits(G4int H)
{ Hits.at(ActiveReadout) = H; }

G4int ASIMReadoutManager::GetHits(G4int R)
{ return Hits.at(R); }

void ASIMReadoutManager::SetRunEDep(G4double E)
{ RunEDep.at(ActiveReadout) = E; }

G4double ASIMReadoutManager::GetRunEDep(G4int R)
{ return RunEDep.at(R); }

void ASIMReadoutManager::SetPhotonsCreated(G4int P)
{ PhotonsCreated.at(ActiveReadout) = P; }

G4int ASIMReadoutManager::GetPhotonsCreated(G4int R)
{ return PhotonsCreated.at(R); }

void ASIMReadoutManager::SetPhotonsDetected(G4int P)
{ PhotonsDetected.at(ActiveReadout) = P; }

G4int ASIMReadoutManager::GetPhotonsDetected(G4int R)
{ return PhotonsDetected.at(R); }
