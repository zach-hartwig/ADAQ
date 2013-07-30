///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQDigitizer.cc
// date: 18 MAY 12
// auth: Zach Hartwig

// desc: The ADAQHighVoltage class facilitiates communication with the
//       V1720 digitizer board with VME communications via the
//       CAENComm and CAENVME libraries. The purpose of
//       ADAQHighVoltage is to obscure the nitty-gritty-details of
//       interfacing with the V1720 board and present the user with a
//       relatively simple set of methods and variables that can be
//       easibly used in his/her ADAQ projects by instantiating a
//       single ADAQDigitizer "manager" class. Technically, this class
//       should probably be made into a Meyer's singleton for
//       completeness' sake, but the present code should be sufficient
//       for anticipated applications and userbase.
//        
//       At present, the ADAQDigitizer class is compiled into two
//       shared object libraries: libADAQ.so (C++) and libPyADAQ.so
//       (Python). C++ and Python ADAQ projects can then link against
//       these libraries to obtain the provided functionality. The
//       purpose is to ensure that a single version of the ADAQ
//       libraries exist since they are now used in multiple C++
//       projects, multiple ROOT analysis projects, and new Python
//       projects for interfacing ADAQ with MDSplus data system.
//
///////////////////////////////////////////////////////////////////////////////

// C++
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

// CAEN
extern "C" {
#include "CAENDigitizer.h"
}

// ADAQ
#include "ADAQDigitizer.hh"


ADAQDigitizer::ADAQDigitizer()
  : 
  // CAEN digitizer type
  BoardType(-1),
  
  // The V1720 board address and handle
  BoardAddress(-1), BoardHandle(-1),
  
  // int stores return value of CAEN library call (arbitrarily
  // initialized to -42 to differentiate from CAEN values. See
  // "CAENDigitizerType.h" for possible return values
  CommandStatus(-42),
  
  // Bool to determine state of VME connection to V1720
  LinkEstablished(false),

  // Bool to determine if messages to std::cout
  Verbose(false),

  // The number of channels on the V1720
  NumChannels(8), 

  // The number of bits on the V1720 (2**12 = 4096)
  NumBits(4096),

  // The minimum and maximum bit
  MinBit(0), MaxBit(4095),

  // The number of nanoseconds per sample [1. / 250 MS/s]
  NanosecondsPerSample(4), 
  
  // The number of millivolts per bit is the dynamic input range (2
  // volts) divided by total number of bits (2**12 == 4096)
  MillivoltsPerBit(2000.0/4096),

  // Initialize the pointers used during the data acquisition process
  // to readout the digitized waveforms from the V1720 to the PC
  Buffer_Py(NULL), EventPointer_Py(NULL), EventWaveform_Py(NULL)
{;}


ADAQDigitizer::~ADAQDigitizer()
{;}


int ADAQDigitizer::OpenLink(uint32_t BrdAddr)
{
  // Set the V1720 32-bit hex board address, which must correspond to
  // the settings on the physical potentiometers on the V1720 board
  BoardAddress = BrdAddr;
  
  // If the link is not currently valid then establish one!
  int Status = -42;
  if(!LinkEstablished)
    Status = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 
				     0, 
				     0, 
				     BoardAddress,
				     &BoardHandle);
  else
    if(Verbose)
      std::cout << "\nADAQDigitizer : Error opening link! Link is already open!"
		<< std::endl;
  
  if(Status==0){
    LinkEstablished = true;
    if(Verbose){
      std::cout << "\nADAQDigitizer : Link successfully established!\n"
		<<   "                --> V1720 base address: 0x" 
		<< std::setw(8) << std::setfill('0') << std::hex << BoardAddress << "\n"
		<<   "                --> V1720 handle: " << BoardHandle
		<< std::endl;
      
      // Get and output the AMC (Advanced Mezzanine Card) firmware
      uint32_t amcFirmwareRevision;
      CAEN_DGTZ_ReadRegister(BoardHandle, 0x108c, &amcFirmwareRevision);
      std::cout << "                --> V1720 AMC FGPA firmware:  " << std::dec << (amcFirmwareRevision>>8 & 0xff) 
		<< "." << (amcFirmwareRevision & 0xff) << std::endl;
      
      // Get and output the ROC (Read Out Card) FPGA firmware
      uint32_t rocFirmwareRevision;
      CAEN_DGTZ_ReadRegister(BoardHandle, CAEN_DGTZ_FW_REV_ADD, &rocFirmwareRevision);
      std::cout << "                --> V1720 ROC FPGA firmware:  " << std::dec << (rocFirmwareRevision>>8 & 0xff) 
		<< "." << (rocFirmwareRevision & 0xff) << "\n" << std::endl;
    }
  }
  else
    if(Verbose and !LinkEstablished)
      std::cout << "\nADAQDigitizer : Error opening link: Error code: " << Status << "\n"
		<< std::endl;
  
  return Status;
}


