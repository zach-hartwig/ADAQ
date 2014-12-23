#ifndef __ADAQSimulationReadout_hh__
#define __ADAQSimulationReadout_hh__ 1

#include <TObject.h>
#include <TList.h>
#include <TTree.h>

#include <iostream>

#include "ADAQRootEvent.hh"


class ADAQSimulationReadout : public TObject
{
public:
  ADAQSimulationReadout()
    : EventTreeList(new TList)
  {;}
  
  ~ADAQSimulationReadout()
  { delete EventTreeList; }

  void CreateEventTree(string Name, string Desc, ADAQRootEvent *Evt){
    TTree *T = new TTree(Name.c_str(), Desc.c_str());
    T->Branch("ADAQRootEvent", "An ADAQRootEvent branch", &Evt, 32000, 99);
    EventTreeList->Add(T);
  }

  void RemoveEventTree(string Name){
    TTree *T = dynamic_cast<TTree *>(EventTreeList->FindObject(Name.c_str()));
    EventTreeList->Remove(T);
  }
  
  TTree *GetEventTree(string Name){
    TTree *T = dynamic_cast<TTree *>(EventTreeList->FindObject(Name.c_str()));
    return T;
  }
  
  void *ListEventTrees(){
    TIter It(EventTreeList);
    TTree *T;
    while(T = dynamic_cast<TTree *>(It.Next())){
      std::cout << "Tree name: " << T->GetName() << std::endl;
    }
  }
  
private:

  TList *EventTreeList;
  

  ClassDef(ADAQSimulationReadout, 1);
};

#endif
