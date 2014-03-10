///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQHighVoltage.cc
// date: 26 JUN 12
// auth: Zach Hartwig
//
// desc: The ADAQHighVoltage class facilitates communication with the
//       V6534 high voltage board with VME communications via the
//       CAENComm and CAENVME libraries. The purpose of
//       ADAQHighVoltage is to obscure the nitty-gritty-details of
//       interfacing with the V6534 board and present the user with a
//       relatively simple set of methods and variables that can be
//       easibly used in his/her ADAQ projects by instantiating a
//       single ADAQHighVoltage "manager" class. Technically, this
//       class should probably be made into a Meyer's singleton for
//       completeness' sake, but the present code should be sufficient
//       for anticipated applications and userbase.
//        
//       At present, the ADAQHighVoltage class is compiled into two
//       shared object libraries: libADAQ.so (C++) and libPyADAQ.so
//       (Python). C++ and Python ADAQ projects can then link against
//       these libraries to obtain the provided functionality. The
//       purpose is to ensure that a single version of the ADAQ
//       libraries exist since they are now used in multiple C++
//       projects, multiple ROOT analysis projects, and new Python
//       projects for interfacing ADAQ with MDSplus data system.
//
//        
//       At present, the ADAQHighVoltage class is compiled into two
//       shared object libraries: libADAQ.so (C++) and libPyADAQ.so
//       (Python). C++ and Python ADAQ projects can then link against
//       these libraries to obtain the provided functionality. The
//       purpose is to ensure that a single version of the ADAQ
//       libraries exist since they are now used in multiple C++
//       projects, multiple ROOT analysis projects, and new Python
//       projects for interfacing ADAQ with MDSplus data system
//
///////////////////////////////////////////////////////////////////////////////

// C++
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
using namespace std;

// Boost
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;


// CAEN
extern "C" {
#include "CAENComm.h"
}

// ADAQ
#include "ADAQHighVoltage.hh"

// Create a namespace to hold the relevant addresses and some simple
// register value of the V6534 board
namespace{
  
  // V6534 global register addresses
  uint32_t VMAX = 0x0050;
  uint32_t IMAX = 0x0054;
  uint32_t STATUS = 0x0058;
  uint32_t FIRMREl = 0x005C;

  // V6534 individual channel register addresses
  // Register Addr.   CH0(-)    CH1(-)   CH2(-)   CH3(+)   CH4(+)   CH5(+)
  uint32_t VSET[6] = {0x0080,  0x0100,  0x0180,  0x0200,  0x0280,  0x0300}; // Voltage set
  uint32_t ISET[6] = {0x0084,  0x0104,  0x0184,  0x0204,  0x0284,  0x0304}; // Current set
  uint32_t VMON[6] = {0x0088,  0x0108,  0x0188,  0x0208,  0x0288,  0x0308}; // Voltage monitor
  uint32_t IMON[6] = {0x008c,  0x010c,  0x018c,  0x020c,  0x028c,  0x030c}; // Current monitor (max)
  uint32_t   PW[6] = {0x0090,  0x0110,  0x0190,  0x0210,  0x0290,  0x0310}; // Power 
  uint32_t  POL[6] = {0x00ac,  0x012c,  0x01ac,  0x022c,  0x02ac,  0x032c}; // Polarity
  uint32_t TEMP[6] = {0x00b0,  0x0130,  0x01b0,  0x0230,  0x02b0,  0x0330}; // Temperature

  // V6534 register values to control the power state (on/off) at
  // registers "PW" specified above for each channel
  uint16_t POWEROFF = 0x00;
  uint16_t POWERON  = 0x01;
}


ADAQHighVoltage::ADAQHighVoltage()
  :
  // The board address and handle 
  BoardAddress(-1), BoardHandle(-1),

  // Bool to determine VME connection state to V6534
  LinkEstablished(false),

  // Bool to determine if output printed to cout
  Verbose(false),

  // The number of high voltage channels
  NumChannels(6), 

  // Maximum voltage supported by the V6534 board 
  MaxVoltage(6000), // [V]
  
  // Maximum current supported by the V6534 board 
  MaxCurrent(1000), // [uA]

  // The desired channel voltage (in volts) must be set in the
  // registers as (V[volts]*10). Define a conversion value.
  volts2input(10),

  // The desired channel current (in microamps) must be set in the
  // registers as (I[microamps]*50). Define a conversion value.
  microamps2input(50)
{
  // Set the voltage, current, and power status for all HV
  // channels. By default, all voltage/current is initialized to a
  // value of zero and the channels are all turned off
  ChannelSetVoltage += 0, 0, 0, 0, 0, 0;
  ChannelSetCurrent += 0, 0, 0, 0, 0, 0;
  ChannelPowerState += POWEROFF, POWEROFF, POWEROFF, POWEROFF, POWEROFF, POWEROFF;
}


