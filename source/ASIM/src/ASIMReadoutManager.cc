/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMReadoutManager.cc
// date: 11 Jan 16
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMReadoutManager class handles the automated extraction
//       of Geant4 simulated detector data for registered "readouts"
//       (data readout from a single G4SensitiveDetector object) and
//       "arrays" (data readout from a collection of readouts); arrays
//       have the option of readout only if a coincidence if found
//       between all readouts that compose the array. The event- and
//       run-level data is aggregated and made available to the user
//       via standard "Get" methods. Data can optionally be stored
//       into an ASIM file for offline analysis.
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

// Boost
#include <boost/tokenizer.hpp>

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
    NumReadouts(0), SelectedReadout(0), NumArrays(0), SelectedArray(0),
    ASIMFileOpen(false), ASIMFileName("ASIMDefault.asim.root"),
    ASIMStorageMgr(new ASIMStorageManager), ASIMRunSummary(new ASIMRun),
    ASIMReadoutIDOffset(0), ASIMArrayIDOffset(1000)
{
  if(ASIMReadoutMgr != NULL)
    G4Exception("ASIMReadoutManager::ASIMReadoutManager()", 
		"ASIMReadoutManager-Exception00", 
		FatalException, 
		"\nThe Meyer's singleton ASIMReadoutManager was constructed twice!\n");
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

  for(It=ASIMArrayEvents.begin(); It!=ASIMArrayEvents.end(); It++)
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
  ASIMArrayEvents.clear();
  
  // Iterate over the register readouts and create new event trees
  for(size_t r=0; r<ASIMReadoutID.size(); r++)
    ASIMEvents.push_back(ASIMStorageMgr->CreateEventTree(ASIMReadoutID.at(r),
							 ASIMReadoutName.at(r),
							 ASIMReadoutDesc.at(r)));
  
  for(size_t a=0; a<ASIMArrayID.size(); a++)
    ASIMArrayEvents.push_back(ASIMStorageMgr->CreateEventTree(ASIMArrayID.at(a),
							      ASIMArrayName.at(a),
							      ASIMArrayDesc.at(a)));
  
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
  ReadoutEnabled.push_back(true);
  RIncidents.push_back(0);
  RHits.push_back(0);
  REDep.push_back(0.);
  RPhotonsCreated.push_back(0);
  RPhotonsDetected.push_back(0);

  // Event-level variables
  EventEDep.push_back(0.);
  EventActivated.push_back(false);

  // Readout-specific settings
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

void ASIMReadoutManager::CreateArray(G4String ArrayName,
				     vector<int> ArrayList,
				     G4bool ArrayCoincident=false)
{
  // Increment total number of registered arrays
  NumArrays++;
  
  // Create a unique ID for the new array
  G4int ArrayID = ASIMArrayIDOffset + NumArrays;
  
  vector<G4bool> Array(NumReadouts, false);
  
  stringstream SS;
  SS << "ASIM array of readouts ";

  vector<int>::iterator It=ArrayList.begin();
  for(; It!=ArrayList.end(); It++){
    
    if((*It) < NumReadouts){
      
      // Mark this readout ID as part of the array
      Array.at(*It) = true;
      
      // Add the readout ID to the array description
      SS << (*It) << " ";
    }
    else{
      G4cout << "\nASIMReadoutManager::CreateArray():\n"
	     <<   "  Warning! A readout ID was specified in the array that does not exist,\n"
	     <<   "  i.e. (ReadoutID >= NumReadouts)! The array has not been created!\n"
	     << G4endl;
      
      return;
    }
  }
  
  // Create an array description that contains the involved readouts
  G4String ArrayDesc = SS.str();
  
  // Store the array descriptors for later use
  ASIMArrayID.push_back(ArrayID);
  ASIMArrayName.push_back(ArrayName);
  ASIMArrayDesc.push_back(ArrayDesc);
  ASIMArrayCoincident.push_back(ArrayCoincident);
  
  // Create a new TTree on the ASIM file to hold array data
  ASIMArrayEvents.push_back(ASIMStorageMgr->CreateEventTree(ArrayID,
							    ArrayName,
							    ArrayDesc));
  
  // Add this array to the store of readout arrays
  ArrayStore.push_back(Array);

  // Create run-level aggregators for this array
  ArrayEnabled.push_back(true);
  AIncidents.push_back(0);
  AHits.push_back(0);
  AEDep.push_back(0.);
  APhotonsCreated.push_back(0);
  APhotonsDetected.push_back(0);

  UseArrayEnergyThresholds.push_back(true);
  ArrayLowerEnergyThreshold.push_back(0.);
  ArrayUpperEnergyThreshold.push_back(1.*TeV);
  UseArrayPhotonThresholds.push_back(false);
}


void ASIMReadoutManager::ClearReadoutsAndArrays()
{
  NumReadouts = 0;

  ASIMReadoutID.clear();
  ASIMReadoutName.clear();
  ASIMReadoutDesc.clear();
  ASIMReadoutNameMap.clear();
  ASIMEvents.clear();

  ScintillatorSDNames.clear();
  PhotodetectorSDNames.clear();

  ReadoutEnabled.clear();
  RIncidents.clear();
  RHits.clear();
  REDep.clear();
  RPhotonsCreated.clear();
  RPhotonsDetected.clear();

  NumArrays = 0;

  ASIMArrayID.clear();
  ASIMArrayName.clear();
  ASIMArrayDesc.clear();
  ASIMArrayCoincident.clear();
  ASIMArrayEvents.clear();
  ArrayStore.clear();

  ArrayEnabled.clear();
  AIncidents.clear();
  AHits.clear();
  AEDep.clear();
  APhotonsCreated.clear();
  APhotonsDetected.clear();

  UseArrayEnergyThresholds.clear();
  ArrayLowerEnergyThreshold.clear();
  ArrayUpperEnergyThreshold.clear();
  UseArrayPhotonThresholds.clear();

  EventEDep.clear();
  EventActivated.clear();

  EnergyBroadening.clear();
  EnergyResolution.clear();
  EnergyEvaluation.clear();
  UseEnergyThresholds.clear();
  LowerEnergyThreshold.clear();
  UpperEnergyThreshold.clear();
  UsePhotonThresholds.clear();
  LowerPhotonThreshold.clear();
  UpperPhotonThreshold.clear();
  WaveformStorage.clear();
}


void ASIMReadoutManager::InitializeForRun()
{
  // Set all run-level readout and array aggreators to zero

  for(G4int r=0; r<NumReadouts; r++){
    RIncidents[r] = 0;
    RHits[r] = 0;
    REDep[r] = 0;
    RPhotonsCreated[r] = 0;
    RPhotonsDetected[r] = 0;
  }

  for(G4int a=0; a<NumArrays; a++){
    AIncidents[a] = 0;
    AHits[a] = 0;
    AEDep[a] = 0;
    APhotonsCreated[a] = 0;
    APhotonsDetected[a] = 0;
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
	  RIncidents[r]++;
	
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
	
	// Activate the readout for this event
	EventActivated[r] = true;
      }
    }
    else if(UsePhotonThresholds[r]){
      if(ASIMEvents[r]->GetPhotonsDetected() > LowerPhotonThreshold[r] and
	 ASIMEvents[r]->GetPhotonsDetected() < UpperPhotonThreshold[r]){
	
	// Activate the readout for this event
	EventActivated[r] = true;
      }
    }
  }
  AnalyzeAndStoreEvent();
}


