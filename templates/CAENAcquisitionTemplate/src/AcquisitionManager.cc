////////////////////////////////////////////////////////////////////////////////
//
// name: AcquisitionManager.cc
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

// C++ 
#include <iostream>
using namespace std;

// Boost
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

// ADAQ 
#include "AcquisitionManager.hh"
#include "ADAQDigitizer.hh"


AcquisitionManager::AcquisitionManager()
  :  V1720Enable(true), V1720LinkOpen(false), V1720BoardAddress(0x00420000),
     Verbose(true), Debug(false)
{
  ///////////////////////////////////////////////////////////////////
  // Instantiate a manager to control all interactions with the V1720
  DGManager = new ADAQDigitizer;
  DGManager->SetVerbose(true);

  //////////////////////////////////////
  // Initialize the digitizer parameters 

  // The "width" or "length" of the acquisition window in V1720
  // samples (1 sample == 4ns)
  RecordLength = 1024;

  // The position of the acquisition window relative to the trigger sample
  PostTriggerSize = 50;

  // Max number of accumulated events that will trigger transfer
  MaxBLTEvents = 10;

  // Digitizer channel enable
  DGChannelEnabled += true, false, false, false, false, false, false, false;

  ChannelTriggerThreshold += 2000, 0, 0, 0, 0, 0, 0, 0; // [ADC]

  ChannelDCOffset += 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000;

  ////////////////////////////////////////////////////////////////
  // Initialize the connection to the VME crate for control of the
  // V1720 digitizer and V6534 high voltage boards

  InitVMEConnection();
}


AcquisitionManager::~AcquisitionManager()
{
  if(V1720Enable)
    delete DGManager;
}


void AcquisitionManager::InitVMEConnection()
{
  if(Debug)
    return;

  //////////////////////////////
  // VME link to the V1720 board
  
  if(DGManager and V1720Enable){
    
    if(Verbose)
      cout << "\nAcquisitionManager (main thread) : Opening a link to the V1720 digitizer board ...\n"
	   <<   "                                   --> V1720Address = 0x" << hex << setw(8) << setfill('0') << V1720BoardAddress << "\n"
	   << endl;

    int Status = DGManager->OpenLink(V1720BoardAddress);
    (Status==0) ? V1720LinkOpen = true : V1720LinkOpen = false;
    
    if(V1720Enable and !V1720LinkOpen){
      if(Verbose)
	cout << "AcquisitionManager (main thread) : Error! A VME link to the V1720 board could not be established! Exiting...\n"
	     <<   "                                 --> CAEN_DGTZ_ErrorCode == " << dec << Status << "\n"
	     << endl;
      exit(-42);
    }
    else
      if(Verbose)
	cout << "AcquisitionManager (main thread) : A VME link to the V1720 board has been established!\n"
	     << endl;
  }
}


void AcquisitionManager::Arm()
{
  if(Debug)
    return;
  
  InitDigitizer();
}


