////////////////////////////////////////////////////////////////////////////////
//
// name: AcquisitionManager.cc
// date: 15 Jul 16
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

// C++ 
#include <iostream>
using namespace std;

// Boost
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "AcquisitionManager.hh"


AcquisitionManager::AcquisitionManager()
  :  DGType(zDT5790M), DGEnable(true), DGLinkOpen(false), DGBoardAddress(0x00000000),
     Verbose(true), Debug(false)
{
  //////////////////////////////////
  // Instantiate a digitizer manager
  
  DGManager = new ADAQDigitizer(DGType,         // ADAQ-specified CAEN device type
				0,              // User-specified ID
				DGBoardAddress, // Address in VME space
				0,              // USB link number
				0);             // CONET node number
  DGManager->SetVerbose(true);
}


AcquisitionManager::~AcquisitionManager()
{
  if(DGEnable)
    delete DGManager;
}


void AcquisitionManager::Arm()
{
  if(Debug)
    return;
  
  InitConnection();
  InitParameters();
  InitDigitizer();
}


void AcquisitionManager::InitConnection()
{
  if(Debug)
    return;
  
  ////////////////////////////
  // VME link to the digitizer
  
  if(DGEnable){
    
    if(Verbose)
      cout << "\nAcquisitionManager (main thread) : Opening a link to the digitizer ...\n"
	   << endl;
    
    int Status = DGManager->OpenLink();
    (Status==0) ? DGLinkOpen = true : DGLinkOpen = false;
    
    if(!DGLinkOpen){
      if(Verbose)
	cout << "AcquisitionManager (main thread) : Error! A link to the digitizer could not be established! Exiting...\n"
	     << endl;
      exit(-42);
    }
    else
      if(Verbose)
	cout << "AcquisitionManager (main thread) : A link to the digitizer has been established!\n"
	     << endl;
  }
}


void AcquisitionManager::InitParameters()
{
  /////////////////////////////
  // Determine firmware type //
  /////////////////////////////
  
  string FirmwareType = DGManager->GetBoardFirmwareType();
  if(FirmwareType == "STD"){
    DGStandardFW = true;
    DGPSDFW = false;
  }
  else if(FirmwareType == "PSD"){
    DGStandardFW = false;
    DGPSDFW = true;
  }

  /////////////////////////////
  // Init readout parameters //
  /////////////////////////////
  
  Int_t NumChannels = DGManager->GetNumChannels();

  if(DGStandardFW){
    EventPointer = NULL;
    EventWaveform = NULL;
    DGManager->AllocateEvent(&EventWaveform);
  }
  else if(DGPSDFW){
    PSDWaveforms = NULL;
  }

  Buffer = NULL;

  BufferSize = ReadSize = FPGAEvents = PCEvents = 0;
  for(Int_t ch=0; ch<NumChannels; ch++)
    NumPSDEvents.push_back(0);
  
  DGManager->MallocReadoutBuffer(&Buffer, &BufferSize);
  
  if(DGPSDFW){
    DGManager->MallocDPPEvents(PSDEvents, &PSDEventSize);
    DGManager->MallocDPPWaveforms(&PSDWaveforms, &PSDWaveformSize); 
  }

  
  /////////////////////////////
  // Init control parameters //
  /////////////////////////////
  
  for(Int_t ch=0; ch<NumChannels; ch++){
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
  
  if(DGStandardFW){

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
  else if(DGPSDFW){
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
  Int_t NumChannels = DGManager->GetNumChannels();

  /////////////////////////////////////////
  // Program the STD firmware digitizers //
  /////////////////////////////////////////

  if(DGStandardFW){
    
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

  else if(DGPSDFW){
    
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
    
    for(Int_t ch=0; ch<NumChannels; ch++){
      
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


void AcquisitionManager::StartAcquisition2()
{
  cout << "AcquisitionManager (acquisition thread) : Beginning waveform acquisition...\n"
       << endl;
  
  while(Debug){
    boost::posix_time::milliseconds Time(250);
    cout << "AcquisitionManager (acquisition thread) : Acquisition loop is running in debug mode!" << endl;
    boost::this_thread::sleep(Time);
  }






  
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

  // Stop the digitizer acquisition
  DGManager->SWStopAcquisition();
  
  // Free the PC memory associated with digitizer readout
  DGManager->FreeReadoutBuffer(&Buffer);

  if(DGStandardFW){
    DGManager->FreeEvent(&EventWaveform);
  }
  else if(DGPSDFW){
    DGManager->FreeDPPEvents((void **)PSDEvents);
    DGManager->FreeDPPWaveforms(PSDWaveforms);
  }
  
  // Close a VME link to the digitizer
  if(DGEnable){
    DGManager->CloseLink();
  }
}
