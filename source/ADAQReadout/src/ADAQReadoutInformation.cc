#include "ADAQReadoutInformation.hh"

ADAQReadoutInformation::ADAQReadoutInformation()
  : DGModelName(""), DGSerialNumber(0), DGNumChannels(0),
    DGBitDepth(0), DGSamplingRate(0), 
    DGROCFWRevision(""), DGAMCFWRevision(""), DGFWType(""),
    RecordLength(0), CoincidenceLevel(0), PostTrigger(0.),
    TriggerType(""), TriggerEdge(""), AcquisitionType(""),
    DataReductionMode(false), ZeroSuppressionMode(false),
    AcquisitionTimer(false), AcquisitionTime(0),
    HVType(""), HVNumChannels(0),
    StoreRawWaveforms(true), StoreEnergyData(false), StorePSDData(false)
{;}


ADAQReadoutInformation::~ADAQReadoutInformation()
{;}
