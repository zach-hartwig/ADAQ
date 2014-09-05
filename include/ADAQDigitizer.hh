///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQDigitizer.hh 
// date: 18 Feb 14
// auth: Zach Hartwig
//
// desc: The ADAQDigitizer class facilitiates communication with the
//       CAEN V1720 digitizer board ia VME communications via the
//       CAENComm, CAENDigitizer,and CAENVME libraries. Its purpose is
//       to obscure the nitty-gritty-details of interfacing with the
//       V1720 board and present the user with a relatively simple set
//       of methods and variables (both C++ and Python via
//       Boost.Python) that can be easibly used in his/her ADAQ
//       projects by instantiating a single ADAQDigitizer "manager"
//       class. Technically, this class should probably be made into a
//       Meyer's singleton for completeness' sake, but the present
//       code should be sufficient for anticipated applications and
//       userbase.
//        
//       At present, the ADAQDigitizer class is compiled into two
//       shared object libraries: libADAQ.so (C++) and libPyADAQ.so
//       (Python). C++ and Python ADAQ projects can then link against
//       these libraries to obtain the provided functionality. The
//       purpose is to ensure that a single version of the ADAQ
//       libraries exist since they are now used in multiple C++,
//       ROOT, and Python acquisition/analysis projects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQDigitizer_hh__
#define __ADAQDigitizer_hh__ 1

// C++
#include <string>
#include <vector>
#include <iostream>
using namespace std;

// Boost libraries provide explicit integer types
#include <boost/cstdint.hpp>

// CAEN digitizer libraries
extern "C"{
#include "CAENDigitizer.h"
}


class ADAQDigitizer
{

public:
  ADAQDigitizer();
  ~ADAQDigitizer();

  ////////////////////////////////////
  // Enhanced ADAQDigitizer methods //
  ////////////////////////////////////

  // Open/close the VME link to the V1720 board
  int OpenLink(uint32_t);
  int CloseLink();

  // Initialize the V1720 board 
  int Initialize();

  // Enable/disable external triggering via NIM/TTL signals
  int EnableExternalTrigger(string SignalLogic="NIM");
  int DisableExternalTrigger();
  
  // Enable/disable channel self-triggering ("auto" triggering)
  int EnableAutoTrigger(uint32_t);
  int DisableAutoTrigger(uint32_t);

  // Enable/disable triggering via software ("SW") commands
  int EnableSWTrigger();
  int DisableSWTrigger();

  // Set the acquisition mode
  int SetSWAcquisitionMode();
  int SetSInAcquisitionMode();
  
  // Set/get V1720 32-bit VME address
  void SetBoardAddress(uint32_t BrdAddr) {BoardAddress = BrdAddr;}
  int GetBoardAddress() {return BoardAddress;}

  // Get the integer for easy access to V1720 
  int GetBoardHandle() {return BoardHandle;}

  // Set/get whether information delivered to stdout
  void SetVerbose(bool V) {Verbose = V;}
  bool GetVerbose() {return Verbose;}

  // Get the number of V1720 digitizer channels (8)
  int GetNumChannels() {return NumChannels;}

  // Get the total number of bits (4096)
  int GetNumBits() {return NumBits;}

  // Get the smallest (0) and largest (4095) bit values
  int GetMinBit() {return MinBit;}
  int GetMaxBit() {return MaxBit;}

  // Get the number of nanoseconds per sample (4ns/sample)
  int GetNanosecondsPerSample() {return NanosecondsPerSample;}

  // Get the number of millivolts per bit (~0.48828125 mV/bit)
  int GetMillivoltsPerBit() {return MillivoltsPerBit;}

  // Set/get V1720 register values
  int SetRegisterValue(uint32_t, uint32_t);
  int GetRegisterValue(uint32_t, uint32_t *);
  uint32_t GetRegisterValue(uint32_t);

  // Prevent overwriting restricted V1720 registers
  bool CheckRegisterForWriting(uint32_t);
  
  // Check to see if the V1720 FPGA buffer is full
  int CheckBufferStatus(bool *);


  



