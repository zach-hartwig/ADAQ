#ifndef __ADAQWaveformData__
#define __ADAQWaveformData__ 1

#include <TObject.h>

class ADAQWaveformData : public TObject
{
public:
  ADAQWaveformData();
  ~ADAQWaveformData();

  void Initialize();

private:

  // Analyzed waveform data
  
  Double_t Integral, Area, Baseline;
  Double_t PSDTotalIntegral, PSDTailIntegral;
  
  // Standard waveform data
  
  Int_t TimeStamp;
  Int_t ChannelID, BoardID;

  ClassDef(ADAQWaveformData, 1);
};


#endif
