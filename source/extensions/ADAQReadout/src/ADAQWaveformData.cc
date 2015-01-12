#include "ADAQWaveformData.hh"


ADAQWaveformData::ADAQWaveformData()
  : PulseHeight(0.), PulseArea(0.), Baseline(0.),
    PSDTotalIntegral(0.), PSDTailIntegral(0.),
    TimeStamp(0), ChannelID(0), BoardID(0)
{;}


ADAQWaveformData::~ADAQWaveformData()
{;}


void ADAQWaveformData::Initialize()
{
  PulseHeight = PulseArea = Baseline = 0.;
  PSDTotalIntegral = PSDTailIntegral = 0.;
  TimeStamp = ChannelID = BoardID = 0;
}
