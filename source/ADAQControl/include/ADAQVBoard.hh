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
// name: ADAQVBoard.hh
// date: 17 Oct 14
// auth: Zach Hartwig
//
// desc: ADAVBoard is a pure abstract class that provides a high-level
//       base class description of CAEN VME boards and desktop
//       digitizers. To implement specific types (digitizers, high
//       voltages, VME-USB bridges, etc), users should inherit from
//       this class for development since it provides a serious of
//       common member data and variables across all type categories.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQVBoard_hh__
#define __ADAQVBoard_hh__ 1

// Boost 
#include <boost/cstdint.hpp>


// Unique IDs for each type board/desktop unit. The following
// enumerator contains those types presently supported by ADAQ.
enum ZBoardType{

  // VME-USB boards
  zV1718,

  // VME Digitizer boards
  zV1720,
  zV1724,
  zV1725,
  
  // Desktop digitizers
  zDT5720,
  zDT5730,

  // Desktop digitizer + high voltage boards
  zDT5790M,
  zDT5790N,
  zDT5790P,

  // VME High voltage boards
  zV6533M,
  zV6533N,
  zV6533P,
  zV6534M,
  zV6534N,
  zV6534P
};


class ADAQVBoard
{
  
public:
  ADAQVBoard(ZBoardType T, int ID, uint32_t A, int LN, int CN){
    BoardType = T;
    BoardID = ID;
    BoardAddress = A;
    BoardLinkNumber = LN;
    BoardCONETNode = CN;
    
    BoardHandle = -42;
    LinkEstablished = false;
    Verbose = false;
    CommandStatus = 0;
  };
  
  ~ADAQVBoard(){};

  //////////////////////
  // Abstract methods //
  //////////////////////
  
  // The following methods should be implemented in derived classes

  virtual int OpenLink() = 0;
  virtual int CloseLink() = 0;
  virtual int Initialize() = 0;
  // virtual int SetRegisterValue(uint32_t, uint32_t) = 0;
  // virtual int GetRegisterValue(uint32_t, uint32_t *) = 0;
  virtual bool CheckRegisterForWriting(uint32_t) = 0;

  /////////////////////
  // Set/get methods //
  /////////////////////

  // Standard methods to manipulate and retrieve member data

  // An ADAQ-specific VME/desktop unit identifier
  void SetBoardType(ZBoardType BT) {BoardType = BT;}
  ZBoardType GetBoardType() {return BoardType;}

    // An ADAQ-specific user-specified ID for the board
  void SetBoardID(int BID) {BoardID = BID;}
  int GetBoardID() {return BoardID;}

  // The VME board/desktop digitizer address
  void SetBoardAddress(uint32_t BA) {BoardAddress = BA;}
  uint32_t GetBoardAddress() {return BoardAddress;}

  // The USB port number assigned to VME/desktop unit
  void SetBoardLinkNumber(int BLN) {BoardLinkNumber = BLN;}
  int GetBoardLinkNumber() {return BoardLinkNumber;}
  
  // The CONET node ID of the VME/desktop unit
  void SetBoardCONETNode(int BCN) {BoardCONETNode = BCN;}
  int GetBoardCONETNode() {return BoardCONETNode;}
  
  // The VME board/desktop digitizer handle
  void SetBoardHandle(int BH) {BoardHandle = BH;}
  int GetBoardHandle() {return BoardHandle;}

  // Bool for PC - VME board/desktop digitizer connection
  void SetLinkEstablished(bool LE) {LinkEstablished = LE;}
  bool GetLinkEstablished() {return LinkEstablished;}
  
  // Bool for whether to output information to stdout
  void SetVerbose(bool V) {Verbose = V;}
  bool GetVerbose() {return Verbose;}

  // Bool to get status of most recently executed command
  void SetCommandStatus(int CS) {CommandStatus = CS;}
  int GetCommandStatus() {return CommandStatus;}
  
protected:
  // The type of board described by inherited class
  ZBoardType BoardType;

  // The user-assigned board ID
  int BoardID;

  // The user-assigned address in VME space (corresponds to physical
  // potentiometer settings on VME board)
  uint32_t BoardAddress;

  // The PC-assigned USB port number of the device
  int BoardLinkNumber;

  // The user-assignedCONET node ID within set of optically daisy-chained devices
  int BoardCONETNode;

  // The CAEN-assigned board handle
  int BoardHandle;
  
  // Boolean to determine if a connection has been made to the board
  bool LinkEstablished;

  // Boolean to set various output to stdout 
  bool Verbose;
  
  // Integer to hold returned value of ADAQ and CAEN libary methods
  int CommandStatus;
};

#endif
