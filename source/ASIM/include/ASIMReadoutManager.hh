/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMReadoutManager.hh
// date: 11 Jan 16
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMReadoutManager class handles the automated extraction
//       of Geant4 simulated detector data for registered "readouts"
//       (data readout from a single G4SensitiveDetector object) and
//       "arrays" (data readout from a collection of readouts); arrays
//       have the option of readout only if a coincidence if found
//       between all readouts that compose the array. The event- and
//       run-level data is aggregated and made available to the user
//       via standard "Get" methods. Data can optionally be stored
//       into an ASIM file for offline analysis.
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

  void CreateArray(G4String, vector<G4int>, G4bool);

  void ClearReadoutsAndArrays();

  void InitializeForRun();
  void ReadoutEvent(const G4Event *);
  void AnalyzeAndStoreEvent();
  void CreateRunSummary(const G4Run *);
  
  void HandleOpticalPhotonCreation(const G4Track *);
  void HandleOpticalPhotonDetection(const G4Step *);
  
  

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

  void SetThresholdType(G4String);
  G4String GetThresholdType(G4int);

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

  // Set/Get methods for array settings

  void SelectArray(G4int);
  G4int GetSelectedArray();

  void SetArrayEnabled(G4bool);
  G4bool GetArrayEnabled(G4int);

  void SetArrayThresholdType(G4String);
  G4String GetArrayThresholdType(G4int);

  void EnableArrayEnergyThresholds();
  G4bool GetEnableArrayEnergyThresholds(G4int);

  void SetArrayLowerEnergyThreshold(G4double);
  G4double GetArrayLowerEnergyThreshold(G4int);

  void SetArrayUpperEnergyThreshold(G4double);
  G4double GetArrayUpperEnergyThreshold(G4int);

  // Set/Get methods for event-level data

  void SetEventActivated(G4bool);
  G4bool GetEventActivated(G4int);

  // Set/Get methods for run-level data

  G4int GetReadoutIncidents(G4int);
  G4int GetReadoutHits(G4int);
  G4double GetReadoutEDep(G4int);
  G4int GetReadoutPhotonsCreated(G4int);
  G4int GetReadoutPhotonsDetected(G4int);

  G4int GetArrayIncidents(G4int);
  G4int GetArrayHits(G4int);
  G4double GetArrayEDep(G4int);
  G4int GetArrayPhotonsCreated(G4int);
  G4int GetArrayPhotonsDetected(G4int);

  // Set/Get methods for general purpose data members

  void EnableSequentialMode() {parallelProcessing = false;}
  void EnableParallelMode() {parallelProcessing = true;}

  // Get total number of registered readouts
  G4int GetNumReadouts() {return NumReadouts;}
  G4int GetNumArrays() {return NumArrays;}
  
  // Get the readout name using the readout ID
  G4String GetReadoutName(G4int ID) {return ASIMReadoutName.at(ID);}
  G4String GetArrayName(G4int ID) {return ASIMArrayName.at(ID);}

  // Get the readout ID using the readout name
  G4int GetReadoutID(G4String Name) {return ASIMReadoutNameMap.at(Name);}

  G4bool CheckForOpenASIMFile() {return ASIMFileOpen;}
  

private:
  static ASIMReadoutManager *ASIMReadoutMgr;
  
  // Variables to handle parellel architectures
  G4bool parallelProcessing;
  G4int MPI_Rank, MPI_Size;
  vector<G4String> slaveFileNames;

  // High level readout variables
  G4int NumReadouts, SelectedReadout;
  vector<G4String> ScintillatorSDNames, PhotodetectorSDNames;

  // High level array variables
  G4int NumArrays, SelectedArray;
  vector<vector<G4bool> > ArrayStore;
  
  // Run-level readout aggregators

  vector<G4bool> ReadoutEnabled;
  vector<G4int> RIncidents;
  vector<G4int> RHits;
  vector<G4double> REDep;
  vector<G4int> RPhotonsCreated, RPhotonsDetected;

  // Run-level array aggregators

  vector<G4bool> ArrayEnabled;
  vector<G4int> AIncidents;
  vector<G4int> AHits;
  vector<G4double> AEDep;
  vector<G4int> APhotonsCreated, APhotonsDetected;

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

  vector<G4bool> UseArrayEnergyThresholds;
  vector<G4double> ArrayLowerEnergyThreshold, ArrayUpperEnergyThreshold;
  vector<G4bool> UseArrayPhotonThresholds;
  vector<G4double> ArrayLowerPhotonThreshold, ArrayUpperPhotonThreshold;

  // ASIM file variables

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
  vector<G4bool> ASIMArrayCoincident;
  G4int ASIMArrayIDOffset;
  
  // Messenger class for runtime command
  ASIMReadoutMessenger *theMessenger;
};

#endif
