/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMReadoutManager.hh
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMReadoutManager class handles the automated extraction
//       of Geant4 simulated detector data for "readouts" that are
//       registered by the user. Each "readout" is attached to an
//       individual G4SensitiveDetector object as the hook into
//       extracting Geant4 data. Once extracted, the data is handed to
//       its sister class ASIMStorageManager for persistent storage to
//       disk in a standardized ROOT file known as an ASIM File.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ASIMReadoutManager_hh__
#define __ASIMReadoutManager_hh__ 1

// Geant4
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"

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
  ASIMReadoutManager();
  ~ASIMReadoutManager();
  
  static ASIMReadoutManager *GetInstance();
  
  void InitializeASIMFile();
  void WriteASIMFile(G4bool emergencyWrite=false);

  void RegisterNewReadout(G4String,
			  G4VPhysicalVolume *,
			  G4VPhysicalVolume *Photodetector = NULL);
  void ClearReadouts();
  void InitializeForRun();
  void ReadoutEvent(const G4Event *);
  void AnalyzeAndStoreEvent();
  void IncrementRunLevelData();
  void FillRunSummary(const G4Run *);
  
  void HandleOpticalPhotonCreation(const G4Track *);
  void HandleOpticalPhotonDetection(const G4Step *);
  
  void CreateArray(G4String, vector<G4int>);
  void ClearArrayStore();

  void CreateCoincidence(G4String, vector<G4int>);
  void ClearCoincidenceStore();
  
  void ReduceSlaveValuesToMaster();

  
  /////////////////////
  // Set/Get Methods //
  /////////////////////
  
  // For all 'Set' methods, the user must first select the readout for
  // which all following settings will be applied.

  // For all 'Get' methods, the user should pass the readout ID as the
  // method argument to return the desired member data.

  // Set/Get methods for high-level readout control

  void SetFileName(G4String FN) {ASIMFileName = FN;}
  G4String GetFileName() {return ASIMFileName;}

  void SetCoincidenceEnabled(G4bool CE) {CoincidenceEnabled = CE;}
  G4bool GetCoincidenceEnabled() {return CoincidenceEnabled;}

  // Set/Get methods for readout settings

  void SelectReadout(G4int);
  G4int GetSelectedReadout();
  
  void SetReadoutEnabled(G4bool);
  G4bool GetReadoutEnabled(G4int);
  
  void SetEnergyBroadening(G4bool);
  G4bool GetEnergyBroadening(G4int);

  void SetEnergyResolution(G4double);
  G4double GetEnergyResolution(G4int);

  void SetEnergyEvaluation(G4double);
  G4double GetEnergyEvaluation(G4int);

  void EnableEnergyThresholds();
  G4bool GetUseEnergyThresholds(G4int);

  void SetLowerEnergyThreshold(G4double);
  G4double GetLowerEnergyThreshold(G4int);

  void SetUpperEnergyThreshold(G4double);
  G4double GetUpperEnergyThreshold(G4int);
  
  void EnablePhotonThresholds();
  G4bool GetUsePhotonThresholds(G4int);

  void SetLowerPhotonThreshold(G4int);
  G4int GetLowerPhotonThreshold(G4int);
  
  void SetUpperPhotonThreshold(G4int);
  G4int GetUpperPhotonThreshold(G4int);

  void SetWaveformStorage(G4bool);
  G4bool GetWaveformStorage(G4int);

  /*
  void SetDetectorPSDStatus(G4bool){};
  G4bool GetDetectorPSDStatus(){return false;}

  void SetDetectorPSDParticle(G4String){};
  G4String GetDetectorPSDParticle(){return "";}
  */

  // Set/Get methods for event-level data

  void SetEventActivated(G4bool);
  G4bool GetEventActivated(G4int);

  // Set/Get methods for run-level data

  void SetIncidents(G4int);
  G4int GetIncidents(G4int);

  void SetHits(G4int);
  G4int GetHits(G4int);

  void SetRunEDep(G4double);
  G4double GetRunEDep(G4int);

  void SetPhotonsCreated(G4int);
  G4int GetPhotonsCreated(G4int);

  void SetPhotonsDetected(G4int);
  G4int GetPhotonsDetected(G4int);

  // Get methods for voincidence data

  int GetNumCoincidences() {return CoincidenceHits.size();}
  int GetCoincidenceHits(G4int C) {return CoincidenceHits.at(C);}
  int GetNonCoincidenceHits() {return NonCoincidenceHits;}
  
  // Set/Get methods for general purpose data members

  void EnableSequentialMode() {parallelProcessing = false;}
  void EnableParallelMode() {parallelProcessing = true;}

  // Get total number of registered readouts
  G4int GetNumReadouts() {return NumReadouts;}
  
  // Get the readout name using the readout ID
  G4String GetReadoutName(G4int ID) {return ASIMReadoutName.at(ID);}

  // Get the readout ID using the readout name
  G4int GetReadoutID(G4String Name) {return ASIMReadoutNameMap.at(Name);}

  G4bool CheckForOpenASIMFile() {return ASIMFileOpen;}
  

private:
  static ASIMReadoutManager *ASIMReadoutMgr;
  
  // Variables to handle parellel architectures
  G4bool parallelProcessing;
  G4int MPI_Rank, MPI_Size;
  vector<G4String> slaveFileNames;

  // High-level readout variables
  G4int NumReadouts, SelectedReadout;

  G4bool CoincidenceEnabled;
  vector<vector<G4bool> > CoincidenceStore;
  vector<int> CoincidenceHits;
  int NonCoincidenceHits;

  vector<vector<G4int> > ArrayStore;
  
  vector<G4String> ScintillatorSDNames, PhotodetectorSDNames;

  // Run-level aggregator variables
  vector<G4bool> ReadoutEnabled;
  vector<G4int> Incidents;
  vector<G4int> Hits;
  vector<G4double> RunEDep;
  vector<G4int> PhotonsCreated, PhotonsDetected;

  // Event-level variables
  vector<G4double> EventEDep;
  vector<G4bool> EventActivated;
  
  // ASIM readout-specific settings
  vector<G4bool> EnergyBroadening;
  vector<G4double> EnergyResolution, EnergyEvaluation;
  vector<G4bool> UseEnergyThresholds;
  vector<G4double> LowerEnergyThreshold, UpperEnergyThreshold;
  vector<G4bool> UsePhotonThresholds;
  vector<G4int> LowerPhotonThreshold, UpperPhotonThreshold;
  vector<G4bool> WaveformStorage;

  // Variables for SD readout into an ASIM file
  G4bool ASIMFileOpen;
  G4String ASIMFileName;
  ASIMStorageManager *ASIMStorageMgr;
  ASIMRun *ASIMRunSummary;
  vector<ASIMEvent *> ASIMEvents;
  vector<G4int> ASIMReadoutID;
  vector<G4String> ASIMReadoutName, ASIMReadoutDesc;
  map<G4String, G4int> ASIMReadoutNameMap;
  G4int ASIMReadoutIDOffset;

  vector<ASIMEvent *> ASIMArrayEvents;
  vector<G4int> ASIMArrayID;
  vector<G4String> ASIMArrayName, ASIMArrayDesc;
  G4int ASIMArrayIDOffset;
  
  // Messenger class for runtime command
  ASIMReadoutMessenger *theMessenger;
};

#endif
