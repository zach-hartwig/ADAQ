///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQHighVoltage.hh
// date: 06 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQHighVoltage_hh__
#define __ADAQHighVoltage_hh__ 1

// C++
#include <vector>
using namespace std;

// Boost 
#include <boost/cstdint.hpp>

// ADAQ
#include "ADAQVBoard.hh"


class ADAQHighVoltage : public ADAQVBoard
{

public:
  ADAQHighVoltage(ZBoardType, int, uint32_t);
  ~ADAQHighVoltage();

  /////////////////////////////////////////////////
  // Mandatory implementation of virtual methods //
  /////////////////////////////////////////////////
  
  int OpenLink();
  int CloseLink();  
  int Initialize();
  int SetRegisterValue(uint32_t, uint16_t);
  int GetRegisterValue(uint32_t, uint16_t *);
  bool CheckRegisterForWriting(uint32_t);

  
  ///////////////////////////////////////////
  // Enhanced high voltage control methods //
  ///////////////////////////////////////////

  // Voltages/currents to 0; all channel's power to off
  int SetToSafeState();

  // Compares "set" voltage to "active" voltage for specified channel
  bool CheckChannelSteadyState(int);

  // Print present status of all channels to stdout
  int PrintStatus();

  // Set/get individual channel voltages
  int SetVoltage(int, uint16_t);
  int GetVoltage(int, uint16_t *);
  uint16_t GetVoltage(int);
  
  // Set/get individual channel max. currents
  int SetCurrent(int, uint16_t);
  int GetCurrent(int, uint16_t *);
  uint16_t GetCurrent(int);

  // Set individual channels on/off; get individual channel status
  int SetPowerOn(int);
  int SetPowerOff(int);
  int GetPowerState(int, uint16_t *);
  uint16_t GetPowerState(int);
  
  // Get individual channel polarity (+ or -)
  int GetPolarity(int, uint16_t *);
  uint16_t GetPolarity(int);

  // Get individual channel temperature
  int GetTemperature(int, uint16_t *);
  uint16_t GetTemperature(int);


  /////////////////////////////////////////
  // Get methods for private member data //
  /////////////////////////////////////////
  
  int GetNumChannels() {return NumChannels;}
  int GetMaxVoltage() {return MaxVoltage;}
  int GetMaxCurrent() {return MaxCurrent;}
  
  // HV channel IDs
  enum {Channel0, Channel1, Channel2, Channel3, Channel4, Channel5};

private:
  int NumChannels, MinChannel, MaxChannel;
  int MaxVoltage, MaxCurrent;

  const int volts2input, microamps2input;

  vector<uint16_t> ChannelSetVoltage; // [V]
  vector<uint16_t> ChannelSetCurrent; // [uA]
  vector<uint16_t> ChannelPowerState;
  vector<uint16_t> ChannelPolarity;
};

#endif
