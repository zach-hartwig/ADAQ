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
// name: ADAQHighVoltage.hh
// date: 17 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQHighVoltage is a derived class that is intended to
//       provide full control of CAEN VME high voltage boards,
//       including VME connection, register read/write, programming,
//       and high level voltage/current supply methods. This class
//       inherits all the general member data and methods contained in
//       ADAQVBoard.
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

#ifndef __ADAQHighVoltage_hh__
#define __ADAQHighVoltage_hh__ 1

// C++
#include <vector>
#include <map>
#include <string>
using namespace std;

// Boost 
#include <boost/cstdint.hpp>

// ADAQ
#include "ADAQVBoard.hh"


class ADAQHighVoltage : public ADAQVBoard
{
  
public:
  ADAQHighVoltage(ZBoardType, int, uint32_t, int, int = 0);
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

  string GetPolarityString(int);
  int SetPolarityString(int, string);

  // Get individual channel temperature
  int GetTemperature(int, uint16_t *);
  uint16_t GetTemperature(int);


  /////////////////////////////////////////
  // Get methods for private member data //
  /////////////////////////////////////////
  
  int GetNumChannels() {return NumChannels;}
  int GetMaxVoltage() {return MaxVoltage;}
  int GetMaxCurrent() {return MaxCurrent;}
  
private:
  int NumChannels, MinChannel, MaxChannel;
  int MaxVoltage, MaxCurrent;
  
  const int volts2input, microamps2input;
  
  vector<uint16_t> ChannelSetVoltage; // [V]
  vector<uint16_t> ChannelSetCurrent; // [uA]
  vector<uint16_t> ChannelPowerState;
  vector<int16_t> ChannelPolarity;
  vector<string> ChannelPolarityString;

  map<int,string> TypeToName;
};

#endif
