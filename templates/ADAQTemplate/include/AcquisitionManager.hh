////////////////////////////////////////////////////////////////////////////////
//
// name: AcquisitionManager.hh
// date: 20 Dec 23 (last updated)
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The purpose of the AcquisitionManager class is to handle all
//       aspects of interfacing with the CAEN hardware for control and
//       readout. In this template (meant to be customized and
//       expanded by the user), we show how to use the ADAQDigitizer
//       class to program, arm, acquire waveform data, and safely
//       shutdown a CAEN digitizer unit in a standard acquisition
//       thread; control and readout of other hardware (e.g. high
//       voltage, VME controllers, etc.) can be easily added. While
//       basic functionality is provided within the acquisition loop
//       when data is retrieved, the user can implement whatever
//       method(s) they like to operate on the data.
//
//       To enable user-control during acquisition, two threads (using
//       the Boost library implementation) are created: an
//       "acquisition thread" that runs the acquisition loop to
//       readout and unpack digitized data; and a "control thread"
//       that enables the user to manually intervene to, for example,
//       terminate the acquisition process.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __AcquisitionManager_hh__
#define __AcquisitionManager_hh__ 1

// C++
#include <vector>
#include <string>
using namespace std;

// Boost
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>

// ADAQ
#include "ADAQDigitizer.hh"

class AcquisitionManager
{
public:
  AcquisitionManager();
  ~AcquisitionManager();
  
  // Methods to prepare for and cleanup after data acquisition
  void Arm();
  void InitParameters();
  void InitConnection();
  void InitDigitizer();
  void Disarm();

  // Methods to start/stop data acquisition
  void RunAcquisitionLoop();
  void RunControlLoop(boost::thread *);

private:
  // ADAQ manager object to control interface to the digitizer
  ADAQDigitizer *DGManager;
  
  // Initialization variables for the  digitizer

  bool DGLinkOpen;
  string DGFirmwareType;
  
  // General program control variables
  bool Debug;

  // Variables for the digitizer common to all firmware types
  
  vector<bool> ChEnabled, ChPosPolarity, ChNegPolarity;
  vector<uint32_t> ChDCOffset, ChTriggerThreshold;
  int EventsBeforeReadout;
  uint32_t ChannelEnableMask;
  bool TriggerEdgeRising, TriggerEdgeFalling;
  bool TriggerTypeAutomatic, TriggerTypeSoftware;
  
  // Variables for CAEN standard firmware (STD) settings

  uint32_t RecordLength, PostTriggerSize;
  vector<uint32_t> ChBaselineCalcMin, ChBaselineCalcMax; 

  // Variables for CAEN DPP-PSD firwmare (PSD) settings

  vector<uint32_t> ChRecordLength, ChBaselineSamples, ChChargeSensitivity;
  vector<uint32_t> ChShortGate, ChLongGate, ChPreTrigger, ChGateOffset;
  uint32_t TriggerHoldoff;

  // Variables for digitizer readout with STD firmware

  char *EventPointer;
  CAEN_DGTZ_EventInfo_t EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *EventWaveform;

  // Variables for digitizer readout with DPP-PSD firmware

  CAEN_DGTZ_DPP_PSD_Params_t *PSDParams[16];
  CAEN_DGTZ_DPP_PSD_Event_t *PSDEvents[16];
  CAEN_DGTZ_DPP_PSD_Waveforms_t *PSDWaveforms;
  vector<uint32_t> NumPSDEvents;

  // Variables for PC buffer readout

  char *Buffer;
  uint32_t BufferSize, PSDEventSize, PSDWaveformSize, ReadSize;
  uint32_t FPGAEvents, PCEvents;
  vector<bool> BufferFull;

  // A double-vector for readout of digitizer channels
  vector< vector<uint16_t> > Waveforms;

};

#endif
