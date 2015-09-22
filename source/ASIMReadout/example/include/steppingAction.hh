#ifndef steppingAction_hh
#define steppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4Track.hh"

using namespace std;


class steppingAction : public G4UserSteppingAction
{

public:
  steppingAction();
  ~steppingAction();

  void UserSteppingAction(const G4Step *);

private:
};

#endif
