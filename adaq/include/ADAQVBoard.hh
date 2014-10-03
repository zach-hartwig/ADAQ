///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQVBoard.hh
// date: 02 Oct 14
// auth: Zach Hartwig
//
// desc: 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQVBoard_hh__
#define __ADAQVBoard_hh__ 1

// Boost 
#include <boost/cstdint.hpp>


class ADAQVBoard
{
  
public:
  ADAQVBoard();
  ~ADAQVBoard();

  //////////////////////
  // Abstract methods //
  //////////////////////
  
  // The following methods must be implemented in derived classes

  virtual int OpenLink(uint32_t) = 0;
  virtual int CloseLink() = 0;
  virtual int SetRegisterValue(uint32_t, uint32_t) = 0;
  virtual int GetRegisterValue(uint32_t, uint32_t *) = 0;
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

  // Bool for PC - VME board/desktop digitizer connection
  void SetLinkEstablished(bool LE) {LinkEstablished = LE;}
  bool GetLinkEstablished() {return LinkEstablished;}
  
  // Bool for whether to output information to stdout
  void SetVerbose(bool V) {Verbose = V;}
  bool GetVerbose() {return Verbose;}

private:
  int BoardType, BoardHandle;
  uint32_t BoardAddress;
  bool LinkEstablished, Verbose;
};

#endif
