#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"

#include "ASIMReadoutManager.hh"
#include "MPIManager.hh"

#include "runAction.hh"
#include "geometryConstruction.hh"
#include "PGA.hh"


runAction::runAction()
{
  TheMessenger = new runActionMessenger(this);
}


runAction::~runAction()
{
  delete TheMessenger;
}


void runAction::BeginOfRunAction(const G4Run *)
{
  // Initialize run-level variables for a new run
  ASIMReadoutManager::GetInstance()->InitializeForRun();
}


void runAction::EndOfRunAction(const G4Run *currentRun)
{
  ASIMReadoutManager *ARMgr = ASIMReadoutManager::GetInstance();
  
#ifdef MPI_ENABLED
  MPIManager *MPIMgr = MPIManager::GetInstance();
  MPIMgr->ForceBarrier("runAction::EndOfRunAction()");
#endif

  ARMgr->CreateRunSummary(currentRun);

  const G4int NumReadouts = ARMgr->GetNumReadouts();
  
  G4cout << "\n\n"
	 << "\n============== ASIMExample run-level results ==============\n\n";
  
  for(G4int r=0; r<NumReadouts; r++){
    G4int Incs = ARMgr->GetReadoutIncidents(r);
    G4int Hits = ARMgr->GetReadoutHits(r);
    G4double Eff = 0.;
    if(Incs > 0)
      Eff = Hits * 1. / Incs;

    G4int Created = ARMgr->GetReadoutPhotonsCreated(r);
    G4int Detected = ARMgr->GetReadoutPhotonsDetected(r);
    G4double OpticalEff = 0.;
    if(Created > 0)
      OpticalEff = Detected * 1. / Created;
    
    G4cout << "   Run summary for the " << ARMgr->GetReadoutName(r) << ":\n"
	   << "      Incidents  : " << Incs << "\n"
	   << "      Hits       : " << Hits << "\n"
	   << "      Efficiency : " << Eff << "\n"
	   << "\n"
	   << "      Photons created    : " << Created << "\n"
	   << "      Photons detected   : " << Detected << "\n"
	   << "      Optical efficiency : " << OpticalEff << "\n"
	   << G4endl;
  }
  
  G4cout << "===========================================================\n\n"
	 <<  G4endl;
}