  int SetZSMode(string);

  int SetZLEChannelSettings(uint32_t, uint32_t, uint32_t, uint32_t, bool);

  int SetTriggerEdge(int, string);

  int SetTriggerCoincidence(bool, int);










  
  // Store the integer reprsenting a CAEN digitizer
  int BoardType;

  // Store the VME digitizer board's VME address
  uint32_t BoardAddress;

  // An integer for easy access to the V1720
  int BoardHandle;

  int MemoryBlock;

  // int representing result of CAENDigitizer/CAENComm/CAENVME call
  int CommandStatus;
  
  // Bool representing status of VME link to V1720
  bool LinkEstablished;

  // Bool representing wether information to stdout
  bool Verbose;

  // Number of digitizer channels
  const int NumChannels;

  // Number of bits
  const int NumBits;

  // Min/Max bit
  const int MinBit, MaxBit;

  // Nanoseconds per sample
  const int NanosecondsPerSample;

  // Millivolts per bit
  const double MillivoltsPerBit;


  ////////////////////////////////////////
  // ADAQDigitizer Boost.Python methods //
  ////////////////////////////////////////
  //
  //               ** WARNING : PROTOTYPE FUNCTIONS!! **
  //                                                                                          
  // ZSH: The following ADAQDigitizer member data and functions form a
  //      prototype method for complete control of the waveform
  //      digitization portion of the acquisition process from the
  //      Python side. Most of the following member data (especially
  //      the CAEN structures) would be difficult if not impossible to
  //      declare on the Python side, pass to the C++ side for
  //      operation, and then pass back to the Python side. Rather,
  //      they are simply declared as data member of the ADAQDigitizer
  //      class and manipulated on the C++ side via the ADAQDigitizer
  //      member functions, which can be easily called from the Python
  //      side. Most of the functions are of type "void"; however,
  //      several functions return either integers to the Python side
  //      (since they are extremely useful on the Python side) or a
  //      vector-of-vector<uint16_t>s representing the digitized
  //      waveform data from all 8 V1720 channels. The
  //      vector-of-vector<uint16_t>s variable (on the C++ side) is
  //      converted into a 2-dimensional list on the Python side,
  //      where it can be easily manipulated.
  //
  //      Note that as of 06 JUL 12, most the the Boost.Python wrapper
  //      functions have been provided but only minimal testing has
  //      been accomplished. The testing that has been done shows that
  //      the ADAQ libraries (both C++ and Python) compile perfectly,
  //      and that the new functions provided below can be called from
  //      Python without error or segfaulting. Testing has also shown
  //      that test vector-of-vector<uint16_t>s can be successfully
  //      passed into Python as a 2D list and manipulated.

  uint32_t BufferSize_Py, Size_Py, NumEvents_Py;
  CAEN_DGTZ_EventInfo_t EventInfo_Py;
  CAEN_DGTZ_UINT16_EVENT_t *EventWaveform_Py;

  char *Buffer_Py, *EventPointer_Py;

  vector<uint16_t> ChannelWaveform_Py;
  vector< vector<uint16_t> > Waveforms_Py;

  uint32_t RecordLength;

  void MallocReadoutBuffer_Python()
  { CommandStatus = CAEN_DGTZ_MallocReadoutBuffer(BoardHandle, &Buffer_Py, &Size_Py); }

  void ReadData_Python()
  { CommandStatus = CAEN_DGTZ_ReadData(BoardHandle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, Buffer_Py, &BufferSize_Py); }

  uint32_t GetNumEvents_Python()
  {
    CommandStatus = CAEN_DGTZ_GetNumEvents(BoardHandle, Buffer_Py, BufferSize_Py, &NumEvents_Py);
    return NumEvents_Py;
  }

  void GetEventInfo_Python(uint32_t Event_Py)
  { CommandStatus = CAEN_DGTZ_GetEventInfo(BoardHandle, Buffer_Py, BufferSize_Py, Event_Py, &EventInfo_Py, &EventPointer_Py); }

