#ifndef ASIMReadoutManager_hh
#define ASIMReadoutManager_hh 1

// Geant4
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"

// ROOT
#include "TFile.h"
#include "TTree.h"

// C++
#include <vector>
#include <map>
using namespace std;

// ASIM
#include "ASIMStorageManager.hh"
#include "ASIMReadoutMessenger.hh"
#include "ASIMEvent.hh"
#include "ASIMRun.hh"

class ASIMReadoutManager
{
public:
  ASIMReadoutManager(G4bool SequentialArchitecture=true);
  ~ASIMReadoutManager();

  static ASIMReadoutManager *GetInstance();
  
  void InitializeASIMFile();
  void WriteASIMFile(G4bool emergencyWrite=false);

  void RegisterNewReadout(G4String,
			  G4VPhysicalVolume *,
			  G4VPhysicalVolume *Photodetector = NULL);

  void InitializeForRun();
  void ReadoutEvent(const G4Event *);
  void FillRunSummary(const G4Run *);
  void IncrementRunLevelData(vector<G4bool> &);
  
  void HandleOpticalPhotonCreation(const G4Track *);
  void HandleOpticalPhotonDetection(const G4Step *);
  
  void ReduceSlaveValuesToMaster();


  // Set/Get methods for member data. First arg is always readout
  // number to account for multiple readout channels

  void SetActiveReadout(G4int);
  G4int GetActiveReadout();

  void SetEnergyBroadeningStatus(G4bool);
  G4bool GetEnergyBroadeningStatus(G4int);

  void SetEnergyResolution(G4double);
  G4double GetEnergyResolution(G4int);

  void SetEnergyEvaluation(G4double);
  G4double GetEnergyEvaluation(G4int);

  void EnableEnergyThresholds();
  G4bool GetUseEnergyThresholds(G4int);

  void EnablePhotonThresholds();
  G4bool GetUsePhotonThresholds(G4int);

  void SetLowerEnergyThreshold(G4double);
  G4double GetLowerEnergyThreshold(G4int);

  void SetUpperEnergyThreshold(G4double);
  G4double GetUpperEnergyThreshold(G4int);

  void SetLowerPhotonThreshold(G4int);
  G4int GetLowerPhotonThreshold(G4int);
  
  void SetUpperPhotonThreshold(G4int);
  G4int GetUpperPhotonThreshold(G4int);

  void SetDetectorPSDStatus(G4bool){};
  G4bool GetDetectorPSDStatus(){return false;}

  void SetDetectorPSDParticle(G4String){};
  G4String GetDetectorPSDParticle(){return "";}
  
  void SetWaveformStorage(G4bool);
  G4bool GetWaveformStorage(G4int);
  
  void SetReadoutEnabled(G4bool);
  G4bool GetReadoutEnabled(G4int);

  void SetIncidents(G4int);
  G4int GetIncidents(G4int);

  void SetHits(G4int);
  G4int GetHits(G4int);

  void SetRunEDep(G4double);
  G4double GetRunEDep(G4int);

  void SetPhotonsCreated(G4int);
  G4int GetPhotonsCreated(G4int);

  void SetPhotonsCounted(G4int);
  G4int GetPhotonsCounted(G4int);
  
  void SetCoincidentReadoutStatus(G4bool);
  G4bool GetCoincidentReadoutStatus();

  G4int GetASIMNumReadouts() {return ASIMNumReadouts;}
    
  void SetFileName(G4String FN) {ASIMFileName = FN;}
  G4String GetFileName() {return ASIMFileName;}

  G4bool CheckForOpenASIMFile() {return ASIMStorageMgr->GetASIMFileOpen(); }


private:
  static ASIMReadoutManager *ASIMReadoutMgr;
  G4bool parallelArchitecture;
  G4int MPI_Rank, MPI_Size;

  G4int ActiveReadout;
  vector<G4bool> ReadoutEnabled;

  vector<G4int> Incidents;
  vector<G4int> Hits;
  vector<G4double> RunEDep;
  vector<G4int> PhotonsCreated, PhotonsCounted;
  
  vector<G4bool> EnergyBroadeningEnable;
  vector<G4double> EnergyResolution, EnergyEvaluation;
  vector<G4bool> UseEnergyThresholds;
  vector<G4double> LowerEnergyThreshold, UpperEnergyThreshold;
  vector<G4bool> UsePhotonThresholds;
  vector<G4int> LowerPhotonThreshold, UpperPhotonThreshold;
  vector<G4bool> WaveformStorageEnable;

  vector<G4double> EventEDep;
  vector<G4bool> EventActivated;
  
  G4String fileName;
  std::vector<G4String> slaveFileNames;

  vector<G4String> ScintillatorSDNames, PhotodetectorSDNames;

  // Variables for SD readout into an ASIM file

  G4String ASIMFileName;
  ASIMStorageManager *ASIMStorageMgr;
  ASIMRun *ASIMRunSummary;

  G4int ASIMNumReadouts;
  
  vector<ASIMEvent *> ASIMEvents;
  vector<G4int> ASIMTreeID;
  vector<G4String> ASIMTreeName, ASIMTreeDesc;

  // Messenger class for runtime command
  ASIMReadoutMessenger *theMessenger;
};

#endif
