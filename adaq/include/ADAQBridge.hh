///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQBridge.hh
// date: 06 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: 
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
  ADAQBridge(ZBoardType, int, uint32_t Address = 0x00000000);
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