  void DecodeEvent_Python()
  { CommandStatus = CAEN_DGTZ_DecodeEvent(BoardHandle, EventPointer_Py, (void **)&EventWaveform_Py); }
  
  void FreeEvent_Python()
  { CommandStatus = CAEN_DGTZ_FreeEvent(BoardHandle, (void **)&EventWaveform_Py); }

  void FreeReadoutBuffer_Python()
  { CommandStatus = CAEN_DGTZ_FreeReadoutBuffer(&Buffer_Py); }      

  uint32_t GetTriggerTimeStamp_Python()
  { return EventInfo_Py.TriggerTimeTag; }

  vector< vector<uint16_t> > GetWaveforms_Python()
  {		
    cout << "ADAQDigitizer::GetWaveforms_Python() : RecordLength is hardcoded at 512!" << endl;

    RecordLength = 512;
    
    Waveforms_Py.clear();
    for(int ch=0; ch<NumChannels; ch++){

      // The CAEN_DGTZ_UINT16_EVENT_t EventWaveform_Py contains the
      // primary information on the digitized waveform in two arrays:
      //
      // -- "uint32_t ChSize[8]" stores the number of samples contained for
      //    each digitizer channel. A digitizer channel that is not
      //    enabled in the ChannelEnableMask will have a value of 0.
      //
      // -- "uint16_t DataChannel[8]" stores 8 arrays for each of the
      //    8 digitizer channels containing the digitized
      //    waveform. The length of the array ("width" of the
      //    acquisition window in time) is set by the
      //    RecordLength. The value of the digitized voltage is stored
      //    in each of these 8 arrays as a 16-bit integer

      // Ensure that the channel has been enabled in the channel
      // enable mask; if so, then copy the contents of that array by
      // assignment into a one-dimensional channel vector


      if(EventWaveform_Py->ChSize[ch] != 0)


	ChannelWaveform_Py.assign(EventWaveform_Py->DataChannel[ch],
				  EventWaveform_Py->DataChannel[ch] + RecordLength);


      
      // Push the one-dimensional vector containing the current
      // channel's digitized waveform into the double-vector
      Waveforms_Py.push_back(ChannelWaveform_Py);
      
    }
    
    // Return the double-vector containing (potentially) all 8
    // channel's digitized waveforms as vector<uint16_t>s

    return Waveforms_Py;

  }
  
  // Function that provides test retrieval of a complex ADAQDigitizer
  // data member from the C++ side (as a double-vector) to the Python
  // side (as a two-dimensional list) to mimick the retrieval of the
  // digitized waveforms double-vector
  vector< vector<uint16_t> > GetDoubleVector_Python()
  {
    vector< vector<uint16_t> > double_vec;
    for(int i=0; i<4; i++){
      vector<uint16_t> single_vec;
      for(uint16_t j=0; j<8; j++)
	single_vec.push_back(j);
      double_vec.push_back(single_vec);
    }
    return double_vec;
  }
  //
  //
  ////////////////////////////////////////////////////////////////////////////////////


  ////////////////////////////
  // CAEN Digitizer Wrappers//
  ////////////////////////////
  //
  // The member functions that follow are wrappers for the functions
  // contained in the CAENDigitizer library. The purpose is to obscure
  // many of the details and inconsistencies contained within the
  // CAENDigitizer/Comm/VME libraries that are required to interface
  // with the V1720 digitizer via the VME link. The user may call the
  // following simple, straightforward, object-oriented methods from
  // his/her ADAQ project. The complete definitions of the
  // CAENDigitizer functions that are wrapped below (for the ambitious
  // user/developer) may be found in the files CAENDigitzer.h and
  // CAENDigitzerTypes.h files included with CAENDigitizer, as well as
  // the CAENDigitizer manual.
  //
  // Note : Care has been given to ensure that all CAENDigitizer
  // functions are available in ADAQDigizier, but this is not
  // guaranteed. Please cross check function below with CAENDigitizer
  // manual in the case of potentially missing functions.
  //
  // Last updated : 18 Feb 14 (CAENDigitizer v2.3.2)

  int Reset() {return CAEN_DGTZ_Reset(BoardHandle);}

