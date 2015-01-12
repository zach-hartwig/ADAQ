#include <sstream>
#include <iostream>

#include "ADAQReadoutManager.hh"

ADAQReadoutManager::ADAQReadoutManager()
  : ADAQFile(new TFile), ADAQFileName(""), ADAQFileOpen(false),
    WaveformTree(new TTree), WaveformDataTree(new TTree),
    ReadoutInformation(new ADAQReadoutInformation)
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
}


void ADAQReadoutManager::WriteMetadata()
{
  MachineName->Write("MachineName");
  MachineUser->Write("MachineUser");
  FileDate->Write("FileDate");
  FileVersion->Write("FileVersion");
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
  
  CreateWaveformDataTree();
  
  CreateReadoutInformation();
}


void ADAQReadoutManager::WriteFile()
{
  if(!ADAQFileOpen)
    return;
  
  WriteMetadata();
  
  WaveformTree->Write();
  
  WaveformDataTree->Write();
  
  ReadoutInformation->Write("ReadoutInformation");

  ADAQFile->Close();
  ADAQFileOpen = false;
}


void ADAQReadoutManager::CreateWaveformTree()
{
  if(!ADAQFileOpen)
    return;
  
  // Note that TTree memory is automatically freed when the TFile in
  // which the TTree lives is deleted

  WaveformTree = new TTree("WaveformTree", 
			   "TTree to hold digitized waveforms for ADAQ files");
}  


void ADAQReadoutManager::CreateWaveformTreeBranch(Int_t Channel, 
						  vector<uint16_t> *ChannelWaveform)
{
  if(!ADAQFileOpen)
    return;

  std::stringstream SS;
  SS << "WaveformCh" << Channel;
  TString BranchName = SS.str();
  WaveformTree->Branch(BranchName, ChannelWaveform);
}


void ADAQReadoutManager::CreateWaveformDataTree()
{
  if(!ADAQFileOpen)
    return;
  
  // Note that TTree memory is automatically freed when the TFile in
  // which the TTree lives is deleted
  
  WaveformDataTree = new TTree("WaveformDataTree", 
			       "TTree to hold analyzed waveform data for ADAQ files");
}


void ADAQReadoutManager::CreateWaveformDataTreeBranch(Int_t Channel, 
						      ADAQWaveformData *ChannelWaveformData)
{
  if(!ADAQFileOpen)
    return;
  
  std::stringstream SS;
  SS << "WaveformDataCh" << Channel;
  TString BranchName = SS.str();
  WaveformDataTree->Branch(BranchName, 
			   "ADAQWaveformData",
			   ChannelWaveformData,
			   32000,
			   99);
}


void ADAQReadoutManager::CreateReadoutInformation()
{
  if(!ADAQFileOpen)
    return;

  if(ReadoutInformation) delete ReadoutInformation;
  ReadoutInformation  = new ADAQReadoutInformation;
}
