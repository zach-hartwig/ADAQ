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

#include <TChain.h>

#include <iostream>
#include <sstream>
#include <sys/unistd.h>

#include "ADAQSimulationReadout.hh"

ADAQSimulationReadout::ADAQSimulationReadout()
  : ASIMFile(new TFile), ASIMFileName(""), ASIMFileNameSet(false),
    MPI_Rank(0), MPI_Size(0),
    EventTreeList(new TList), RunList(new TList)
{ PopulateMetadata(); }


ADAQSimulationReadout::~ADAQSimulationReadout()
{
  //  delete RunList;
  //  delete EventTreeList;
  //  delete ASIMFile;
}


void ADAQSimulationReadout::CreateSequentialFile(std::string Name)
{
  if(ASIMFile){
    
    // Emergency close if the ASIM file is somehow still open
    if(ASIMFile->IsOpen())
      ASIMFile->Close();
    
    // Release allocated memory for depracated TFile
    delete ASIMFile;
  }

  ASIMFileName = Name;
  ASIMFileNameSet = true;
  
  // Recreate a new TFile
  ASIMFile = new TFile(ASIMFileName, "recreate");
}


void ADAQSimulationReadout::CreateParallelFile(std::string Name,
					       Int_t Rank,
					       Int_t Size)
{
  if(ASIMFile){
    
    // Emergency close if the ASIM file is somehow still open
    if(ASIMFile->IsOpen())
      ASIMFile->Close();
    
    // Release allocated memory for depracated TFile
    delete ASIMFile;
  }
  
  std::stringstream SS;
  SS << Name << ".slave" << Rank;

  ASIMFileName = SS.str();
  ASIMFileNameSet = true;

  MPI_Rank = Rank;
  MPI_Size = Size;
  
  GenerateSlaveFileNames();
  
  ASIMFile = new TFile(ASIMFileName, "recreate");
}




void ADAQSimulationReadout::GenerateSlaveFileNames()
{
  if(!ASIMFileNameSet){
    // Should generate exception
    return;
  }

  SlaveFileNames.clear();
  for(Int_t rank=0; rank<MPI_Size; rank++){

    std::string Name = ASIMFileName.Data();
    std::stringstream SS;
    
    size_t pos = Name.find("slave");
    if(pos != std::string::npos){
      SS << Name.substr(0,pos) << "slave" << rank;
      SlaveFileNames.push_back((TString)SS.str());
    }
    else
      // Should generate exception
      {}
  }
}


void ADAQSimulationReadout::WriteSequentialFile()
{
  // Write the metadata
  WriteMetadata();
  
  // Write out each individual tree in the EventTreeList
  EventTreeList->Write();
  
  // Write out each of the run objects in the RunList
  WriteRuns();

  ASIMFile->Close();
}


void ADAQSimulationReadout::WriteParallelFile()
{
  std::string Name = ASIMFileName.Data();

  // All slaves should write out the event TTrees contained on each
  // slave node to a node-specific ROOT file and then close the file
  EventTreeList->Write();
  ASIMFile->Close();
  ListEventTrees();

  // Only a single process (the master) should handle the aggregation
  // of slave ROOT files containing the event treesinto a single
  // master ASIM file that contains all event- and run-level data

  if(MPI_Rank == 0){
    // Create the master ASIM file name
    TString FinalFileName = Name.substr(0, Name.find(".slave"));
    
    TIter It(EventTreeList);
    TTree *T;
    while((T = (TTree *)It.Next())){

      std::cout << T->GetName() << std::endl;

      //      TChain *EventTreeChain = new TChain(T->GetName());

      /*
      
      // Add all the node-specific ASIM files to the chain
      std::vector<TString>::iterator it;
      for(it = SlaveFileNames.begin(); it != SlaveFileNames.end(); it++){
	EventTreeChain->Add((*it));
      }
      
      EventTreeChain->Merge(FinalFileName);
      */
      
      //      delete EventTreeChain;
    }
    
    /*
    // Update the master ASIM file with mandatory ASIM file metadata
    TFile *FinalFile = new TFile(FinalFileName, "update");
    WriteMetadata();
    WriteRuns();
    FinalFile->Close();
    delete FinalFile;
      
    // Remove the node-specific ASIM files
    std::vector<TString>::iterator it;
    for(it = SlaveFileNames.begin(); it != SlaveFileNames.end(); it++){
      std::string RemoveSlaveFileCmd = "rm -f " + Name;
      system(RemoveSlaveFileCmd.c_str());
    }
    */
  }
}


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


void ADAQSimulationReadout::WriteMetadata()
{
  MachineName->Write("MachineName");
  MachineUser->Write("MachineUser");
  FileDate->Write("FileDate");
  FileVersion->Write("FileVersion");
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
{
  TIter It(RunList);
  ADAQSimulationRun *R;
  while((R = (ADAQSimulationRun *)It.Next())){
  }
}

void ADAQSimulationReadout::WriteRuns()
{
  TIter It(RunList);
  ADAQSimulationRun *R;
  while((R = (ADAQSimulationRun *)It.Next())){
    std::stringstream SS;
    SS << "Run" << R->GetRunID();
    TString Name = SS.str();
    R->Write(Name);
  }
}