  //////////////////////
  // Trigger settings //
  //////////////////////
  int SendSWTrigger() {return CAEN_DGTZ_SendSWtrigger(BoardHandle);}
  int SetSWTriggerMode(CAEN_DGTZ_TriggerMode_t tM) {return CAEN_DGTZ_SetSWTriggerMode(BoardHandle, tM);}
  int GetSWTriggerMode(CAEN_DGTZ_TriggerMode_t *tM) {return CAEN_DGTZ_GetSWTriggerMode(BoardHandle, tM);}

  int SetExtTriggerInputMode(CAEN_DGTZ_TriggerMode_t mode) {return CAEN_DGTZ_SetExtTriggerInputMode(BoardHandle, mode);}
  int GetExtTriggerInputMode(CAEN_DGTZ_TriggerMode_t *mode) {return CAEN_DGTZ_GetExtTriggerInputMode(BoardHandle, mode);}

  int SetChannelSelfTrigger(CAEN_DGTZ_TriggerMode_t mode, uint32_t channelMask) {return CAEN_DGTZ_SetChannelSelfTrigger(BoardHandle, mode, channelMask);}
  int GetChannelSelfTrigger(uint32_t channel, CAEN_DGTZ_TriggerMode_t *mode) {return CAEN_DGTZ_GetChannelSelfTrigger(BoardHandle, channel, mode);}

  int SetPostTriggerSize(uint32_t percent) {return CAEN_DGTZ_SetPostTriggerSize(BoardHandle, percent);}
  int GetPostTriggerSize(uint32_t *percent) {return CAEN_DGTZ_GetPostTriggerSize(BoardHandle, percent);}

  int SetChannelTriggerThreshold(uint32_t channel, uint32_t threshold) {return CAEN_DGTZ_SetChannelTriggerThreshold(BoardHandle, channel, threshold);}
  int GetChannelTriggerThreshold(uint32_t channel, uint32_t *threshold) {return CAEN_DGTZ_GetChannelTriggerThreshold(BoardHandle, channel, threshold);}

  int SetGroupTriggerThreshold(uint32_t group, uint32_t threshold) {return CAEN_DGTZ_SetGroupTriggerThreshold(BoardHandle, group, threshold);}
  int GetGroupTriggerThreshold(uint32_t group, uint32_t *threshold) {return CAEN_DGTZ_GetGroupTriggerThreshold(BoardHandle, group, threshold);}

  int SetGroupSelfTrigger(CAEN_DGTZ_TriggerMode_t mode, uint32_t groupMask) {return CAEN_DGTZ_SetGroupSelfTrigger(BoardHandle, mode, groupMask);}
  int GetGroupSelfTrigger(uint32_t group, CAEN_DGTZ_TriggerMode_t *mode) {return CAEN_DGTZ_GetGroupSelfTrigger(BoardHandle, group, mode);}

  int SetTriggerPolarity(uint32_t channel, CAEN_DGTZ_TriggerPolarity_t polarity) {return CAEN_DGTZ_SetTriggerPolarity(BoardHandle, channel, polarity);}
  int GetTriggerPolarity(uint32_t channel, CAEN_DGTZ_TriggerPolarity_t *polarity) {return CAEN_DGTZ_GetTriggerPolarity(BoardHandle, channel, polarity);}


  //////////////////////////////////
  // Non-trigger channel settings //
  //////////////////////////////////
  int SetChannelEnableMask(uint32_t CEM) {return CAEN_DGTZ_SetChannelEnableMask(BoardHandle, CEM);}
  int GetChannelEnableMask(uint32_t *CEM) {return CAEN_DGTZ_GetChannelEnableMask(BoardHandle, CEM);}

  int SetGroupEnableMask(uint32_t mask) {return CAEN_DGTZ_SetGroupEnableMask(BoardHandle, mask);}
  int GetGroupEnableMask(uint32_t *mask) {return CAEN_DGTZ_GetGroupEnableMask(BoardHandle, mask);}

