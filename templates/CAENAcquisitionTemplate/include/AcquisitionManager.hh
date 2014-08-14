////////////////////////////////////////////////////////////////////////////////
//
// name: AcquisitionManager.hh
// date: 14 Aug 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The purpose of the AcquisitionManager class is to handle all
//       aspects of interfacing with the CAEN hardware and readout,
//       from setting up the digitizer to processing readout data. In
//       the template, access is provided to the EventWaveform object
//       containing the sampled data for 8 channels of the V1720 with
//       the acquisition loop. Basic digitizer settings are
//       implemented. The class uses the ADAQDigitizer class to
//       facilite interaction with the V1720 card via the V1718
//       USB/VME module. Note that the ::StartAcquisition() and
//       ::StopAcquisition() methods are run in two different threads
//       (see CAENAcquisitionTemplate.cc) to support keyboard entry by
//       the user to control start/stop of the acquisition and
//       software triggering.
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
class ADAQDigitizer;


class AcquisitionManager : public TObject
{
public:
  AcquisitionManager();
  ~AcquisitionManager();
  
  // Initialize the VME connection to V1720
  void InitVMEConnection();  

  // Prepare the DAQ for data acquisition
  void Arm();

  // Prepare the V1720 digitizer board
  void InitDigitizer();

  // Begin acquiring waveforms
  void StartAcquisition();

  // Stop acquiring waveforms; note that the Boost.Thread
  // functionality must be protected from ROOT's CINT
#ifndef __CINT__
  void StopAcquisition(boost::thread *);
#endif

  // Safely close the DAQ
  void Disarm();


private:
  // Manager objects for the V1720 board
  ADAQDigitizer *DGManager;

  // Init variable for the V1720 board
  bool V1720Enable;
  bool V1720LinkOpen;
  int V1720BoardAddress;

  // General program control variables
  bool Verbose, Debug;

  /*
    Important: Anything that uses Boost's Integer types (uint16_t,
    uint32_t, etc) or CAEN's defined types from "CAENDigitizerType.h"
    must be protected from ROOT's CINT
  */

#ifndef __CINT__
  // Variables for the V1720 digitizer ("DG") board
  vector<bool> DGChannelEnabled;
  uint32_t RecordLength, PostTriggerSize, MaxBLTEvents;
  vector<uint32_t> ChannelTriggerThreshold;
  vector<uint32_t> ChannelDCOffset;

  // Variables for readout of the digitized waveforms
  uint32_t BufferSize, Size, NumEvents;
  CAEN_DGTZ_EventInfo_t EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *EventWaveform;
#endif

  // Variables for readout of the digitized waveforms
  char *EventPointer;
  char *Buffer;

  // A double-vector for readout of 8 channels of waveforms
  vector< vector<int> > Waveforms;

  // Define the AcquisitionManager class to ROOT
  ClassDef(AcquisitionManager,1);
};

#endif
