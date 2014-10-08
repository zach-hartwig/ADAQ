///////////////////////////////////////////////////////////////////////////////
//
// name: ADAQDigitizer.hh 
// date: 02 Oct 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQDigitizer_hh__
#define __ADAQDigitizer_hh__ 1

// C++
#include <string>
#include <vector>
#include <iostream>
using namespace std;

// Boost
#include <boost/cstdint.hpp>

// CAEN
extern "C"{
#include "CAENDigitizer.h"
}

// ADAQ
#include "ADAQVBoard.hh"


class ADAQDigitizer : public ADAQVBoard
{

public:
  ADAQDigitizer(ZBoardType, int, uint32_t);
  ~ADAQDigitizer();


  ///////////////////////////////////////////////
  // Mandatory implemention of virtual methods //
  ///////////////////////////////////////////////
  
  int OpenLink();
  int CloseLink();
  int Initialize();
  int SetRegisterValue(uint32_t, uint32_t);
  int GetRegisterValue(uint32_t, uint32_t *);
  bool CheckRegisterForWriting(uint32_t);


  ////////////////////////////////////////
  // Enhanced digitizer control methods //
  ////////////////////////////////////////

  // Trigger control

  int EnableAutoTrigger(uint32_t);
  int DisableAutoTrigger(uint32_t);

  int EnableExternalTrigger(string SignalLogic="NIM");
  int DisableExternalTrigger();

  int EnableSWTrigger();
  int DisableSWTrigger();

  int SetTriggerEdge(int, string);

  int SetTriggerCoincidence(bool, int);

  // Acquisition control

  int SetAcquisitionControl(string);
  int SetZSMode(string);
  int SetZLEChannelSettings(uint32_t, uint32_t, uint32_t, uint32_t, bool);
  
  // Readout

  int CheckBufferStatus(bool *);
  int GetNumFPGAEvents(uint32_t *);

  int GetZLEWaveform(char *, int, vector<vector<uint16_t> > &);
  int PrintZLEEventInfo(char *);
  
  
  /////////////////////////////////////////
  // Get methods for private member data //
  /////////////////////////////////////////

  int GetNumChannels() {return NumChannels;}
  int GetNumADCBits() {return NumADCBits;}
  int GetMinADCBit() {return MinADCBit;}
  int GetMaxADCBit() {return MaxADCBit;}
  
private:
  int NumChannels;
  int NumADCBits, MinADCBit, MaxADCBit;

public:

  ////////////////////////////
  // CAEN Digitizer Wrappers//
  ////////////////////////////
  //
  // The member functions that follow are custom ADAQ "wrappers" for
  // the functions contained in the CAENDigitizer library. The purpose
  // is prevent the user of the ADAQ libraries with a straightforward
  // and consistent interface to the CAEN digitizers via the
  // ADAQDigitizer class, in addition to the powerful methods
  // implemented above. The user may utilize the following methods in
  // his/her code, which intentionally obscures many of the CAEN
  // library details and provides consistenty across all ADAQ
  // methods.
  //
  // For reference, the complete definitions of the CAENDigitizer
  // functions that are wrapped below may be found in the files
  // CAENDigitzer.h and CAENDigitzerTypes.h files included with
  // CAENDigitizer, as well as the CAENDigitizer manual.
  //
  // Note : Care has been given to ensure that all CAENDigitizer
  // functions are available in ADAQDigizier, but this is not
  // guaranteed. Please cross check function below with CAENDigitizer
  // manual in the case of potentially missing functions.
  //
  // Last updated : 06 Oct 14 (CAENDigitizer-2.6.0)

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

  // Generic methods

  int ClearData() {return CAEN_DGTZ_ClearData(BoardHandle);}
  int ReadData(char *buffer, uint32_t *bufferSize) {return CAEN_DGTZ_ReadData(BoardHandle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, bufferSize);}

  int SetEventPackaging(CAEN_DGTZ_EnaDis_t mode) {return CAEN_DGTZ_SetEventPackaging(BoardHandle, mode);}
  int GetEventPackaging(CAEN_DGTZ_EnaDis_t *mode) {return CAEN_DGTZ_GetEventPackaging(BoardHandle, mode);}

  int SetMaxNumEventsBLT(uint32_t numEvents) {return CAEN_DGTZ_SetMaxNumEventsBLT(BoardHandle, numEvents);}
  int GetMaxNumEventsBLT(uint32_t *numEvents) {return CAEN_DGTZ_GetMaxNumEventsBLT(BoardHandle, numEvents);}

  int MallocReadoutBuffer(char **buffer, uint32_t *size) {return CAEN_DGTZ_MallocReadoutBuffer(BoardHandle, buffer, size);}
  int FreeReadoutBuffer(char **buffer) {return CAEN_DGTZ_FreeReadoutBuffer(buffer);}

  // Methods for standard firmware waveform readout