  int SetChannelGroupMask(uint32_t group, uint32_t channelMask) {return CAEN_DGTZ_SetChannelGroupMask(BoardHandle, group, channelMask);}
  int GetChannelGroupMask(uint32_t group, uint32_t *channelMask) {return CAEN_DGTZ_GetChannelGroupMask(BoardHandle, group, channelMask);}
  
  int SetChannelDCOffset(uint32_t channel, uint32_t offset) {return CAEN_DGTZ_SetChannelDCOffset(BoardHandle, channel, offset);}
  int GetChannelDCOffset(uint32_t channel, uint32_t *offset) {return CAEN_DGTZ_GetChannelDCOffset(BoardHandle, channel, offset);}

  int SetGroupDCOffset(uint32_t group, uint32_t offset) {return CAEN_DGTZ_SetGroupDCOffset(BoardHandle, group, offset);}
  int GetGroupDCOffset(uint32_t group, uint32_t *offset) {return CAEN_DGTZ_GetGroupDCOffset(BoardHandle, group, offset);}

  int SetChannelPulsePolarity(uint32_t channel, CAEN_DGTZ_PulsePolarity_t polarity) {return  CAEN_DGTZ_SetChannelPulsePolarity(BoardHandle, channel, polarity);}
  int GetChannelPulsePolarity(uint32_t channel, CAEN_DGTZ_PulsePolarity_t *polarity) {return  CAEN_DGTZ_GetChannelPulsePolarity(BoardHandle, channel, polarity);}


  //////////////////////
  // Zero suppression //
  //////////////////////
  int SetZeroSuppressionMode(uint32_t mode) {return CAEN_DGTZ_SetZeroSuppressionMode(BoardHandle, (CAEN_DGTZ_ZS_Mode_t)mode);}
  int GetZeroSuppressionMode(uint32_t *mode) {return CAEN_DGTZ_GetZeroSuppressionMode(BoardHandle, (CAEN_DGTZ_ZS_Mode_t *)mode);}
  
  int SetChannelZSParams(uint32_t channel, uint32_t weight, int32_t  threshold, int32_t nsamp)
  {return CAEN_DGTZ_SetChannelZSParams(BoardHandle, channel, (CAEN_DGTZ_ThresholdWeight_t)weight, threshold, nsamp);}
  int GetChannelZSParams(uint32_t channel, uint32_t *weight, int32_t  *threshold, int32_t *nsamp)
  {return CAEN_DGTZ_GetChannelZSParams(BoardHandle, channel, (CAEN_DGTZ_ThresholdWeight_t *)weight, threshold, nsamp);}


  //////////////////////////////////////
  // Acquisition settings and control //
  //////////////////////////////////////
  int SWStartAcquisition() {return CAEN_DGTZ_SWStartAcquisition(BoardHandle);}
  int SWStopAcquisition() {return CAEN_DGTZ_SWStopAcquisition(BoardHandle);}

  int SetAcquisitionMode(CAEN_DGTZ_AcqMode_t mode) {return CAEN_DGTZ_SetAcquisitionMode(BoardHandle, mode);}
  int GetAcquisitionMode(CAEN_DGTZ_AcqMode_t *mode) {return CAEN_DGTZ_GetAcquisitionMode(BoardHandle, mode);}

  int SetRecordLength(uint32_t length) {return CAEN_DGTZ_SetRecordLength(BoardHandle, length);}
  int GetRecordLength(uint32_t *length) {return CAEN_DGTZ_GetRecordLength(BoardHandle, length);}
  
  int SetRunSynchronizationMode(CAEN_DGTZ_RunSyncMode_t mode) {return CAEN_DGTZ_SetRunSynchronizationMode(BoardHandle, mode);}
  int GetRunSynchronizationMode(CAEN_DGTZ_RunSyncMode_t *mode) {return CAEN_DGTZ_GetRunSynchronizationMode(BoardHandle, mode);}


