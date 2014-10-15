///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQDigitizer.cc
// date: 02 Oct 14
// auth: Zach Hartwig
//
// desc: 
//
///////////////////////////////////////////////////////////////////////////////

// C++
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <bitset>

// CAEN
extern "C" {
#include "CAENDigitizer.h"
}

// ADAQ
#include "ADAQDigitizer.hh"


namespace ZLE{

  // Bit mask to get the size (number of 32-bit words) of a ZLE event
  const uint32_t ZLEEventSizeMask = 0x0fffffff;

  // Bit masks to extract two ZLE samples from single 32-bit word
  const uint32_t ZLESampleAMask = 0x0000ffff;
  const uint32_t ZLESampleBMask = 0xffff0000;

  // Bit mask to obtain the ZLE control word
  const uint32_t ZLEControlMask = 0xc0000000;

  // Bit mask to get the number of words following the 'good', 'skip'
  // and 'data' flags in the control word
  const uint32_t ZLENumWordMask = 0x000fffff;

  // Number of words in the ZLE event header
  const uint32_t ZLEHeaderSize = 4; // [32-bit words]

  // Masks to determine if a control word is 'good' or 'skip'
  const uint32_t ZLEControlWordGoodMask = 0b11;
  const uint32_t ZLEControlWordSkipMask = 0b01;

  // Number of bits to right-shift for control word testing
  const uint32_t ZLEControlWordBitShift = 30;

  // Number of bit to right-shift to get sample B data
  const uint32_t ZLESampleBBitShift = 16;
};
using namespace ZLE;



ADAQDigitizer::ADAQDigitizer(ZBoardType Type, int ID, uint32_t Address)
  : ADAQVBoard(Type, ID, Address),
    NumChannels(0), NumADCBits(0), MinADCBit(0), MaxADCBit(0),
    ZLEStartWord(0), ZLEWordCounter(0)
{;}
// Initialize the pointers used during the data acquisition process
// to readout the digitized waveforms from the V1720 to the PC
//    Buffer_Py(NULL), EventPointer_Py(NULL), EventWaveform_Py(NULL)


ADAQDigitizer::~ADAQDigitizer()
{;}


int ADAQDigitizer::OpenLink()
{
  CommandStatus = -42;
  
  if(LinkEstablished){
    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error opening link! Link is already open!"
		<< std::endl;
  }
  else{
    CommandStatus = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 
					    0, 
					    0, 
					    BoardAddress,
					    &BoardHandle);
  }
  
  if(CommandStatus == CAEN_DGTZ_Success){
    
    LinkEstablished = true;
    
    if(Verbose){
      
      CAEN_DGTZ_BoardInfo_t BoardInformation;
      CAEN_DGTZ_GetInfo(BoardHandle, &BoardInformation);
      
      std::cout << "ADAQDigitizer[" << BoardID << "] : Link successfully established!\n"
		<< "--> Type     : " << BoardInformation.ModelName << "\n"
		<< "--> Address  : 0x" << std::setw(8) << std::setfill('0') << std::hex << BoardAddress << "\n"
		<< "--> User ID  : " << std::dec << BoardID << "\n"
		<< "--> Handle   : " << BoardHandle << "\n"
		<< "--> Channels : " << BoardInformation.Channels << "\n"
		<< "--> AMC FW   : " << BoardInformation.ROC_FirmwareRel << "\n"
		<< "--> ROC FW   : " << BoardInformation.AMC_FirmwareRel << "\n"
		<< "--> ADC bits : " << BoardInformation.ADC_NBits << "\n"
		<< "--> Serial # : " << BoardInformation.SerialNumber << "\n"

		<< std::endl;
      
      NumChannels = BoardInformation.Channels;
      NumADCBits = BoardInformation.ADC_NBits;
      MinADCBit = 0;
      MaxADCBit = pow(2, NumADCBits);
    }
  }
  else
    if(Verbose and !LinkEstablished)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error opening link! Returned error code: " << CommandStatus << "\n"
		<< std::endl;
  
  return CommandStatus;
}