void ASIMReadoutManager::AnalyzeAndStoreEvent()
{
  /////////////////////////
  // Individual readouts //
  /////////////////////////

  // Iterate over all individual readouts
  for(G4int r=0; r<NumReadouts; r++){
    
    // Skip disabled readouts
    if(!ReadoutEnabled[r])
      continue;
    
    // Aggregate run-level readout data for activated readouts
    if(EventActivated[r]){
      RHits[r]++;
      REDep[r] += EventEDep[r];
      RPhotonsCreated[r] += ASIMEvents[r]->GetPhotonsCreated();
      RPhotonsDetected[r] += ASIMEvents[r]->GetPhotonsDetected();
    }
    
    // Fill event trees if the event passed energy/photon threshold
    if(EventActivated[r] and ASIMFileOpen)
      ASIMStorageMgr->GetEventTree(ASIMReadoutID[r])->Fill();
  }

  
  ///////////////////
  // Array storage //
  ///////////////////
  
  // Iterate over all arrays in the store
  for(size_t a=0; a<ArrayStore.size(); a++){
    
    // Reset array event aggregators to zero
    ASIMArrayEvents[a]->Initialize();
    
    // Create and initialize local aggregators
    G4double EDep = 0.;
    G4int PhotonsCreated = 0, PhotonsDetected = 0;
    
    // Get the array vector: the length is the total number of
    // readouts; the index is the readout ID; the value is a bool
    // specifying if the corresponding readout is part of the array
    vector<G4bool> Array = ArrayStore[a];
    
    if(ASIMArrayCoincident.at(a) and EventActivated != Array)
      continue;
    
    // Iterate over all readouts 
    for(size_t r=0; r<Array.size(); r++){
      
      // Skip readouts that are not part of the array
      if(!Array[r])
	continue;
      
      // Aggregate event-level data for readouts in the array. Note
      // that energy is stored in the ASIMEvent class in units of MeV
      // so we'll multiply by MeV here to return to default G4 unit
      
      if(EventActivated[r]){
	EDep += ASIMEvents[r]->GetEnergyDep()*MeV;
	PhotonsCreated += ASIMEvents[r]->GetPhotonsCreated();
	PhotonsDetected += ASIMEvents[r]->GetPhotonsDetected();
      }
    }

    // Set values in the array's ASIMArrayEvent object
    ASIMArrayEvents[a]->SetEnergyDep(EDep/MeV);
    ASIMArrayEvents[a]->SetPhotonsCreated(PhotonsCreated);
    ASIMArrayEvents[a]->SetPhotonsDetected(PhotonsDetected);

    // Boolean to determine if event meets threshold criterion
    G4bool ArrayActivated = false;

    if(UseArrayEnergyThresholds[a]){
      if(EDep > ArrayLowerEnergyThreshold[a] and
	 EDep < ArrayUpperEnergyThreshold[a])
	ArrayActivated = true;
    }
    else if(UseArrayPhotonThresholds[a]){
      if(PhotonsDetected > ArrayLowerPhotonThreshold[a] and
	 PhotonsDetected < ArrayUpperPhotonThreshold[a])
	ArrayActivated = true;
    }

    if(ArrayActivated){
      
      // Aggregate run-level data for the array

      AHits[a]++;
      AEDep[a] += EDep;
      APhotonsCreated[a] += PhotonsCreated;
      APhotonsDetected[a] += PhotonsDetected;

      // Write the event data to the tree
      
      if(ASIMFileOpen)
	ASIMStorageMgr->GetEventTree(ASIMArrayID[a])->Fill();
    }
  }
}


