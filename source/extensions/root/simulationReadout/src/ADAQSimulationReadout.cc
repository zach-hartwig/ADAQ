#include <iostream>

#include "ADAQSimulationReadout.hh"

ADAQSimulationReadout::ADAQSimulationReadout()
  : EventTreeList(new TList)
{;}


ADAQSimulationReadout::~ADAQSimulationReadout()
{ delete EventTreeList; }


void ADAQSimulationReadout::CreateEventTree(std::string Name,
					    std::string Desc,
					    ADAQSimulationEvent *Evt)
{
  TTree *T = new TTree(Name.c_str(), Desc.c_str());
  T->Branch("ADAQSimulationEvent_Branch", "ADAQSimulationEvent", &Evt, 32000, 99);
  EventTreeList->Add(T);
}


void ADAQSimulationReadout::RemoveEventTree(std::string Name)
{
  TTree *T = (TTree *)EventTreeList->FindObject(Name.c_str());
  EventTreeList->Remove(T);
}


TTree *ADAQSimulationReadout::GetEventTree(std::string Name)
{
  TTree *T = (TTree *)EventTreeList->FindObject(Name.c_str());
  return T;
}


void ADAQSimulationReadout::ListEventTrees()
{
  TIter It(EventTreeList);
  TTree *T;
  while((T = (TTree *)It.Next())){
    std::cout << "Tree name: " << T->GetName() << std::endl;
  }
}
