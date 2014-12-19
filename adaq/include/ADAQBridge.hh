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
// name: ADAQBridge.hh
// date: 17 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQBridge is a derived class that is intended to provide
//       full control over CAEN USB-VME bridge boards, including VME
//       connection, register read/write, pulser control, and other
//       high level functions. This class inherits all the general
//       member data and methods contained in ADAQVBoard.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQBridge_hh__
#define __ADAQBridge_hh__ 1

// C++
#include <vector>
#include <string>
using namespace std;

// Boost 
#include <boost/cstdint.hpp>

// ADAQ
#include "ADAQVBoard.hh"


// Two structures that are useful for settings the pulsers

struct PulserSettings{
  int PulserToSet;
  int Period;
  int Width;
  int TimeUnit;
  int PulseNumber;
  int StartSource;
  int StopSource;
};

struct PulserOutputSettings{
  int OutputLine;
  int OutputPolarity;
  int LEDPolarity;
  int Source;
};


class ADAQBridge : public ADAQVBoard
{
  
public:
  ADAQBridge(ZBoardType, int, uint32_t = 0x00000000, int = 0, int = 0);
  ~ADAQBridge();
  
  ///////////////////////////////////////////////
  // Mandatory implemention of virtual methods //
  ///////////////////////////////////////////////
  
  int OpenLink();
  int CloseLink();  
  int Initialize();
  int SetRegisterValue(uint32_t, uint32_t);
  int GetRegisterValue(uint32_t, uint32_t *);
  bool CheckRegisterForWriting(uint32_t);

  
  /////////////////////////////////////
  // Enhanced VME/USB bridge methods //
  /////////////////////////////////////

  int OpenLinkDirectly();
  int OpenLinkViaDigitizer(uint32_t, bool);

  int SetPulserSettings(PulserSettings *);
  int SetPulserOutputSettings(PulserOutputSettings *);

  int StartPulser(uint32_t);
  int StopPulser(uint32_t);

private:
  string BoardName, ConnectionName;

};

#endif