int ADAQDigitizer::CloseLink()
{
  int Status = -42;
  if(LinkEstablished)
    Status = CAEN_DGTZ_CloseDigitizer(BoardHandle);
  else
    if(Verbose)
      std::cout << "\nADAQDigitizer : Error closing link! Link is already closed!\n"
		<< std::endl;

  if(Status==0){
    LinkEstablished = false;
    if(Verbose)
      std::cout << "\nADAQDigitizer : Link successfully closed!\n"
		<< std::endl;
  }
  else
    if(Verbose and LinkEstablished)
      std::cout << "\nADAQDigitizer : Error closing link! Error code: " << Status << "\n"
		<< std::endl;

  return Status;
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
  
  uint32_t d32;
  // Get the board information
  CAEN_DGTZ_ReadRegister(BoardHandle, CAEN_DGTZ_BOARD_INFO_ADD, &d32);
  
  // Store the digitizer board type (bits 0-7 of d32)
  BoardType = d32 & 0xff; 
  
  // Store the number of memory blocks; V1720 == 2
  MemoryBlock = (d32 >> 8) & 0xff;
}


int ADAQDigitizer::EnableExternalTrigger(std::string SignalLogic)
{
  SetExtTriggerInputMode(CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
  
  // Get the value of the front panel I/O control register
  uint32_t FrontPanelIOControlRegister = 0x811C;
  uint32_t FrontPanelIOControlValue;
  GetRegisterValue(FrontPanelIOControlRegister, &FrontPanelIOControlValue);
  
  // When Bit[0] of 0x811C == 0, NIM logic is used for input; so
  // clear Bit[0] using bitwise ops if "NIM" is specified
  if(SignalLogic=="NIM")
    FrontPanelIOControlValue &= ~(1<<0);
  else if(SignalLogic=="TTL")
    FrontPanelIOControlValue |= 1<<0;
  else
    if(Verbose)
      std::cout << "\nADAQDigitizer : Error! Unsupported external trigger logic ("
		<< SignalLogic << ") was specified!" << "\n"
		<<   "                Select 'NIM' or 'TTL'!\n"
		<< std::endl;
  
  SetRegisterValue(FrontPanelIOControlRegister, FrontPanelIOControlValue);
}


int ADAQDigitizer::DisableExternalTrigger()
{ return SetExtTriggerInputMode(CAEN_DGTZ_TRGMODE_DISABLED); }


int ADAQDigitizer::EnableAutoTrigger(uint32_t ChannelEnableMask)
{ return SetChannelSelfTrigger(CAEN_DGTZ_TRGMODE_ACQ_ONLY, ChannelEnableMask); }


int ADAQDigitizer::DisableAutoTrigger(uint32_t ChannelEnableMask)
{ return SetChannelSelfTrigger(CAEN_DGTZ_TRGMODE_DISABLED, ChannelEnableMask); }


int ADAQDigitizer::EnableSWTrigger()
{ SetSWTriggerMode(CAEN_DGTZ_TRGMODE_ACQ_ONLY); }


int ADAQDigitizer::DisableSWTrigger()
{ SetSWTriggerMode(CAEN_DGTZ_TRGMODE_DISABLED); }


int ADAQDigitizer::SetSWAcquisitionMode()
{ return SetAcquisitionMode(CAEN_DGTZ_SW_CONTROLLED); }

int ADAQDigitizer::SetSInAcquisitionMode()
{ return SetAcquisitionMode(CAEN_DGTZ_S_IN_CONTROLLED); }


int ADAQDigitizer::SetRegisterValue(uint32_t addr32, uint32_t data32)
{ return CAEN_DGTZ_WriteRegister(BoardHandle, addr32, data32); }


int ADAQDigitizer::GetRegisterValue(uint32_t addr32, uint32_t *data32)
{ return CAEN_DGTZ_ReadRegister(BoardHandle, addr32, data32); }


uint32_t ADAQDigitizer::GetRegisterValue(uint32_t addr32)
{
  uint32_t data32;
  CommandStatus = CAEN_DGTZ_ReadRegister(BoardHandle, addr32, &data32);
  return data32;
}

bool ADAQDigitizer::CheckRegisterForWriting(uint32_t addr32)
{ return true; }

int ADAQDigitizer::CheckBufferStatus(bool *BufferStatus)
{
  // V1720 Channel Status Register : 0x1n88 where 'n' == channel #

  // bit[0] : 0 = memory not full; 1 = memory full
  // bit[1] : 0 = memory not empty; 1 = memory empty
  // bit[2] : 0 = DAC not busy; 1 = DAC busy
  // bit[3] : Reserved
  // bit[4] : Reserved
  // bit[5] : Buffer free error

  int Status = 0;

  // Channel register addresses and channel-to-channel increment
  uint32_t start = 0x1088;
  uint32_t offset = 0x0100;

  for(int n=0; n<NumChannels; n++){

    uint32_t addr32 = start + offset*n;
    uint32_t data32 = 0;
    int Status = 0;
    // Skip channels that are not currently enabled
    Status = GetChannelEnableMask(&data32);
    if(!(data32 & (1 << n)))
      continue;
    
    // Check to see if the 0-th of each channel's status register bit
    // is set; if any of the channel buffers are full then set the
    // BufferFull flag to true
    Status = CAEN_DGTZ_ReadRegister(BoardHandle, addr32, &data32);
    if(data32 & (1 << 0))
      BufferStatus[n] = true;
    else
      BufferStatus[n] = false;
  }
  return Status;
}
