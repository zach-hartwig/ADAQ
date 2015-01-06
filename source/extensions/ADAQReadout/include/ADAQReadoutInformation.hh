#ifndef __ADAQReadoutInformation_hh__
#define __ADAQReadoutInformation_hh__ 1

#include <TObject.h>
#include <TString.h>
#include <TTree.h>

#include <vector>
using namespace std;


class ADAQReadoutInformation : public TObject
{
public:
  ADAQReadoutInformation();
  ~ADAQReadoutInformation();

  ///////////////////////////////////////////////
  // Set/Get methods for digitizer information //
  ///////////////////////////////////////////////

  // Device information

  void SetDGType(TString T) {DGType = T;}
  TString GetDGType() {return DGType;}

  void SetDGSerialNum(Int_t SN) {DGSerialNum = SN;}
  Int_t GetDGSerialNum() {return DGSerialNum;}
  
  void SetDGNumChannels(Int_t NC) {DGNumChannels = NC;}
  Int_t GetDGNumChannels() {return DGNumChannels;}
  
  void SetDGBitDepth(Int_t BN) {DGBitDepth = BN;}
  Int_t GetDGBitDepth() {return DGBitDepth;}
  
  void SetDGSamplingRate(Int_t SR) {DGSamplingRate = SR;}
  Int_t GetDGSamplingRate() {return DGSamplingRate;}
  
  void SetDGROCFWRevision(Int_t FWR) {DGROCFWRevision = FWR;}
  Int_t GetDGROCFWRevision() {return DGROCFWRevision;}

  void SetDGAMCFWRevision(Int_t FWR) {DGAMCFWRevision = FWR;}
  Int_t GetDGAMCFWRevision() {return DGAMCFWRevision;}
  
  void SetDGFWType(TString FWT) {DGFWType = FWT;}
  TString GetDGFWType() {return DGFWType;}

  // Global acquisition settings

  void SetRecordLength(Int_t RL) {RecordLength = RL;}
  Int_t GetRecordLength() {return RecordLength;}

  void SetPostTrigger(Int_t RL) {PostTrigger = RL;}
  Int_t GetPostTrigger() {return PostTrigger;}

  void SetCoincidenceLevel(Int_t RL) {CoincidenceLevel = RL;}
  Int_t GetCoincidenceLevel() {return CoincidenceLevel;}

  void SetTriggerType(TString TT) {TriggerType = TT;}
  TString GetTriggerType() {return TriggerType;}

  void SetTriggerEdge(TString TT) {TriggerEdge = TT;}
  TString GetTriggerEdge() {return TriggerEdge;}

  void SetAcquisitionType(TString TT) {AcquisitionType = TT;}
  TString GetAcquisitionType() {return AcquisitionType;}

  // Channel-specific acquisition settings

  void SetTrigger(vector<Int_t> T) {Trigger = T;}
  vector<Int_t> GetTrigger() {return Trigger;}

  void SetBaselineCalcMin(vector<Int_t> T) {BaselineCalcMin = T;}
  vector<Int_t> GetBaselineCalcMin() {return BaselineCalcMin;}

  void SetBaselineCalcMax(vector<Int_t> T) {BaselineCalcMax = T;}
  vector<Int_t> GetBaselineCalcMax() {return BaselineCalcMax;}

  void SetChannelEnable(vector<Int_t> T) {ChannelEnable = T;}
  vector<Int_t> GetChannelEnable() {return ChannelEnable;}

  void SetDCOffset(vector<Int_t> T) {DCOffset = T;}
  vector<Int_t> GetDCOffset() {return DCOffset;}
  
  void SetZLEFwd(vector<Int_t> T) {ZLEFwd = T;}
  vector<Int_t> GetZLEFwd() {return ZLEFwd;}
  
  void SetZLEBck(vector<Int_t> T) {ZLEBck = T;}
  vector<Int_t> GetZLEBck() {return ZLEBck;}
  
  void SetZLEThreshold(vector<Int_t> T) {ZLEThreshold = T;}
  vector<Int_t> GetZLEThreshold() {return ZLEThreshold;}


  //////////////////////////////////////////////////
  // Set/Get methods for high voltage information //
  //////////////////////////////////////////////////
  
  void SetHVType(TString T) {HVType = T;}
  TString GetHVType() {return HVType;}
  
  void SetHVNumChannels(Int_t NC) {HVNumChannels = NC;}
  Int_t GetHVNumChannels() {return HVNumChannels;}

  void SetHVVoltage(vector<Int_t> V) {HVVoltage = V;}
  vector<Int_t> GetHVVoltage() {return HVVoltage;}

  void SetHVCurrent(vector<Int_t> C) {HVCurrent = C;}
  vector<Int_t> GetHVCurrent() {return HVCurrent;}

  void SetHVPolarity(vector<Int_t> P) {HVPolarity = P;}
  vector<Int_t> GetHVPolarity() {return HVPolarity;}


  //////////////////////////////////////////////
  // Set/Get methods for waveform information //
  //////////////////////////////////////////////

  void SetStoreRawWaveforms(bool SRW) {StoreRawWaveforms = SRW;}
  bool GetStoreRawWaveforms() {return StoreRawWaveforms;}

  void SetStoreEnergyData(bool SED) {StoreEnergyData = SED;}
  bool GetStoreEnergyDate() {return StoreEnergyData;}
  
  void SetStorePSDData(bool SPP) {StorePSDData = SPP;}
  bool GetStorePSDData() {return StorePSDData;}
  

private:

  ///////////////////////////
  // Digitizer information //
  ///////////////////////////
  
  // Device information
  TString DGType;
  Int_t DGSerialNum;
  Int_t DGNumChannels;
  Int_t DGBitDepth;
  Int_t DGSamplingRate;
  Int_t DGROCFWRevision;
  Int_t DGAMCFWRevision;
  TString DGFWType;

  // Global acquisition settings
  Int_t RecordLength, CoincidenceLevel;
  Double_t PostTrigger;
  TString TriggerType, TriggerEdge, AcquisitionType;

  // Channel specific acquisition settings
  vector<Int_t> Trigger, BaselineCalcMin, BaselineCalcMax;
  vector<Int_t> ChannelEnable;
  vector<Int_t> DCOffset;
  vector<Int_t> ZLEFwd, ZLEBck, ZLEThreshold;
  
 
  //////////////////////////////
  // High voltage information //
  //////////////////////////////
  
  TString HVType;
  Int_t HVNumChannels;
  
  vector<Int_t> HVVoltage, HVCurrent, HVPolarity;
  vector<bool> HVStatus;


  //////////////////////////
  // Waveform information //
  //////////////////////////
  
  // Booleans for what information will be stored in an ADAQ file
  bool StoreRawWaveforms;
  bool StoreEnergyData;
  bool StorePSDData;
  

  ////////////////////////////
  // ROOT class declaration //
  ////////////////////////////

  ClassDef(ADAQReadoutInformation, 1);
};

#endif