void AcquisitionManager::InitDigitizer()
{
  // If the DGManager has not been instantiated, simply return to the
  // AcquisitionManager::Arm() function to prevent triggering segfaults
  // by calling member functions on a non-existent object
  if(!DGManager)
    return;

  // Initialize the V1720 digitizer board
  DGManager->Initialize();

  ///////////////////////////////////////////////
  // Variables for digitizer settings and readout
  
  // Variable to hold the channel enable mask, ie, sets which
  // digitizer channels are actively taking data
  uint32_t ChannelEnableMask = 0;

  // Variable for total number of enabled digitizer channels
  uint32_t NumDGChannelsEnabled = 0;

  // Calculate the channel enable mask, which is a 32-bit integer
  // describing which of the 8 digitizer channels are enabled. A
  // 32-bit integer has 8 bytes or 8 "hex" digits; a hex digit set
  // to "1" in the n-th position in the hex representation indicates
  // that the n-th channel is enabled. For example, if the
  // ChannelEnableMask is equal to 0x00110100 then channels 2, 4 and
  // 5 are enabled for digitization
  for(int ch=0; ch<DGManager->GetNumChannels(); ch++)
    if(DGChannelEnabled[ch]){
      uint32_t Ch = 0x00000001<<ch;
      ChannelEnableMask |= Ch;
      NumDGChannelsEnabled++;
    }
  
  // Ensure that at least one channel is enabled in the channel
  // enabled bit mask; if not, return without starting the acquisition
  // loop, since...well...there ain't shit to acquisition.
  if((0xff & ChannelEnableMask)==0){
    cout << "\nAcquisitionManager (main thread) : Error! No digitizer channels were enabled, ie, ChannelEnableMask==0!\n"
	 << endl;
    exit(-42);
  }
  
  // Define a vector of vectors that will hold the digitized waveforms
  // in all channels (units of [ADC]). The outer vector (size 8)
  // represents each digitizer channel; the inner vector (size
  // RecordLength) represents the waveform. The start address of each
  // outer vector will be used to create a unique branch in the
  // waveform TTree object to store each of the 8 digitizer channels 
  
  // Resize the outer and inner vector to the appropriate, fixed size
  Waveforms.resize(DGManager->GetNumChannels());
  for(int i=0; i<DGManager->GetNumChannels(); i++)
    Waveforms[i].resize(RecordLength);
  

  ///////////////////////////////////////////////////////
  // Program V1720 digitizer with acquisition settings //
  ///////////////////////////////////////////////////////

  // Reset the digitizer to default state
  DGManager->Reset();
  
  // Set the trigger threshold individually for each of the 8
  // digitizer channels [ADC] and the DC offsets for each channel
  for(int ch=0; ch<DGManager->GetNumChannels(); ch++){
    DGManager->SetChannelTriggerThreshold(ch, ChannelTriggerThreshold[ch]);
    DGManager->SetChannelDCOffset(ch, ChannelDCOffset[ch]);
  }

  // Set the V1720 triggering configuration
  DGManager->EnableSWTrigger();
  DGManager->DisableAutoTrigger(ChannelEnableMask);
  DGManager->DisableExternalTrigger();
  
  // Set the record length of the acquisition window
  DGManager->SetRecordLength(RecordLength);

  // Set the channel enable mask
  DGManager->SetChannelEnableMask(ChannelEnableMask);

  // Set the maximum number of events that will be accumulated before
  // the V1720 FPGA buffer is dumped to PC memory
  DGManager->SetMaxNumEventsBLT(MaxBLTEvents);

  // Set the percentage of acquisition window that occurs after trigger
  DGManager->SetPostTriggerSize(PostTriggerSize);
}


