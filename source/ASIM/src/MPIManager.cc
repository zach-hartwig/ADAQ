/////////////////////////////////////////////////////////////////////////////////
//
// name: MPIManager.cc
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The MPIManager is a meyer's singleton class that provides the
//       ability to rapidly parallelize a Geant4 simulation for use on
//       multicore of high-performance computing systems. While it
//       provided as part of the ASIM library, it is completely
//       generic and can be utilized in any Geant4 simulation to
//       provide rapid parallelization by simply compiling and linking
//       against the ASIM headers and libary in the standard way (An
//       example G4 simulation and GNU makefile is provided within the
//       example' directory). The MPIManager class is achieved using
//       and has been tested using the Open MPI
//       (http://www.open-mpi.org/) implementation of the MPI
//       standard.
//
//       It is important to ensure that a user's simulation that
//       incorporates the MPIManager can be built in sequential
//       architectures on systems that do not have Open MPI
//       installed. The 'MPI_ENABLED' macro enables the user's
//       makefile to include/exclude the Open MPI relevant code.
//
//       MPIManager is accompanied by the complementary MPIMessenger
//       class. The messenger provides the "beam on" command for
//       launching particles, and the user has the option to
//       distribute N particles as evenly as possible across all
//       available nodes or to distribute N particle on each node.
//
/////////////////////////////////////////////////////////////////////////////////

// Exclude MPI-relevant code from sequential simulation builds
#ifdef MPI_ENABLED

// Geant4
#include "G4RunManager.hh"

// C/C++
#include "limits.h"

// MPI
#include "mpi.h"

// ASIM
#include "MPIManager.hh"
#include "MPIMessenger.hh"


MPIManager *MPIManager::theMPImanager = 0;


MPIManager *MPIManager::GetInstance()
{ return theMPImanager; }


MPIManager::MPIManager(int argcMPI, char *argvMPI[])
{
  // Initialize the MPI execution environment.  The MPI::Init_thread
  // method is more specific than MPI::Init, allowing control of
  // thread level support.  Here, we use MPI::THREAD_SERIALIZED to
  // ensure that if multiple threads are present that only 1 thread
  // will make calls the MPI libraries at one time.
  MPI::Init_thread(argcMPI, argvMPI, MPI::THREAD_SERIALIZED);
  
  // Get the size (number of) and the rank the present process
  size = MPI::COMM_WORLD.Get_size();
  rank = MPI::COMM_WORLD.Get_rank();
  
  // Set G4bools for master/slave identification
  isMaster = (rank == RANK_MASTER);
  isSlave = (rank != RANK_MASTER);

  // If the present process is a slave, redirect its std::output to
  // the slaveForum, a special directory for output. The output file
  // is the base specified by argvMPI[1] combined with the slave rank.
  if(isSlave){
    G4String fBase = argvMPI[1];
    std::ostringstream slaveRank;
    slaveRank << rank;
    G4String fName = fBase + slaveRank.str() + ".out";

    // Redirect G4cout to the slave output file
    slaveOut.open(fName.c_str(), std::ios::out);
    G4cout.rdbuf(slaveOut.rdbuf());
  }

  // Initialize the number of events to be processed on all nodes
  totalEvents = 0;
  masterEvents = 0;
  slaveEvents = 0;

  // Initialize distribute events bool
  distributeEvents = true;

  // Create the seeds to ensure randomness in each node
  CreateSeeds();

  // Distribute the seeds to the nodes
  DistributeSeeds();

  if(theMPImanager)
    G4Exception("MPIManager::MPIManager()",
		"MPIManager-Exception00",
		FatalException,
		"The MPIManager singleton was constructed twice!\n");
  
  theMPImanager = this;

  theMPImessenger = new MPIMessenger(this);
}


MPIManager::~MPIManager()
{
  delete theMPImessenger;

  if(isSlave and slaveOut.is_open())
    slaveOut.close();
  
  MPI::Finalize(); 
}