int ADAQDigitizer::CloseLink()
{
  CommandStatus = -42;
  
  if(LinkEstablished)
    CommandStatus = CAEN_DGTZ_CloseDigitizer(BoardHandle);
  else
    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error closing link! Link is already closed!\n"
		<< std::endl;
  
  if(CommandStatus == CAEN_DGTZ_Success){

    LinkEstablished = false;

    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Link successfully closed!\n"
		<< std::endl;
  }
  else
    if(Verbose and LinkEstablished)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error closing link! Error code: " << CommandStatus << "\n"
		<< std::endl;
  
  return CommandStatus;
}


int ADAQDigitizer::Initialize()
{
  // Reset the board firmware
  CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_SW_RESET_ADD, 0x00000000);
  
  // Set the VME control: all disabled, enable BERR
  CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_VME_CONTROL_ADD, 0x00000010);
  
  // Set front panel I/O controls 
  CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_FRONT_PANEL_IO_CTRL_ADD, 0x00000000);
  
  // Set the trigger source enable mask 
  CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_TRIGGER_SRC_ENABLE_ADD, 0xC0000080);
  
  // Set the channel trigger enable mask
  CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_FP_TRIGGER_OUT_ENABLE_ADD, 0x00000000);
  
  // Set the channel configuration
  CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_BROAD_CH_CTRL_ADD, 0x00000050);
}


int ADAQDigitizer::SetRegisterValue(uint32_t Addr32, uint32_t Data32)
{ 
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, Addr32, Data32); 
  return CommandStatus;
}


int ADAQDigitizer::GetRegisterValue(uint32_t Addr32, uint32_t *Data32)
{
  CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, Addr32, Data32);
  return CommandStatus;
}


bool ADAQDigitizer::CheckRegisterForWriting(uint32_t Addr32)
{ return true; }


////////////////////////
// Triggering methods //
////////////////////////

int ADAQDigitizer:: EnableAutoTrigger(uint32_t ChannelEnableMask)
{ return SetChannelSelfTrigger(CAEN_DGTZ_TRGMODE_ACQ_ONLY, ChannelEnableMask); }


int ADAQDigitizer::DisableAutoTrigger(uint32_t ChannelEnableMask)
{ return SetChannelSelfTrigger(CAEN_DGTZ_TRGMODE_DISABLED, ChannelEnableMask); }