void ASIMReadoutManager::CreateRunSummary(const G4Run *currentRun)
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
    RIncidents[r] = theMPImanager->SumIntsToMaster(RIncidents[r]);
    RHits[r] = theMPImanager->SumIntsToMaster(RHits[r]);
    REDep[r] = theMPImanager->SumDoublesToMaster(REDep[r]);
    RPhotonsCreated[r] = theMPImanager->SumDoublesToMaster(RPhotonsCreated[r]);
    RPhotonsDetected[r] = theMPImanager->SumDoublesToMaster(RPhotonsDetected[r]);
  }

  for(G4int a=0; a<NumArrays; a++){
    AIncidents[a] = theMPImanager->SumIntsToMaster(AIncidents[a]);
    AHits[a] = theMPImanager->SumIntsToMaster(AHits[a]);
    AEDep[a] = theMPImanager->SumDoublesToMaster(AEDep[a]);
    APhotonsCreated[a] = theMPImanager->SumDoublesToMaster(APhotonsCreated[a]);
    APhotonsDetected[a] = theMPImanager->SumDoublesToMaster(APhotonsDetected[a]);
  }
  
  G4cout << "\nASIMReadoutManager : Finished the MPI reduction of values to the master!\n"
	 << G4endl;
#endif
}


//////////////////////////////////////////
// Set/Get methods for readout settings //
//////////////////////////////////////////

