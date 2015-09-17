/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMReadoutManager.hh
// date: 23 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMReadoutManager class provides a framework for
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
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ASIMReadoutManager_hh__
#define __ASIMReadoutManager_hh__ 1

#include <TObject.h>
#include <TObjString.h>
#include <TList.h>
#include <TString.h>
#include <TTree.h>
#include <TFile.h>

#include <string>
#include <map>
#include <ctime>

#include "ASIMEvent.hh"
#include "ASIMRun.hh"

class ASIMReadoutManager : public TObject
{
public:
  ASIMReadoutManager();
  ~ASIMReadoutManager();

  // Methods for ASIM file handling //

  void CreateSequentialFile(std::string);
  void CreateParallelFile(std::string, Int_t, Int_t);
  void GenerateSlaveFileNames();
  void WriteSequentialFile();
  void WriteParallelFile();
  void PopulateMetadata();
  void WriteMetadata();

  // Methods for handling ASIM events

  ASIMEvent *CreateEventTree(Int_t, TString, TString);
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

  // Methods for handling ASIM runs

  void AddRun(ASIMRun *);
  ASIMRun *GetRun(Int_t);
  Int_t GetNumberOfRuns();
  void ListRuns();
  void WriteRuns();

  // Methods for 




  

  // Set/Get methods for class member data
  
  Bool_t GetASIMFileOpen() {return ASIMFileOpen;}
  
  TString GetMachineName() {return MachineName->GetString();}
  TString GetMachineUser() {return MachineUser->GetString();}
  TString GetFileDate() {return FileDate->GetString();}
  TString GetFileVersion() {return FileVersion->GetString();}
  
  void SetFileComment(TString T) {FileComment->SetString(T);}
  TString GetFileComment() {return FileComment->GetString();}
  
  TList *GetEventTreeList() {return EventTreeList;}
  TList *GetRunList() {return RunList;}
  
private:

  // ASIM file objects
  
  TFile *ASIMFile;
  TString ASIMFileName;
  Bool_t ASIMFileOpen;
  
  // Objects to handle optional parallel processing

  Int_t MPI_Rank, MPI_Size;
  std::vector<TString> SlaveFileNames;

  // Metadata 
  
  TObjString *MachineName, *MachineUser;
  TObjString *FileDate, *FileVersion, *FileComment;

  // Objects to handle event-level information

  TList *EventTreeList;
  std::map<Int_t, std::string> EventTreeNameMap;
  std::map<std::string, Int_t> EventTreeIDMap;

  // Objects to handle run-level information;

  TList *RunList;

  // Objects to handle readout registration
  Int_t ReadoutID;

  ClassDef(ASIMReadoutManager, 1);
};

#endif