// An MPI specific method to run the beam (primary particles),
// allowing the user to specify the number of events to run, as well
// as whether or not to distribute those events evenly across the
// available nodes or to run the same number of events on all
// nodes. Note that "events" is of type G4double in order to handle
// a total number of events greater then 2.1e9 (range of G4int data type)
void MPIManager::BeamOn(G4double events, G4bool distribute)
{
  // Set distribute events bool
  distributeEvents = distribute;

  // Obtain the run mananager
  G4RunManager *runManager = G4RunManager::GetRunManager();

  // If set, distribute the total events to be processed evenly across
  // all available nodes, assigning possible "remainders" to master
  if(distributeEvents){ 
    
    slaveEvents = G4int(events/size);
    masterEvents = G4int(events-slaveEvents*(size-1));
    
    if(isMaster) {
      G4cout << "\nMPIManager : # events in master = " << masterEvents 
	     << " / # events in slave = "  << slaveEvents << "\n" << G4endl;
    }
    
    totalEvents = events;

    // Error check to ensure events < range_G4int
    if(masterEvents > 2e9 or slaveEvents > 2e9)
      ThrowEventError();
    
    // "Prepare to run the beam!"  "You're always preparing!  Just run
    // it!"  "Sir, you already used that joke in SWS.cc..."
    if(isMaster) 
      G4RunManager::GetRunManager()->BeamOn(masterEvents);
    else
      G4RunManager::GetRunManager()->BeamOn(slaveEvents);
  }
  
  // Otherwise, each node will run totalEvents number of events
  else {

    slaveEvents = G4int(events);
    masterEvents = G4int(events);

    if(isMaster)
      G4cout << "\nMPIManager : # events in master = " << masterEvents 
	     << " / # events in slave = "  << slaveEvents << G4endl;
    
    // Error check to ensure events < range_G4int
    if(events>2e9)
      ThrowEventError();
      
    // Store the total number of events on all nodes
    totalEvents = events*size;  
    
     // Ruuuuuuuuuuuuuuuuun that baby!
    runManager->BeamOn(G4int(events));
  }
}


void MPIManager::ThrowEventError()
{
  // Because events variable must be passed to the G4RunManager,
  // which is expecting a G4int, unspecified behavior could result
  // if the events variable (which is a G4double and can exceed the
  // 2.1e9 data range of G4int) is actually passed. Therefore,
  // ensure that events is < 2.1e9 if the user has chosen NOT to
  // distribute events across the slaves
  G4cout << "\nMPIManager : You have chosen to run (n_events > n_G4int_range) on each of the \n"
	 <<   "             slaves! Since MPIManager passes n_events to the runManager\n"
	 <<   "             this could result in unspecified behavior. Please choose another\n"
	 <<   "             number of primary events or distribute them across nodes such that\n"
	 <<   "             (n_events < n_G4int_range = 2,147,483,647). SWS will now abort...\n"
	 << G4endl;
  
  G4Exception("MPIManager::ThrowEventError()",
	      "MPIManagerException002",
	      FatalException,
	      "MPIManager : Crashing this ship like the Hindenburg!\n");
}


// Method to create randomized seeds for each node.  I currently store
// the seeds in vector in case I want them later on in the executable.
// I may also want to replicate runs, in which case a master seed that
// generates subsequent seeds should be used.

void MPIManager::CreateSeeds()
{
  // Create a list of seeds that will be distributed to all the nodes
  for(G4int i=0; i<size; i++){
    G4double x = G4UniformRand();
    G4long seed = G4long(x*LONG_MAX);
    seedPacket.push_back(seed);
  }
  
  // Check to ensure that no two seeds are alike
  G4bool doubleCount = false;
  while(doubleCount){
    for(G4int i=0; i<size; i++)
      for(G4int j=0; j<size; j++)
	
	// If two seeds are the same, create a new seed
	if( (i!=j) and (seedPacket[i] == seedPacket[j]) ){
	  G4double x = G4UniformRand();
	  seedPacket[j] = G4long(x*LONG_MAX);
	  doubleCount=true;
	}
    doubleCount = false;
  }
}

// Distribute the seeds to the random number generator for each node
void MPIManager::DistributeSeeds()
{
  /*
  if(rank==0){
    for(size_t i=0; i<seedPacket.size(); i++)
      G4cout << "Rank[" << i << "] seed = " << seedPacket[i] << G4endl;
  }
  */
  CLHEP::HepRandom::setTheSeed(seedPacket[rank]);
}


// MPI::COMM_WORLD.Barrier forces all nodes to reach a common point
// before proceeding. Useful to ensure that nodes are communicating as
// well as synchronized.
void MPIManager::ForceBarrier(G4String location)
{ 
  MPI::COMM_WORLD.Barrier();

  G4cout << "\n\nMPIManager : All nodes have reached the MPI barrier at " << location << "!\n"
         <<     "             Nodes now proceeding onward ..."
	 << G4endl;
}


// MPI::COMM_WORLD.Reduce reduces values on all nodes to a single
// value on the specified node by using an MPI predefined operation
// or a user specified operation. 
G4double MPIManager::SumDoublesToMaster(G4double slaveValue)
{
  G4double masterSum = 0.;
  MPI::COMM_WORLD.Reduce(&slaveValue, &masterSum, 1, MPI::DOUBLE, MPI::SUM, 0);
  return masterSum;
}


G4int MPIManager::SumIntsToMaster(G4int slaveValue)
{
  G4int masterSum = 0;
  MPI::COMM_WORLD.Reduce(&slaveValue, &masterSum, 1, MPI::INT, MPI::SUM, 0);
  return masterSum;
}

#endif
