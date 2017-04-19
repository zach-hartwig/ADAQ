/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                           Copyright (C) 2012-2015                           //
//                 Zachary Seth Hartwig : All rights reserved                  //
//                                                                             //
//      The ADAQ libraries source code is licensed under the GNU GPL v3.0.     //
//      You have the right to modify and/or redistribute this source code      //      
//      under the terms specified in the license, which may be found online    //
//      at http://www.gnu.org/licenses or at $ADAQ/License.md.                 //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQDigitizer.cc
// date: 17 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQDigitizer is a derived class that is intended to provide
//       full control over a CAEN digitizer (either VME or desktop),
//       including VME connection, register read/write, programming,
//       and high level data acquisition and readout methods. The
//       class inherits all the general member data and methods
//       contained in ADAQVBoard.
//
//       The class is intended to be completely adaptable to any type
//       of CAEN digitizer. All digitizer-specific information (number
//       of channels, bit-depth, etc) is set upon establishing a valid
//       VME connection to the unit, while all methods are intended to
//       be as general as possible across all supported digitizers. 
//
//       Two sets of "wrappers" are provided. First, the class
//       provides complete "wrapping" of the functions contained in
//       the CAENDigitizer library. The main purposes are to (a)
//       provide a straightforward and uniform set of methods for
//       digitizer control amd (b) to obscure the nitty-gritty of the
//       CAEN library functions from the user (if he/she desires).
//
//       Second, a number of Python-friendly methods are provided that
//       are required for various set/get methods (Python has no
//       concept of passing by reference, of which CAEN makes heavy
//       use so we have to provide these methods). These methods are
//       utilized in the ADAQ Python module that is created with
//       Boost.Python during the ADAQ library build process. WARNING:
//       These are experimental methods that haven't been touched
//       since 2012! 
//
///////////////////////////////////////////////////////////////////////////////

// C++
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <bitset>
#include <map>

// CAEN
extern "C" {
#include "CAENDigitizer.h"
}

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

// ADAQ
#include "ADAQDigitizer.hh"


// The ZLE namespace contains a variety of hex and bitwise variables
// that are necessary to properly readout the ZLE event buffer
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


namespace BlockMgmt{
  

}
using namespace BlockMgmt;


