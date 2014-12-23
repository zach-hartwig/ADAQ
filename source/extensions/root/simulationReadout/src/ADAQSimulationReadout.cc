#include <iostream>
#include <sys/unistd.h>

#include "ADAQSimulationReadout.hh"

ADAQSimulationReadout::ADAQSimulationReadout()
  : EventTreeList(new TList), 
    RunSummaryList(new TList)
{ PopulateMetadata(); }


ADAQSimulationReadout::~ADAQSimulationReadout()
{ delete EventTreeList; }


void ADAQSimulationReadout::PopulateMetadata()
{
  char Host[128], User[128];
  gethostname(Host, sizeof Host);
  getlogin_r(User, sizeof User);

  MachineName = Host;
  MachineUser = User;

  FileCreationTime = time(0);
}



/////////////////////////////////////////////
// Methods for ADAQSimulationEvent objects //
/////////////////////////////////////////////

void ADAQSimulationReadout::CreateEventTree(Int_t ID,
					    std::string Name,
					    std::string Desc,
					    ADAQSimulationEvent *Evt)
{
  TTree *T = new TTree(Name.c_str(), Desc.c_str());
  T->Branch("ADAQSimulationEvent_Branch", "ADAQSimulationEvent", &Evt, 32000, 99);
  
  EventTreeIDMap[Name] = ID;
  EventTreeNameMap[ID] = Name;
  
  EventTreeList->Add(T);
}


void ADAQSimulationReadout::RemoveEventTree(std::string Name)
{
  TTree *T = (TTree *)EventTreeList->FindObject(Name.c_str());
  EventTreeList->Remove(T);
}


void ADAQSimulationReadout::RemoveEventTree(Int_t ID)
{ RemoveEventTree(EventTreeNameMap[ID]); }


TTree *ADAQSimulationReadout::GetEventTree(std::string Name)
{
  TTree *T = (TTree *)EventTreeList->FindObject(Name.c_str());
  return T;
}


TTree *ADAQSimulationReadout::GetEventTree(Int_t ID)
{ return GetEventTree(EventTreeNameMap[ID]); }


void ADAQSimulationReadout::ListEventTrees()
{
  TIter It(EventTreeList);
  TTree *T;
  while((T = (TTree *)It.Next())){
    std::cout << "Tree name: " << T->GetName() << std::endl;
  }
}


Int_t ADAQSimulationReadout::GetNumberOfEventTrees()
{ return EventTreeList->GetSize(); }


Int_t ADAQSimulationReadout::GetEventTreeID(std::string Name)
{ return EventTreeIDMap[Name]; }


std::string ADAQSimulationReadout::GetEventTreeName(Int_t ID)
{ return EventTreeNameMap[ID]; }


///////////////////////////////////////////
// Methods for ADAQSimulationRun objects //
///////////////////////////////////////////


void ADAQSimulationReadout::AddRunSummary(ADAQSimulationRun *Run)
{ RunSummaryList->Add(Run); }


ADAQSimulationRun *ADAQSimulationReadout::GetRunSummary(Int_t ID)
{ return (ADAQSimulationRun *)RunSummaryList->At(ID); }


Int_t ADAQSimulationReadout::GetNumberOfRunSummaries()
{ return RunSummaryList->GetSize(); }


void ADAQSimulationReadout::ListRunSummaries()
{;}
