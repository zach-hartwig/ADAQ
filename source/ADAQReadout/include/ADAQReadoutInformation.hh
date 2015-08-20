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

  void SetDGModelName(TString MN) {DGModelName = MN;}
  TString GetDGModelName() {return DGModelName;}

  void SetDGSerialNumber(Int_t SN) {DGSerialNumber = SN;}
  Int_t GetDGSerialNumber() {return DGSerialNumber;}
  
  void SetDGNumChannels(Int_t NC) {DGNumChannels = NC;}
  Int_t GetDGNumChannels() {return DGNumChannels;}
  
  void SetDGBitDepth(Int_t BN) {DGBitDepth = BN;}
  Int_t GetDGBitDepth() {return DGBitDepth;}
  
  void SetDGSamplingRate(Int_t SR) {DGSamplingRate = SR;}
  Int_t GetDGSamplingRate() {return DGSamplingRate;}
  
  void SetDGROCFWRevision(TString FWR) {DGROCFWRevision = FWR;}
  TString GetDGROCFWRevision() {return DGROCFWRevision;}
  
  void SetDGAMCFWRevision(TString FWR) {DGAMCFWRevision = FWR;}
  TString GetDGAMCFWRevision() {return DGAMCFWRevision;}
  
  void SetDGFWType(TString FWT) {DGFWType = FWT;}
  TString GetDGFWType() {return DGFWType;}

  // Global settings

  void SetTriggerType(TString TT) {TriggerType = TT;}
  TString GetTriggerType() {return TriggerType;}

  void SetTriggerEdge(TString TT) {TriggerEdge = TT;}
  TString GetTriggerEdge() {return TriggerEdge;}

  void SetAcquisitionType(TString TT) {AcquisitionType = TT;}
  TString GetAcquisitionType() {return AcquisitionType;}
  
  void SetDataReductionMode(Bool_t DRM) {DataReductionMode = DRM;}
  Bool_t GetDataReductionMode() {return DataReductionMode;}

  void SetZeroSuppressionMode(Bool_t DRM) {ZeroSuppressionMode = DRM;}
  Bool_t GetZeroSuppressionMode() {return ZeroSuppressionMode;}

  void SetAcquisitionTimer(Bool_t AT) {AcquisitionTimer = AT;}
  Bool_t GetAcquisitionTimer() {return AcquisitionTimer;}
  
  void SetAcquisitionTime(Int_t AT) {AcquisitionTime = AT;}
  Int_t GetAcquisitionTime() {return AcquisitionTime;}

  void SetCoincidenceLevel(Int_t RL) {CoincidenceLevel = RL;}
  Int_t GetCoincidenceLevel() {return CoincidenceLevel;}

  // CAEN firmware-agnostic channel-specific settings
  
  void SetChannelEnable(vector<Bool_t> T) {ChannelEnable = T;}
  vector<Bool_t> GetChannelEnable() {return ChannelEnable;}

  void SetDCOffset(vector<Int_t> T) {DCOffset = T;}
  vector<Int_t> GetDCOffset() {return DCOffset;}

  void SetTrigger(vector<Int_t> T) {Trigger = T;}
  vector<Int_t> GetTrigger() {return Trigger;}

  void SetBaselineCalcMin(vector<Int_t> T) {BaselineCalcMin = T;}
  vector<Int_t> GetBaselineCalcMin() {return BaselineCalcMin;}

  void SetBaselineCalcMax(vector<Int_t> T) {BaselineCalcMax = T;}
  vector<Int_t> GetBaselineCalcMax() {return BaselineCalcMax;}
  
  void SetPSDTotalStart(vector<Int_t> T) {PSDTotalStart = T;}
  vector<Int_t> GetPSDTotalStart() {return PSDTotalStart;}

  void SetPSDTotalStop(vector<Int_t> T) {PSDTotalStop = T;}
  vector<Int_t> GetPSDTotalStop() {return PSDTotalStop;}
  
  void SetPSDTailStart(vector<Int_t> T) {PSDTailStart = T;}
  vector<Int_t> GetPSDTailStart() {return PSDTailStart;}
  
  void SetPSDTailStop(vector<Int_t> T) {PSDTailStop = T;}
  vector<Int_t> GetPSDTailStop() {return PSDTailStop;}

  // CAEN standard firmware specific settings

  void SetRecordLength(Int_t RL) {RecordLength = RL;}
  Int_t GetRecordLength() {return RecordLength;}

  void SetPostTrigger(Int_t RL) {PostTrigger = RL;}
  Int_t GetPostTrigger() {return PostTrigger;}
  
  void SetZLEFwd(vector<Int_t> T) {ZLEFwd = T;}
  vector<Int_t> GetZLEFwd() {return ZLEFwd;}
  
  void SetZLEBck(vector<Int_t> T) {ZLEBck = T;}
  vector<Int_t> GetZLEBck() {return ZLEBck;}

  void SetZLEThreshold(vector<Int_t> T) {ZLEThreshold = T;}
  vector<Int_t> GetZLEThreshold() {return ZLEThreshold;}

  // CAEN DPP-PSD firmware specific settings

  void SetChRecordLength(vector<Int_t> CRL) {ChRecordLength = CRL;}
  vector<Int_t> GetChRecordLength() {return ChRecordLength;}

  void SetChChargeSensitivity(vector<Int_t> CCS) {ChChargeSensitivity = CCS;}
  vector<Int_t> GetChChargeSensitivity() {return ChChargeSensitivity;}

  void SetChPSDCut(vector<Int_t> CRL) {ChPSDCut = CRL;}
  vector<Int_t> GetChPSDCut() {return ChPSDCut;}
  
  void SetChTriggerConfig(vector<Int_t> CRL) {ChTriggerConfig = CRL;}
  vector<Int_t> GetChTriggerConfig() {return ChTriggerConfig;}
  
  void SetChTriggerValidation(vector<Int_t> CRL) {ChTriggerValidation = CRL;}
  vector<Int_t> GetChTriggerValidation() {return ChTriggerValidation;}
  
  void SetChShortGate(vector<Int_t> CRL) {ChShortGate = CRL;}
  vector<Int_t> GetChShortGate() {return ChShortGate;}
  
  void SetChLongGate(vector<Int_t> CRL) {ChLongGate = CRL;}
  vector<Int_t> GetChLongGate() {return ChLongGate;}
  
  void SetChPreTrigger(vector<Int_t> CRL) {ChPreTrigger = CRL;}
  vector<Int_t> GetChPreTrigger() {return ChPreTrigger;}
  
  void SetChGateOffset(vector<Int_t> CRL) {ChGateOffset = CRL;}
  vector<Int_t> GetChGateOffset() {return ChGateOffset;}
  

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
  bool GetStoreEnergyData() {return StoreEnergyData;}
  
  void SetStorePSDData(bool SPP) {StorePSDData = SPP;}
  bool GetStorePSDData() {return StorePSDData;}
  

