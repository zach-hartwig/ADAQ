#include "ADAQReadoutInformation.hh"

ADAQReadoutInformation::ADAQReadoutInformation()
  : DGType(""), DGSerialNum(0), DGNumChannels(0),
    DGBitDepth(0), DGSamplingRate(0), 
    DGROCFWRevision(0), DGAMCFWRevision(0), DGFWType(""),
    RecordLength(0), CoincidenceLevel(0), PostTrigger(0.),
    TriggerType(""), TriggerEdge(""), AcquisitionType(""),
    HVType(""), HVNumChannels(0),
    StoreRawWaveforms(true), StoreEnergyData(false), StorePSDData(false)
{;}


ADAQReadoutInformation::~ADAQReadoutInformation()
{;}