int ADAQDigitizer:: EnableExternalTrigger(std::string SignalLogic)
{
  CommandStatus = SetExtTriggerInputMode(CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
  
  // Get the value of the front panel I/O control register
  uint32_t FrontPanelIOControlRegister = CAEN_DGTZ_FRONT_PANEL_IO_CTRL_ADD;
  uint32_t FrontPanelIOControlValue = 0;
  CommandStatus = GetRegisterValue(FrontPanelIOControlRegister, &FrontPanelIOControlValue);
  
  // When Bit[0] of return value == 0, NIM logic is used for input; so
  // clear Bit[0] using bitwise ops if "NIM" is specified
  if(SignalLogic=="NIM")
    FrontPanelIOControlValue &= ~(1<<0);
  else if(SignalLogic=="TTL")
    FrontPanelIOControlValue |= 1<<0;
  else
    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error! Unsupported external trigger logic ("
		<< SignalLogic << ") was specified!" << "\n"
		<< "                Select 'NIM' or 'TTL'!\n"
		<< std::endl;
  
  CommandStatus = SetRegisterValue(FrontPanelIOControlRegister, FrontPanelIOControlValue);
  return CommandStatus;
}


int ADAQDigitizer::DisableExternalTrigger()
{ 
  CommandStatus = SetExtTriggerInputMode(CAEN_DGTZ_TRGMODE_DISABLED); 
  return CommandStatus;
}


int ADAQDigitizer:: EnableSWTrigger()
{ 
  CommandStatus = SetSWTriggerMode(CAEN_DGTZ_TRGMODE_ACQ_ONLY);
  return CommandStatus;
}


int ADAQDigitizer::DisableSWTrigger()
{ 
  CommandStatus = SetSWTriggerMode(CAEN_DGTZ_TRGMODE_DISABLED);
  return CommandStatus;
}


int ADAQDigitizer::SetTriggerEdge(int Channel, string TriggerEdge)
{
  CommandStatus = -42;

  if(TriggerEdge == "Rising")
    CommandStatus = SetTriggerPolarity(Channel, 
				       CAEN_DGTZ_TriggerOnRisingEdge);
  else if(TriggerEdge == "Falling")
    CommandStatus = SetTriggerPolarity(Channel, 
				       CAEN_DGTZ_TriggerOnFallingEdge);
  else
    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error! Unsupported trigger edge type ("
		<< TriggerEdge << ") was specified!\n"
		<< "               Select 'Rising' or 'Falling'\n"
		<< std::endl;
  
  return CommandStatus;
}


int ADAQDigitizer::SetTriggerCoincidence(bool Enable, int Level)
{
  CommandStatus = -42;

  if(Enable){
    
    uint32_t TriggerSourceEnableMask = 0;

    uint32_t TriggerCoincidenceLevel_BitShifted = Level << 24;

    CommandStatus = GetRegisterValue(0x810C,&TriggerSourceEnableMask);
    
    TriggerSourceEnableMask = TriggerSourceEnableMask | TriggerCoincidenceLevel_BitShifted;
    
    CommandStatus = SetRegisterValue(0x810C,TriggerSourceEnableMask);
  }
  return CommandStatus;
}


/////////////////////////
// Acquisition methods //
/////////////////////////

int ADAQDigitizer::SetAcquisitionControl(string AcqControl)
{ 
  CommandStatus = -42;

  if(AcqControl == "Software")
    CommandStatus = SetAcquisitionMode(CAEN_DGTZ_SW_CONTROLLED);
  else if(AcqControl == "Gated (NIM)" or AcqControl == "Gated (TTL)"){
    //CommandStatus = SetAcquisitionMode(CAEN_DGTZ_S_IN_CONTROLLED);

    uint32_t Data32 = 0;
    CommandStatus = GetRegisterValue(CAEN_DGTZ_FRONT_PANEL_IO_CTRL_ADD, &Data32);
    
    bitset<32> Data32Bitset0(Data32);

    if(AcqControl == "Gated (NIM)")
      Data32Bitset0.set(0, 0);
    else if(AcqControl == "Gated (TTL)")
      Data32Bitset0.set(0, 1);

    Data32 = (uint32_t)Data32Bitset0.to_ulong();
    CommandStatus = SetRegisterValue(CAEN_DGTZ_FRONT_PANEL_IO_CTRL_ADD, Data32);
    
    //    uint32_t Data32 = 0;
    CommandStatus = GetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, &Data32);
    
    bitset<32> Data32Bitset1(Data32);
    Data32Bitset1.set(0,1);
    Data32Bitset1.set(1,0);

    Data32 = (uint32_t)Data32Bitset1.to_ulong();
    CommandStatus = SetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, Data32);
  }
  else
    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error! Unsupported acquisition control style ("
		<< AcqControl << ") was specified!\n"
		<< "                Select 'Software', 'Gated (NIM)' or 'Gated (TTL)'.\n"
		<< std::endl;
  
  return CommandStatus;
}


int ADAQDigitizer::SInArmAcquisition()
{
  uint32_t Data32 = 0;
  CommandStatus = GetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, &Data32);
  
  bitset<32> Data32Bitset(Data32);
  Data32Bitset.set(2,1);
  Data32 = (uint32_t)Data32Bitset.to_ulong();
  
  CommandStatus = SetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, Data32);

  return CommandStatus;
}


int ADAQDigitizer::SInDisarmAcquisition()
{;}


int ADAQDigitizer::SetZSMode(string ZSMode)
{
  CommandStatus = -42;
  
  if(ZSMode == "None")
    CommandStatus = SetZeroSuppressionMode(CAEN_DGTZ_ZS_NO);
  else if(ZSMode == "ZLE")
    CommandStatus = SetZeroSuppressionMode(CAEN_DGTZ_ZS_ZLE);
  else
    if(Verbose)
      std::cout << "ADAQDigitizer[" << BoardID << "] : Error! Unsupported zero suppression mode ("
		<< ZSMode << ") was specified!\n"
		<< "                Select 'None' or 'ZLE'.\n"
		<< std::endl;
  
  return CommandStatus;
}


