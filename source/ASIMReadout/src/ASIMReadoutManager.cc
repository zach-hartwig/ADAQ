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

// ROOT 
#include "TChain.h"

// C++
#include <sstream>

// SWS
#include "ASIMScintillatorSD.hh"
#include "ASIMOpticalReadoutSD.hh"
#include "ASIMReadoutManager.hh"
#include "ASIMReadoutMessenger.hh"
//#include "MPIManager.hh"

ASIMReadoutManager *ASIMReadoutManager::ASIMReadoutMgr = NULL;


ASIMReadoutManager *ASIMReadoutManager::GetInstance()
{ return ASIMReadoutMgr; }


ASIMReadoutManager::ASIMReadoutManager(G4bool arch)
  : parallelArchitecture(!arch), MPI_Rank(0), MPI_Size(1),
    ASIMNumReadouts(0)
{
  if(ASIMReadoutMgr != NULL)
    G4Exception("ASIMReadoutManager::ASIMReadoutManager()", 
		"ASIMReadoutManager-Excepction00", 
		FatalException, 
		"\nThe Meyer's singletone ASIMReadoutManager was constructed twice!\n");
  else 
    ASIMReadoutMgr = this;
  
  // Initialize ASIM readout classes
  
  ASIMFileName = "ASIMReadoutDefault.asim.root";
  ASIMStorageMgr = new ASIMStorageManager;
  ASIMRunSummary = new ASIMRun;
  
  theMessenger = new ASIMReadoutMessenger(this);
}


ASIMReadoutManager::~ASIMReadoutManager()
{
  delete theMessenger;
  
  for(size_t r=0; r<ASIMEvents.size(); r++)
    delete ASIMEvents.at(r);

  delete ASIMRunSummary;

  delete ASIMStorageMgr;
}


void ASIMReadoutManager::InitializeASIMFile()
{
  if(ASIMStorageMgr->GetASIMFileOpen()){
    G4cout << "\nASIMReadoutManager : An ASIM file is presently open for data output! A new file cannot be opened\n"
	   <<   "                     until the existing ASIM file is written to disk via the /ASIM/write command.\n"
	   << G4endl;
    
    return;
  }
  
#ifdef ASIM_MPI_ENABLED
  MPIManager *theMPIManager = MPIManager::GetInstance();
  MPI_Rank = theMPIManager->GetRank();
  MPI_Size = theMPIManager->GetSize();
#endif
  
  if(parallelArchitecture)
    ASIMStorageMgr->CreateParallelFile(ASIMFileName, MPI_Rank, MPI_Size);
  else
    ASIMStorageMgr->CreateSequentialFile(ASIMFileName);
}
  
  
void ASIMReadoutManager::WriteASIMFile(G4bool EmergencyWrite)
{
  if(!ASIMStorageMgr->GetASIMFileOpen()){
    G4cout << "\nASIMReadoutManager : There is no valid ASIM file presently open for writing! A new ASIM file should\n"
           <<   "                     first be created via the /ASIM/init command.\n"
	   << G4endl;
    
    return;
  }
  
  if(EmergencyWrite)
    G4cout << "\nASIMReadoutManager : The ASIM file that presently exists with data is being written to disk in\n"
           <<   "                     emergency fashion to avoid losing critical data before the simulation\n"
	   <<   "                     terminates. Please issue the /ASIM/write command before exiting next time!\n"
	   << G4endl;
  
  // Parallel readout to the ASIM file
  if(parallelArchitecture)
    ASIMStorageMgr->WriteParallelFile();
  else
    ASIMStorageMgr->WriteSequentialFile();
}


void ASIMReadoutManager::RegisterNewReadout(G4String ReadoutDesc,
					    G4VPhysicalVolume *Scintillator,
					    G4VPhysicalVolume *Photodetector)
{
  G4int ReadoutID = ASIMNumReadouts;
  G4String ReadoutName = Scintillator->GetLogicalVolume()->GetSensitiveDetector()->GetName();
 
  ASIMNumReadouts++;

  ASIMTreeID.push_back(ReadoutID);
  ASIMTreeName.push_back(ReadoutName);
  ASIMTreeDesc.push_back(ReadoutDesc);
  
  ASIMEvents.push_back(ASIMStorageMgr->CreateEventTree(ReadoutID,
						       ReadoutName,
						       ReadoutDesc));
  
  string ScintillatorSDName = ReadoutName + "Collection";
  ScintillatorSDNames.push_back(ScintillatorSDName);

  if(Photodetector != NULL){
    G4String PReadoutName = Photodetector->GetLogicalVolume()->GetSensitiveDetector()->GetName();
    PhotodetectorSDNames.push_back(PReadoutName);
  }
  else
    PhotodetectorSDNames.push_back("Photodetector does not exist for this readout!");    
  
  ReadoutEnabled.push_back(0);

  // Event-level variables
  EventEDep.push_back(0.);
  EventActivated.push_back(false);

  // Run-level aggregators
  Incidents.push_back(0);
  Hits.push_back(0);
  RunEDep.push_back(0.);
  PhotonsCreated.push_back(0);
  PhotonsCounted.push_back(0);

  // Readout settings
  EnergyBroadeningEnable.push_back(false);
  EnergyResolution.push_back(0.);
  EnergyEvaluation.push_back(0.);
  UseEnergyThresholds.push_back(true);
  LowerEnergyThreshold.push_back(0.);
  UpperEnergyThreshold.push_back(1.*TeV);
  UsePhotonThresholds.push_back(true);
  LowerPhotonThreshold.push_back(0);
  UpperPhotonThreshold.push_back(1000000000);
}


