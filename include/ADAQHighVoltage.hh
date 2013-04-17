///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQHighVoltage.hh
// date: 18 MAY 12
// auth: Zach Hartwig
//
// desc: The ADAQHighVoltage class facilitates communication with the
//       V6534 high voltage board with VME communications via the
//       CAENComm and CAENVME libraries. The purpose of
//       ADAQHighVoltage is to obscure the nitty-gritty-details of
//       interfacing with the V6534 board and present the user with a
//       relatively simple set of methods and variables that can be
//       easibly used in his/her ADAQ projects by instantiating a
//       single ADAQHighVoltage class. Technically, this class should
//       probably be made into a Meyer's singleton for completeness'
//       sake, but the present code should be sufficient for
//       anticipated applications and userbase.
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

#ifndef __ADAQHighVoltage_hh__
#define __ADAQHighVoltage_hh__ 1

// C++
#include <vector>
using namespace std;

// Boost 
#include <boost/cstdint.hpp>

class ADAQHighVoltage
{

public:
  ADAQHighVoltage();
  ~ADAQHighVoltage();

  // Open/close VME link to V6534 board
  int OpenLink(uint32_t);
  int CloseLink();  

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

  // Set/get V6534 VME board address
  void SetBoardAddress(uint32_t BrdAddr) {BoardAddress = BrdAddr;}
  int GetBoardAddress() {return BoardAddress;}
  
  // Set/get level of information output to stdout
  void SetVerbose(bool V) {Verbose = V;}
  bool GetVerbose() {return Verbose;}

  // Get number of V6534 channels (6)
  int GetNumChannels() {return NumChannels;}

  // Get max. allowed channel voltage (6 kV)
  int GetMaxVoltage() {return MaxVoltage;}

  // Get max. allowed channel current (1 mA)
  int GetMaxCurrent() {return MaxCurrent;}

  // Set/get V6534 register values
  int SetRegisterValue(uint32_t, uint16_t);
  int GetRegisterValue(uint32_t, uint16_t *);
  uint16_t GetRegisterValue(uint32_t);

  // Check to ensure restricted registers are not overwritten
  bool CheckRegisterForWriting(uint32_t);

  // HV channel IDs
  enum {Channel0, Channel1, Channel2, Channel3, Channel4, Channel5};

private:
  // V6534 32-bit address in VME space
  uint32_t BoardAddress;

  // Integer handle for easy access to V6534
  int BoardHandle;

  // Integer representing result of CAENComm/CAENVME call
  int CommandStatus;
  
  // Bool determining state of VME link to V6534
  bool LinkEstablished;

  // Bool determing if information goes to stdout
  bool Verbose;
  
  // Number of HV channels on V6534
  const int NumChannels;

  // Max. set channel voltage
  const int MaxVoltage; 

  // Max. drawn channel current 
  const int MaxCurrent; 

  // Conversion from volts to V6534 voltage setting
  const int volts2input;

  // Conversion from microamps to V6534 current setting
  const int microamps2input;

  // std::vectors containing specified individual channel values used
  // by the ADAQHighVoltage class internally
  vector<uint16_t> ChannelSetVoltage; // [V]
  vector<uint16_t> ChannelSetCurrent; // [uA]
  vector<uint16_t> ChannelPowerState;
  vector<uint16_t> ChannelPolarity;
};

#endif
