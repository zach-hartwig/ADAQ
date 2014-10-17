///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQHighVoltage.cc
// date: 17 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQHighVoltage is a derived class that is intended to
//       provide full control a CAEN VME high voltage board, including
//       VME connection, register read/write, programming, and high
//       level voltage/current supply methods. This class inherits all
//       the general member data and methods contained in ADAQVBoard.
//
//       CAEN did provide any API for their VME high voltage boards
//       (at least at the time this code began to be developed in
//       2012...); thus, one had to be written and this is the
//       result. The methods provide fairly straightforward control of
//       what are typically relatively simple boards, as well as some
//       safety features since we are dealing with high voltage after
//       all. The class is intended to be completely adaptaptable to
//       any VME high voltage board (see the ZBoardTypes enumerator in
//       ADAQVBoard for presently supported types).
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
#include "ADAQHighVoltageRegisters.hh"
using namespace V6534Registers;


ADAQHighVoltage::ADAQHighVoltage(ZBoardType Type, int ID, uint32_t Address)
  : ADAQVBoard(Type, ID, Address),
    NumChannels(0), MaxVoltage(0), MaxCurrent(0),
    
    // The desired channel voltage (in volts) must be set in the
    // registers as (V[volts]*10). Here define a conversion value.
    volts2input(10),
    
    // The desired channel current (in microamps) must be set in the
    // registers as (I[microamps]*50). Here define a conversion value.
    microamps2input(50)
{
  if(BoardType == zV6533M or BoardType == zV6533N or BoardType == zV6533P){
    NumChannels = 6;
    MinChannel = 0;
    MaxChannel = 5;
    MaxVoltage = 4000; // [V]
    MaxCurrent = 3000; // [uA]

    switch(BoardType){
    case zV6533M:
      ChannelPolarity += -1, -1, -1, 1, 1, 1;
      ChannelPolarityString += "-", "-", "-", "+", "+", "+";
      break;
      
    case zV6533N:
      ChannelPolarity += -1, -1, -1, -1, -1, -1;
      ChannelPolarityString += "-", "-", "-", "-", "-", "-";
      break;

    case zV6533P:
      ChannelPolarity += 1, 1, 1, 1, 1, 1;
      ChannelPolarityString += "+", "+", "+", "+", "+", "+";
      break;
    }
  }
  
  else if(BoardType == zV6534M or BoardType == zV6534N or BoardType == zV6534P){
    NumChannels = 6;
    MinChannel = 0;
    MaxChannel = 5;
    MaxVoltage = 6000; // [V]
    MaxCurrent = 1000; // [uA]
    
    switch(BoardType){
    case zV6534M:
      ChannelPolarity += -1, -1, -1, 1, 1, 1;
      ChannelPolarityString += "-", "-", "-", "+", "+", "+";
      break;

    case zV6534N:
      ChannelPolarity += -1, -1, -1, -1, -1, -1;
      ChannelPolarityString += "-", "-", "-", "-", "-", "-";
      break;

    case zV6534P:
      ChannelPolarity += 1, 1, 1, 1, 1, 1;
      ChannelPolarityString += "+", "+", "+", "+", "+", "+";
      break;
    }
  }
  else{
    cout << "ADAQHighVoltage [" << BoardID << "] : Error! Unrecognized board type '" << BoardType 
	 << "'!\n" << endl;
  }
  
  // Set the voltage, current, and power status for all HV
  // channels. By default, all voltage/current is initialized to a
  // value of zero and the channels are all turned off
  for(int ch=0; ch<NumChannels; ch++){
    ChannelSetVoltage.push_back(0);
    ChannelSetCurrent.push_back(0);
    ChannelPowerState.push_back(POWEROFF);
  }

  // Provide std::map to convert ZBoardType to string name
  insert(TypeToName)
    ((int)zV6533M,"V6533M") ((int)zV6533N,"V6533N") ((int)zV6533P,"V6533P")
    ((int)zV6534M,"V6534M") ((int)zV6534N,"V6534N") ((int)zV6534P,"V6534P");
}


