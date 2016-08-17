////////////////////////////////////////////////////////////////////////////////
//
// name: AcquisitionManager.hh
// date: 16 Jul 16
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The purpose of the AcquisitionManager class is to handle all
//       aspects of interfacing with the CAEN hardware and readout,
//       from setting up the digitizer to processing readout data. In
//       the template, the following standard features are provided:
//       setup of the ROOT objects necessary for readout, programming
//       of the digitizer hardware, readout via a standard acquisition
//       loop, shutdown of the acquisition. It is the task of the user
//       to (a) modify the above as necessary and (b) to actually
//       implement something useful to do with the data that is
//       readout.
//
//       Control of the acquisition is provided via the separate
//       ::StartAcquisition() and ::StopAcquisition() Boost threads
//       (see CAENAcquisitionTemplate.cc). This enables keyboard entry
//       by the user to initiate acquisition during acquisition.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __AcquisitionManager_hh__
#define __AcquisitionManager_hh__ 1

// ROOT
#include <TObject.h>
#include <TTimer.h>

// C++
#include <vector>
#include <map>
#include <string>
using namespace std;

// Boost and CAEN
#ifndef __CINT__
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include "CAENDigitizerType.h"
#endif

// ADAQ
#include "ADAQRootClasses.hh"
#include "ADAQDigitizer.hh"


class AcquisitionManager : public TObject
{
public:
  AcquisitionManager();
  ~AcquisitionManager();
  
  // Prepare for data acquisition
  void Arm();
  void InitConnection();
  void InitParameters();
  void InitDigitizer();

  
  // Begin data acquisition
  void StartAcquisition();
  void StartAcquisition2();

  // Stop data acquisition; note that the Boost.Thread
  // functionality must be protected from ROOT's CINT
#ifndef __CINT__
  void StopAcquisition(boost::thread *);
#endif

  // Safely shutdown the DAQ
  void Disarm();


private:
  // ADAQ manager object for the digitizer
  ADAQDigitizer *DGManager;
  
  // Initialization variables for the  digitizer
  ZBoardType DGType;
  bool DGEnable, DGLinkOpen;
  int DGBoardAddress;
  bool DGStandardFW, DGPSDFW;

  // General program control variables
  bool Verbose, Debug;

  /*
    Important: Anything that uses Boost's Integer types (uint16_t,
    uint32_t, etc) or CAEN's defined types from "CAENDigitizerType.h"
    must be protected from ROOT's CINT
  */

#ifndef __CINT__

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

#endif
  
  // Define the AcquisitionManager class to ROOT
  ClassDef(AcquisitionManager,1);
};

#endif