void ASIMReadoutManager::InitializeForRun()
{
  for(G4int r=0; r<ASIMNumReadouts; r++){
    Incidents[r] = 0;
    Hits[r] = 0;
    RunEDep[r] = 0;
    PhotonsCreated[r] = 0;
    PhotonsCounted[r] = 0;
  }
}


void ASIMReadoutManager::ReadoutEvent(const G4Event *currentEvent)
{
  G4int TheCollectionID = -1;
  G4SDManager *TheSDManager = G4SDManager::GetSDMpointer();
  
  for(G4int r=0; r<ASIMNumReadouts; r++){

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
	  if( (*ScintillatorHC)[i]->GetIsOpticalPhoton() )
	    ASIMEvents[r]->IncrementPhotonsCreated();
	  else
	    EventEDep[r] += (*ScintillatorHC)[i]->GetEnergyDep();
	}

	// Enable artificial gaussian energy broadening
	if(EnergyBroadeningEnable[r]){
	  
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
	
	ASIMOpticalReadoutSDHitCollection const *ReadoutHC =
	  dynamic_cast<ASIMOpticalReadoutSDHitCollection *>(HCE->GetHC(TheCollectionID));
	
	for(G4int i=0; i<ReadoutHC->entries(); i++)
	  ASIMEvents.at(r)->IncrementPhotonsDetected();
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
	if(ASIMStorageMgr->GetASIMFileOpen())
	  ASIMStorageMgr->GetEventTree(ASIMTreeID[r])->Fill();
      }
    }
    
    else if(UsePhotonThresholds[r]){
      if(ASIMEvents[r]->GetPhotonsDetected() > LowerPhotonThreshold[r] and
	 ASIMEvents[r]->GetPhotonsDetected() < UpperPhotonThreshold[r]){

	// Activate the detector for this event
	EventActivated[r] = true;
	
	// Fill detector trees if output to ASIM has been activated
	if(ASIMStorageMgr->GetASIMFileOpen())
	  ASIMStorageMgr->GetEventTree(ASIMTreeID[r])->Fill();
      }
    }
  }
  
  // Handle incrementing run-level information
  IncrementRunLevelData(EventActivated);
}


void ASIMReadoutManager::IncrementRunLevelData(vector<G4bool> &EventActivated)
{
  G4int EventSum = 0;
  
  for(G4int r=0; r<ASIMNumReadouts; r++){

    EventSum += EventActivated[r];
    
    if(EventActivated[r]){
      Hits[r]++;
      RunEDep[r] += EventEDep[r];
      PhotonsCreated[r] += ASIMEvents[r]->GetPhotonsCreated();
      PhotonsCounted[r] += ASIMEvents[r]->GetPhotonsDetected();
    }
  }
}


void ASIMReadoutManager::FillRunSummary(const G4Run *currentRun)
{
  if(parallelArchitecture)
    ReduceSlaveValuesToMaster();

  // In sequential or in parallel on the master node, add a class with
  // information from this run...

  if(ASIMStorageMgr->GetASIMFileOpen() and MPI_Rank == 0){

    ASIMRun *ASIMRunSummary = new ASIMRun;

    // Fill class with run-level data

    ASIMRunSummary->SetRunID( currentRun->GetRunID() );

#ifdef ASIM_MPI_ENABLED
    if(parallelArchitecture)
      ASIMRunSummary->SetTotalEvents( MPIManager::GetInstance()->GetTotalEvents() );
    else
#endif
      ASIMRunSummary->SetTotalEvents( currentRun->GetNumberOfEvent() );
    
    /*
      ASIMRunSummary->SetParticlesIncident( detectorIncidents );
      ASIMRunSummary->SetParticlesBetweenThresholds( detectorHits );
      ASIMRunSummary->SetDetectorLowerThresholdInMeV( detectorEnergyLowerThreshold/MeV );
      ASIMRunSummary->SetDetectorUpperThresholdInMeV( detectorEnergyUpperThreshold/MeV );
      ASIMRunSummary->SetPhotonsCreated( detectorPhotonsCreated );
      ASIMRunSummary->SetPhotonsDetected( detectorPhotonsCounted );
    */

    // Add the run class to the list for later readout
    ASIMStorageMgr->AddRun(ASIMRunSummary);
  }
}


