/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMStorageManager.hh
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMStorageManager class handles the persistent storage
//       of simulated Geant4 detector data in a standardized ROOT file
//       known as an ASIM file. The data is obtained by its sister
//       class ASIMReadoutManager, which handles the extraction of
//       data from within the Geant4 simulation and then hands it off
//       to the ASIMStorageManager for persistent storage to
//       disk. This class makes heavy use of ROOT classes to perform
//       its data storage task. Stored data includes thereadout of
//       file metadata, event-level information (e.g. energy
//       deposition), and run-level information (e.g. total hits on a
//       sensitive detector).
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ASIMStorageManager_hh__
#define __ASIMStorageManager_hh__ 1

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

class ASIMStorageManager : public TObject
{
public:
  ASIMStorageManager();
  ~ASIMStorageManager();

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

  // Set/Get methods for class member data
  
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

  ClassDef(ASIMStorageManager, 1);
};

#endif