ADAQHighVoltage::~ADAQHighVoltage()
{;}


int ADAQHighVoltage::OpenLink(uint32_t BrdAddr)
{
  // Set the V6534 32-bit hex board address, which must correspond to
  // the settings on the physical potentiometers on the V6534 board
  BoardAddress = BrdAddr;
  
  // If the link is not currently valid then establish one!
  int Status = -42;
  if(!LinkEstablished)
    Status = CAENComm_OpenDevice(CAENComm_USB, 0, 0, BoardAddress, &BoardHandle);
  else
    if(Verbose)
      cout << "ADAQHighVoltage: Error opening link! Link is already open!\n"
	   << endl;
  
  // Set the LinkEstablished bool to indicate that a valid link nto
  // the V6534 has been established and output if Verbose set
  if(Status==0){
    LinkEstablished = true;
    if(Verbose)
      cout << "ADAQHighVoltage : Link successfully established!\n"
	   <<   "                  --> V6534 base address: 0x" 
	   << setw(8) << setfill('0') << hex << BoardAddress << "\n"
	   <<   "                  --> V6534 handle: " << BoardHandle << "\n"
	   << endl;
  }
  else
    if(Verbose and !LinkEstablished)
      cout << "ADAQHighVoltage : Error opening link! Error code: " << Status << "\n"
	   << endl;
  
  // Return success/failure 
  return Status;
}


int ADAQHighVoltage::CloseLink()
{
  // If the link is presently established then close it!
  int Status = -42;
  if(LinkEstablished)
    Status = CAENComm_CloseDevice(BoardHandle);
  else
    if(Verbose)
      cout << "ADAQHighVoltage : Error closing link! Link is already closed!\n"
	   << endl;
  
  if(Status==0){
    LinkEstablished = false;
    if(Verbose)
      cout << "ADAQHighVoltage : Link successfully closed!\n"
	   << endl;
  }
  else
    if(Verbose and LinkEstablished)
      cout << "ADAQHighVoltage : Error closing link! Error code: " << Status << "\n"
	   << endl;

  return Status;
}


int ADAQHighVoltage::SetToSafeState()
{
  // Set the V6534 board to "safe" state ie, set all channels'
  // voltages and currents to 0 and turn all channels off. This
  // function is useful for initialization at the instantiation of an
  // object of type ADAQHighVoltage as well as at shutdown to ensure
  // that the V6534 board is not only safe but ready to have it's VME
  // link disconnected.

  if(Verbose)
    cout << "ADAQHighVoltage : Setting the V6534 to 'safe' mode! Channels will have voltages\n"
<<<<<<< HEAD
	 << "                  and currents set to 0 and then powered off ...";
=======
<<<<<<< HEAD
	 << "                  and currents set to 0 and then powered off ...";
=======
	 << "                  and currents set to 0 and then powered off ... \n";
>>>>>>> 0c3534c16aff51243de90fd078851c9dd29b4a74
>>>>>>> a92bb70fcafab757772852484534da8265fcc860
  
  for(int ch=0; ch<NumChannels; ch++){
    SetVoltage(ch, 0);
    SetCurrent(ch, 0);
    SetPowerOff(ch);
  }
  
  if(Verbose)
    cout << " done!" << "\n" << endl;
  
  return 0;
}


bool ADAQHighVoltage::CheckChannelSteadyState(int Channel)
{
  // If the channel is not powered on then return false
  if(ChannelPowerState[Channel] == POWEROFF)
    return false;
  
  // If the channel is powered on then...
  else if(ChannelPowerState[Channel] == POWERON)
    
    // Compare the "active" voltage at the present moment to the "set"
    // voltage (value of voltage desired by the user and stored in the
    // ADAQHighVoltage member data). Return "true" if the "active"
    // voltage is within +/- 10 volts of the "set" voltage
    if(GetVoltage(Channel) < ChannelSetVoltage[Channel]+10 and
       GetVoltage(Channel) > ChannelSetVoltage[Channel]-10)
      return true;
    else
      return false;
}


