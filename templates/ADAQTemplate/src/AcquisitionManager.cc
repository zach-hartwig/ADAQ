////////////////////////////////////////////////////////////////////////////////
//
// name: AcquisitionManager.cc
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

// C++ 
#include <iostream>
using namespace std;

// Boost
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "AcquisitionManager.hh"


AcquisitionManager::AcquisitionManager()
  :  DGLinkOpen(false), Debug(true)
{
  //////////////////////////////////
  // Instantiate a digitizer manager
  
  DGManager = new ADAQDigitizer(zDT5790M,   // ADAQ-specified CAEN device type
				0,          // User-specified ID
				0x00000000, // Address in VME space
				0,          // USB link number
				0);         // CONET node number

  DGManager->SetVerbose(true);
}


AcquisitionManager::~AcquisitionManager()
{
  delete DGManager;
}


void AcquisitionManager::Arm()
{
  InitConnection();
  InitParameters();
  InitDigitizer();
}


void AcquisitionManager::InitConnection()
{
  ////////////////////////////
  // VME link to the digitizer
  
  cout << "AcquisitionManager (main thread) : Opening a link to the digitizer ...\n"
       << endl;
  
  int Status = DGManager->OpenLink();
  (Status==0) ? DGLinkOpen = true : DGLinkOpen = false;
  
  if(!DGLinkOpen){
    cout << "AcquisitionManager (main thread) : Error! A link to the digitizer could not be established! Exiting...\n"
	 << endl;
    exit(-42);
  }
  else
    cout << "AcquisitionManager (main thread) : A link to the digitizer has been established!\n"
	 << endl;
}


void AcquisitionManager::InitParameters()
{
  if(!DGLinkOpen)
    return;
  
  /////////////////////////////
  // Determine firmware type //
  /////////////////////////////
  
  DGFirmwareType = DGManager->GetBoardFirmwareType();

  
  /////////////////////////////
  // Init readout parameters //
  /////////////////////////////
  
  int NumChannels = DGManager->GetNumChannels();

  if(DGFirmwareType == "STD"){
    EventPointer = NULL;
    EventWaveform = NULL;
    DGManager->AllocateEvent(&EventWaveform);
  }
  else if(DGFirmwareType == "PSD"){
    PSDWaveforms = NULL;
  }

  Buffer = NULL;

  BufferSize = ReadSize = FPGAEvents = PCEvents = 0;
  for(int ch=0; ch<NumChannels; ch++)
    NumPSDEvents.push_back(0);
  
  DGManager->MallocReadoutBuffer(&Buffer, &BufferSize);
  
  if(DGFirmwareType == "PSD"){
    DGManager->MallocDPPEvents(PSDEvents, &PSDEventSize);
    DGManager->MallocDPPWaveforms(&PSDWaveforms, &PSDWaveformSize); 
  }

  
  /////////////////////////////
  // Init control parameters //
  /////////////////////////////
  
  for(int ch=0; ch<NumChannels; ch++){
    ChEnabled.push_back(false);
    ChPosPolarity.push_back(false);
    ChNegPolarity.push_back(true);
    ChDCOffset.push_back(0x8000);
    ChTriggerThreshold.push_back(2000);
  }
  ChEnabled[0] = true;

  // Events to accumulate on the digitizer before transfer to PC
  EventsBeforeReadout = 25;
  
  // Trigger type
  TriggerTypeAutomatic = false;
  TriggerTypeSoftware = true;

  // Trigger edge type
  TriggerEdgeRising = true;
  TriggerEdgeFalling = !TriggerEdgeRising;

  // Calculate the channel enable mask, which is a 32-bit integer
  // describing which of the digitizer channels are enabled.
  
  for(int ch=0; ch<DGManager->GetNumChannels(); ch++){
    if(ChEnabled[ch]){
      uint32_t Ch = 0x00000001<<ch;
      ChannelEnableMask |= Ch;
    }
  }
  
  // Ensure that at least one channel is enabled 
  if((0xff & ChannelEnableMask)==0){
    cout << "\nAcquisitionManager (main thread) : Error! No digitizer channels were enabled, ie, ChannelEnableMask==0!\n"
	 << endl;
    exit(-42);
  }
  
  if(DGFirmwareType == "STD"){

    // Initialize vector-of-vectors; outer vector is size of channels,
    // inner vector is length of waveform in samples

    Waveforms.resize(DGManager->GetNumChannels());
    for(int ch=0; ch<DGManager->GetNumChannels(); ch++)
      Waveforms[ch].resize(RecordLength);
    
    // The "width" or "length" of the acquisition window [samples]
    RecordLength = 512;
    
    // Position of the trigger within the acquisition window [%]
    PostTriggerSize = 50;
    
    for(int ch=0; ch<NumChannels; ch++){
      ChBaselineCalcMin.push_back(0);
      ChBaselineCalcMax.push_back(50);
    }
  }
  else if(DGFirmwareType == "PSD"){
    for(int ch=0; ch<NumChannels; ch++){
      ChRecordLength.push_back(512);
      ChBaselineSamples.push_back(0);
      ChChargeSensitivity.push_back(0);
      ChShortGate.push_back(50);
      ChLongGate.push_back(462);
      ChPreTrigger.push_back(100);
      ChGateOffset.push_back(50);
    }
  }
}  


