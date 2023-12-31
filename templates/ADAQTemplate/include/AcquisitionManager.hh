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
  void OpenConnection();
  void InitializeParameters();
  void ProgramDigitizer();
  void Disarm();
  
  // Methods to start/stop data acquisition
  void RunAcquisitionLoop();
  void RunDebugLoop();
  void RunControlLoop(boost::thread *);

private:
  // ADAQ manager object to control interface to the digitizer
  ADAQDigitizer *DGManager;
  
  // Initialization variables for the  digitizer

  bool DGLinkOpen;
  string DGFirmwareType;
  int DGNumChannels;
  
  // General program control variables
  bool Debug;

  ///////////////////////////////////////////
  // Variables used for digitizer programming

  // These variables hold values that the user can specify to control
  // the behavior of the digitizer. They are optional in the sense
  // that the user could hardcode them directly into the various
  // function calls. They are created as class member variables for
  // clarity and could, if so desired, be set via loading a parameter
  // file. At present, they are simply set in the member function
  // AcquisitionManager::InitParams().
  
  // All firwmare 
  
  vector<bool> ChEnabled;
  vector<CAEN_DGTZ_PulsePolarity_t> ChPulsePolarity;
  vector<uint32_t> ChDCOffset, ChTriggerThreshold;
  int EventsBeforeReadout;
  uint32_t ChannelEnableMask;
  bool TriggerEdgeRising, TriggerEdgeFalling;
  bool TriggerTypeAutomatic, TriggerTypeSoftware;
  
  // Standard (STD) firmware 

  uint32_t RecordLength, PostTriggerSize;
  vector<uint32_t> ChBaselineCalcMin, ChBaselineCalcMax; 

  // Pulse Shape Discrimination (PSD) firmware

  CAEN_DGTZ_DPP_PSD_Params_t PSDParameters;
  vector<uint32_t> PSDChRecordLength, PSDChPreTrigger;


  //////////////////////////////////
  // Variables for digitizer readout

  // These variables are mandatory and CAEN-specific for proper
  // readout of the digitizer waveform data from the FPGA to the
  // PC. Different firmware require different variables for readout to
  // the differences in FPGA operations and data structures.

  char *Buffer = NULL;
  uint32_t BufferSize;
  vector<bool> BufferFull;

  // Standard (STD) firmware
  uint32_t ReadSize, FPGAEvents, PCEvents;
  char *EventPointer = NULL;
  CAEN_DGTZ_EventInfo_t EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *EventWaveform = NULL;;
  
  // Pulse Shape Discrimination (PSD) firmware
  uint32_t PSDEventSize, PSDWaveformSize;
  CAEN_DGTZ_DPP_PSD_Event_t *PSDEvents[2];
  uint32_t NumPSDEvents[2];
  CAEN_DGTZ_DPP_PSD_Waveforms_t *PSDWaveforms = NULL;
  
  // A double-vector for readout of digitizer channels
  vector< vector<uint16_t> > Waveforms;

};

#endif