void ASIMReadoutManager::SelectReadout(G4int R)
{
  if(R<NumReadouts)
    SelectedReadout = R;
  else
    G4cout << "\nASIMReadoutManager::SelectReadout():\n"
	   <<   "  The readout selected does not exist!\n"
	   << G4endl;
}


G4int ASIMReadoutManager::GetSelectedReadout()
{ return SelectedReadout;}


void ASIMReadoutManager::SetReadoutEnabled(G4bool RE)
{ ReadoutEnabled.at(SelectedReadout) = RE; }


G4bool ASIMReadoutManager::GetReadoutEnabled(G4int R)
{ return ReadoutEnabled.at(R); }


void ASIMReadoutManager::SetEnergyBroadening(G4bool B)
{ EnergyBroadening.at(SelectedReadout) = B; }


G4bool ASIMReadoutManager::GetEnergyBroadening(G4int R)
{ return EnergyBroadening.at(R); }


void ASIMReadoutManager::SetEnergyResolution(G4double E)
{ EnergyResolution.at(SelectedReadout) = E; }


G4double ASIMReadoutManager::GetEnergyResolution(G4int R)
{ return EnergyResolution.at(R); }


void ASIMReadoutManager::SetEnergyEvaluation(G4double E)
{ EnergyEvaluation.at(SelectedReadout) = E; }


G4double ASIMReadoutManager::GetEnergyEvaluation(G4int R)
{ return EnergyEvaluation.at(R); }


void ASIMReadoutManager::SetThresholdType(G4String TT)
{
  if(TT == "energy"){
    UseEnergyThresholds.at(SelectedReadout) = true;
    UsePhotonThresholds.at(SelectedReadout) = false;
  }
  else if (TT == "photon"){
    UseEnergyThresholds.at(SelectedReadout) = false;
    UsePhotonThresholds.at(SelectedReadout) = true;
  }
}


void ASIMReadoutManager::SetLowerEnergyThreshold(G4double LET)
{ LowerEnergyThreshold.at(SelectedReadout) = LET; }


G4double ASIMReadoutManager::GetLowerEnergyThreshold(G4int R)
{ return LowerEnergyThreshold.at(R); }


void ASIMReadoutManager::SetUpperEnergyThreshold(G4double UET)
{ UpperEnergyThreshold.at(SelectedReadout) = UET; }


G4double ASIMReadoutManager::GetUpperEnergyThreshold(G4int R)
{ return UpperEnergyThreshold.at(R); }


void ASIMReadoutManager::SetLowerPhotonThreshold(G4int LPT)
{ LowerPhotonThreshold.at(SelectedReadout) = LPT; }


G4int ASIMReadoutManager::GetLowerPhotonThreshold(G4int R)
{ return LowerPhotonThreshold.at(R); }


void ASIMReadoutManager::SetUpperPhotonThreshold(G4int UPT)
{ UpperPhotonThreshold.at(SelectedReadout) = UPT; }


G4int ASIMReadoutManager::GetUpperPhotonThreshold(G4int R)
{ return UpperPhotonThreshold.at(R); }


void ASIMReadoutManager::SetWaveformStorage(G4bool WS)
{ WaveformStorage.at(SelectedReadout) = WS; }


G4bool ASIMReadoutManager::GetWaveformStorage(G4int R)
{ return WaveformStorage.at(R); }


////////////////////////////////////////
// Set/Get methods for array settings //
////////////////////////////////////////

void ASIMReadoutManager::SelectArray(G4int A)
{
  if(A<NumArrays)
    SelectedArray = A;
  else
    G4cout << "\nASIMReadoutManager::SelectArray():\n"
	   <<   "  The array selected does not exist!\n"
	   << G4endl;
}