  int AllocateEvent(void **Evt) {return CAEN_DGTZ_AllocateEvent(BoardHandle, Evt);}
  int DecodeEvent(char *evtPtr, CAEN_DGTZ_UINT16_EVENT_t **Evt) {return CAEN_DGTZ_DecodeEvent(BoardHandle, evtPtr, (void **)Evt);}
  int FreeEvent(CAEN_DGTZ_UINT16_EVENT_t **Evt) {return CAEN_DGTZ_FreeEvent(BoardHandle, (void **)Evt);}

  int GetNumEvents(char *buffer, uint32_t buffsize, uint32_t *numEvents) {return CAEN_DGTZ_GetNumEvents(BoardHandle, buffer, buffsize, numEvents);}
  int GetEventInfo(char *buffer, uint32_t buffsize, uint32_t numEvent, CAEN_DGTZ_EventInfo_t *eventInfo, char **EventPtr)
  {return CAEN_DGTZ_GetEventInfo(BoardHandle, buffer, buffsize, numEvent, eventInfo, EventPtr);}

  // Methods for zero length encoding (zero suppression) readout

  int MallocZLEWaveforms(void **Waveforms, uint32_t *Size) {return CAEN_DGTZ_MallocZLEWaveforms(BoardHandle, Waveforms, Size);}
  int DecodeZLEWaveforms(void *Event, void *Waveforms) {return CAEN_DGTZ_DecodeZLEWaveforms(BoardHandle, Event, Waveforms);}
  int FreeZLEWaveforms(void *Waveforms) {return CAEN_DGTZ_FreeZLEWaveforms(BoardHandle, Waveforms);}

  int MallocZLEEvents(void **Events, uint32_t *Size) {return CAEN_DGTZ_MallocZLEEvents(BoardHandle, Events, Size);}
  int FreeZLEEvents(void **Events) {return CAEN_DGTZ_FreeZLEEvents(BoardHandle, Events);}
    
  int GetZLEEvents(char *Buffer, uint32_t BufferSize, void **Events, uint32_t *NumEventsArray) 
  {return CAEN_DGTZ_GetZLEEvents(BoardHandle, Buffer, BufferSize, Events, NumEventsArray);}
  
  int SetZLEParaments(uint32_t ChannelMask, void *Params) {return CAEN_DGTZ_SetZLEParameters(BoardHandle, ChannelMask, Params);}


  
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
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetCorrectionTables(int handle, int frequency, void *CTable);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_EnableDRS4Correction(int handle);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_DisableDRS4Correction(int handle);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetSAMCorrectionLevel(int handle, CAEN_DGTZ_SAM_CORRECTION_LEVEL_t *level);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetSAMCorrectionLevel(int handle, CAEN_DGTZ_SAM_CORRECTION_LEVEL_t level);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_EnableSAMPulseGen(int handle, int channel, unsigned short  pulsePattern, CAEN_DGTZ_SAMPulseSourceType_t pulseSource);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_DisableSAMPulseGen(int handle, int channel);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetSAMPostTriggerSize(int handle, int SamIndex, uint8_t value);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetSAMPostTriggerSize(int handle, int SamIndex, uint32_t *value);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetSAMSamplingFrequency(int handle, CAEN_DGTZ_SAMFrequency_t frequency);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetSAMSamplingFrequency(int handle, CAEN_DGTZ_SAMFrequency_t *frequency);
CAEN_DGTZ_ErrorCode CAENDGTZ_API _CAEN_DGTZ_Read_EEPROM(int handle, int EEPROMIndex, unsigned short add, int nbOfBytes, unsigned char* buf);
CAEN_DGTZ_ErrorCode CAENDGTZ_API _CAEN_DGTZ_Write_EEPROM(int handle, int EEPROMIndex, unsigned short add, int nbOfBytes, void* buf);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_LoadSAMCorrectionData(int handle);
CAEN_DGTZ_ErrorCode CAENDGTZ_API _CAEN_DGTZ_TriggerThreshold(int handle, CAEN_DGTZ_EnaDis_t endis);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SendSAMPulse(int handle);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetSAMAcquisitionMode(int handle, CAEN_DGTZ_AcquisitionMode_t mode);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetSAMAcquisitionMode(int handle, CAEN_DGTZ_AcquisitionMode_t *mode);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetTriggerLogic(int handle,  uint32_t channelmask, CAEN_DGTZ_TrigerLogic_t logic, uint32_t windows, uint32_t majorityLevel);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetTriggerLogic(int handle,  uint32_t channelmask, CAEN_DGTZ_TrigerLogic_t *logic, uint32_t *windows, uint32_t *majorityLevel);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelPairTriggerLogic(int handle,  uint32_t channelA, uint32_t channelB, CAEN_DGTZ_TrigerLogic_t logic, uint32_t windows);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelPairTriggerLogic(int handle,  uint32_t channelA, uint32_t channelB, CAEN_DGTZ_TrigerLogic_t *logic, uint32_t *windows);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDecimationFactor(int handle, uint16_t factor);
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDecimationFactor(int handle, uint16_t *factor);
  */



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


};

#endif
