#ifndef __ADAQWaveformData__
#define __ADAQWaveformData__ 1

#include <TObject.h>

#include <iostream>

class ADAQWaveformData : public TObject
{
public:
  ADAQWaveformData();
  ~ADAQWaveformData();
  
  void Initialize();

  void SetPulseHeight(Double_t PH) {PulseHeight = PH;}
  Double_t GetPulseHeight() {return PulseHeight;}
  
  void SetPulseArea(Double_t PA) {PulseArea = PA;}
  Double_t GetPulseArea() {return PulseArea;}
  
  void SetBaseline(Double_t B) {Baseline = B;}
  Double_t GetBaseline() {return Baseline;}

  void SetPSDTotalIntegral(Double_t PTI) {PSDTotalIntegral = PTI;}
  Double_t GetPSDTotalIntegral() {return PSDTotalIntegral;}

  void SetPSDTailIntegral(Double_t PTI) {PSDTailIntegral = PTI;}
  Double_t GetPSDTailIntegral() {return PSDTailIntegral;}
  
  void SetTimeStamp(ULong64_t TS) {TimeStamp = TS;}
  ULong64_t GetTimeStamp() {return TimeStamp;}
  
  void SetChannelID(Int_t CID) {ChannelID = CID;}
  Int_t GetChannelID() {return ChannelID;}
  
  void SetBoardID(Int_t BID) {BoardID = BID;}
  Int_t GetBoardID() {return BoardID;}
  
private:
  
  // Analyzed waveform data
  
  Double_t PulseHeight, PulseArea, Baseline;
  Double_t PSDTotalIntegral, PSDTailIntegral;
  
  // Standard waveform data
  
  ULong64_t TimeStamp;
  Int_t ChannelID, BoardID;
  
  ClassDef(ADAQWaveformData, 1);
};

#endif
