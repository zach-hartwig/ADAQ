#include <sstream>
#include <iostream>

#include "ADAQReadoutManager.hh"

ADAQReadoutManager::ADAQReadoutManager()
  : ADAQFile(new TFile), ADAQFileName(""), ADAQFileOpen(false),
    WaveformTree(new TTree), ReadoutInformation(new ADAQReadoutInformation)
{;}


ADAQReadoutManager::~ADAQReadoutManager()
{;}


void ADAQReadoutManager::PopulateMetadata()
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


void ADAQReadoutManager::WriteMetadata()
{
  MachineName->Write("MachineName");
  MachineUser->Write("MachineUser");
  FileDate->Write("FileDate");
  FileVersion->Write("FileVersion");
  FileComment->Write("FileComment");
}


void ADAQReadoutManager::CreateFile(std::string Name)
{
  if(ADAQFileOpen)
    return;
  
  if(ADAQFile) delete ADAQFile;
  ADAQFile = new TFile(Name.c_str(), "recreate");
  ADAQFileOpen = true;
  
  PopulateMetadata();
  
  CreateWaveformTree();
  
  CreateReadoutInformation();
}


void ADAQReadoutManager::WriteFile()
{
  // This method handles the writing of all necessary objects to the
  // open ROOT file and ensures that all objects that are necessary to
  // define an ADAQ-formatted file are present and written to disk.

  if(!ADAQFileOpen)
    return;

  // Write necessary data to disk
  WriteMetadata();
  WaveformTree->Write();
  ReadoutInformation->Write("ReadoutInformation");

  // Close the TFile and set boolean accordingly
  ADAQFile->Close();
  ADAQFileOpen = false;
}


void ADAQReadoutManager::CreateWaveformTree()
{
  // The method creates a TTree that will be used to hold the
  // digitized waveform and analyzed waveform data for all
  // digitizer channels.

  if(!ADAQFileOpen)
    return;
  
  // Note that TTree memory is automatically freed when the TFile in
  // which the TTree lives is deleted so there is no need to delete
  
  WaveformTree = new TTree("WaveformTree", 
			   "TTree to hold digitized waveform data in ADAQ files");

}  


void ADAQReadoutManager::CreateWaveformTreeBranches(Int_t Channel, 
						    vector<uint16_t> *Waveform,
						    ADAQWaveformData *WaveformData)
{
  // This method is called for each digitizer channel for which the
  // user desired to save data. For each channel, two branches are
  // created in order to isolate different / encapsulate same data:
  //
  // 0. A branch to hold the entire digitized waveform for the
  //    specified channel. The branch is automatically named
  //    "WaveformChX", where X is the channel number.
  //
  // 1. A branch to hold an ADAQWaveformData class, which contains
  //    analyzed waveform data for the specified channel's
  //    waveform. The branch is automatically named "WaveformDataChX",
  //    where X is the channel number.

  if(!ADAQFileOpen)
    return;

  // First create a branch to hold the digitized waveform 
  std::stringstream SS;
  SS << "WaveformCh" << Channel;
  TString WaveformBranchName = SS.str();
  WaveformTree->Branch(WaveformBranchName, Waveform);
  
  // Then create a branch to hold the analyzed waveform data
  SS.str("");
  SS << "WaveformDataCh" << Channel;
  TString WaveformDataBranchName = SS.str();
  WaveformTree->Branch(WaveformDataBranchName, 
		       "ADAQWaveformData",
		       WaveformData,
		       128000, // Buffer size [bytes]
		       0);     // Maximum TTree splitting
}


void ADAQReadoutManager::SetWaveformBranchStatus(Int_t Channel, Bool_t Status)
{
  std::stringstream SS;
  SS << "WaveformCh" << Channel;
  TString BranchName = SS.str();
  WaveformTree->SetBranchStatus(BranchName, Status);
}


Bool_t ADAQReadoutManager::GetWaveformBranchStatus(Int_t Channel)
{
  std::stringstream SS;
  SS << "WaveformCh" << Channel;
  TString BranchName = SS.str();
  Bool_t Status = WaveformTree->GetBranchStatus(BranchName);
  return Status;
}


void ADAQReadoutManager::SetDataBranchStatus(Int_t Channel, Bool_t Status)
{
  std::stringstream SS;
  SS << "WaveformDataCh" << Channel;
  TString BranchName = SS.str();
  WaveformTree->SetBranchStatus(BranchName, Status);
}


Bool_t ADAQReadoutManager::GetDataBranchStatus(Int_t Channel)
{
  std::stringstream SS;
  SS << "WaveformDataCh" << Channel;
  TString BranchName = SS.str();
  Bool_t Status = WaveformTree->GetBranchStatus(BranchName);
  return Status;
}


void ADAQReadoutManager::CreateReadoutInformation()
{
  if(!ADAQFileOpen)
    return;

  if(ReadoutInformation) delete ReadoutInformation;
  ReadoutInformation  = new ADAQReadoutInformation;
}
