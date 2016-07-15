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
class ADAQDigitizer;


class AcquisitionManager : public TObject
{
public:
  AcquisitionManager();
  ~AcquisitionManager();
  
  // Initialize the VME connection to the digitizer
  void InitVMEConnection();  

  // Prepare for data acquisition
  void Arm();

  // Initialize the digitizer
  void InitDigitizer();

  // Begin acquiring waveforms
  void StartAcquisition();

  // Stop acquiring waveforms; note that the Boost.Thread
  // functionality must be protected from ROOT's CINT
#ifndef __CINT__
  void StopAcquisition(boost::thread *);
#endif

  // Safely shutdown the DAQ
  void Disarm();


private:
  // Manager objects for the digitizer
  ADAQDigitizer *DGManager;

  // Init variable for the digitizer
  bool DGEnable, DGLinkOpen;
  int DGBoardAddress;

  // General program control variables
  bool Verbose, Debug;

  /*
    Important: Anything that uses Boost's Integer types (uint16_t,
    uint32_t, etc) or CAEN's defined types from "CAENDigitizerType.h"
    must be protected from ROOT's CINT
  */

#ifndef __CINT__

  // Variables for the digitizer
  vector<bool> DGChannelEnabled;
  uint32_t RecordLength, PostTriggerSize, EventsBeforeReadout;
  vector<uint32_t> ChannelTriggerThreshold;
  vector<uint32_t> ChannelDCOffset;

  // Variables for readout of the digitized data
  uint32_t BufferSize, Size, NumEvents;
  CAEN_DGTZ_EventInfo_t EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *EventWaveform;
#endif

  // Variables for readout of the digitized waveforms
  char *EventPointer;
  char *Buffer;

  // A double-vector for readout of digitizer channels
  vector< vector<int> > Waveforms;

  // Define the AcquisitionManager class to ROOT
  ClassDef(AcquisitionManager,1);
};

#endif
