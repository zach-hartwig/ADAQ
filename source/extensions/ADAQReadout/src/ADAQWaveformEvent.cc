#include "ADAQWaveformEvent.hh"


ADAQWaveformEvent::ADAQWaveformEvent()
  : WaveformIntegral(0.), WaveformArea(0.),
    PSDTotalIntegral(0.), PSDTailIntegral(0.),
    Baseline(0.), TimeStamp(0), BoardID(0)
{;}


ADAQWaveformEvent::~ADAQWaveformEvent()
{;}


void ADAQWaveformEvent::Initialize()
{
  WaveformIntegral = WaveformArea = 0.;
  PSDTotalIntegral = PSDTailIntegral = 0.;
  Baseline = 0.;
  TimeStamp = ChannelID = BoardID = 0;
}
