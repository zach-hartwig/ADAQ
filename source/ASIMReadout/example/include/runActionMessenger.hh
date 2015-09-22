#ifndef runActionMessenger_hh
#define runActionMessenger_hh 1

#include "G4UImessenger.hh"

class runAction;

class runActionMessenger : public G4UImessenger
{

public:
  runActionMessenger(runAction *);
  ~runActionMessenger();

  void SetNewValue(G4UIcommand *, G4String);
  
private:
  runAction *RunAction;
};

#endif
