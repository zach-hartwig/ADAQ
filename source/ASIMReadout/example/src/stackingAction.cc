#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "stackingAction.hh"
#include "runAction.hh"

#include <fstream>


stackingAction::stackingAction(runAction *theRun)
  : RunAction(theRun)
{;}


stackingAction::~stackingAction()
{;}


G4ClassificationOfNewTrack stackingAction::ClassifyNewTrack(const G4Track*)
{ return fUrgent;}
