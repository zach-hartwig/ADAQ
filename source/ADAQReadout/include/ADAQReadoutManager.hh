#ifndef __ADAQReadoutManager_hh__
#define __ADAQReadoutManager_hh__ 1

// ROOT
#include <TObject.h>
#include <TObjString.h>
#include <TTree.h>
#include <TFile.h>
#include <TGText.h>

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

  // Action methods for metadata objects

  void PopulateMetadata();
  void WriteMetadata();

  // Action methods for the ADAQ file
  
  void CreateFile(std::string);
  void WriteFile();
  
  // Action methods for event-level data
  
  void CreateWaveformTree();
#ifndef __CINT__
  void CreateWaveformTreeBranches(Int_t, vector<uint16_t> *, ADAQWaveformData *);
#endif
  TTree *GetWaveformTree() {return WaveformTree;}

  void SetWaveformBranchStatus(Int_t, Bool_t);
  Bool_t GetWaveformBranchStatus(Int_t);

  void SetDataBranchStatus(Int_t, Bool_t);
  Bool_t GetDataBranchStatus(Int_t);
  
  // Action methods for run-level data
  
  void CreateReadoutInformation();
  ADAQReadoutInformation *GetReadoutInformation() {return ReadoutInformation;}
  
  // Set/Get methods for member data
  
  Bool_t GetADAQFileOpen() {return ADAQFileOpen;}

  TString GetMachineName() {return MachineName->GetString();}
  TString GetMachineUser() {return MachineUser->GetString();}
  TString GetFileDate() {return FileDate->GetString();}
  TString GetFileVersion() {return FileVersion->GetString();}

  void SetFileComment(TString T) {FileComment->SetString(T);}
  TString GetFileComment() {return FileComment->GetString();}
  
private:
  
  // ADAQ file objects

  TFile *ADAQFile;
  TString ADAQFileName;
  Bool_t ADAQFileOpen;

  // File metadata

  TObjString *MachineName, *MachineUser;
  TObjString *FileDate, *FileVersion;
  TObjString *FileComment;
  TObjString *ADAQVersion;

  // Objects for event-level information

  TTree *WaveformTree;
  
  // Objects for run-level information

  ADAQReadoutInformation *ReadoutInformation;

  ClassDef(ADAQReadoutManager, 1);
};


#endif