void AcquisitionManager::StartAcquisition()
{
  cout << "AcquisitionManager (acquisition thread) : Beginning waveform acquisition...\n"
       << endl;
  
  while(Debug){
    boost::posix_time::milliseconds Time(250);
    cout << "AcquisitionManager (acquisition thread) : Acquisition loop is running in debug mode!" << endl;
    boost::this_thread::sleep(Time);
  }
  
  // Set the EventPointer and the EventWaveform pointer data members
  // to point to nothing; their destination addresses will be set in
  // the acquisition 'while' loop below as events are acquired
  EventPointer = NULL;
  EventWaveform = NULL;

  // Set the Buffer membger data to point to nothing; destination
  // address will be set in the acquisition 'while' loop below
  Buffer = NULL;  

  // Total number of accumulated events, which is useful to output to
  // stdout for the user's benefit
  int TotalEvents = 0;
  
  // The array (of length 'RecordLength' in units of V1720 samples)
  // used to receive the digitized waveform from the V1720 board
  double Voltage[RecordLength]; // [ADC]
  
  // Allocate memory for the readout buffer on the PC
  DGManager->MallocReadoutBuffer(&Buffer, &Size);

  // Set the V1720 to begin acquiring data
  DGManager->SWStartAcquisition();


  /////////////////////////////////
  // V1720 digitizer acquisition //
  /////////////////////////////////
  // The following loops reads digitized data from the digitizers into
  // local PC memory, principally as arrays of voltage versus time (or
  // sample). To maximize data throughput, the following loop should
  // be be as efficient as possible.

  // The following terminology is important:
  // V1720 buffer == the memory buffer onboard the FPGA of the V1720 board
  // PC buffer == the memory buffer allocated locally on the PC
  // Event == an acquisition window caused by a channel trigger threshold being exceeded
  // NumEvents == the number of events that is allowed to accumulate on the V1720 buffer
  //              before being automatically readout into the PC buffer
  // Record Length == the length of the acquisition window in 4 ns units
  // Sample ==  a single value between 0 and 4095 of digitized voltage

  while(true){
    
    // Read data from the V1720 buffer into the PC buffer
    DGManager->ReadData(Buffer, &BufferSize);    
    
    // Determine the number of events in the buffer
    DGManager->GetNumEvents(Buffer, BufferSize, &NumEvents);

    // If there are no events in the current buffer then continue in
    // the 'while' loop without executing the CPU intensive 'for'
    // loops on the next lines. This maximizes code efficiency and
    // only scans/processes events when there is need to do so.
    if(NumEvents==0)
      continue;
    
    // For each event in the PC memory buffer...
    for(uint32_t evt=0; evt<NumEvents; evt++){
      
      // Get the event information
      DGManager->GetEventInfo(Buffer, BufferSize, evt, &EventInfo, &EventPointer);
      
      // Decode the event and obtain the waveform (voltage as a function of time)
      DGManager->DecodeEvent(EventPointer, &EventWaveform);
      
      // If there is no waveform in the PC buffer, continue in the
      // while loop to avoid segfaulting
      if(EventWaveform==NULL)
	continue;
      
      // For each channel...
      for(int ch=0; ch<DGManager->GetNumChannels(); ch++){
	
	// Only proceed to waveform analysis if the channel is enabled
	if(!DGChannelEnabled[ch])
	  continue;

	for(uint32_t sample=0; sample<RecordLength; sample++){
	  // Get the digitized voltage in units of analog-to-digital conversion bits
	  Voltage[sample] = EventWaveform->DataChannel[ch][sample]; // [ADC]
	}
      }
      // Free the memory allocated to the digitizer event
      DGManager->FreeEvent(&EventWaveform);
      
      TotalEvents++;
      
      if(TotalEvents % 1 == 0){
	cout << "\r" << flush;
	cout << "Accumulated events = " << TotalEvents << flush;
      }
    }

    // Check to see if an interrupt has been requested of the
    // thread. Note that if the Escape_thread object requests an
    // interrupt, this (Acquisition_thread) will terminate. 
    boost::this_thread::interruption_point();
  }
}


void AcquisitionManager::StopAcquisition(boost::thread *Acquisition_thread)
{

  // This member function is run in the Escape_thread and receives the
  // Acquisition_thread as a function argument. The purpose of this
  // thread is primarily to provide an escape mechanism from the
  // Acquisition_thread via Boost's thread interrupt method. At
  // present, the user must physically stroke a "0" then "Enter" to
  // terminate the acquisition.

  cout << "AcquisitionManager (escape thread) : Enter a '0' to terminate acquisition!\n" 
       << "                                     Enter 1 '1' to trigger the V1720!\n" 
       << endl;

  int Choice = -1;
  bool RunLoop = true;

  // Loop until the user decides to terminate the acquisition
  while(RunLoop){
    cin >> Choice;

    // Terminate the Acquisition_thread and proceed to "disarm" phase
    // of the acquisition
    if(Choice == 0){
      Acquisition_thread->interrupt();
      RunLoop = false;
      DGManager->SendSWTrigger();
    }

    // Provide ability for user to send a software trigger
    if(Choice == 1){
      DGManager->SendSWTrigger();
    }
  }
}      


void AcquisitionManager::Disarm()
{
  if(Debug)
    return;

  // Stop the V1720 acquisition
  DGManager->SWStopAcquisition();
  
  // Free the PC memory associated with V1720 digitizer readout
  DGManager->FreeReadoutBuffer(&Buffer);

  // Close a VME link to the V1720 board
  if(V1720Enable){
    DGManager->CloseLink();
  }
}