ADAQHighVoltage::~ADAQHighVoltage()
{;}


int ADAQHighVoltage::OpenLink()
{
  CommandStatus = -42;
 
  if(LinkEstablished){
    if(Verbose)
      cout << "ADAQHighVoltage: Error opening link! Link is already open!"
	   << endl;
  }
  else{
    CommandStatus = CAENComm_OpenDevice(CAENComm_USB, 
					0, 
					0, 
					BoardAddress, 
					&BoardHandle);
  }
  
  if(CommandStatus == CAENComm_Success){

    LinkEstablished = true;
    
    if(Verbose){
      cout << "ADAQHighVoltage[" << BoardID << "] : Link successfully established!\n"
	   << "--> Type      : " << TypeToName[BoardType] << "\n"
	   << "--> Address   : 0x" << setw(8) << setfill('0') << hex << BoardAddress << "\n"
	   << "--> User ID   : " << dec << BoardID << "\n"
	   << "--> Handle    : " << BoardHandle << "\n"
	   << "--> Channels  : " << NumChannels << "\n"
	   << "--> VMax [V]  : " << MaxVoltage << "\n"
	   << "--> IMax [uA] : " << MaxCurrent << "\n"
	   << "--> Polarity  : [";
      
      for(int ch=0; ch<NumChannels; ch++){
	cout << ChannelPolarityString[ch] << flush;
	if(ch < 5)
	  cout << "," << flush;
	else
	  cout << "]" << endl;
      }
      cout << endl;
    }
  }
  else
    if(Verbose and !LinkEstablished)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error opening link! Error code: " << CommandStatus << "\n"
	   << endl;
  
  return CommandStatus;
}


int ADAQHighVoltage::CloseLink()
{
  CommandStatus = -42;

  if(LinkEstablished)
    CommandStatus = CAENComm_CloseDevice(BoardHandle);
  else
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error closing link! Link is already closed!\n"
	   << endl;
  
  if(CommandStatus == CAENComm_Success){

    LinkEstablished = false;

    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Link successfully closed!\n"
	   << endl;
  }
  else
    if(Verbose and LinkEstablished)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error closing link! Error code: " << CommandStatus << "\n"
	   << endl;
  
  return CommandStatus;
}


int ADAQHighVoltage::Initialize()
{;}


int ADAQHighVoltage::SetRegisterValue(uint32_t Addr32, uint16_t Data16)
{
  CommandStatus = -42;
  
  // Ensure the user-specified register is OK to be written to
  // (i.e. prevent overwriting critical registers)
  if(CheckRegisterForWriting(Addr32))
    CommandStatus =  CAENComm_Write16(BoardHandle, Addr32, Data16); 
  
  return CommandStatus;
}


int ADAQHighVoltage::GetRegisterValue(uint32_t Addr32, uint16_t *Data16)
{ 
  CommandStatus = CAENComm_Read16(BoardHandle, Addr32, Data16); 
  return CommandStatus;
}


bool ADAQHighVoltage::CheckRegisterForWriting(uint32_t Addr32)
{
  // The following conditional checks to ensure that critical
  // registers are not overwritten by the user. At present, these
  // registers were taken from the V6534 specificiations. Return
  // 'true' if proposed write address (Addr32) refers to an acceptable
  // register for user writing; else return 'false'

  if((Addr32 <= 0x004c) or
     (Addr32 >= 0x0060 and Addr32 <=0x007c) or
     (Addr32 >= 0x00b4 and Addr32 <=0x00fc) or
     (Addr32 >= 0x0134 and Addr32 <=0x017c) or
     (Addr32 >= 0x01b4 and Addr32 <=0x01fc) or
     (Addr32 >= 0x0234 and Addr32 <=0x017c) or
     (Addr32 >= 0x01b4 and Addr32 <=0x02fc) or
     (Addr32 >= 0x0334 and Addr32 <=0x037c)){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error attempting to access a protected register address\n"
	   << endl;
    return false;
  }
  else
    return true;
}


