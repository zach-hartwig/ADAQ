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
// name: ADAQHighVoltage.cc
// date: 21 Jul 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQHighVoltage is a derived class that is intended to
//       provide full control a CAEN VME high voltage
//       units. Functionality includes connection, individual register
//       read/write, programming, and high level voltage/current
//       supply methods. This class inherits all the general member
//       data and methods contained in ADAQVBoard.
//
//       CAEN did not provide any API for their VME high voltage
//       boards (at least at the time this code began to be developed
//       in 2012...); thus, one had to be written and this is the
//       result. The methods provide fairly straightforward control of
//       what are typically relatively simple boards, as well as some
//       safety features since we are dealing with high voltage after
//       all. The class is intended to be completely adaptaptable to
//       any VME high voltage board (see the ZBoardTypes enumerator in
//       ADAQVBoard for presently supported types).
//
//       Presently supported CAEN high voltage units:
//
//       - VME V6533{Mixed, Negative, Positive} : 4 kV, 3 mA, 6 ch
//       - VME V6534{Mixed, Negative, Positive} : 6 kV, 1 mA, 6 ch
//       - Desktop DT5790{Mixed, Negative, Positive} : 4kV, 3mA, 2 ch
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
#include "CAENDigitizer.h"
}

// ADAQ
#include "ADAQHighVoltage.hh"
#include "ADAQHighVoltageRegisters.hh"


ADAQHighVoltage::ADAQHighVoltage(ZBoardType Type,  // ADAQ-specific device type identifier
				 int ID,           // ADAQ-specific user-specified device ID
				 uint32_t Address, // 8 hex digitizer VME base address
				 int BN,           // USB link number
				 int CN)           // CONET node ID

  : ADAQVBoard(Type, ID, Address, BN, CN),
    
    // The desired channel voltage (in volts) must be set in the
    // registers as (V[volts]*10). Here define a conversion value.
    volts2input(10),

    // The desired maximum channel voltage (in volts) must be set in
    // the regsisters as (V[volts]/20). Here define a conversion value.
    maxVolts2input(1./20),
    
    // The desired channel current (in microamps) must be set in the
    // registers as (I[microamps]*50). Here define a conversion value.
    microamps2input(50)
{
  // Configure member data based on HV unit type
  ConfigureVariables();
  
  // Configure register mapping based on HV unit type
  MapRegisters();
}


ADAQHighVoltage::~ADAQHighVoltage()
{;}


void ADAQHighVoltage::ConfigureVariables()
{
  if(BoardType == zV6533M or BoardType == zV6533N or BoardType == zV6533P){
    NumChannels = 6;
    MinChannel = 0;
    MaxChannel = NumChannels-1;
    MaxVoltage = 4000; // [V]
    MaxCurrent = 3000; // [uA]
    IsV653X = true;
    IsDT5790 = false;

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
      
    default:
      break;
    }
  }
  
  else if(BoardType == zV6534M or BoardType == zV6534N or BoardType == zV6534P){
    NumChannels = 6;
    MinChannel = 0;
    MaxChannel = NumChannels-1;
    MaxVoltage = 6000; // [V]
    MaxCurrent = 1000; // [uA]
    IsV653X = true;
    IsDT5790 = false;
    
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

    default:
      break;
    }
  }
  else if(BoardType == zDT5790M or BoardType == zDT5790N or BoardType == zDT5790P){
    NumChannels = 2;
    MinChannel = 0;
    MaxChannel = NumChannels-1;
    MaxVoltage = 4000; // [V]
    MaxCurrent = 3000; // [uA]
    IsV653X = false;
    IsDT5790 = true;
    
    switch(BoardType){
    case zDT5790M:
      ChannelPolarity += 1, -1;
      ChannelPolarityString += "+", "-";
      break;
      
    case zDT5790N:
      ChannelPolarity += -1, -1;
      ChannelPolarityString += "-", "-";
      break;
      
    case zDT5790P:
      ChannelPolarity += 1, 1;
      ChannelPolarityString += "+", "+";
      break;

    default:
      break;
    }
  }
  
  else{
    cout << "ADAQHighVoltage [" << BoardID << "] : Error! Unrecognized board type '" << BoardType 
	 << "'!\n" << endl;
  }

  // Provide std::map to convert ZBoardType to string name 

  insert(TypeToName)
    ((int)zV6533M,"V6533M") ((int)zV6533N,"V6533N") ((int)zV6533P,"V6533P")
    ((int)zV6534M,"V6534M") ((int)zV6534N,"V6534N") ((int)zV6534P,"V6534P")
    ((int)zDT5790M,"DT5790M") ((int)zDT5790N,"DT5790N") ((int)zDT5790P,"DT5790P");
  
  // Initialize the channel settings for the voltage, current, and
  // power status to zero/off for all channels

  for(int ch=0; ch<NumChannels; ch++){
    ChannelSetVoltage.push_back(0x0000);
    ChannelSetCurrent.push_back(0x0000);
    ChannelPowerState.push_back(0x0000);
  }
}


