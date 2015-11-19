/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMStorageManager.cc
// date: 26 Oct 15
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
//       its data storage task. Stored data includes the readout of
//       file metadata, event-level information (e.g. energy
//       deposition), and run-level information (e.g. total hits on a
//       sensitive detector).
//
//       Note that an object of type ASIMStorageManager is uniquely
//       instantiated by the ASIMReadoutManager to handle only a
//       single ASIM file. Thus, multiple ASIMStorageManager classes
//       will be created/deleted if multiple ASIM files are created
//       during a Geant4 session.
//
/////////////////////////////////////////////////////////////////////////////////

// ROOT
#include <TChain.h>

// C++
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>

// ASIM
#include "ASIMStorageManager.hh"


ASIMStorageManager::ASIMStorageManager()
  : ASIMFile(new TFile), ASIMFileName(""), ASIMFileOpen(false), 
    MPI_Rank(0), MPI_Size(0),
    EventTreeList(new TList), RunList(new TList)
{ PopulateMetadata(); }


ASIMStorageManager::~ASIMStorageManager()
{
  delete RunList;
  delete EventTreeList;
  delete ASIMFile;
}


//////////////////////////////////////
// Methods for ASIM file management //
//////////////////////////////////////

void ASIMStorageManager::CreateSequentialFile(std::string Name)
{
  if(ASIMFileOpen){
    std::cout << "ASIMStorageManager::CreateSequentialFile():\n"
	      << "  An ASIM file is presently open for data output! A new ASIM file\n"
	      << "  cannot be opened until the existing ASIM file is written to disk\n"
	      << "  and closed. Nothing to be done.\n"
	      << std::endl;
    
    return;
  }
  
  ASIMFileName = Name;
  
  // Recreate a new ROOT TFile for the ASIM file
  if(ASIMFile) delete ASIMFile;
  ASIMFile = new TFile(ASIMFileName, "recreate");
  
  ASIMFileOpen = true;
}


void ASIMStorageManager::CreateParallelFile(std::string Name,
					    Int_t Rank,
					    Int_t Size)
{
  if(ASIMFileOpen){
    std::cout << "ASIMStorageManager::CreateParallelFile():\n"
	      << "  An ASIM file is presently open for data output! A new ASIM file\n"
	      << "  cannot be opened until the existing ASIM file is written to disk\n"
	      << "  and closed. Nothing to be done.\n"
	      << std::endl;
    
    return;
  }

  // Set the MPI rank and size data members for later use
  MPI_Rank = Rank;
  MPI_Size = Size;
  
  std::stringstream SS;
  SS << Name << ".slave" << MPI_Rank;
  ASIMFileName = SS.str();
  GenerateSlaveFileNames();
  
  // Recreate slave ASIM files
  if(ASIMFile) delete ASIMFile;
  ASIMFile = new TFile(ASIMFileName, "recreate");
  
  ASIMFileOpen = true;
}


void ASIMStorageManager::GenerateSlaveFileNames()
{
  if(ASIMFileOpen)
    return;
  
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
      {}
  }
}


void ASIMStorageManager::WriteSequentialFile()
{
  if(!ASIMFileOpen)
    return;


  // Write the metadata
  WriteMetadata();
  
  // Write out each individual tree in the EventTreeList
  EventTreeList->Write();
  
  // Write out each of the run objects in the RunList
  WriteRuns();

  ASIMFile->Close();

  ASIMFileOpen = false;
}


void ASIMStorageManager::WriteParallelFile()
{
  if(!ASIMFileOpen)
    return;
  
  std::string Name = ASIMFileName.Data();
  
  // All slaves should write out the event TTrees contained on each
  // slave node to a node-specific ROOT file and then close the file
  EventTreeList->Write();
  ASIMFile->Close();

  // Only a single process (the master) should handle the aggregation
  // of slave ROOT files containing the event treesinto a single
  // master ASIM file that contains all event- and run-level data

  if(MPI_Rank == 0){
    // Create the master ASIM file name
    TString FinalFileName = Name.substr(0, Name.find(".slave"));
    
    // Create a TChain for all existing event TTrees by iterating over
    // the TTree names stored in the std::map. Because the slave
    // ASIMFiles have been written and closed at this point, the
    // TTrees have been purged from ROOT memory so it's easier to just
    // grab the names from the std::map.

    std::map<Int_t, std::string>::iterator It0;
    Int_t Index = 0;
    for(It0 = EventTreeNameMap.begin(); It0!=EventTreeNameMap.end(); It0++){

      TChain *EventTreeChain = new TChain(It0->second.c_str());

      std::vector<TString>::iterator It1;
      for(It1 = SlaveFileNames.begin(); It1 != SlaveFileNames.end(); It1++)
	EventTreeChain->Add((*It1));
      
      // The following method enables multiple TChains to be written
      // to the final ASIM ROOT file without overwriting each other
      
      if(Index == 0)
	EventTreeChain->Merge(FinalFileName);
      else{
	TFile *FinalFile = new TFile(FinalFileName, "update");
	EventTreeChain->Merge(FinalFile, 0);
      }
      
      delete EventTreeChain;

      Index++;
    }

    // Update the master ASIM file with file metadata and run-level
    // data, which is not dependent on the slave ASIM files
    TFile *FinalFile = new TFile(FinalFileName, "update");
    WriteMetadata();
    WriteRuns();

    // Write, close, and delete the master ASIM TFile object
    FinalFile->Close();
    delete FinalFile;
      
    // Delete the node-specific ASIM files from the operating system

    std::vector<TString>::iterator It2;
    for(It2 = SlaveFileNames.begin(); It2 != SlaveFileNames.end(); It2++){
      std::string FileName = (*It2).Data();
      std::string RemoveSlaveFileCmd = "rm -f " + FileName;
      system(RemoveSlaveFileCmd.c_str());
    }
  }
  ASIMFileOpen = false;
}


