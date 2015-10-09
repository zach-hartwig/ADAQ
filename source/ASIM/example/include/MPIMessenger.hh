#ifndef MPIMessenger_hh
#define MPIMessenger_hh 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

#include "MPIManager.hh"

class MPIMessenger : public G4UImessenger
{
public:
  MPIMessenger(MPIManager *);
  ~MPIMessenger();

  void SetNewValue(G4UIcommand *, G4String);

private:
  MPIManager *theMPImanager;
			   
  G4UIcmdWithAString *mpiBeamOnCmd;
};

#endif