int ADAQHighVoltage::PrintStatus()
{
  // Prevent printing the V6534 status is Verbose is not 'true'
  if(!Verbose)
    return 0;
  
  cout <<  "ADAQHighVoltage : V6534 board status:\n" << endl;
  
  // Iterate through each channel...
  for(int ch=0; ch<NumChannels; ch++){
    
    // ... getting the voltage, current, power state, and polarity
    uint16_t Voltage, PowerState, Polarity;
    GetVoltage(ch, &Voltage);
    GetPowerState(ch, &PowerState);
    GetPolarity(ch,&Polarity);

    // Set an appopriate +/- char for each channel
    char sign;
    (Polarity==0) ? sign='-' : sign='+';

    // Output each channel's parameters, being certain to convert the
    // V6534 input units for voltage and current ([V]*10 and [uA]*50)
    // into output units ([V] and [uA]) for the user's benefit
    cout << "     CH[" << ch << "] voltage : " << sign << dec << (Voltage/volts2input) << " V\n"
	 << "     CH[" << ch << "] current : " << dec <<(ChannelSetCurrent[ch]/microamps2input) << " uA\n";
    
    if(PowerState==POWEROFF)
      cout << "     CH[" << ch << "] power : OFF\n"
	   << "     CH[" << ch << "] advice : No worries!\n"
	   << endl;
    else if(PowerState==POWERON)
      cout << "     CH[" << ch << "] power :  ON\n"
	   << "     CH[" << ch << "] advice : Achtung! Hochspannung!\n"
	   << endl;
  }

  uint16_t BoardStatus;
  CAENComm_Read16(BoardHandle, STATUS, &BoardStatus);
  if(BoardStatus==0)
    cout << "     V6534 board status OK!\n" << endl;
  else
    cout << "     V6534 board status ERROR! Register(0x0058) = 0x" 
	 << setw(8) << setfill('0') << hex << BoardStatus << "\n"
	 << endl;
  return 0;
}


// Method to set individual channel's voltage
int ADAQHighVoltage::SetVoltage(int Channel, uint16_t VoltageSet)
{ 
  if(Channel>5 or Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error setting HV Voltage! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    ChannelSetVoltage[Channel] = VoltageSet;
    VoltageSet*=volts2input;
    return CAENComm_Write16(BoardHandle, VSET[Channel], VoltageSet);
  }
}


// Method to get individual channel's operating voltage
int ADAQHighVoltage::GetVoltage(int Channel, uint16_t *VoltageGet)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV Voltage! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    int ret = CAENComm_Read16(BoardHandle, VMON[Channel], VoltageGet);
    (*VoltageGet/=volts2input);
    return ret;
  }
}


// Method to get individual channel's operating voltage. 
//
// NOTE: ADAQ is migrating to member functions that *return* the "get"
//       value rather than setting the "get" value by reference. This
//       is mainly for modernity and compatability with Boost.Python
//       ZSH (18 MAY 12)
uint16_t ADAQHighVoltage::GetVoltage(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV Voltage! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t VoltageGet;
    CommandStatus = CAENComm_Read16(BoardHandle, VMON[Channel], &VoltageGet);
    VoltageGet /= volts2input;
    return VoltageGet;
  }
}


// Method to set individual channel's max current
int ADAQHighVoltage::SetCurrent(int Channel, uint16_t CurrentSet)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error setting HV current! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    ChannelSetCurrent[Channel] = CurrentSet;
    CurrentSet*=microamps2input;
    return CAENComm_Write16(BoardHandle, ISET[Channel], CurrentSet);
  }
}


// Method to get individual channel's drawn current
int ADAQHighVoltage::GetCurrent(int Channel, uint16_t *CurrentGet)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV current! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    int ret = CAENComm_Read16(BoardHandle, IMON[Channel], CurrentGet);
    (*CurrentGet)/=microamps2input;
    return ret;
  }
}


// NEW Method to get individual channel's drawn current
//
// NOTE: ADAQ is migrating to member functions that *return* the "get"
//       value rather than setting the "get" value by reference. This
//       is mainly for modernity and compatability with Boost.Python
//       ZSH (18 MAY 12)
uint16_t ADAQHighVoltage::GetCurrent(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV current! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t CurrentGet;
    CommandStatus = CAENComm_Read16(BoardHandle, IMON[Channel], &CurrentGet);
    CurrentGet/=microamps2input;
    return CurrentGet;
  }
}


// Method to set individual channel's power state to "ON"
int ADAQHighVoltage::SetPowerOn(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error setting HV power on! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    ChannelPowerState[Channel] = POWERON;
    return CAENComm_Write16(BoardHandle, PW[Channel], POWERON);
  }
}


// Method to set individual channel's power state to "OFF"
int ADAQHighVoltage::SetPowerOff(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error setting HV power off! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    ChannelPowerState[Channel] = POWEROFF;
    return CAENComm_Write16(BoardHandle, PW[Channel], POWEROFF);
  }
}