void ASIMStorageManager::PopulateMetadata()
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
  FileComment = new TObjString("");
}


void ASIMStorageManager::WriteMetadata()
{
  MachineName->Write("MachineName");
  MachineUser->Write("MachineUser");
  FileDate->Write("FileDate");
  FileVersion->Write("FileVersion");
  FileComment->Write("FileComment");
}


///////////////////////////////////
// Methods for ASIMEvent objects //
///////////////////////////////////

ASIMEvent *ASIMStorageManager::CreateEventTree(Int_t ID,
					       TString Name,
					       TString Desc)
{
  ASIMEvent *Event = new ASIMEvent;

  TTree *T = new TTree(Name, Desc);
  T->Branch("ASIMEventBranch", "ASIMEvent", Event, 32000, 99);
  
  EventTreeIDMap[(std::string)Name] = ID;
  EventTreeNameMap[ID] = (std::string)Name;
  
  EventTreeList->Add(T);

  return Event;
}

void ASIMStorageManager::AddEventTree(Int_t ID,
				      TTree *T)
{
  EventTreeIDMap[T->GetName()] = ID;
  EventTreeNameMap[ID] = T->GetName();
  
  EventTreeList->Add(T);
}


void ASIMStorageManager::RemoveEventTree(std::string Name)
{
  TTree *T = (TTree *)EventTreeList->FindObject(Name.c_str());
  EventTreeList->Remove(T);
}


void ASIMStorageManager::RemoveEventTree(Int_t ID)
{ RemoveEventTree(EventTreeNameMap[ID]); }


TTree *ASIMStorageManager::GetEventTree(std::string Name)
{
  TTree *T = (TTree *)EventTreeList->FindObject(Name.c_str());
  return T;
}


TTree *ASIMStorageManager::GetEventTree(Int_t ID)
{ return GetEventTree(EventTreeNameMap[ID]); }


void ASIMStorageManager::ListEventTrees()
{
  TIter It(EventTreeList);
  TTree *T;
  while((T = (TTree *)It.Next())){
    std::cout << "Tree name: " << T->GetName() << std::endl;
  }
}


void ASIMStorageManager::WriteEventTrees()
{
  if(!ASIMFileOpen)
    return;
  
  TIter It(EventTreeList);
  TTree *T;
  while((T = (TTree *)It.Next()))
    T->Write();
}


Int_t ASIMStorageManager::GetNumberOfEventTrees()
{ return EventTreeList->GetSize(); }


Int_t ASIMStorageManager::GetEventTreeID(std::string Name)
{ return EventTreeIDMap[Name]; }


std::string ASIMStorageManager::GetEventTreeName(Int_t ID)
{ return EventTreeNameMap[ID]; }


/////////////////////////////////
// Methods for ASIMRun objects //
/////////////////////////////////


void ASIMStorageManager::AddRun(ASIMRun *Run)
{ RunList->Add(Run); }


ASIMRun *ASIMStorageManager::GetRun(Int_t ID)
{ return (ASIMRun *)RunList->At(ID); }


Int_t ASIMStorageManager::GetNumberOfRuns()
{ return RunList->GetSize(); }


void ASIMStorageManager::ListRuns()
{
  TIter It(RunList);
  ASIMRun *R;
  while((R = (ASIMRun *)It.Next())){
  }
}

void ASIMStorageManager::WriteRuns()
{
  if(!ASIMFileOpen)
    return;

  TIter It(RunList);
  ASIMRun *R;
  while((R = (ASIMRun *)It.Next())){
    std::stringstream SS;
    SS << "Run" << R->GetRunID();
    TString Name = SS.str();
    R->Write(Name);
  }
}
