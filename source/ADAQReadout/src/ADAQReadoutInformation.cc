#include "ADAQReadoutInformation.hh"

ADAQReadoutInformation::ADAQReadoutInformation()
  : DGModelName(""), DGSerialNumber(0), DGNumChannels(0),
    DGBitDepth(0), DGSamplingRate(0), DGROCFWRevision(""),
    DGAMCFWRevision(""), DGFWType(""),
    TriggerType(""), TriggerEdge(""), AcquisitionType(""),
    DataReductionMode(false), ZeroSuppressionMode(false),
    AcquisitionTimer(false), AcquisitionTime(0), CoincidenceLevel(0),
    RecordLength(0), PostTrigger(0.), PSDMode(0),
    HVType(""), HVNumChannels(0),
    StoreRawWaveforms(true), StoreEnergyData(false), StorePSDData(false)
{;}


ADAQReadoutInformation::~ADAQReadoutInformation()
{;}
