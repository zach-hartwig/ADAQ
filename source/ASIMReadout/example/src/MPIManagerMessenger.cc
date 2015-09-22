#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <sstream>

#include "MPIManagerMessenger.hh"


MPIManagerMessenger::MPIManagerMessenger(MPIManager *MPI)
  : theMPImanager(MPI)
{
  // Create a command that will replace /run/beamOn when using MPI
  // parallelization.  There are two arguments: the first is the
  // number of events to be processed.  The second is whether or not
  // to distribute the events evenly across all the available nodes.
  // If distributing, the total events for all nodes will equal the
  // events specified by the first argument; if not distributing, the
  // total events for all nodes will equal the first argument times
  // the number of nodes used for processing.  The default is to
  // distribute events across all nodes.

  mpiBeamOnCmd = new G4UIcmdWithAString("/Sparrow/mpiBeamOn", this);
  mpiBeamOnCmd->SetGuidance("Start a parallel run using MPI");
  mpiBeamOnCmd->SetGuidance("Args: int = <numberOfEvents>;  bool = <distributeEvents>");
}


MPIManagerMessenger::~MPIManagerMessenger()
{
  delete mpiBeamOnCmd;
}


void MPIManagerMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if(cmd == mpiBeamOnCmd){
    std::istringstream is(newValue);

    // Parse the command line

    // Number of events to process for this run. Note that
    // eventsToProcess is of type G4double to avoid the G4int range of
    // 2,147,483,647. Previously, specifying more than this number of
    // events was impossible using a G4int; however, G4double has a
    // range of ~10e15 and this should never be a problem.
    G4double eventsToProcess;

    // String to control distribute/no-distribute of events to slaves
    G4String distributeString;
    G4bool distributeBool = true;

    is >> eventsToProcess >> distributeString;
    
    if(distributeString=="false")
      distributeBool = false;
    
#ifdef SWS_MPI_ENABLED
    theMPImanager->BeamOn(eventsToProcess, distributeBool);
#endif
  }
}


