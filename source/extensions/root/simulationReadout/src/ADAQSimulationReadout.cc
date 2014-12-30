/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQSimulationReadout.cc
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
//       at the end of each simulated event, and an ADAQSimultionRun
//       concrete object at the end of each simulation run. These
//       objects can be stored within this class using ROOT's
//       TCollection-derived "TList" class. After the simulation is
//       complete, the ADAQSimulationReadout class should be written
//       to a ROOT file with the extension ".asim" to indicate that it
//       is an ADAQSimulationReadout-formated ROOT file.
//
/////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sys/unistd.h>

#include "ADAQSimulationReadout.hh"

ADAQSimulationReadout::ADAQSimulationReadout()
  : EventTreeList(new TList), 
    RunList(new TList)
{ PopulateMetadata(); }


ADAQSimulationReadout::~ADAQSimulationReadout()
{ delete EventTreeList; }


void ADAQSimulationReadout::PopulateMetadata()
{
  char Host[128], User[128];
  gethostname(Host, sizeof Host);
  getlogin_r(User, sizeof User);

  MachineName = new TObjString(Host);
  MachineUser = new TObjString(User);

  time_t RawTime;
  time(&RawTime);
  FileDate = new TObjString(ctime(&RawTime));
  FileVersion = new TObjString("1.0");
}



/////////////////////////////////////////////
// Methods for ADAQSimulationEvent objects //
/////////////////////////////////////////////

ADAQSimulationEvent *ADAQSimulationReadout::CreateEventTree(Int_t ID,
							    TString Name,
							    TString Desc)
{
  ADAQSimulationEvent *Event = new ADAQSimulationEvent;

  TTree *T = new TTree(Name, Desc);
  T->Branch("ADAQSimulationEventBranch", "ADAQSimulationEvent", Event, 32000, 99);
  
  EventTreeIDMap[(std::string)Name] = ID;
  EventTreeNameMap[ID] = (std::string)Name;
  
  EventTreeList->Add(T);

  return Event;
}

void ADAQSimulationReadout::AddEventTree(Int_t ID,
					 TTree *T)
{
  EventTreeIDMap[T->GetName()] = ID;
  EventTreeNameMap[ID] = T->GetName();
  
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


void ADAQSimulationReadout::WriteEventTrees()
{
  TIter It(EventTreeList);
  TTree *T;
  while((T = (TTree *)It.Next()))
    T->Write();
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


void ADAQSimulationReadout::AddRun(ADAQSimulationRun *Run)
{ RunList->Add(Run); }


ADAQSimulationRun *ADAQSimulationReadout::GetRun(Int_t ID)
{ return (ADAQSimulationRun *)RunList->At(ID); }


Int_t ADAQSimulationReadout::GetNumberOfRuns()
{ return RunList->GetSize(); }


void ADAQSimulationReadout::ListRuns()
{;}


void ADAQSimulationReadout::WriteToFile()
{
  MachineName->Write("MachineName");
  MachineUser->Write("MachineUser");
  FileDate->Write("FileDate");
  FileVersion->Write("FileVersion");

  EventTreeList->Write();
}
