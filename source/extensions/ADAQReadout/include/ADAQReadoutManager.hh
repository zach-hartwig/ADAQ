#ifndef __ADAQReadoutManager_hh__
#define __ADAQReadoutManager_hh__ 1

// ROOT
#include <TObject.h>
#include <TObjString.h>
#include <TTree.h>
#include <TFile.h>

// Boost
#ifndef __CINT__
#include <boost/cstdint.hpp>
#endif

#include <vector>

#include "ADAQReadoutInformation.hh"
#include "ADAQWaveformData.hh"


class ADAQReadoutManager : public TObject
{
public:
  ADAQReadoutManager();
  ~ADAQReadoutManager();

  // Action methods for metadate objects

  void PopulateMetadata();
  void WriteMetadata();

  // Action methods for the ADAQ ROOT file
  
  void CreateFile(std::string);
  void WriteFile();
  
  // Action methods for event-level data
  
  void CreateWaveformTree();
#ifndef __CINT__
  void CreateWaveformTreeBranch(Int_t, vector<uint16_t> *);
#endif
    TTree *GetWaveformTree() {return WaveformTree;}
  
  void CreateWaveformDataTree();
  void CreateWaveformDataTreeBranch(Int_t, ADAQWaveformData *);
  TTree *GetWaveformDataTree() {return WaveformDataTree;}
  
  // Action methods for run-level data
  
  void CreateReadoutInformation();
  ADAQReadoutInformation *GetReadoutInformation() {return ReadoutInformation;}
  
  // Set/Get methods for member data
  
  Bool_t GetADAQFileOpen() {return ADAQFileOpen;}

  TString GetMachineName() {return MachineName->GetString();}
  TString GetMachineUser() {return MachineUser->GetString();}
  TString GetFileDate() {return FileDate->GetString();}
  TString GetFileVersion() {return FileVersion->GetString();}

private:
  
  // ADAQ ROOT file objects

  TFile *ADAQFile;
  TString ADAQFileName;
  Bool_t ADAQFileOpen;

  // File metadata

  TObjString *MachineName, *MachineUser;
  TObjString *FileDate, *FileVersion;
  TObjString *ADAQVersion;

  // Objects for event-level information

  TTree *WaveformTree, *WaveformDataTree;
  
  // Objects for run-level information

  ADAQReadoutInformation *ReadoutInformation;

  ClassDef(ADAQReadoutManager, 1);
};


#endif
