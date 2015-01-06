#ifndef __ADAQWaveformEvent__
#define __ADAQWaveformEvent__ 1

#include <TObject.h>

class ADAQWaveformEvent : public TObject
{
public:
  ADAQWaveformEvent();
  ~ADAQWaveformEvent();

  void Initialize();

private:

  // Analyzed waveform data

  Double_t WaveformIntegral, WaveformArea;
  Double_t PSDTotalIntegral, PSDTailIntegral;
  Double_t Baseline;

  // Standard waveform data

  Int_t TimeStamp;
  Int_t ChannelID, BoardID;

  ClassDef(ADAQWaveformEvent, 1);
};


#endif
