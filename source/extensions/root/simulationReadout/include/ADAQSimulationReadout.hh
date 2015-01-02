/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQSimulationReadout.hh
// date: 23 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ADAQSimulationReadout class provides a framework for
//       reading out simulated detector data into event- and run-level
//       information class containers. The primary purpose is to
//       provide a standardized, flexible method for high efficiency
//       detector data persistency and post-simulation data analysis
//       using software tools (e.g. the ADAQAnalysis program). The
//       class combines the ADAQSimulationEvent and ADAQSimultionRun
//       classes for containerized data storage with ROOT TCollection
//       objects to facilitate data storage. The class is primarily
//       developed for use with Geant4 "Sensitive Detectors" but in
//       principle could be used by any Monte Carlo simulation that
//       can be integrated with the ROOT toolkit.
//
// 2use: The user should fill an ADAQSimulationEvent concrete object
//       at the end of each simulated event, and an ADAQSimulationRun
//       concrete object at the end of each simulation run. These
//       objects can be stored within this class using ROOT's
//       TCollection-derived "TList" class. After the simulation is
//       complete, the ADAQSimulationReadout class should be written
//       to a ROOT file with the extension ".asim" to indicate that it
//       is an ADAQSimulationReadout-formated ROOT file.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQSimulationReadout_hh__
#define __ADAQSimulationReadout_hh__ 1

#include <TObject.h>
#include <TList.h>
#include <TString.h>
#include <TTree.h>

#include <string>
#include <map>
#include <ctime>

#include "ADAQSimulationEvent.hh"
#include "ADAQSimulationRun.hh"

class ADAQSimulationReadout : public TObject
{
public:
  ADAQSimulationReadout();
  ~ADAQSimulationReadout();

  // Action methods for metadata objects

  void PopulateMetadata();
  void WriteMetadata();

  // Action methods for the Event TTrees

  ADAQSimulationEvent *CreateEventTree(Int_t, TString, TString);
  void AddEventTree(Int_t, TTree *);
  void ListEventTrees();
  TTree *GetEventTree(std::string);
  TTree *GetEventTree(Int_t);
  Int_t GetEventTreeID(std::string);
  std::string GetEventTreeName(Int_t);
  Int_t GetNumberOfEventTrees();
  void RemoveEventTree(std::string);
  void RemoveEventTree(Int_t);
  void WriteEventTrees();

  // Action methods for the Runs

  void AddRun(ADAQSimulationRun *);
  ADAQSimulationRun *GetRun(Int_t);
  Int_t GetNumberOfRuns();
  void ListRuns();
  void WriteRuns();

  // Set/Get methods for class member data

  TString GetMachineName() {return MachineName->GetString();}
  TString GetMachineUser() {return MachineUser->GetString();}
  TString GetFileDate() {return FileDate->GetString();}
  TString GetFileVersion() {return FileVersion->GetString();}

  TList *GetEventTreeList() {return EventTreeList;}
  TList *GetRunList() {return RunList;}

  void WriteToFile();
  
private:
  // Metadata
  TObjString *MachineName, *MachineUser;
  TObjString *FileDate, *FileVersion;

  TList *EventTreeList;
  std::map<Int_t, std::string> EventTreeNameMap;
  std::map<std::string, Int_t> EventTreeIDMap;

  TList *RunList;

  ClassDef(ADAQSimulationReadout, 1);
};

#endif