int ADAQDigitizer::SetZLEChannelSettings(uint32_t Channel, uint32_t Threshold,
					 uint32_t NBackward, uint32_t NForward,
					 bool PosLogic)
{
  CommandStatus = -42;

  CAEN_DGTZ_ThresholdWeight_t DummyWeight = CAEN_DGTZ_ZS_FINE;
  int32_t DummySamples = 4242;

  ////////////////////////
  // Set the ZLE threshold
  
  CommandStatus = SetChannelZSParams(Channel, 
				     DummyWeight, // Arbitrary; unused in ZLE algorithm
				     Threshold,
				     DummySamples); // Arbitrary; unused in ZLE algorithm
  
  //////////////////////////////////////////////
  // Set the ZLE backward/forward sample numbers
  
  uint32_t ChannelOffset = 0x100; 
  
  uint32_t Addr32 = CAEN_DGTZ_CHANNEL_ZS_NSAMPLE_BASE_ADDRESS;
  Addr32 += (ChannelOffset * Channel);
  
  uint32_t Data32 = (NBackward << 16) + NForward;
  CommandStatus = SetRegisterValue(Addr32, Data32);
  
  //////////////////////////////
  // Set the ZLE algorithm logic
  
  Addr32 = CAEN_DGTZ_CHANNEL_ZS_THRESHOLD_BASE_ADDRESS;
  Addr32 += (ChannelOffset * Channel);
  
  CommandStatus = GetRegisterValue(Addr32, &Data32);
  
  // ZLE requires bit 31 == 0 (positive logic), == 1 (negative logic)
  bitset<32> Data32Bitset(Data32);
  (PosLogic) ? Data32Bitset.set(31,0) : Data32Bitset.set(31.1);
  
  // Convert bitset back to 32-bit integer and set register value
  Data32 = (uint32_t)Data32Bitset.to_ulong();
  CommandStatus = SetRegisterValue(Addr32, Data32);


  
  return CommandStatus;
}


int ADAQDigitizer::PrintZLEEventInfo(char *Buffer, 
				     int Event)

{
  cout << "\n\n\n"
       << "******************************  NEW ZLE EVENT  ******************************\n"
       << endl;

  uint32_t *Words = (uint32_t *)Buffer;

  int ZLEStartWord = 0;
  if(Event == 0){
    ZLEStartWord = 0;
    ZLEWordCounter = 0;
  }
  else
    ZLEStartWord = ZLEWordCounter;
  
  uint32_t EventSize = Words[ZLEStartWord] & ZLEEventSizeMask;
  ZLEWordCounter += EventSize;
  
  cout << "  Event number : " << Event << " (within FPGA readout buffer)\n"
       << "  Event size   : " << EventSize << " (Number 32-bit words in this ZLE event)\n"
       << "  Event span   : Words[" << ZLEStartWord << "] to Words[" << ((ZLEStartWord+EventSize)-1) << "]\n"
       << "\n"
       << "  Word  Type  Value\n"
       << "---------------------------------------------------------------------------------"
       << endl;
  
  for(int word=ZLEStartWord; word<(ZLEStartWord+ZLEHeaderSize); word++){
    bitset<32> BinaryOut(Words[word]);
    cout << "  " << word << "\t" << "header[" << (word-ZLEStartWord) << "]\t" << BinaryOut << endl;
  }

  int SizeStartWord = ZLEStartWord + ZLEHeaderSize;
  cout << "  " << SizeStartWord << "\tsize" << "\t" << Words[SizeStartWord] << endl;
  
  int ZLEWaveformStartWord = SizeStartWord + 1;
  uint32_t ZLEWaveformWordCounter = 0;

  for(int word=ZLEWaveformStartWord; word<(ZLEStartWord+EventSize); word++){
    
    uint32_t ZLEControl = Words[word] & ZLEControlMask;
    ZLEControl = ZLEControl >> 30;
    
    if(ZLEControl == ZLEControlWordGoodMask){
      uint32_t NumWords = Words[word] & ZLENumWordMask;
      
      bitset<32> TMP(NumWords);
      
      cout << "  " << word << "\t" << "GOOD!"  << "\t"
	   << NumWords << " words to follow!"
	   << endl;

      ZLEWaveformWordCounter = 1;
    }
    else if(ZLEControl == ZLEControlWordSkipMask){
      uint32_t NumWords = Words[word] & ZLENumWordMask;
      
      cout << "  " << word << "\t" << "skip" << "\t"
	   << NumWords << " words have been skipped!"
	   << endl;
    }
    else{
      cout << "  " << word << "\t" << "data" << "\t" 
	   << "Word number: " << ZLEWaveformWordCounter << "\t\t";
      
      uint32_t SampleN = Words[word] & ZLESampleAMask;
      
      uint32_t SampleN_plus_1 = Words[word] & ZLESampleBMask;
      SampleN_plus_1 = SampleN_plus_1 >> 16;
      
      cout << "Sample[" << (ZLEWaveformWordCounter*2)-1 << "] = " << SampleN << "\t"
	   << "Sample[" << (ZLEWaveformWordCounter*2) << "] = " << SampleN_plus_1 << "\t"
	   << endl;
      
      ZLEWaveformWordCounter++;
    }
  }
}