void ADAQHighVoltage::MapRegisters()
{
  if(IsV653X){
    
    FirmRel = V653X::FIRMREL;
    
    for(int ch=0; ch<NumChannels; ch++){
      VMax.push_back(V653X::VMAX);
      IMax.push_back(V653X::IMAX);
      Status.push_back(V653X::STATUS);
      VSet.push_back(V653X::VSET[ch]);
      ISet.push_back(V653X::ISET[ch]);
      VMon.push_back(V653X::VMON[ch]);
      IMon.push_back(V653X::IMON[ch]);
      Pw.push_back(V653X::PW[ch]);
      Pol.push_back(V653X::POL[ch]);
      Temp.push_back(V653X::TEMP[ch]);
    }
    PowerOn = V653X::POWERON;
    PowerOff = V653X::POWEROFF;
  }
  else if(IsDT5790){
    
    FirmRel = -1; // Unavailable for DT5790 unit
    
    for(int ch=0; ch<NumChannels; ch++){
      VMax.push_back(DT5790::VMAX[ch]);
      IMax.push_back(DT5790::IMAX[ch]);
      Status.push_back(DT5790::STATUS[ch]);
      
      VSet.push_back(DT5790::VSET[ch]);
      ISet.push_back(DT5790::ISET[ch]);
      VMon.push_back(DT5790::VMON[ch]);
      IMon.push_back(DT5790::IMON[ch]);
      Pw.push_back(DT5790::PW[ch]);
      Pol.push_back(DT5790::POL[ch]);
      Temp.push_back(DT5790::TEMP[ch]);
    }
    PowerOn = DT5790::POWERON;
    PowerOff = DT5790::POWEROFF;
  }
}