ADAQDigitizer::ADAQDigitizer(ZBoardType Type,  // ADAQ-specific device type identifier
			     int ID,           // ADAQ-specific user-specified ID
			     uint32_t Address, // 8 hex digit VME base address
			     int LN,           // USB link number
			     int CN)           // CONET node ID
  : ADAQVBoard(Type, ID, Address, LN, CN),
    BoardSerialNumber(0), BoardModelName(""),
    BoardROCFirmwareRevision(""), BoardAMCFirmwareRevision(""),
    BoardFirmwareCode(0), BoardFirmwareType(""),
    NumChannels(0), NumADCBits(0), MinADCBit(0), MaxADCBit(0), SamplingRate(0),
    TimeStampSize(31), TimeStampUnit(8),
    ZLEStartWord(0), ZLEWordCounter(0)
    //Buffer_Py(NULL), EventPointer_Py(NULL), EventWaveform_Py(Null)
{

  // Create a std::map that specifies the digitization rate for each
  // ADAQ-supported digitizer since this information is not encoded in
  // the CAEN board information structure. Units of digitization are
  // [Megasamples / second]
  insert(SamplingRateMap)
    (zV1720, 250)
    (zV1724, 100)
    (zV1725, 250)
    (zDT5720, 250)
    (zDT5730, 500)
    (zDT5790M, 250)
    (zDT5790N, 250)
    (zDT5790P, 250);

  // Similarly, maps for the size and unit of the timestamp for each digitizer
  // model (which can additionally vary by firmware, which is corrected when the
  // link is opened and the firmware type determined)

  insert(TimeStampSizeMap) // Size of timestamp in bits
    (zV1720, 31)
    (zV1724, 31)
    (zV1725, 31)
    (zDT5720, 31)
    (zDT5730, 31)
    (zDT5790M, 32)
    (zDT5790N, 32)
    (zDT5790P, 32);

  insert(TimeStampUnitMap) // Timestamp unit in ns.  NOTE: CAEN's spec sheets
    (zV1720, 8)               // often quote a time stamp resolution followed by
    (zV1724, 10)              // a total time range for the stamp that is half
    (zV1725, 8)               // that would be expected given the timestamp
    (zDT5720, 8)              // resolution and storage size.  This is due to
    (zDT5730, 8)              // fact that many CAEN units only read out on even
    (zDT5790M, 4)             // even numbered timestamps (LSB=0), reducing the
    (zDT5790N, 4)             // resolution relative to the quote range.  These
    (zDT5790P, 4);            // numbers are the single timestamp units, which 
                              // DO NOT NECESSARILY MATCH THE "RESOLUTION"
}


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
					    BoardLinkNumber,
					    BoardCONETNode,
					    BoardAddress,
					    &BoardHandle);
  }
  
  if(CommandStatus == CAEN_DGTZ_Success){
    
    LinkEstablished = true;
    
    if(Verbose){
      
      // Readout information about digitizer device into class data members

      CAEN_DGTZ_BoardInfo_t BoardInformation;
      CAEN_DGTZ_GetInfo(BoardHandle, &BoardInformation);
      
      // High-level Physical information about the board
      BoardSerialNumber = BoardInformation.SerialNumber;
      BoardModelName = BoardInformation.ModelName;
      BoardROCFirmwareRevision = BoardInformation.ROC_FirmwareRel;
      BoardAMCFirmwareRevision = BoardInformation.AMC_FirmwareRel;

      // Parse the AMC firmware string to get the encoded firmware number
      size_t Pos = BoardAMCFirmwareRevision.find(".");
      if(Pos != string::npos)
	BoardFirmwareCode = std::atoi(BoardAMCFirmwareRevision.substr(0,Pos).c_str());
      else
	std::cout << "ADAQDigitizer[" << BoardID << "] : Error finding board firmware code! Defaulting to std firmware code!\n"
		  << std::endl;

      // Set the CAEN DPP firmware type string based on hex code from
      // the CAENDigitizerType.h header file. This format is a bit
      // more useful for ADAQ-derived codes.
      //
      //  STD : CAEN standard firmware
      //  PHA : Pulse height analysis
      //  CI  : Charge integration
      //  PSD : Pulse shape discrimination

      switch(BoardFirmwareCode){

      case STANDARD_FW_CODE:
	BoardFirmwareType = "STD";
	break;

      case V1724_DPP_PHA_CODE:
      case V1730_DPP_PHA_CODE:
	BoardFirmwareType = "PHA";
	break;

      case V1720_DPP_CI_CODE:
      case V1743_DPP_CI_CODE:
	BoardFirmwareType = "CI";
	break;

      case V1720_DPP_PSD_CODE:
      case V1751_DPP_PSD_CODE:
      case V1730_DPP_PSD_CODE:
	BoardFirmwareType = "PSD";
	break;
	
      case V1751_DPP_ZLE_CODE:
	BoardFirmwareType = "ZLE";
	break;
	
      default:
	BoardFirmwareType = "STD";
	break;
      }

      
      
      // Conceptual information about the digitization
      NumChannels = BoardInformation.Channels;
      NumADCBits = BoardInformation.ADC_NBits;
      MinADCBit = 0;
      MaxADCBit = pow(2, NumADCBits);
      SamplingRate = SamplingRateMap[BoardType];
      TimeStampSize = TimeStampSizeMap[BoardType];
      TimeStampUnit = TimeStampUnitMap[BoardType];

      // Special handling for boards on which the firmware alters the time stamp
      // structure/resolution
      if (BoardType==zV1725 && (BoardFirmwareType==string("PSD") || BoardFirmwareType==string("PHA")))
        TimeStampUnit = 4; // ns
      else if (BoardType==zDT5730 && (BoardFirmwareType==string("PSD")))
        TimeStampUnit = 2; // ns
      else if (BoardType==zDT5730 && (BoardFirmwareType==string("PHA"))){
        TimeStampUnit = 2; // ns
        TimeStampSize = 30; // bits
      }

      std::cout << "ADAQDigitizer[" << BoardID << "] : Link successfully established!\n"
        << "--> Type           : " << BoardModelName << "\n"
        << "--> Channels       : " << NumChannels << "\n"
        << "--> ADC bits       : " << NumADCBits << "\n"
        << "--> Dgtz rate      : " << SamplingRate << " MS/s\n"
        << "--> Time stamp size: " << TimeStampSize << " bits\n"
        << "--> Time stamp unit: " << TimeStampUnit << " ns\n"
        << "--> AMC FW         : " << BoardAMCFirmwareRevision << "\n"
        << "--> ROC FW         : " << BoardROCFirmwareRevision << "\n"
        << "--> FW Type        : " << BoardFirmwareType << "\n"
        << "--> Serial #       : " << BoardSerialNumber << "\n"
        << "--> USB link       : " << BoardLinkNumber << "\n"
        << "--> CONET ID       : " << BoardCONETNode << "\n"
        << "--> Address        : 0x" << std::setw(8) << std::setfill('0') << std::hex << BoardAddress << "\n"
        << "--> User ID        : " << std::dec << BoardID << "\n"
        << "--> Handle         : " << BoardHandle << "\n"
        << std::endl;
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
  CommandStatus = -42;
  
  // Reset the board firmware
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_SW_RESET_ADD, 0x00000000);
  
  // Set the VME control: all disabled, enable BERR
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_VME_CONTROL_ADD, 0x00000010);
  
  // Set front panel I/O controls 
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_FRONT_PANEL_IO_CTRL_ADD, 0x00000000);
  
  // Set the trigger source enable mask 
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_TRIGGER_SRC_ENABLE_ADD, 0xC0000080);
  
  // Set the channel trigger enable mask
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_FP_TRIGGER_OUT_ENABLE_ADD, 0x00000000);
  
  // Set the channel configuration
  CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, CAEN_DGTZ_BROAD_CH_CTRL_ADD, 0x00000050);

  return CommandStatus;
}