  //////////////////////////////////////////////
  // Data readout and readout buffer settings //
  //////////////////////////////////////////////
  int MallocReadoutBuffer(char **buffer, uint32_t *size) {return CAEN_DGTZ_MallocReadoutBuffer(BoardHandle, buffer, size);}
  int FreeReadoutBuffer(char **buffer) {return CAEN_DGTZ_FreeReadoutBuffer(buffer);}

  int AllocateEvent(void **Evt) {return CAEN_DGTZ_AllocateEvent(BoardHandle, Evt);}
  int FreeEvent(CAEN_DGTZ_UINT16_EVENT_t **Evt) {return CAEN_DGTZ_FreeEvent(BoardHandle, (void **)Evt);}

  int ClearData() {return CAEN_DGTZ_ClearData(BoardHandle);}
  int ReadData(char *buffer, uint32_t *bufferSize) {return CAEN_DGTZ_ReadData(BoardHandle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, bufferSize);}
  
  int GetNumEvents(char *buffer, uint32_t buffsize, uint32_t *numEvents) {return CAEN_DGTZ_GetNumEvents(BoardHandle, buffer, buffsize, numEvents);}
  int GetEventInfo(char *buffer, uint32_t buffsize, uint32_t numEvent, CAEN_DGTZ_EventInfo_t *eventInfo, char **EventPtr)
  {return CAEN_DGTZ_GetEventInfo(BoardHandle, buffer, buffsize, numEvent, eventInfo, EventPtr);}

  int DecodeEvent(char *evtPtr, CAEN_DGTZ_UINT16_EVENT_t **Evt) {return CAEN_DGTZ_DecodeEvent(BoardHandle, evtPtr, (void **)Evt);}

  int SetEventPackaging(CAEN_DGTZ_EnaDis_t mode) {return CAEN_DGTZ_SetEventPackaging(BoardHandle, mode);}
  int GetEventPackaging(CAEN_DGTZ_EnaDis_t *mode) {return CAEN_DGTZ_GetEventPackaging(BoardHandle, mode);}

  int SetMaxNumEventsBLT(uint32_t numEvents) {return CAEN_DGTZ_SetMaxNumEventsBLT(BoardHandle, numEvents);}
  int GetMaxNumEventsBLT(uint32_t *numEvents) {return CAEN_DGTZ_GetMaxNumEventsBLT(BoardHandle, numEvents);}

  
  //////////////////////////////////////
  // Miscellaneous digitizer settings //
  //////////////////////////////////////
  int SetAnalogMonOutput(CAEN_DGTZ_AnalogMonitorOutputMode_t mode) {return CAEN_DGTZ_SetAnalogMonOutput(BoardHandle, mode);}
  int GetAnalogMonOutput(CAEN_DGTZ_AnalogMonitorOutputMode_t *mode) {return CAEN_DGTZ_GetAnalogMonOutput(BoardHandle, mode);}

  int SetAnalogInspectionMonParams(uint32_t channelMask, uint32_t offset, CAEN_DGTZ_AnalogMonitorMagnify_t mf, CAEN_DGTZ_AnalogMonitorInspectorInverter_t ami)
  {return CAEN_DGTZ_SetAnalogInspectionMonParams(BoardHandle, channelMask, offset, mf, ami);}
  int GetAnalogInspectionMonParams(uint32_t *channelMask, uint32_t *offset, CAEN_DGTZ_AnalogMonitorMagnify_t *mf, CAEN_DGTZ_AnalogMonitorInspectorInverter_t *ami)
  {return CAEN_DGTZ_GetAnalogInspectionMonParams(BoardHandle, channelMask, offset, mf, ami);}

  int DisableEventAlignedReadout(int handle) {return CAEN_DGTZ_DisableEventAlignedReadout(BoardHandle);}
  int GetInfo(CAEN_DGTZ_BoardInfo_t *boardInfo) {return CAEN_DGTZ_GetInfo(BoardHandle, boardInfo);}

