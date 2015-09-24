#ifndef stackingAction_hh
#define stackingAction_hh 1

#include "G4UserStackingAction.hh"

#include <fstream>

class runAction;

class stackingAction : public G4UserStackingAction
{
public:
  stackingAction();
  ~stackingAction();
  
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
  
private:
};

#endif