int ADAQHighVoltage::SetToSafeState()
{
  if(Verbose)
    cout << "ADAQHighVoltage [" << BoardID << "] : Setting the HV board to 'safe' mode! All channels will have\n"
	 << "                      voltages and currents set to 0 and then powered off ...";
  
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
    // voltage is within +/- 5 volts of the "set" voltage
    if(GetVoltage(Channel) < ChannelSetVoltage[Channel]+5 and
       GetVoltage(Channel) > ChannelSetVoltage[Channel]-5)
      return true;
    else
      return false;
}


int ADAQHighVoltage::PrintStatus()
{
  // Prevent printing the V6534 status is Verbose is not 'true'
  if(!Verbose)
    return 0;
  
  cout <<  "ADAQHighVoltage [" << BoardID << "] : High voltage board status:\n" << endl;
  
  // Iterate through each channel...
  for(int ch=0; ch<NumChannels; ch++){
    
    // ... getting the voltage, current, power state, and polarity
    uint16_t Voltage, PowerState, Polarity;
    GetVoltage(ch, &Voltage);
    GetPowerState(ch, &PowerState);
    GetPolarity(ch,&Polarity);

    // Set an appopriate +/- char for each channel
    char sign;
    (Polarity == 0) ? sign='-' : sign='+';

    // Output each channel's parameters, being certain to convert the
    // V6534 input units for voltage and current ([V]*10 and [uA]*50)
    // into output units ([V] and [uA]) for the user's benefit
    cout << "     CH[" << ch << "] voltage : " << sign << dec << (Voltage/volts2input) << " V\n"
	 << "     CH[" << ch << "] current : " << dec <<(ChannelSetCurrent[ch]/microamps2input) << " uA\n";
    
    if(PowerState == POWEROFF)
      cout << "     CH[" << ch << "] power : OFF\n"
	   << "     CH[" << ch << "] advice : No worries!\n"
	   << endl;
    else if(PowerState == POWERON)
      cout << "     CH[" << ch << "] power :  ON\n"
	   << "     CH[" << ch << "] advice : Achtung! Hochspannung!\n"
	   << endl;
  }

  uint16_t BoardStatus;
  CAENComm_Read16(BoardHandle, STATUS, &BoardStatus);
  if(BoardStatus==0)
    cout << "     Board status OK!\n" << endl;
  else
    cout << "     Board status ERROR! Register(0x0058) = 0x" 
	 << setw(8) << setfill('0') << hex << BoardStatus << "\n"
	 << endl;
  return 0;
}


/////////////////////
// Set/get methods //
/////////////////////

// Method to set individual channel voltage
int ADAQHighVoltage::SetVoltage(int Channel, uint16_t VoltageSet)
{
  CommandStatus = -42;
 
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting HV Voltage! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    ChannelSetVoltage[Channel] = VoltageSet;
    VoltageSet*=volts2input;
    CommandStatus = CAENComm_Write16(BoardHandle, VSET[Channel], VoltageSet);
  }
  return CommandStatus;
}


// Method to get individual channel operating voltage
int ADAQHighVoltage::GetVoltage(int Channel, uint16_t *VoltageGet)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV Voltage! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    CommandStatus = CAENComm_Read16(BoardHandle, VMON[Channel], VoltageGet);
    (*VoltageGet/=volts2input);
  }
  return CommandStatus;
}


// Method to get individual channel operating voltage. 
uint16_t ADAQHighVoltage::GetVoltage(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV Voltage! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    uint16_t VoltageGet;
    CommandStatus = CAENComm_Read16(BoardHandle, VMON[Channel], &VoltageGet);
    VoltageGet /= volts2input;
    return VoltageGet;
  }
}


