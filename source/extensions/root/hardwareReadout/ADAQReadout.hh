#ifndef __ADAQReadout_hh__
#define __ADAQReadout_hh__ 1

#include <TObject.h>
#include <TString.h>
#include <TTree.h>

#include <vector>
using namespace std;


class ADAQReadout : public TObject
{
public:
  ADAQReadout();
  ~ADAQReadout();

  void Init

private:

  //////////////
  // Metadata //
  //////////////

  // File format ID
  Int_t ADAQFileFormat;

  // File creation date
  Int_t MM,DD,YYYY,Hour,Min,Sec;

  // Computer information
  TString *MachineName, *MachineUser;


  ///////////////////////////
  // Digitizer information //
  ///////////////////////////

  // Device information
  TString *DGType;
  Int_t DGSerialNum;
  Int_t DGNumChannels;
  Int_t DGBitDepth;
  Int_t DGSamplingRate;
  Int_t DGFWRevision;
  TString DGFWType;

  // Channel specific acquisition settings
  vector<Int_t> Trigger, BaselineCalcMin, BaselineCalcMax;
  vector<Int_t> ChannelEnable;
  vector<Int_t> DCOffset;
  vector<Int_t> ZLEFrwd, ZLEBack, ZLEThreshold;
  
  // Global acquisition settings
  Int_t RecordLength, PostTrigger, CoincidenceLevel;
  TString TriggerType, TriggerEdge, AcquisitionType;

  // Booleans for what information will be stored in an ADAQ file
  bool StoreRawWaveforms;
  bool StoreWaveformAreaAndHeight;
  bool StoreWaveformPSDIntegrals;


  //////////////////////////////
  // High voltage information //
  //////////////////////////////

  TString *HVType;
  Int_t HVNumChannels;
  
  vector<Int_t> HVVoltage, HVCurrent, HVPolarity;
  vector<bool> HVStatus;


  ////////////////////////////
  // ROOT class declaration //
  ////////////////////////////

  ClassDef(ADAQReadout, 1);
};

#endif