void ASIMReadoutManager::HandleOpticalPhotonCreation()
{

  


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

	ASIMScintillatorSD *PostVolumeSD =
	  dynamic_cast<ASIMScintillatorSD *>(PostVolume->GetLogicalVolume()->GetSensitiveDetector());
	
	if(PostVolumeSD)
	  PostVolumeSD->ManualTrigger(CurrentStep->GetTrack());
	else{
	  
	  G4String ExceptionString = "\nThe derived G4VSensitiveDetector class object of type ASIMPhotodetectorSD\nassociated with the G4VPhysicalVolume '" +
	    PostVolume->GetName() +
	    "' could not be found!\nPlease register an ASIMPhotodetectorSD object in your detector construction!\n";
	  
	  G4Exception("ASIMReadoutManager::HandleOpticalPhotonDetection()",
		      "ASIMReadoutManager-Exception01",
		      FatalException,
		      ExceptionString);
	}
	break;
      }

      default:
	break;
      }
    }
  }
}


void ASIMReadoutManager::ReduceSlaveValuesToMaster()
{
#ifdef SPARROW_MPI_ENABLED
  
  G4cout << "\nASIMReadoutManager : Beginning the MPI reduction of data to the master!"
	 << G4endl;

  MPIManager *theMPImanager = MPIManager::GetInstance();

  /*
  detectorIncidents = theMPImanager->SumDoublesToMaster(detectorIncidents);
  detectorHits = theMPImanager->SumDoublesToMaster(detectorHits);
  detectorPhotonsCreated = theMPImanager->SumDoublesToMaster(detectorPhotonsCreated);
  detectorPhotonsCounted = theMPImanager->SumDoublesToMaster(detectorPhotonsCounted);
  */
  G4cout << "\nASIMReadoutManager : Finished the MPI reduction of values to the master!\n"
	 << G4endl;
#endif
}


void ASIMReadoutManager::SetActiveReadout(G4int R)
{
  if(R<ASIMNumReadouts)
    ActiveReadout = R;
  else
    G4cout << "\nASIMReadoutManager : Error! Specified readout number does not exist!\n"
	   << G4endl;
}


G4int ASIMReadoutManager::GetActiveReadout()
{ return ActiveReadout;}


void ASIMReadoutManager::SetReadoutEnabled(G4bool RE)
{ ReadoutEnabled.at(ActiveReadout) = RE; }


G4bool ASIMReadoutManager::GetReadoutEnabled(G4int R)
{ return ReadoutEnabled.at(R); }


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


void ASIMReadoutManager::SetPhotonsCounted(G4int P)
{ PhotonsCounted.at(ActiveReadout) = P; }


G4int ASIMReadoutManager::GetPhotonsCounted(G4int R)
{ return PhotonsCounted.at(R); }


void ASIMReadoutManager::SetEnergyBroadeningStatus(G4bool B)
{ EnergyBroadeningEnable.at(ActiveReadout) = B; }


G4bool ASIMReadoutManager::GetEnergyBroadeningStatus(G4int R)
{ return EnergyBroadeningEnable.at(R); }


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


void ASIMReadoutManager::EnablePhotonThresholds()
{
  UseEnergyThresholds.at(ActiveReadout) = false;
  UsePhotonThresholds.at(ActiveReadout) = true;
}


G4bool ASIMReadoutManager::GetUsePhotonThresholds(G4int R)
{ return UsePhotonThresholds.at(R); }


void ASIMReadoutManager::SetLowerEnergyThreshold(G4double LET)
{ LowerEnergyThreshold.at(LET); }


G4double ASIMReadoutManager::GetLowerEnergyThreshold(G4int R)
{ return LowerEnergyThreshold.at(R); }


void ASIMReadoutManager::SetUpperEnergyThreshold(G4double UET)
{ UpperEnergyThreshold.at(UET); }


G4double ASIMReadoutManager::GetUpperEnergyThreshold(G4int R)
{ return UpperEnergyThreshold.at(R); }


void ASIMReadoutManager::SetLowerPhotonThreshold(G4int LPT)
{ LowerPhotonThreshold.at(LPT); }


G4int ASIMReadoutManager::GetLowerPhotonThreshold(G4int R)
{ return LowerPhotonThreshold.at(R); }


void ASIMReadoutManager::SetUpperPhotonThreshold(G4int UPT)
{ UpperPhotonThreshold.at(UPT); }


G4int ASIMReadoutManager::GetUpperPhotonThreshold(G4int R)
{ return UpperPhotonThreshold.at(R); }
