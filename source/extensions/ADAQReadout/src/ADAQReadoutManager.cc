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
