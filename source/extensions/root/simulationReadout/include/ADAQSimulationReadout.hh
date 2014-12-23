#ifndef __ADAQSimulationReadout_hh__
#define __ADAQSimulationReadout_hh__ 1

#include <TObject.h>
#include <TList.h>
#include <TTree.h>

#include <string>

#include "ADAQSimulationEvent.hh"

class ADAQSimulationReadout : public TObject
{
public:
  ADAQSimulationReadout();
  ~ADAQSimulationReadout();

  void CreateEventTree(std::string, std::string, ADAQSimulationEvent *);
  void RemoveEventTree(std::string);
  TTree *GetEventTree(std::string);
  void ListEventTrees();
  
private:
  TList *EventTreeList;

  ClassDef(ADAQSimulationReadout, 1);
};

#endif
