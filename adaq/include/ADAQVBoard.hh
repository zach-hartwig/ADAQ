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

  // Digitizer boards
  zV1720,
  zV1724,
  zDT5720,
  
  // High voltage boards
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
  ADAQVBoard(ZBoardType T, int ID, uint32_t A){
    BoardType = T;
    BoardID = ID;
    BoardAddress = A;
    
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

  // The VME board/desktop digitizer address
  void SetBoardAddress(uint32_t BA) {BoardAddress = BA;}
  int GetBoardAddress() {return BoardAddress;}
  
  // The VME board/desktop digitizer handle
  void SetBoardHandle(int BH) {BoardHandle = BH;}
  int GetBoardHandle() {return BoardHandle;}

  // A unique user-specified ID for the board
  void SetBoardID(int BID) {BoardID = BID;}
  int GetBoardID() {return BoardID;}

  // An ADAQ specific VME/desktop unit identifier
  void SetBoardType(ZBoardType BT) {BoardType = BT;}
  ZBoardType GetBoardType() {return BoardType;}

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
