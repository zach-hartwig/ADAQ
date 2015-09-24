#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"

#include "ASIMReadoutManager.hh"

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


void runAction::EndOfRunAction(const G4Run *)
{
  G4RunManager *theRunManager = G4RunManager::GetRunManager();

  ASIMReadoutManager *ARMgr = ASIMReadoutManager::GetInstance();

  G4cout << "\n\n"
	 << "\n======================= ASIMExample run-level results =======================\n"
	 << "\n=============================================================================\n\n"
	 <<  G4endl;
}
