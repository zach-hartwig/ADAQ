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

#ifndef __MPIManager_hh__
#define __MPIManager_hh__ 1

// Geant4
#include "globals.hh"

// C++
#include <fstream>

/// ASIM
class MPIMessenger;

class MPIManager
{
  // Exclude MPI-relevant code from sequential simulation builds
#ifdef MPI_ENABLED
 public:
  MPIManager(int, char **);
  ~MPIManager();
  
  // Static method to obtain the singleton MPI manager
  static MPIManager *GetInstance();
  
  // Method to run the beam used by MPImessenger
  void BeamOn(G4double, G4bool);
  
  // Methods to create and distribute seeds to nodes
  void CreateSeeds();
  void DistributeSeeds();

  // Method to obtain number of procs
  G4int GetSize() {return size;}

  // Method to obtain the node rank
  G4int GetRank() {return rank;}

  // Method for obtaining/operatong on the events to be processed
  G4double GetTotalEvents() {return totalEvents;}
  G4int GetMasterEvents() {return masterEvents;}
  G4int GetSlaveEvents() {return slaveEvents;}
  void ThrowEventError();

  // Method to determine whether events are distributed
  G4bool GetDistributeEvents() {return distributeEvents;}

  // Method to force an MPI barrier for all nodes
  void ForceBarrier(G4String);

  // Method to sum doubles on all nodes to a single value on the master
  G4double SumDoublesToMaster(G4double);

  // Method to sum ints on all nodes to a single value on the master
  G4int SumIntsToMaster(G4int);

private:
  // The singleton object
  static MPIManager *theMPImanager;

  // The messenger interface for run time commands
  MPIMessenger *theMPImessenger;

  // size == number of procs; rank == process id
  G4int size, rank;
  G4bool isMaster, isSlave;
  enum {RANK_MASTER, RANK_SLAVE};
  std::ofstream slaveOut;
  
  G4double totalEvents;
  G4int masterEvents, slaveEvents;
  G4bool distributeEvents;

  std::vector<G4long> seedPacket;
#endif
};

#endif