G4int ASIMReadoutManager::GetSelectedArray()
{ return SelectedArray;}


void ASIMReadoutManager::SetArrayEnabled(G4bool AE)
{ ArrayEnabled.at(SelectedArray) = AE; }


G4bool ASIMReadoutManager::GetArrayEnabled(G4int A)
{ return ArrayEnabled.at(A); }


void ASIMReadoutManager::SetArrayThresholdType(G4String TT)
{
  if(TT == "energy"){
    UseArrayEnergyThresholds.at(SelectedArray) = true;
    UseArrayPhotonThresholds.at(SelectedArray) = false;
  }
  else if (TT == "photon"){
    UseArrayEnergyThresholds.at(SelectedArray) = false;
    UseArrayPhotonThresholds.at(SelectedArray) = true;
  }
}

void ASIMReadoutManager::SetArrayLowerEnergyThreshold(G4double LET)
{ ArrayLowerEnergyThreshold.at(SelectedArray) = LET; }


G4double ASIMReadoutManager::GetArrayLowerEnergyThreshold(G4int A)
{ return ArrayLowerEnergyThreshold.at(A); }


void ASIMReadoutManager::SetArrayUpperEnergyThreshold(G4double UET)
{ ArrayUpperEnergyThreshold.at(SelectedArray) = UET; }


G4double ASIMReadoutManager::GetArrayUpperEnergyThreshold(G4int A)
{ return ArrayUpperEnergyThreshold.at(A); }


//////////////////////////////////////////
// Set/Get methods for event-level data //
//////////////////////////////////////////

void ASIMReadoutManager::SetEventActivated(G4bool EA)
{ EventActivated.at(SelectedReadout) = EA; }


G4bool ASIMReadoutManager::GetEventActivated(G4int R)
{ return EventActivated.at(R); }


////////////////////////////////////
// Get methods for run-level data //
////////////////////////////////////

// Readouts

G4int ASIMReadoutManager::GetReadoutIncidents(G4int R)
{
  if(R < (G4int)ASIMReadoutID.size())
    return RIncidents[R];
  else
    return 0;
}


G4int ASIMReadoutManager::GetReadoutHits(G4int R)
{
   if(R < (G4int)ASIMReadoutID.size())
     return RHits[R];
   else
     return 0;
}


G4double ASIMReadoutManager::GetReadoutEDep(G4int R)
{
  if(R < (G4int)ASIMReadoutID.size())
    return REDep[R];
  else
    return 0.;
}

G4int ASIMReadoutManager::GetReadoutPhotonsCreated(G4int R)
{
  if(R < (G4int)ASIMReadoutID.size())
    return RPhotonsCreated[R];
  else
    return 0;
}

G4int ASIMReadoutManager::GetReadoutPhotonsDetected(G4int R)
{
  if(R < (G4int)ASIMReadoutID.size())
    return RPhotonsDetected[R];
  else
    return 0;
}

// Arrays

G4int ASIMReadoutManager::GetArrayIncidents(G4int A)
{
  if(A < (G4int)ASIMArrayID.size())
    return AIncidents[A];
  else
    return 0;
}


G4int ASIMReadoutManager::GetArrayHits(G4int A)
{
  if(A < (G4int)ASIMArrayID.size())
    return AHits[A];
  else
    return 0;
}


G4double ASIMReadoutManager::GetArrayEDep(G4int A)
{
  if(A < (G4int)ASIMArrayID.size())
    return AEDep[A];
  else
    return 0.;
}


G4int ASIMReadoutManager::GetArrayPhotonsCreated(G4int A)
{
  if(A < (G4int)ASIMArrayID.size())
    return APhotonsCreated[A];
  else
    return 0;
}


G4int ASIMReadoutManager::GetArrayPhotonsDetected(G4int A)
{
  if(A < (G4int)ASIMArrayID.size())
    return APhotonsDetected[A];
  else
    return 0;
}

