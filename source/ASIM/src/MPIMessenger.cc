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

// Geant4
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

// C++
#include <sstream>

// ASIM
#include "MPIMessenger.hh"


MPIMessenger::MPIMessenger(MPIManager *MPI)
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

  mpiBeamOnCmd = new G4UIcmdWithAString("/MPIManager/beamOn", this);
  mpiBeamOnCmd->SetGuidance("Start a parallel run using the MPIManager to distribute events to N nodes.");
  mpiBeamOnCmd->SetGuidance("Cmd line arguments: int  = <M numberOfEvents> (mandatory)");
  mpiBeamOnCmd->SetGuidance("                    bool = <distributeEvents> (optional; default == true)");
  mpiBeamOnCmd->SetGuidance("  --> If distributeEvents == true: each node receives N/M events to process");
  mpiBeamOnCmd->SetGuidance("  --> If distributeEvents == false: each node receives N events to process");
}


MPIMessenger::~MPIMessenger()
{
  delete mpiBeamOnCmd;
}


void MPIMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if(cmd == mpiBeamOnCmd){
    std::istringstream is(newValue);

    // Parse the command line

    // Number of events to process for this run. Note that
    // eventsToProcess is of type G4double to avoid the G4int range of
    // 2,147,483,647. Previously, specifying more than this number of
    // events was impossible using a G4int; however, G4double has a
    // range of ~10e15 and this should never be a problem.

    // Note that eventsToProcess is a G4double type as opposed to
    // G4int since the latter's range of 2,147,483,647 restricts the
    // ability to specify a total number of particle distributed
    // across all nodes that exceeds this limit. A check is put in
    // place within MPIManager to ensure that the number of particles
    // handled by each slave is less than this limit.

    // Total number of events to be processed
    G4double eventsToProcess = 0.;

    // String to control distribute/no-distribute of events to
    // slaves. The default behavior is to distribute
    G4String distributeString = "true";

    // Split the string command into number of events and distribute string
    is >> eventsToProcess >> distributeString;
    
#ifdef MPI_ENABLED

    // Run eventToProcess on each node
    if(distributeString == "false")
      theMPImanager->BeamOn(eventsToProcess, false);

    // Distribute eventsToProcess evenly across all nodes
    else if(distributeString == "true")
      theMPImanager->BeamOn(eventsToProcess, true);
    
    // Issue exception for incorrect second argument
    else
      G4Exception("MPIMessenger::SetNewValue()",
		  "MPIMessenger-Exception00",
		  FatalException,
		  "The second argument to /MPIManager/beamOn must be either 'true' or 'false'!");
#endif
  }
}