int ADAQHighVoltage::OpenLink()
{
  CommandStatus = -42;
  
  if(LinkEstablished){
    if(Verbose)
      cout << "ADAQHighVoltage: Error opening link! Link is already open!"
	   << endl;
  }
  else{
    if(IsV653X)
      CommandStatus = CAENComm_OpenDevice(CAENComm_USB,
					  BoardLinkNumber,
					  BoardCONETNode,
					  BoardAddress, 
					  &BoardHandle);
    else if(IsDT5790){
      CommandStatus = CAENComm_Success;
    }
  }
  
  if(CommandStatus == CAENComm_Success){
    
    LinkEstablished = true;
    
    // For the DT5790X units, set each channels' maximum voltage to
    // the highest value to correct unpredictable default settings
    if(BoardType == zDT5790M or BoardType == zDT5790N or BoardType == zDT5790P){
      for(int ch=0; ch<NumChannels; ch++)
	SetMaxVoltage(ch, MaxVoltage);
    }
    
    if(Verbose){
      cout << "ADAQHighVoltage[" << BoardID << "] : Link successfully established!\n"
	   << "--> Type     : " << TypeToName[BoardType] << "\n"
	   << "--> Channels : " << NumChannels << "\n"
	   << "--> VMax     : " << MaxVoltage << " V\n"
	   << "--> IMax     : " << MaxCurrent << " uA\n"
	   << "--> Polarity : [";
      
      for(int ch=0; ch<NumChannels; ch++){
	cout << ChannelPolarityString[ch] << flush;
	if(ch < (NumChannels-1))
	  cout << "," << flush;
	else
	  cout << "]" << endl;
      }
      
      cout << "--> Address  : 0x" << setw(8) << setfill('0') << hex << BoardAddress << "\n"
	   << "--> User ID  : " << dec << BoardID << "\n"
	   << "--> Handle   : " << BoardHandle << flush;
      
      if(IsDT5790)
	cout << " (must be identical to ADAQDigitizer handle!)\n";
      else
	cout << "\n";
      
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
{ return 0; }


int ADAQHighVoltage::SetRegisterValue(uint32_t Addr32, uint16_t Data16)
{
  CommandStatus = -42;
  
  if(CheckRegisterForWriting(Addr32))
    CommandStatus =  CAENComm_Write16(BoardHandle, Addr32, Data16);
  
  return CommandStatus;
}


int ADAQHighVoltage::GetRegisterValue(uint32_t Addr32, uint16_t *Data16)
{
  CommandStatus = -42;
  
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

  if(IsV653X){
    if((Addr32 <= 0x004c) or
       (Addr32 >= 0x0060 and Addr32 <=0x007c) or
       (Addr32 >= 0x00b4 and Addr32 <=0x00fc) or
       (Addr32 >= 0x0134 and Addr32 <=0x017c) or
       (Addr32 >= 0x01b4 and Addr32 <=0x01fc) or
       (Addr32 >= 0x0234 and Addr32 <=0x017c) or
       (Addr32 >= 0x01b4 and Addr32 <=0x02fc) or
       (Addr32 >= 0x0334 and Addr32 <=0x037c)){
      if(Verbose)
	cout << "ADAQHighVoltage [" << BoardID << "] : Error! Attempted access to a protected register address.\n"
	     << endl;
      return false;
    }
    else
      return true;
  }
  else if(IsDT5790){
    if((Addr32 <= 0x121f) or
       (Addr32 >= 0x1245 and Addr32 <= 0x131f) or
       (Addr32 >= 0x1345)){
      if(Verbose)
	cout << "ADAQHighVoltage [" << BoardID << "] : Error! Attempted access to a protected register address.\n"
	     << endl;
      return false;
    }
    else
      return true;
  }
  else
    return false;
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
  if(ChannelPowerState[Channel] == PowerOff)
    return false;
  
  // If the channel is powered on then...
  else if(ChannelPowerState[Channel] == PowerOn){
    
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
    
    if(PowerState == PowerOff)
      cout << "     CH[" << ch << "] power : OFF\n"
	   << "     CH[" << ch << "] advice : No worries!\n"
	   << endl;
    else if(PowerState == PowerOn)
      cout << "     CH[" << ch << "] power :  ON\n"
	   << "     CH[" << ch << "] advice : Achtung! Hochspannung!\n"
	   << endl;

    uint16_t BoardStatus;
    CAENComm_Read16(BoardHandle, Status[ch], &BoardStatus);
    
    if(BoardType == zV6533M or BoardType == zV6533N or BoardType == zV6533P or
       BoardType == zV6534M or BoardType == zV6534N or BoardType == zV6534P){
      
      if(BoardStatus == 0)
	cout << "     Board status OK!\n" << endl;
      else
	cout << "     Board status ERROR!\n" << endl;
    }
    
    else if(BoardType == zDT5790M or BoardType == zDT5790N or BoardType == zDT5790P){
      
      if(BoardStatus == 0)
	cout << "     Channel[" << ch << "] status OK!\n" << endl;
      else
	cout << "     Channel[" << ch << "] status ERROR!\n" << endl;
    }
  }
  
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
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting voltage! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    ChannelSetVoltage[Channel] = VoltageSet;
    VoltageSet*=volts2input;
    CommandStatus = CAENComm_Write16(BoardHandle, VSet[Channel], VoltageSet);
  }
  return CommandStatus;
}


// Method to get individual channel operating voltage
int ADAQHighVoltage::GetVoltage(int Channel, uint16_t *VoltageGet)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting voltage! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    CommandStatus = CAENComm_Read16(BoardHandle, VMon[Channel], VoltageGet);
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
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting voltage! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;

    return -1;
  }
  else{
    uint16_t VoltageGet;
    CommandStatus = CAENComm_Read16(BoardHandle, VMon[Channel], &VoltageGet);
    VoltageGet /= volts2input;
    return VoltageGet;
  }
}


// Method to set the maximum allowable channel voltage
int ADAQHighVoltage::SetMaxVoltage(int Channel, uint16_t MaxVoltageSet)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting maximum voltage! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    MaxVoltageSet *= maxVolts2input;
    CommandStatus = CAENComm_Write16(BoardHandle, VMax[Channel], MaxVoltageSet);
  }
  return CommandStatus;
}