// Method to get individual channel's power state
int ADAQHighVoltage::GetPowerState(int Channel, uint16_t *powerGet)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV power status! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else
    return CAENComm_Read16(BoardHandle, PW[Channel], powerGet);
}
   
 
// NEW Method to get individual channel's power state
//
// NOTE: ADAQ is migrating to member functions that *return* the "get"
//       value rather than setting the "get" value by reference. This
//       is mainly for modernity and compatability with Boost.Python
//       ZSH (18 MAY 12)
uint16_t ADAQHighVoltage::GetPowerState(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV power status! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t PowerGet;
    CommandStatus = CAENComm_Read16(BoardHandle, PW[Channel], &PowerGet);
    return PowerGet;
  }
}


// Method to get individual channel's polarity (+ or -)
int ADAQHighVoltage::GetPolarity(int Channel, uint16_t *polarityGet)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV Channel polarity! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else
    return CAENComm_Read16(BoardHandle, POL[Channel], polarityGet);
}


// NEW Method to get individual channel's polarity (+ or -)
//
// NOTE: ADAQ is migrating to member functions that *return* the "get"
//       value rather than setting the "get" value by reference. This
//       is mainly for modernity and compatability with Boost.Python
//       ZSH (18 MAY 12)
uint16_t ADAQHighVoltage::GetPolarity(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV Channel polarity! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t PolarityGet;
    CommandStatus = CAENComm_Read16(BoardHandle, POL[Channel], &PolarityGet);
    return PolarityGet;
  }
}


// Method to get individual channel's operating temperature
int ADAQHighVoltage::GetTemperature(int Channel, uint16_t *temperatureGet)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV Channel temperature! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else
    return CAENComm_Read16(BoardHandle, TEMP[Channel], temperatureGet);
}


// Method to get individual channel's operating temperature
//
// NOTE: ADAQ is migrating to member functions that *return* the "get"
//       value rather than setting the "get" value by reference. This
//       is mainly for modernity and compatability with Boost.Python
//       ZSH (18 MAY 12)
uint16_t ADAQHighVoltage::GetTemperature(int Channel)
{
  if(Channel>5 || Channel<0){
    if(Verbose)
      cout << "ADAQHighVoltage : Error getting HV Channel temperature! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t TemperatureGet;
    CommandStatus = CAENComm_Read16(BoardHandle, TEMP[Channel], &TemperatureGet);
    return TemperatureGet;
  }
}


// Method to set a value to an individual register of the V6534
int ADAQHighVoltage::SetRegisterValue(uint32_t addr32, uint16_t val16)
{
  // Ensure that each register proposed for writing is a register that
  // is valid for user writing to prevent screwing up V6534 firmware
  if(CheckRegisterForWriting(addr32))
    return CAENComm_Write16(BoardHandle, addr32, val16); 
  else
    return -1;
}


// Method to get a value stored at an individual register of the V6534
int ADAQHighVoltage::GetRegisterValue(uint32_t addr32, uint16_t *val16)
{ return CAENComm_Read16(BoardHandle, addr32, val16); }


// Method to get a value stored at an individual register of the V6534
//
// NOTE: ADAQ is migrating to member functions that *return* the "get"
//       value rather than setting the "get" value by reference. This
//       is mainly for modernity and compatability with Boost.Python
//       ZSH (18 MAY 12)
uint16_t ADAQHighVoltage::GetRegisterValue(uint32_t addr32)
{
  uint16_t data16;
  CommandStatus = CAENComm_Read16(BoardHandle, addr32, &data16);
  return data16;
}


// Method to check validity of V6534 register for writing
bool ADAQHighVoltage::CheckRegisterForWriting(uint32_t addr32)
{
  // Check to ensure that the V6534 reserved registers are not
  // accidentally overwritten. Return 'true' if proposed write address
  // (addr32) refers to an acceptable register for user writing; else
  // return 'false'

  if((addr32 <= 0x004c) or
     (addr32 >= 0x0060 and addr32 <=0x007c) or
     (addr32 >= 0x00b4 and addr32 <=0x00fc) or
     (addr32 >= 0x0134 and addr32 <=0x017c) or
     (addr32 >= 0x01b4 and addr32 <=0x01fc) or
     (addr32 >= 0x0234 and addr32 <=0x017c) or
     (addr32 >= 0x01b4 and addr32 <=0x02fc) or
     (addr32 >= 0x0334 and addr32 <=0x037c)){
    if(Verbose)
      cout << "ADAQHighVoltage : Error writing value to protected address!\n"
	   << endl;
    return false;
  }
  else
    return true;
}
