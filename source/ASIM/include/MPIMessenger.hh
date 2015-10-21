/////////////////////////////////////////////////////////////////////////////////
//
// name: MPIMessenger.hh
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The MPIMessenger is a standard G4 interface class that
//       provides a Geant4 command line method to run particles in
//       parallel architecture builds of the user's Geant4 simulation
//       that utilizes the MPIManager class for parallelization. See
//       the comments in the header of MPIManager class for details.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __MPIMessenger_hh__
#define __MPIMessenger_hh__ 1

// Geant4
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

// ASIM
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