  int SetInterruptConfig(CAEN_DGTZ_EnaDis_t state, uint8_t level, uint32_t status_id, uint16_t event_number, CAEN_DGTZ_IRQMode_t mode)
  {return CAEN_DGTZ_SetInterruptConfig(BoardHandle, state, level, status_id, event_number, mode);}
  int GetInterruptConfig(CAEN_DGTZ_EnaDis_t *state, uint8_t *level, uint32_t *status_id, uint16_t *event_number, CAEN_DGTZ_IRQMode_t *mode)
  {return CAEN_DGTZ_GetInterruptConfig(BoardHandle, state, level, status_id, event_number, mode);}

  int IRQWait(int handle, uint32_t timeout) {return CAEN_DGTZ_IRQWait(BoardHandle, timeout);}
  int VMEIRQCheck(int VMEHandle, uint8_t *Mask) {return CAEN_DGTZ_VMEIRQCheck(VMEHandle, Mask);}
  int VMEIRQWait(CAEN_DGTZ_ConnectionType LinkType, int LinkNum, int ConetNode, uint8_t IRQMask, uint32_t timeout, int *VMEHandle)
  {return CAEN_DGTZ_VMEIRQWait(LinkType, LinkNum, ConetNode, IRQMask, timeout, VMEHandle);}

  int IACKCycle(int32_t *board_id) {return CAEN_DGTZ_IACKCycle(BoardHandle, board_id);}
  int VMEIACKCycle(int VMEHandle, uint8_t level, int32_t *board_id) {return CAEN_DGTZ_VMEIACKCycle(VMEHandle, level, board_id);}
  int SetDESMode(CAEN_DGTZ_EnaDis_t enable) {return CAEN_DGTZ_SetDESMode(BoardHandle, enable);}
  int GetDESMode(CAEN_DGTZ_EnaDis_t *enable) {return CAEN_DGTZ_GetDESMode(BoardHandle, enable);}

  int SetIOLevel(CAEN_DGTZ_IOLevel_t level) {return CAEN_DGTZ_SetIOLevel(BoardHandle, level);}
  int GetIOLevel(CAEN_DGTZ_IOLevel_t *level) {return CAEN_DGTZ_GetIOLevel(BoardHandle, level);}

  //////////////////////////////////////////////
  // Presently unused CAENDigitizer functions //
  //////////////////////////////////////////////
  /*
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDRS4SamplingFrequency(int handle, CAEN_DGTZ_DRS4Frequency_t frequency) ;
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDRS4SamplingFrequency(int handle, CAEN_DGTZ_DRS4Frequency_t *frequency) ;
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetOutputSignalMode(int handle, CAEN_DGTZ_OutputSignalMode_t mode);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetOutputSignalMode(int handle, CAEN_DGTZ_OutputSignalMode_t *mode);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupFastTriggerThreshold(int handle, uint32_t group, uint32_t Tvalue);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupFastTriggerThreshold(int handle, uint32_t group, uint32_t *Tvalue);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupFastTriggerDCOffset(int handle, uint32_t group, uint32_t DCvalue);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupFastTriggerDCOffset(int handle, uint32_t group, uint32_t *DCvalue);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetFastTriggerDigitizing(int handle, CAEN_DGTZ_EnaDis_t enable);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetFastTriggerDigitizing(int handle, CAEN_DGTZ_EnaDis_t *enable);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetFastTriggerMode(int handle, CAEN_DGTZ_TriggerMode_t mode);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetFastTriggerMode(int handle, CAEN_DGTZ_TriggerMode_t *mode);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_LoadDRS4CorrectionData(int handle, CAEN_DGTZ_DRS4Frequency_t frequency);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_EnableDRS4Correction(int handle);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_DisableDRS4Correction(int handle);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_DecodeZLEWaveforms(int handle, void *event, void *waveforms);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_FreeZLEWaveforms(int handle, void *waveforms);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_MallocZLEWaveforms(int handle, void **waveforms, uint32_t *allocatedSize);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_FreeZLEEvents(int handle, void **events);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_MallocZLEEvents(int handle, void **events, uint32_t *allocatedSize);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetZLEEvents(int handle, char *buffer, uint32_t buffsize, void **events, uint32_t* numEventsArray);
    CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetZLEParameters(int handle, uint32_t channelMask, void* params);
  */

};

#endif
