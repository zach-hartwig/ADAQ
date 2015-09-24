#ifndef MPIManager_hh
#define MPIManager_hh 1

#include "globals.hh"

#include <fstream>

class MPIManagerMessenger;

/*
  MPIManager is a class that provides an OpenMPI interface for
  ACRONYM.  The MPIManager is constructed as a C++ singleton
  class, ie, only a single object of this type is permitted to exist.
  The MPIManager singleton object may be obtained via the
  MPIManager::GetInstance() method
*/


class MPIManager
{
#ifdef SWS_MPI_ENABLED
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
  MPIManagerMessenger *theMPImessenger;

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
