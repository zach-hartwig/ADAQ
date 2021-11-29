/////////////////////////////////////////////////////////////////////////////////
//
// name: MPIManager.cc
// date: 28 Nov 21
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The MPIManager is a meyer's singleton class that provides the
//       ability to rapidly parallelize a user's Geant4 simulation for
//       use on multicore or high-performance computing systems. It is
//       generic and can be integrated into any Geant4 simulation for
//       straightforward paralellization using Open MPI. The
//       MPIMessenger class is a complementary class to enable
//       particle source (e.g. /run/beamOn) in parallel processing.
//
//       Integration of MPI into Geant4 simulation is simple:
//
//       1. In the Geant4 simulation Makefile, include the ASIM header
//          directory and link the binary against the ASIM library in
//          the Geant4 Makefile. Add the MPI_ENABLED C++ compiler
//          macro to enable excluding Open MPI code, which enables the
//          simulation to be optionally built on systems without Open
//          MPI installed
//
//       2. Declare a concrete instance of the MPIManager (it is a
//          singleton) and utilize within the simulation
//
//       This version has MPIManager has been compiled and tested with
//       OpenMPI version 4.1.2. Note that as of OpenMPI 2.X.X, all C++
//       bindings were depracated; MPIManager (as of 28 Nov 21) has
//       been updated to use the standard C bindings for compliance.
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

  // Messenger class for runtime commands
  MPIMessenger *theMPImessenger;

  // Variables for MPI implementation
  G4int threadSupport;
  G4int size; // Number of active MPI processes
  G4int rank; // MPI process identification number
  G4bool isMaster, isSlave;
  enum {RANK_MASTER, RANK_SLAVE};
  std::ofstream slaveOut;

  // Variables to handle Geant4 event creation
  G4double totalEvents;
  G4int masterEvents, slaveEvents;
  G4bool distributeEvents;

  // Variable to hold unique seeds for each node
  std::vector<G4long> seedPacket;
#endif
};

#endif