int ADAQDigitizer::SetRegisterValue(uint32_t Addr32, uint32_t Data32)
{ 
  CommandStatus = -42;
  
  if(CheckRegisterForWriting(Addr32))
    CommandStatus = CAEN_DGTZ_WriteRegister(BoardHandle, Addr32, Data32); 
  
  return CommandStatus;
}


int ADAQDigitizer::GetRegisterValue(uint32_t Addr32, uint32_t *Data32)
{
  CommandStatus = -42;

  if(CheckRegisterForWriting(Addr32))
    CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, Addr32, Data32);
  
  return CommandStatus;
}


bool ADAQDigitizer::CheckRegisterForWriting(uint32_t Addr32)
{
  if((Addr32 < 0x1024) or
     (Addr32 > 0x814c and Addr32 < 0xef00) or
     (Addr32 > 0xf3fc)){
    if(Verbose)
      cout << "ADAQDigitizer : Error attempting to access a protected register address!\n"
	   << endl;
    return false;
  }
  else
    return true;
}


/////////////////////
// General methods //
/////////////////////

bool ADAQDigitizer::CheckForEnabledChannel()
{
  uint32_t ChannelEnableMask = 0;
  GetChannelEnableMask(&ChannelEnableMask);
  
  if((0xffff & ChannelEnableMask) == 0){
    cout << "ADAQDigitizer : Warning! No channels were enabled for acquisition!\n"
	 << endl;
    return false;
  }
  else 
    return true;
}


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
    CommandStatus = SetAcquisitionMode(CAEN_DGTZ_S_IN_CONTROLLED);
    
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

  // At present, it appears necessary to directly set the bits
  // appropriate to ensure a TTL/NIM signal fed into S-IN (VME) or GPI
  // (DT) can be used to control the acquisition on/off.
  
  bitset<32> Data32Bitset(Data32);
  Data32Bitset.set(0,1);
  Data32Bitset.set(1,0);
  Data32Bitset.set(2,1);
  Data32 = (uint32_t)Data32Bitset.to_ulong();

  CommandStatus = SetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, Data32);

  return CommandStatus;
}


