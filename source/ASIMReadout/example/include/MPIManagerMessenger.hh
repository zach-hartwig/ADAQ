#ifndef MPIManagerMessenger_hh
#define MPIManagerMessenger_hh 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

#include "MPIManager.hh"

class MPIManagerMessenger : public G4UImessenger
{
public:
  MPIManagerMessenger(MPIManager *);
  ~MPIManagerMessenger();

  void SetNewValue(G4UIcommand *, G4String);

private:
  MPIManager *theMPImanager;
			   
  G4UIcmdWithAString *mpiBeamOnCmd;
};

#endif
