#ifndef runAction_hh
#define runAction_hh 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"

#include "runActionMessenger.hh"

class runAction : public G4UserRunAction
{

public:
  runAction();
  ~runAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  
private:
  runActionMessenger *TheMessenger;
};


#endif
