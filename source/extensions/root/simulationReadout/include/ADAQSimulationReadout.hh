#ifndef __ADAQSimulationReadout_hh__
#define __ADAQSimulationReadout_hh__ 1

#include <TObject.h>
#include <TList.h>
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

  void PopulateMetadata();

  void CreateEventTree(Int_t, std::string, std::string, ADAQSimulationEvent *);
  void RemoveEventTree(std::string);
  void RemoveEventTree(Int_t);
  TTree *GetEventTree(std::string);
  TTree *GetEventTree(Int_t);
  void ListEventTrees();
  Int_t GetNumberOfEventTrees();
  Int_t GetEventTreeID(std::string);
  std::string GetEventTreeName(Int_t);

  void AddRunSummary(ADAQSimulationRun *);
  ADAQSimulationRun *GetRunSummary(Int_t);
  Int_t GetNumberOfRunSummaries();
  void ListRunSummaries();

  
private:
  // Metadata
  std::string MachineName, MachineUser;
  time_t FileCreationTime;

  Int_t EventTreeID;  
  TList *EventTreeList;
  std::map<Int_t, std::string> EventTreeNameMap;
  std::map<std::string, Int_t> EventTreeIDMap;

  TList *RunSummaryList;

  ClassDef(ADAQSimulationReadout, 1);
};

#endif
