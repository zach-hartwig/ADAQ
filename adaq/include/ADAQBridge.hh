///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQBridge.hh
// date: 18 MAY 12
// auth: Zach Hartwig
//
// desc: The ADAQBridge class facilitates communication with the V1718
//       USB/VME bridge board with VME communications via the CAENComm
//       and CAENVME libraries. The purpose of ADAQBridge is to
//       obscure the nitty-gritty-details of interfacing with the
//       V1718 board and present the user with a relatively simple set
//       of methods and variables that can be easibly used in his/her
//       ADAQ projects by instantiating a single ADAQBridge
//       class. Technically, this class should probably be made into a
//       Meyer's singleton for completeness' sake, but the present
//       code should be sufficient for anticipated applications and
//       userbase.
//        
//       At present, the ADAQBridge class is compiled into two
//       shared object libraries: libADAQ.so (C++) and libPyADAQ.so
//       (Python). C++ and Python ADAQ projects can then link against
//       these libraries to obtain the provided functionality. The
//       purpose is to ensure that a single version of the ADAQ
//       libraries exist since they are now used in multiple C++
//       projects, multiple ROOT analysis projects, and new Python
//       projects for interfacing ADAQ with MDSplus data system
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQBridge_hh__
#define __ADAQBridge_hh__ 1

// C++
#include <vector>
using namespace std;

// Boost 
#include <boost/cstdint.hpp>

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


class ADAQBridge
{

public:
  ADAQBridge();
  ~ADAQBridge();

  // Open/close VME link to V1718 board
  int OpenLink(uint32_t, uint32_t);
  int CloseLink();  

  int SetPulserSettings(PulserSettings *);
  int SetPulserOutputSettings(PulserOutputSettings *);

  int StartPulser(uint32_t);
  int StopPulser(uint32_t);

  // Set/get verbose value
  void SetVerbose(bool v) {Verbose = v;}
  bool GetVerbose() {return Verbose;}

  // Set/get V6534 register values
  int SetRegisterValue(uint32_t, uint32_t);
  int GetRegisterValue(uint32_t, uint32_t *);

  // Check to ensure restricted registers are not overwritten
  bool CheckRegisterForWriting(uint32_t);

private:
  // Integer handle for easy access to V1718
  long BoardHandle;

  // Integer representing result of CAENComm/CAENVME call
  int CommandStatus;
  
  // Bool determining state of VME link to V6534
  bool LinkEstablished;

  // Bool determing if information goes to stdout
  bool Verbose;
};

#endif