// Method to get the maximum allowable channel voltage
int ADAQHighVoltage::GetMaxVoltage(int Channel, uint16_t *MaxVoltageGet)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting maximum voltage! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    CommandStatus = CAENComm_Read16(BoardHandle, VMax[Channel], MaxVoltageGet);
    cout << VMax[Channel] << endl;
    (*MaxVoltageGet/=maxVolts2input);
  }
  return CommandStatus;
}


// Method to get the maximum allowable channel voltage. 
uint16_t ADAQHighVoltage::GetMaxVoltage(int Channel)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting maximum voltage! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    
    return -1;
  }
  else{
    uint16_t MaxVoltageGet = -1;
    CommandStatus = CAENComm_Read16(BoardHandle, VMax[Channel], &MaxVoltageGet);
    MaxVoltageGet /= maxVolts2input;
    return MaxVoltageGet;
  }
}


// Method to set individual channel max current
int ADAQHighVoltage::SetCurrent(int Channel, uint16_t CurrentSet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting current! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    ChannelSetCurrent[Channel] = CurrentSet;
    CurrentSet*=microamps2input;
    CommandStatus = CAENComm_Write16(BoardHandle, ISet[Channel], CurrentSet);
  }
  return CommandStatus;
}


// Method to get individual channel drawn current
int ADAQHighVoltage::GetCurrent(int Channel, uint16_t *CurrentGet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting current! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    CommandStatus = CAENComm_Read16(BoardHandle, IMon[Channel], CurrentGet);
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
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting current! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;

    return -1;
  }
  else{
    uint16_t CurrentGet;
    CommandStatus = CAENComm_Read16(BoardHandle, IMon[Channel], &CurrentGet);
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
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting power on! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    return -1;
  }
  else{
    ChannelPowerState[Channel] = PowerOn;
    CommandStatus = CAENComm_Write16(BoardHandle, Pw[Channel], PowerOn);
  }
  return CommandStatus;
}


// Method to set individual channel power state to "OFF"
int ADAQHighVoltage::SetPowerOff(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error setting power off! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else{
    ChannelPowerState[Channel] = PowerOff;
    CommandStatus = CAENComm_Write16(BoardHandle, Pw[Channel], PowerOff);
  }
  return CommandStatus;
}


// Method to get individual channel power state
int ADAQHighVoltage::GetPowerState(int Channel, uint16_t *powerGet)
{
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting power status! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
  }
  else
    CommandStatus = CAENComm_Read16(BoardHandle, Pw[Channel], powerGet);
  
  return CommandStatus;
}
   
 
// Method to get individual channel power state
uint16_t ADAQHighVoltage::GetPowerState(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting power status! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;

    return -1;
  }
  else{
    uint16_t PowerGet;
    CommandStatus = CAENComm_Read16(BoardHandle, Pw[Channel], &PowerGet);
    return PowerGet;
  }
}


// Method to get individual channel polarity (+ or -)
int ADAQHighVoltage::GetPolarity(int Channel, uint16_t *polarityGet)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting channel polarity! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    return -1;
  }
  else
    CommandStatus = CAENComm_Read16(BoardHandle, Pol[Channel], polarityGet);
  
  return CommandStatus;
}


// Method to get individual channel polarity (+ or -)
uint16_t ADAQHighVoltage::GetPolarity(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting channel polarity! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t PolarityGet;
    CommandStatus = CAENComm_Read16(BoardHandle, Pol[Channel], &PolarityGet);
    return PolarityGet;
  }
}


// Method to get individual channel polarity string ("+" or "-")
string ADAQHighVoltage::GetPolarityString(int Channel)
{ 
  CommandStatus = -42;
  
  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting channel polarity string! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    return "";
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
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting channel temperature! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    return -1;
  }
  else
    CommandStatus = CAENComm_Read16(BoardHandle, Temp[Channel], temperatureGet);
  
  return CommandStatus;
}


// Method to get individual channel operating temperature
uint16_t ADAQHighVoltage::GetTemperature(int Channel)
{
  CommandStatus = -42;

  if(Channel>MaxChannel or Channel<MinChannel){
    if(Verbose)
      cout << "ADAQHighVoltage [" << BoardID << "] : Error getting channel temperature! Channel out of range (0 <= ch <= " << NumChannels << ")\n"
	   << endl;
    return -1;
  }
  else{
    uint16_t TemperatureGet;
    CommandStatus = CAENComm_Read16(BoardHandle, Temp[Channel], &TemperatureGet);
    return TemperatureGet;
  }
}
