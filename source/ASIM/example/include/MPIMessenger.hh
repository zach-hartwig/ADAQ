/////////////////////////////////////////////////////////////////////////////////
//
// name: MPIMessenger.hh
// date: 09 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The MPIMessenger is a standard G4 interface class that
//       provide a command line method to run particles in parallel
//       architecture builds of the user's Geant4 simulation that
//       utilizes the MPIManager class for parallelization.
//
/////////////////////////////////////////////////////////////////////////////////

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
