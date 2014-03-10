///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQBridge.cc
// date: 10 Mar 14
// auth: Zach Hartwig
//
// desc: The ADAQBridge class facilitates communication with the V1718
//       VME/USB communication board via the CAENComm and CAENVME
//       libraries. The purpose of ADAQBridge is to obscure the
//       nitty-gritty-details of interfacing with the V1718 board and
//       present the user with a relatively simple set of methods and
//       variables that can be easibly used in his/her ADAQ projects
//       by instantiating a single ADAQBridge "manager"
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
//       projects for interfacing ADAQ with MDSplus data system.
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
#include "CAENVMElib.h"
}

// ADAQ
#include "ADAQBridge.hh"


ADAQBridge::ADAQBridge()
  :
  // The board handle
  BoardHandle(-1),

  // Bool to determine VME connection state to V1718
  LinkEstablished(false),

  // Bool to determine if output printed to cout
  Verbose(false)
{;}


ADAQBridge::~ADAQBridge()
{;}


int ADAQBridge::OpenLink(uint32_t V1720Handle, uint32_t V1720LinkEstablished)
{
  int Status = -42;

  // Need to check to ensure V1720 is open
  if(!V1720LinkEstablished){
    cout << "ADAQBridge : Error opening link! A link must be first established with the V1720 board!\n"
	 << endl;
    return Status;
  }

  // If the link is not currently valid then establish one!
  if(!LinkEstablished){

    // A link to the V1718 is technically established automatically
    // when a VME connections is established (through the V1720, for
    // example). Access to the board is _not_ provided by a hex VME
    // address nor the CAENComm_OpenDevice() function. Rather, the
    // handle to the V1718 must be retrieved in a slightly round-
    // about fashion via the CAENCOMM_Info function via the V1720
    // handle rather than directly into the V1718 board
    Status = CAENComm_Info(V1720Handle, CAENComm_VMELIB_handle, &BoardHandle);
  }
  else
    if(Verbose)
      cout << "ADAQBridge: Error opening link! Link is already open!\n"
	   << endl;

  uint32_t data;
  
  // Set the LinkEstablished bool to indicate that a valid link nto
  // the V1718 has been established and output if Verbose set
  if(Status==0){
    LinkEstablished = true;
    if(Verbose)
      cout << "ADAQBridge : Link successfully established!\n"
	   <<   "                  --> V1718 handle: " << BoardHandle << "\n"
	   << endl;
  }
  else
    if(Verbose and !LinkEstablished)
      cout << "ADAQBridge : Error opening link! Error code: " << Status << "\n"
	   << endl;
  
  // Return success/failure 
  return Status;
}


int ADAQBridge::CloseLink()
{
  // There is not really a true VME link to the V1718 board when it is
  // placed in slot 0 (The VME controller slot), i.e. when it
  // functions as the VME/USB bridge. So we'll just pretend...
  int Status = -42;

  if(LinkEstablished)
    Status = 0;
  else
    if(Verbose)
      cout << "ADAQBridge : Error closing link! Link is already closed!\n"
		<< endl;
  
  if(Status==0){
    LinkEstablished = false;
    if(Verbose)
      cout << "ADAQBridge : Link successfully closed!\n"
		<< endl;
  }

  return Status;
}


int ADAQBridge::SetPulserSettings(PulserSettings *PS)
{
  int Status  = -42;

  Status = CAENVME_SetPulserConf(BoardHandle,
				 (CVPulserSelect)PS->PulserToSet,
				 PS->Period,
				 PS->Width,
				 (CVTimeUnits)PS->TimeUnit,
				 PS->PulseNumber,
				 (CVIOSources)PS->StartSource,
				 (CVIOSources)PS->StopSource);

  return Status;
}


int ADAQBridge::SetPulserOutputSettings(PulserOutputSettings *POS)
{
  int Status = -42;

  Status = CAENVME_SetOutputConf(BoardHandle,
				 (CVOutputSelect)POS->OutputLine,
				 (CVIOPolarity)POS->OutputPolarity,
				 (CVLEDPolarity)POS->LEDPolarity,
				 (CVIOSources)POS->Source);

  return Status;
}


// Method to start Pulser A or B
int ADAQBridge::StartPulser(uint32_t PulserToStart)
{
  int Status = -42;
  
  Status = CAENVME_StartPulser(BoardHandle,
			       (CVPulserSelect)PulserToStart);

  return Status;
}


// Method to stop Pulser A or B
int ADAQBridge::StopPulser(uint32_t PulserToStop)
{
  int Status = -42;

  Status = CAENVME_StopPulser(BoardHandle,
			      (CVPulserSelect)PulserToStop);

  return Status;
}


// Method to set a value to an individual register of the V1718. Note
// that V1718 register access is through CAENVME not CAENComm library
int ADAQBridge::SetRegisterValue(uint32_t addr32, uint32_t data32)
{
  // Ensure that each register proposed for writing is a register that
  // is valid for user writing to prevent screwing up V1718 firmware
  if(CheckRegisterForWriting(addr32))
    return CAENVME_WriteRegister(BoardHandle, (CVRegisters)addr32, data32); 
  else
    return -1;
}


// Method to get a value stored at an individual register of the V1718
// Note that V1718 register access is through CAENVME not CAENComm library
int ADAQBridge::GetRegisterValue(uint32_t addr32, uint32_t *data32)
{ return CAENVME_ReadRegister(BoardHandle, (CVRegisters)addr32, data32); }


// Method to check validity of V1718 register for writing
bool ADAQBridge::CheckRegisterForWriting(uint32_t addr32)
{ return true; }