int ADAQDigitizer::SInDisarmAcquisition()
{
  uint32_t Data32 = 0;
  CommandStatus = GetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, &Data32);
  
  bitset<32> Data32Bitset(Data32);
  Data32Bitset.set(0,0);
  Data32Bitset.set(1,0);
  Data32Bitset.set(2,0);
  Data32 = (uint32_t)Data32Bitset.to_ulong();
  
  CommandStatus = SetRegisterValue(CAEN_DGTZ_ACQ_CONTROL_ADD, Data32);

  CommandStatus = SetAcquisitionMode(CAEN_DGTZ_SW_CONTROLLED);
  
  return CommandStatus;
}


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
  (PosLogic) ? Data32Bitset.set(31,0) : Data32Bitset.set(31,1);
  
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

  return 0;
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

  //// WARNING //// WARNING //// WARNING //// WARNING //// WARNING //// WARNING
  ///
  // There appears to be a bug in the CAEN V1720 firmware that causes
  // readout of ZLE events with RecordLength>4030 to shit the bed. The
  // CAEN_DGTZ_GetNumEvents() claims there is only 1 event (despite
  // that the FPGA event register shows it is full) and the buffer if
  // full of garbage. The EventSize is returned as ~(2**28)/2, which
  // is obviously horseshit. This is a hack to prevent this situation
  // from causing segfaults. CAEN has been contacted regarding this
  // issue! ZSH (16 Oct 14)
  
  if(EventSize > 1e5)
    return -42;

  //
  ///
  //// WARNING //// WARNING //// WARNING //// WARNING //// WARNING //// WARNING
  
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
  
  return 0;
}


/////////////////////
// Readout methods //
/////////////////////

int ADAQDigitizer::GetChannelBufferStatus(bool *BufferStatus)
{

  // The following bitmap for digitizer register 0x1n88 contains the
  // channel's FPGA RAM buffer memory full status in bit[0]: 
  //  -- if bit[0] == 0 then memory is not full
  //  -- if bit[0] == 1 then memory is full.
  //
  // However, note that for record lengths below ~285 samples, bit[0]
  // is always forced to be 0 because the number of events waiting for
  // readout in 0x1n94 is always 1 short of full (.e.g 0x3FF = 1023
  // instead of 0x400 = 1024). This problem must be address with CAEN.

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


int ADAQDigitizer::GetBufferStatus(int Channel, bool &BufferStatus)
{
  CommandStatus = -42;
  return CommandStatus;
}


int ADAQDigitizer::GetSTDBufferLevel(double &BufferLevel)
{
  // The following variable maps the return value from digitizer
  // register 0x800C to the number of blocks into which the FPGA
  // channel memory is divided. The default is 0x0A = 1024 blocks.
  
  map<uint32_t, uint32_t> BlockSizeMap;
  BlockSizeMap[0x00] = 1;
  BlockSizeMap[0x01] = 2;
  BlockSizeMap[0x02] = 4;
  BlockSizeMap[0x03] = 8;
  BlockSizeMap[0x04] = 16;
  BlockSizeMap[0x05] = 32;
  BlockSizeMap[0x06] = 64;
  BlockSizeMap[0x07] = 128;
  BlockSizeMap[0x08] = 256;
  BlockSizeMap[0x09] = 512;
  BlockSizeMap[0x0A] = 1024;
  
  // Get the register value from 0x800C in order to determine the
  // number of blocks in segmented memory.
  
  uint32_t Addr32 = CAEN_DGTZ_BROAD_NUM_BLOCK_ADD;
  uint32_t Data32 = 0;
  
  CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, Addr32, &Data32);
  uint32_t MemoryBlocks = BlockSizeMap.at(Data32);
  
  // Get the number of filled blocks (i.e. events) waiting in the FPGA
  // buffer memory to be readout and and use it to calculate the
  // buffer level (percentage of filled blocks)
  
  Addr32 = CAEN_DGTZ_EVENT_STORED_ADD;
  Data32 = 0;
  
  CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, Addr32, &Data32);

  BufferLevel = Data32 * 1. / MemoryBlocks;
  
  return CommandStatus;
}


int ADAQDigitizer::GetPSDBufferLevel(double &BufferLevel)
{
  uint32_t Addr32 = CAEN_DGTZ_ACQ_STATUS_ADD;
  uint32_t Data32 = 0;

  CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, Addr32, &Data32);

  bitset<32> Data32Bitset(Data32);
  
  // Test bit 4 for the following results
  //  0 == No channel has reached the
  //  1 == Maximum number of events has been reached

  if(Data32Bitset.test(4))
    BufferLevel = 1.;
  else
    BufferLevel = 0.;
  
  return CommandStatus;
}


int ADAQDigitizer::GetNumFPGAEvents(uint32_t *Data32)
{ 
  CommandStatus = GetRegisterValue(CAEN_DGTZ_EVENT_STORED_ADD,
				   Data32);
  return CommandStatus;
}