void AcquisitionManager::InitDigitizer()
{
  if(!DGLinkOpen)
    return;
  
  // Initialize the digitizer board and reset
  DGManager->Initialize();

  // Reset the digitizer to default state
  DGManager->Reset();

  // Get the number of digitizer channels
  int NumChannels = DGManager->GetNumChannels();

  /////////////////////////////////////////
  // Program the STD firmware digitizers //
  /////////////////////////////////////////

  if(DGFirmwareType == "STD"){
    
    for(int ch=0; ch<NumChannels; ch++){
      DGManager->SetChannelTriggerThreshold(ch, ChTriggerThreshold[ch]);
      DGManager->SetChannelDCOffset(ch, ChDCOffset[ch]);

      if(ChPosPolarity[ch])
	DGManager->SetChannelPulsePolarity(ch, CAEN_DGTZ_PulsePolarityPositive);
      else
	DGManager->SetChannelPulsePolarity(ch, CAEN_DGTZ_PulsePolarityNegative);

      if(TriggerEdgeRising)
	DGManager->SetTriggerEdge(ch, "Rising");
      else
	DGManager->SetTriggerEdge(ch, "Falling");
    }

    DGManager->SetChannelEnableMask(ChannelEnableMask);
    DGManager->SetRecordLength(RecordLength);
    DGManager->SetPostTriggerSize(PostTriggerSize);
    DGManager->SetAcquisitionControl("Software");
    DGManager->SetZSMode("None");
    DGManager->SetMaxNumEventsBLT(EventsBeforeReadout);

    if(TriggerTypeAutomatic)
      DGManager->EnableAutoTrigger(ChannelEnableMask);
    else
      DGManager->DisableAutoTrigger(ChannelEnableMask);

    if(TriggerTypeSoftware)
      DGManager->EnableSWTrigger();
    else
      DGManager->DisableSWTrigger();

    DGManager->DisableExternalTrigger();
  }

  
  /////////////////////////////////////////
  // Program the PSD firmware digitizers //
  /////////////////////////////////////////

  else if(DGFirmwareType == "PSD"){
    
    CAEN_DGTZ_DPP_PSD_Params_t PSDParameters;
    
    for(int ch=0; ch<NumChannels; ch++){
      
      PSDParameters.nsbl[ch] = ChBaselineSamples[ch];
      PSDParameters.csens[ch] = ChChargeSensitivity[ch];
      
      if(TriggerTypeAutomatic)
	PSDParameters.selft[ch] = 1;
      else if(TriggerTypeSoftware)
	PSDParameters.selft[ch] = 0;
      
      PSDParameters.thr[ch] = ChTriggerThreshold[ch];
      PSDParameters.tvaw[ch] = 0;
      PSDParameters.trgc[ch] = CAEN_DGTZ_DPP_TriggerConfig_Threshold;
      
      PSDParameters.sgate[ch] = ChShortGate[ch];
      PSDParameters.lgate[ch] = ChLongGate[ch]; 
      PSDParameters.pgate[ch] = ChGateOffset[ch];
    }

    DGManager->SetDPPParameters(ChannelEnableMask, &PSDParameters);


    ///////////////////////////////////////////////////////
    // Set channel-specific, non-PSD structure PSD settings
    
    for(int ch=0; ch<NumChannels; ch++){
      
      DGManager->SetRecordLength(ChRecordLength[ch], ch);
      DGManager->SetChannelDCOffset(ch, ChDCOffset[ch]);
      DGManager->SetDPPPreTriggerSize(ch, ChPreTrigger[ch]);
      
      if(ChPosPolarity[ch])
	DGManager->SetChannelPulsePolarity(ch, CAEN_DGTZ_PulsePolarityPositive);
      else if(ChNegPolarity[ch])
	DGManager->SetChannelPulsePolarity(ch, CAEN_DGTZ_PulsePolarityNegative);
    }
    
    
    ////////////////////////////////////////////
    // Set global non-PSD structure PSD settings
    
    DGManager->SetDPPAcquisitionMode(CAEN_DGTZ_DPP_ACQ_MODE_List, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
    DGManager->SetDPPTriggerMode(CAEN_DGTZ_DPP_TriggerMode_Normal);
    DGManager->SetIOLevel(CAEN_DGTZ_IOLevel_TTL);
    DGManager->SetDPPEventAggregation(EventsBeforeReadout, 0);
    DGManager->SetRunSynchronizationMode(CAEN_DGTZ_RUN_SYNC_Disabled);
  }
}


void AcquisitionManager::RunAcquisitionLoop()
{
  if(!DGLinkOpen)
    return;

  if(Debug){
    while(true){
      boost::posix_time::milliseconds Time(500);
      cout << "AcquisitionManager (acquisition thread) : Acquisition loop is running in debug mode!" << endl;
      boost::this_thread::sleep(Time);
      boost::this_thread::interruption_point();
    }
  }
  
  cout << "AcquisitionManager (acquisition thread) : Beginning waveform acquisition...\n"
       << endl;
  
  // Total number of accumulated events, which is useful to output to
  // stdout for the user's benefit
  int TotalEvents = 0;
  
  // The array (of length 'RecordLength' in units of samples) used to
  // receive the digitized waveform from the digitizer [ADC]
  double Voltage[RecordLength];

  // Set the V1720 to begin acquiring data
  DGManager->SWStartAcquisition();

  while(true){
    
    // Read data from the buffer into the PC buffer
    DGManager->ReadData(Buffer, &BufferSize);    
    
    // Determine the number of events in the buffer
    DGManager->GetNumEvents(Buffer, BufferSize, &PCEvents);

    // If there are no events in the current buffer then continue in
    // the 'while' loop without executing the CPU intensive 'for'
    // loops on the next lines. This maximizes code efficiency and
    // only scans/processes events when there is need to do so.
    if(PCEvents==0)
      continue;
    
    // For each event in the PC memory buffer...
    for(uint32_t evt=0; evt<PCEvents; evt++){
      
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
	if(!ChEnabled[ch])
	  continue;

	for(uint32_t sample=0; sample<RecordLength; sample++){
	  // Get the digitized voltage in units of analog-to-digital conversion bits
	  Voltage[sample] = EventWaveform->DataChannel[ch][sample]; // [ADC]
	}
      }

      TotalEvents++;
      
      if(TotalEvents % 1 == 0){
	cout << "\r" << flush;
	cout << "Accumulated events = " << TotalEvents << flush;
      }
    }

    // Check to see if a thread interrupt has been requested. Note
    // that if the Control_thread requests an interrupt, this thread
    // (Acquisition_thread) will terminate.
    boost::this_thread::interruption_point();
  }
}


void AcquisitionManager::RunControlLoop(boost::thread *Acquisition_thread)
{
  // This member function is run in the Escape_thread and receives the
  // Acquisition_thread as a function argument. The purpose of this
  // thread is primarily to provide with the ability to intervene with
  // the program during the acquisition loop via Boost's thread
  // interrupt method.

  cout << "AcquisitionManager (escape thread) : Enter a '0' to terminate acquisition!\n" 
       << "                                     Enter 1 '1' to trigger the digitizer!\n" 
       << endl;

  int Choice = -1;
  bool RunLoop = true;
  
  // Loop until the user decides to terminate the acquisition
  while(RunLoop){
    cin >> Choice;
    
    // Terminate the Acquisition_thread and proceed to disarm phase
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
  
  // Free the PC memory associated with digitizer readout
  DGManager->FreeReadoutBuffer(&Buffer);

  if(DGFirmwareType == "STD"){
    DGManager->FreeEvent(&EventWaveform);
  }
  else if(DGFirmwareType == "PSD"){
    DGManager->FreeDPPEvents((void **)PSDEvents);
    DGManager->FreeDPPWaveforms(PSDWaveforms);
  }
  
  // Close a VME link to the digitizer
  DGManager->CloseLink();
}