int ADAQDigitizer::GetZLEWaveform(char *Buffer, 
				  int Event,
				  vector<vector<uint16_t> > &Waveforms)
{
  // Clear the Waveforms for a new ZLE event
  Waveforms.clear();
  Waveforms.resize(NumChannels);

  // Cast a pointer to the PC buffer containing ZLE events
  uint32_t *Words = (uint32_t *)Buffer;
  
  // Determine which word in the PC buffer should be the first word
  // readout depending on the event number
  if(Event == 0){
    ZLEStartWord = 0;
    ZLEWordCounter = 0;
  }
  else
    ZLEStartWord = ZLEWordCounter;
  
  // Get the total number of 32-bit words in this ZLE event (4 header
  // words plus size, control, and sample words)
  uint32_t EventSize = Words[ZLEStartWord] & ZLEEventSizeMask;

  // Increment the ZLEWordCounter with the number of words in the
  // current ZLE event such that, if there is another event that will
  // be readout after this one, the start word will be set correctly
  ZLEWordCounter += EventSize;

  // Get the number of 32-bit words in the ZLE waveform (control words
  // + waveform samples (remember: 2 samples per word))
  int SizeStartWord = ZLEStartWord + ZLEHeaderSize;
  uint32_t ZLEWaveformSize = Words[SizeStartWord];
  
  int ZLEWaveformStartWord = SizeStartWord + 1;

  uint32_t Channel = 0;

  for(int word=ZLEWaveformStartWord; word<(ZLEStartWord+EventSize); word++){
    uint32_t ControlWord = Words[word] & ZLEControlMask;
    ControlWord >>= ZLEControlWordBitShift;
    
    // ZLE "good" control word
    if(ControlWord == ZLEControlWordGoodMask){
    }

    // ZLE "skip" control word
    else if(ControlWord == ZLEControlWordSkipMask){
    }

    // ZLE "data" word
    else{
      // Readout the samples from the data words. Bits [15:0] are
      // sample A; bits[31:15] are sample B. Note that sample "A"
      // precedes samples "B" in chronological time.
      
      uint32_t SampleA = Words[word] & ZLESampleAMask;
      uint32_t SampleB = Words[word] & ZLESampleBMask;
      SampleB >>= ZLESampleBBitShift;
      
      Waveforms[Channel].push_back(SampleA);
      Waveforms[Channel].push_back(SampleB);
    }
  }
}


/////////////////////
// Readout methods //
/////////////////////

int ADAQDigitizer::CheckBufferStatus(bool *BufferStatus)
{
  // bit[0] : 0 = memory not full; 1 = memory full
  // bit[1] : 0 = memory not empty; 1 = memory empty
  // bit[2] : 0 = DAC not busy; 1 = DAC busy
  // bit[3] : Reserved
  // bit[4] : Reserved
  // bit[5] : Buffer free error

  CommandStatus = -42;

  // Channel register addresses and channel-to-channel increment
  uint32_t Start = CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS;
  uint32_t Offset = 0x0100;

  for(int ch=0; ch<NumChannels; ch++){

    uint32_t Addr32 = Start + Offset*ch;
    uint32_t Data32 = 0;
    int Status = 0;
    // Skip channels that are not currently enabled
    CommandStatus = GetChannelEnableMask(&Data32);
    if(!(Data32 & (1 << ch)))
      continue;
    
    // Check to see if the 0-th of each channel's status register bit
    // is set; if any of the channel buffers are full then set the
    // BufferFull flag to true
    CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, Addr32, &Data32);
    (Data32 & (1 << 0)) ? BufferStatus[ch] = true : BufferStatus[ch] = false;
  }
  return CommandStatus;
}
  

int ADAQDigitizer::GetNumFPGAEvents(uint32_t *Data32)
{ 
  CommandStatus = GetRegisterValue(CAEN_DGTZ_EVENT_STORED_ADD,
				   Data32);
  return CommandStatus;
}

