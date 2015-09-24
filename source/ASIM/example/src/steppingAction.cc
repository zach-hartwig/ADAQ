#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"
#include "G4SDManager.hh"
#include "G4ParticleTypes.hh"
#include "G4HadronicProcess.hh"
#include "G4Nucleus.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4BraggModel.hh"

#include <iomanip>
using namespace std;

#include "ASIMReadoutManager.hh"

#include "steppingAction.hh"


steppingAction::steppingAction()
{;}


steppingAction::~steppingAction()
{;}

 
void steppingAction::UserSteppingAction(const G4Step *currentStep)
{
  ASIMReadoutManager::GetInstance()->HandleOpticalPhotonDetection(currentStep);
}