private:

  ///////////////////////////
  // Digitizer information //
  ///////////////////////////
  
  // Device information

  TString DGModelName;
  Int_t DGSerialNumber;
  Int_t DGNumChannels;
  Int_t DGBitDepth;
  Int_t DGSamplingRate;
  TString DGROCFWRevision;
  TString DGAMCFWRevision;
  TString DGFWType;

  // Global settings

  TString TriggerType, TriggerEdge, AcquisitionType;
  Bool_t DataReductionMode, ZeroSuppressionMode;
  Bool_t AcquisitionTimer;
  Int_t AcquisitionTime;
  Int_t CoincidenceLevel;

  // CAEN firmware-agnostic channel-specific settings

  vector<Bool_t> ChannelEnable;
  vector<Int_t> DCOffset, Trigger;
  vector<Int_t> BaselineCalcMin, BaselineCalcMax;
  vector<Int_t> PSDTotalStart, PSDTotalStop;
  vector<Int_t> PSDTailStart, PSDTailStop;

  // CAEN standard firmware specific settings

  Int_t RecordLength;
  Double_t PostTrigger;
  vector<Int_t> ZLEFwd, ZLEBck, ZLEThreshold;

  // CAEN DPP-PSD firmware specific settings

  vector<Int_t> ChRecordLength;
  vector<Int_t> ChChargeSensitivity;
  vector<Int_t> ChPSDCut;
  vector<Int_t> ChTriggerConfig;
  vector<Int_t> ChTriggerValidation;
  vector<Int_t> ChShortGate;
  vector<Int_t> ChLongGate;
  vector<Int_t> ChPreTrigger;
  vector<Int_t> ChGateOffset;

  Int_t PSDMode;
 
  //////////////////////////////
  // High voltage information //
  //////////////////////////////
  
  TString HVType;
  Int_t HVNumChannels;
  
  vector<Int_t> HVVoltage, HVCurrent, HVPolarity;
  vector<Bool_t> HVStatus;


  //////////////////////////
  // Waveform information //
  //////////////////////////
  
  // BooTeans for what information will be stored in an ADAQ file
  Bool_t StoreRawWaveforms;
  Bool_t StoreEnergyData;
  Bool_t StorePSDData;
  

  ////////////////////////////
  // ROOT class declaration //
  ////////////////////////////

  ClassDef(ADAQReadoutInformation, 1);
};

#endif