// Method to set individual channel max current
int ADAQHighVoltage::SetCurrent(int Channel, uint16_t CurrentSet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting HV current! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    ChannelSetCurrent[Channel] = CurrentSet;
    CurrentSet*=microamps2input;
    CommandStatus = CAENComm_Write16(BoardHandle, ISET[Channel], CurrentSet);
  }
  return CommandStatus;
}


// Method to get individual channel drawn current
int ADAQHighVoltage::GetCurrent(int Channel, uint16_t *CurrentGet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV current! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    CommandStatus = CAENComm_Read16(BoardHandle, IMON[Channel], CurrentGet);
    (*CurrentGet)/=microamps2input;
  }
  return CommandStatus;
}


// Method to get individual channel drawn current
uint16_t ADAQHighVoltage::GetCurrent(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV current! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    uint16_t CurrentGet;
    CommandStatus = CAENComm_Read16(BoardHandle, IMON[Channel], &CurrentGet);
    CurrentGet/=microamps2input;
    return CurrentGet;
  }
}


// Method to set individual channel power state to "ON"
int ADAQHighVoltage::SetPowerOn(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting HV power on! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
    return -1;
  }
  else{
    ChannelPowerState[Channel] = POWERON;
    CommandStatus = CAENComm_Write16(BoardHandle, PW[Channel], POWERON);
  }
  return CommandStatus;
}


// Method to set individual channel power state to "OFF"
int ADAQHighVoltage::SetPowerOff(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting HV power off! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    ChannelPowerState[Channel] = POWEROFF;
    CommandStatus = CAENComm_Write16(BoardHandle, PW[Channel], POWEROFF);
  }
  return CommandStatus;
}


// Method to get individual channel power state
int ADAQHighVoltage::GetPowerState(int Channel, uint16_t *powerGet)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV power status! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else
    CommandStatus = CAENComm_Read16(BoardHandle, PW[Channel], powerGet);
  
  return CommandStatus;
}
   
 
// Method to get individual channel power state
uint16_t ADAQHighVoltage::GetPowerState(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV power status! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    uint16_t PowerGet;
    CommandStatus = CAENComm_Read16(BoardHandle, PW[Channel], &PowerGet);
    return PowerGet;
  }
}


// Method to get individual channel polarity (+ or -)
int ADAQHighVoltage::GetPolarity(int Channel, uint16_t *polarityGet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV channel polarity! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else
    CommandStatus = CAENComm_Read16(BoardHandle, POL[Channel], polarityGet);
  
  return CommandStatus;
}


// Method to get individual channel polarity (+ or -)
uint16_t ADAQHighVoltage::GetPolarity(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV channel polarity! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    uint16_t PolarityGet;
    CommandStatus = CAENComm_Read16(BoardHandle, POL[Channel], &PolarityGet);
    return PolarityGet;
  }
}


// Method to get individual channel polarity string ("+" or "-")
string ADAQHighVoltage::GetPolarityString(int Channel)
{ 
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV channel polarity string! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    CommandStatus = 0;
    return ChannelPolarityString[Channel];
  }
}


// Method to get individual channel operating temperature
int ADAQHighVoltage::GetTemperature(int Channel, uint16_t *temperatureGet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV channel temperature! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else
    CommandStatus = CAENComm_Read16(BoardHandle, TEMP[Channel], temperatureGet);
  
  return CommandStatus;
}


// Method to get individual channel operating temperature
uint16_t ADAQHighVoltage::GetTemperature(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting HV channel temperature! Channel out of range (0 <= ch <= 5)\n"
	   << endl;
  }
  else{
    uint16_t TemperatureGet;
    CommandStatus = CAENComm_Read16(BoardHandle, TEMP[Channel], &TemperatureGet);
    return TemperatureGet;
  }
}
