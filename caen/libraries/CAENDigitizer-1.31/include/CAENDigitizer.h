/******************************************************************************
* 
* CAEN SpA - Computing Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the 
* software, documentation and results solely at his own risk.
*
* \file     CAENDigitizer.h
* \brief    CAEN - Digitizer Library
* \author   Alberto Lucchesi (support.computing@caen.it)
*
* This library provide functions, structures and definitions for the CAEN
* digitizer family
******************************************************************************/

#ifndef __CAENDIGITIZER_H
#define __CAENDIGITIZER_H

#include "CAENDigitizerType.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
* \fn      CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_ConnectionType LinkType, int LinkNum, int ConetNode, uint32_t VMEBaseAddress, int *handle);
* \brief   Opens the Digitizer
*
* \param   [IN]  LinkType      : The link used by the device
* \param   [IN]  LinkNum:
*						 - when using CONET, it is the optical link number to be used
*						 - when using USB, it is the USB device number to be used
* \param   [IN]  ConetNode     :
*						 - for CONET identify  which device in the daisy-chain is addressed
*						 - for USB must be 0.
* \param   [IN]	 VMEBaseAddress: The VME base address of the board in case you want to access a board through VME bus, 0 otherwise.						 
* \param   [OUT] handle        : device handler
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_ConnectionType LinkType, int LinkNum, int ConetNode, uint32_t VMEBaseAddress, int *handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_CloseDigitizer(int handle);
* \brief 	Closes the Digitizer
*
* \param 	[IN] handle: digitizer handle to be closed
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_CloseDigitizer(int handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_WriteRegister(int handle, uint32_t Address, uint32_t Data);
* \brief 	Writes a 32-bit word in a specific address offset of the digitizer
*
* \param 	[IN] handle  : the digitizer handle
* \param    [IN] Address : the register address offset
* \param	[IN] Data    : the 32-bit data to write
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_WriteRegister(int handle, uint32_t Address, uint32_t Data);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_ReadRegister(int handle, uint32_t Address, uint32_t *Data);
* \brief 	Reads a 32-bit word from a specific address offset of the digitizer
*
* \param 	[IN] handle  : the digitizer handle
* \param    [IN] Address : the register address offset
* \param	[IN] Data    : the 32-bit data read from the digitizer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_ReadRegister(int handle, uint32_t Address, uint32_t *Data);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetInfo(int handle, CAEN_DGTZ_BoardInfo_t *BoardInfo)
* \brief 	Retrieves the board information of the digitizer
*
* \param 	[IN] handle     : the digitizer handle
* \param    [OUT] BoardInfo : the CAEN_DGTZ_BoardInfo_t with the board information retrieved from the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetInfo(int handle, CAEN_DGTZ_BoardInfo_t *BoardInfo); 


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_Reset(int handle);
* \brief 	Resets the Digitizer. All internal registers and states are restored
*           to defaults.
*
* \param 	[IN] handle: digitizer handle to be reset
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_Reset(int handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_ClearData(int handle);
* \brief 	Clears the data stored in the buffers of the Digitizer
*
* \param 	[IN] handle: digitizer handle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_ClearData(int handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SendSWtrigger(int handle);
* \brief 	Sends a Software trigger to the Digitizer
*
* \param 	[IN] handle: digitizer handle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SendSWtrigger(int handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SWStartAcquisition(int handle);
* \brief 	Starts Digitizers acquisition.
*
* \param 	[IN] handle: digitizer handle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SWStartAcquisition(int handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SWStopAcquisition(int handle);
* \brief 	Stops Digitizer acquisition 
*
* \param 	[IN] handle: digitizer handle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SWStopAcquisition(int handle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetInterruptConfig(int handle, CAEN_DGTZ_EnaDis_t state, uint8_t level, uint32_t status_id, uint16_t event_number, CAEN_DGTZ_IRQMode_t mode)
* \brief 	Enable/disable Interrupts and set the Interrupt level used by the Digitizer
*
* \param 	[IN] handle       : digitizer handle.
* \param 	[IN] state        : interrupts enable status
* \param 	[IN] level        : VME IRQ Level 
* \param 	[IN] status_id    : VME status_id to assign to the Digitizer
* \param 	[IN] event_number : number of event required to raise interrupt
* \param 	[IN] mode         : interrupt mode [CAEN_DGTZ_IRQ_MODE_RORA|CAEN_DGTZ_IRQ_MODE_ROAK]
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetInterruptConfig(int handle, CAEN_DGTZ_EnaDis_t state, uint8_t level, uint32_t status_id, uint16_t event_number, CAEN_DGTZ_IRQMode_t mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetInterruptConfig(int handle, CAEN_DGTZ_EnaDis_t *state, uint8_t *level, uint32_t *status_id, uint16_t *event_number, CAEN_DGTZ_IRQMode_t *mode)
* \brief 	Gets current Interrupt settings of the Digitizer
*
* \param 	[IN] handle       : digitizer handle.
* \param 	[OUT] state       : interrupts state [CAEN_DGTZ_ENABLE|CAEN_DGTZ_DISABLE]
* \param 	[OUT] level       : current VME IRQ Level of the Digitizer
* \param 	[OUT] status_id   : current VME status_id of the the Digitizer
* \param 	[OUT] event_number: number of event required to raise an interrupt
* \param 	[OUT] mode        : interrupt mode [CAEN_DGTZ_IRQ_MODE_RORA|CAEN_DGTZ_IRQ_MODE_ROAK]
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetInterruptConfig(int handle, CAEN_DGTZ_EnaDis_t *state, uint8_t *level, uint32_t *status_id, uint16_t *event_number, CAEN_DGTZ_IRQMode_t *mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_IRQWait(int handle, uint32_t timeout)
* \brief 	Waits for an interrupt by the Digitizer
*
* \param 	[IN] handle  : digitizer handle
* \param 	[IN] timeout : timeout (in milliseconds)
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_IRQWait(int handle, uint32_t timeout);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_VMEIRQWait(int LinkType, int LinkNum, int ConetNode, uint32_t timeout, int *VMEHandle)
* \brief 	Waits for an interrupt from a CAEN VME Bridge
*
* \param   [IN]  LinkType  : The link used to connect to the CAEN VME Bridge 
* \param   [IN]  LinkNum   :
*							- when using CONET, it is the optical link number to be used
*							- when using USB, it is the USB device number to be used
* \param   [IN]  ConetNode :
*							- for CONET identify  witch device in the daisy-chain is addressed
*							- for USB must be 0.
* \param   [IN]  IRQMask   : A bit-mask indicating the IRQ lines
* \param   [IN]	 timeout   : timeout (in milliseconds)						 
* \param   [OUT] VMEHandle : device handler of the CAEN VME Bridge that received interrupt request
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_VMEIRQWait(CAEN_DGTZ_ConnectionType LinkType, int LinkNum, int ConetNode, uint8_t IRQMask, uint32_t timeout, int *VMEHandle);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_VMEIRQCheck(int VMEHandle, uint8_t *Mask)
* \brief 	Checks VME interrupt level
*
* \param   [IN] VMEhandle : handle of CAEN VME Bridge that raised the interrupt request (retrieved from CAEN_DGTZ_VMEIRQWait function)
* \param   [OUT] Mask     : bitmask representing the active VME interrupt requests. 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_VMEIRQCheck(int VMEHandle, uint8_t *Mask);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_IACKCycle(int handle, int32_t *board_id)
* \brief 	Performs an interrupt acknowledge cycle to know the board_id of the board that raised an interrupt
*
* \param   [IN] handle    : digitizer handle.
* \param   [OUT] board_id : board id of the Digitizer that raised the interrupt.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_IACKCycle(int handle, int32_t *board_id);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_VMEIACKCycle(int VMEHandle, uint8_t level, int32_t *board_id)
* \brief 	Performs a VME Interrupt Acknowledge cycle to know the board_id of the board that raised an interrupt
*
* \param   [IN] handle     : handle of the CAEN VME Bridge that raised the interrupt (retrieved from CAEN_DGTZ_VMEIRQWait function)
* \param   [IN] level      : VME interrupt level to acknowledge 
* \param   [OUT] board_id  : VME Digitizer board id of the interrupter
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_VMEIACKCycle(int VMEHandle, uint8_t level, int32_t *board_id);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDESMode(int handle, CAEN_DGTZ_EnaDis_t enable)
* \brief 	Sets Dual Edge Sampling (DES) mode. Valid only for digitizers that supports this acquisiton mode
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] enable : Enable/Disable Dual Edge Sampling mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDESMode(int handle, CAEN_DGTZ_EnaDis_t enable);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDESMode(int handle, CAEN_DGTZ_EnaDis_t *enable)
* \brief 	Gets Dual Edge Sampling (DES) mode. Valid only for digitizers that supports this acquisiton mode.
*
* \param 	[IN] handle  : digitizer handle
* \param 	[OUT] enable : shows current DES mode status (enabled/disabled)
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDESMode(int handle, CAEN_DGTZ_EnaDis_t *enable);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetRecordLength(int handle, uint32_t size)
* \brief 	Sets acquisition record lenght
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] size   : the size of the record (in samples)
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetRecordLength(int handle, uint32_t size);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetRecordLength(int handle, uint32_t *size)
* \brief 	Gets current acquisition record lenght 
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] size  : the size of the record (in samples)
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetRecordLength(int handle, uint32_t *size);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelEnableMask(int handle, uint32_t mask)
* \brief 	Sets channels that will be  enabled into events
*
* \param 	[IN] handle : digitizer handle.
* \param 	[IN] mask   : enabled channels mask.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelEnableMask(int handle, uint32_t mask);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelEnableMask(int handle, uint32_t *mask)
* \brief 	Gets current mask of enabled channels in events.
*           If a mask bit is set, corresponding channel is currenly enabled for event readout
*
* \param 	[IN] handle : digitizer handle.
* \param 	[OUT] mask  : enabled channels mask.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelEnableMask(int handle, uint32_t *mask);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupEnableMask(int handle, uint32_t mask)
* \brief 	Sets channels group that will be enabled into events. 
*           Valid only for digitizers that supports channel groups (V1740, DT5740 for instance).
*           Please refer to digitizers documentation.
* \param 	[IN] handle : digitizer handle.
* \param 	[IN] mask   : channels group mask.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupEnableMask(int handle, uint32_t mask);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupEnableMask(int handle, uint32_t *mask)
* \brief 	Gets current mask of enabled channel groups in events. 
*           Valid only for digitizers that supports channel groups (V1740, DT5740 for instance).
*
* \param 	[IN] handle : digitizer handle.
* \param 	[OUT] mask  : current channels group mask.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupEnableMask(int handle, uint32_t *mask);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetSWTriggerMode(int handle, CAEN_DGTZ_TriggerMode_t mode)
* \brief 	Sets oneof the available trigger mode
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] mode   : trigger mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetSWTriggerMode(int handle, CAEN_DGTZ_TriggerMode_t mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetExtTriggerInputMode(int handle, CAEN_DGTZ_TriggerMode_t mode)
* \brief 	Sets external trigger input mode
*
* \param 	[IN] handle : digitizer handle.
* \param 	[IN] mode   : external trigger input mode.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetExtTriggerInputMode(int handle, CAEN_DGTZ_TriggerMode_t mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetExtTriggerInputMode(int handle, CAEN_DGTZ_TriggerMode_t *mode)
* \brief 	Gets current external trigger input mode
*
* \param 	[IN] handle : digitizer handle.
* \param 	[OUT] mode  : extrnal input trigger mode.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetExtTriggerInputMode(int handle, CAEN_DGTZ_TriggerMode_t *mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetSWTriggerMode(int handle, CAEN_DGTZ_TriggerMode_t *mode)
* \brief 	Sets software trigger mode
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] mode  : trigger mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetSWTriggerMode(int handle, CAEN_DGTZ_TriggerMode_t *mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetChannelSelfTrigger(int handle, CAEN_DGTZ_TriggerMode_t mode, uint32_t channelmask)
* \brief 	Sets channel self trigger mode according to CAEN_DGTZ_TriggerMode_t. 
*
* \param 	[IN] handle      : digitizer handle
* \param 	[IN] mode        : self trigger mode
* \param    [IN] channelmask : channel mask to select affected channels.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelSelfTrigger(int handle, CAEN_DGTZ_TriggerMode_t mode, uint32_t channelmask);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetChannelSelfTrigger(int handle, uint32_t channel, CAEN_DGTZ_TriggerMode_t *mode)
* \brief 	Gets current channel self trigger mode setting. 
*
* \param 	[IN] handle  : digitizer handle
* \param    [IN] channel : select channel to get the self trigger mode
* \param 	[OUT] mode   : current trigger mode for selected channel
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelSelfTrigger(int handle, uint32_t channel, CAEN_DGTZ_TriggerMode_t *mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetGroupSelfTrigger(int handle, CAEN_DGTZ_TriggerMode_t mode, uint32_t groupmask)
* \brief 	Sets channel group self trigger mode according to mode 
*           Valid only for digitizers that supports channel groups (V1740, DT5740 for instance).
*
* \param 	[IN] handle    : digitizer handle
* \param 	[IN] mode      : trigger mode to set for selected channel groups
* \param    [IN] groupmask : channel group selection mask
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupSelfTrigger(int handle, CAEN_DGTZ_TriggerMode_t mode, uint32_t groupmask);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetGroupSelfTrigger(int handle, uint32_t group, CAEN_DGTZ_TriggerMode_t *mode)
* \brief 	Gets channel group self trigger mode.
*           Valid only for digitizers that supports channel groups (V1740, DT5740 for instance).
*
* \param 	[IN] handle : digitizer handle
* \param    [IN] group  : the group of channels to get the trigger information
* \param 	[OUT] mode  : the mode of the trigger
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupSelfTrigger(int handle, uint32_t group, CAEN_DGTZ_TriggerMode_t *mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetChannelGroupMask(int handle, uint32_t group, uint32_t channelmask)
* \brief 	Selects which channel is enabled to contribute to event among available channels of selected channel group. 
*           Valid only for digitizers that supports channel groups (V1740, DT5740 for instance).
*
* \param 	[IN] handle      : digitizer handle.
* \param 	[IN] group       : channel group.
* \param 	[IN] channelmask : mask of channels to enable in event readout
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelGroupMask(int handle, uint32_t group, uint32_t channelmask);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetChannelGroupMask(int handle, uint32_t group, uint32_t *channelmask)
* \brief 	Gets current channel that are enabled to contribute to event among available channels of selected channel group. 
*           Valid only for digitizers that supports channel groups (V1740, DT5740 for instance).
*
* \param 	[IN] handle       : digitizer handle.
* \param 	[IN] group        : the group of channels.
* \param 	[OUT] channelmask : mask of channels to enable in event readout
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelGroupMask(int handle, uint32_t group, uint32_t *channelmask);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetPostTriggerSize(int handle, uint32_t percent)
* \brief 	Sets post trigger for next acquisitions
*
* \param 	[IN] handle  : digitizer handle
* \param 	[IN] percent : the percent of current record length
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetPostTriggerSize(int handle, uint32_t percent);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetPostTriggerSize(int handle, uint32_t *percent)
* \brief 	Gets current post trigger lenght 
*
* \param 	[IN] handle  : digitizer handle
* \param 	[IN] percent : the percent of the record
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetPostTriggerSize(int handle, uint32_t *percent);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetChannelDCOffset(int handle, uint32_t channel, uint32_t Tvalue)
* \brief 	Sets the DC offset for a specified channel
*
* \param 	[IN] handle  : digitizer handle
* \param 	[IN] channel : the channel to set
* \param    [IN] Tvalue  : the DC offset to set. Tvalue is expressed in channel DAC (Digital to Analog Converter) steps.
*                          Please refer to digitizer documentation for possible value range.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelDCOffset(int handle, uint32_t channel, uint32_t Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetChannelDCOffset(int handle, uint32_t channel, uint32_t *Tvalue)
* \brief 	Gets the DC offset for a specified channel
*
* \param 	[IN]  handle  : digitizer handle.
* \param 	[IN]  channel : the channel which takes the information.
* \param    [OUT] Tvalue  : the DC offset set.  
*                           Tvalue is expressed in channel DAC (Digital to Analog Converter) steps.
*                           Please refer to digitizer documentation for possible value range.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelDCOffset(int handle, uint32_t channel, uint32_t *Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetGroupDCOffset(int handle, uint32_t group, uint32_t Tvalue)
* \brief 	Sets the DC offset for a specified group of channels
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] group  : the group of channels to set
* \param    [IN] Tvalue : the DC offset to set
*                         Tvalue is expressed in channel DAC (Digital to Analog Converter) steps.
*                         Please refer to digitizer documentation for possible value range.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupDCOffset(int handle, uint32_t group, uint32_t Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetGroupDCOffset(int handle, uint32_t group, uint32_t *Tvalue)
* \brief 	Gets the DC offset from a specified group of channels
*
* \param 	[IN]  handle : digitizer handle.
* \param 	[IN]  group  : the group of channels which takes the information.
* \param    [OUT] Tvalue : the DC offset set
*                          Tvalue is expressed in channel DAC (Digital to Analog Converter) steps.
*                          Please refer to digitizer documentation for possible value range.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupDCOffset(int handle, uint32_t group, uint32_t *Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetChannelTriggerThreshold(int handle, uint32_t channel, uint32_t Tvalue)
* \brief 	Sets the Trigger Threshold for a specific channel
*
* \param 	[IN] handle  : digitizer handle
* \param 	[IN] channel : channel to set
* \param    [IN] Tvalue  : threshold value to set
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelTriggerThreshold(int handle, uint32_t channel, uint32_t Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetChannelTriggerThreshold(int handle, uint32_t channel, uint32_t *Tvalue)
* \brief 	Gets current Trigger Threshold from a specified channel
*
* \param 	[IN]  handle  : digitizer handle.
* \param 	[IN]  channel : the channel which takes the information.
* \param    [OUT] Tvalue  : the threshold value set
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelTriggerThreshold(int handle, uint32_t channel, uint32_t *Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetGroupTriggerThreshold(int handle, uint32_t group, uint32_t Tvalue)
* \brief 	Sets the Trigger Threshold for a specified group of channels
*
* \param 	[IN] handle : digitizer handle.
* \param 	[IN] group  : the group of channels to set.
* \param    [IN] Tvalue : the threshold value to set.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetGroupTriggerThreshold(int handle, uint32_t group, uint32_t Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetGroupTriggerThreshold(int handle, uint32_t group, uint32_t *Tvalue)
* \brief 	Gets the Trigger Threshold from a specified group of channels
*
* \param 	[IN]  handle : digitizer handle.
* \param 	[IN]  group  : the group of channels which takes the information.
* \param    [OUT] Tvalue : the threshold value to set.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetGroupTriggerThreshold(int handle, uint32_t group, uint32_t *Tvalue);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetZeroSuppressionMode(int handle, CAEN_DGTZ_ZS_Mode_t mode)
* \brief 	Sets Zero Suppression mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] mode   : Zero Suppression mode.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetZeroSuppressionMode(int handle, CAEN_DGTZ_ZS_Mode_t mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetZeroSuppressionMode(int handle, CAEN_DGTZ_ZS_Mode_t *mode)
* \brief 	Gets current Zero Suppression mode
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] mode  : Zero Suppression mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetZeroSuppressionMode(int handle, CAEN_DGTZ_ZS_Mode_t *mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetChannelZSParams(int handle, uint32_t channel, CAEN_DGTZ_ThresholdWeight_t weight, int32_t  threshold, int32_t nsamp)
* \brief 	Sets Zero Suppression parameters for a specific channel
*
* \param 	[IN] handle    : digitizer handle
* \param 	[IN] channel   : target channel
* \param 	[IN] weight    : Zero Suppression Weight
* \param 	[IN] threshold : Zero Suppression Threshold
* \param 	[IN] nsamp     : Number of samples to store before/after threshold crossing
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetChannelZSParams(int handle, uint32_t channel, CAEN_DGTZ_ThresholdWeight_t weight, int32_t  threshold, int32_t nsamp);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetChannelZSParams(int handle, uint32_t *channel, CAEN_DGTZ_ThresholdWeight_t *weight, int32_t  *threshold, int32_t *nsamp)
* \brief 	Gets current Zero Suppression parameters from a specified channel
*
* \param 	[IN]  handle    : digitizer handle
* \param 	[IN]  channel   : target channel
* \param 	[OUT] weight    : Zero Suppression Weight
* \param 	[OUT] threshold : Zero Suppression Threshold
* \param 	[OUT] nsamp     : Number of samples to store before/after threshold crossing
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetChannelZSParams(int handle, uint32_t channel, CAEN_DGTZ_ThresholdWeight_t *weight, int32_t  *threshold, int32_t *nsamp);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetAcquisitionMode(int handle, CAEN_DGTZ_AcqMode_t mode)
* \brief 	Sets digitizzer acquisition mode
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] mode   : acquisition mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetAcquisitionMode(int handle, CAEN_DGTZ_AcqMode_t mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetAcquisitionMode(int handle, CAEN_DGTZ_AcqMode_t *mode)
* \brief 	Gets the acquisition mode of the digitizer 
*
* \param 	[IN]  handle : digitizer handle
* \param 	[OUT] mode   : the acquisition mode set
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetAcquisitionMode(int handle, CAEN_DGTZ_AcqMode_t *mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetAnalogMonOutput(int handle, CAEN_DGTZ_AnalogMonitorOutputMode_t mode)
* \brief 	Sets waveform to output on Digitizer Analog Monitor Front Panel output 
*
* \param 	[IN] handle : digitizer handle.
* \param 	[IN] mode   : Analog Monitor mode.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetAnalogMonOutput(int handle, CAEN_DGTZ_AnalogMonitorOutputMode_t mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetAnalogMonOutput(int handle, CAEN_DGTZ_AnalogMonitorOutputMode_t *mode)
* \brief 	Gets current waveform selected to drive Digitizer Analog Monitor Front Panel output 
*
* \param 	[IN]  handle : digitizer handle
* \param 	[OUT] mode   : Analog Monitor output mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetAnalogMonOutput(int handle, CAEN_DGTZ_AnalogMonitorOutputMode_t *mode);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetAnalogInspectionMonParams(int handle, uint32_t channelmask, uint32_t offset, CAEN_DGTZ_AnalogMonitorMagnify_t mf, CAEN_DGTZ_AnalogMonitorInspectorInverter_t ami)
* \brief 	Sets the Analog Inspection Monitor parameters for a digitizer
* \param 	[IN] handle      : digitizer handle
* \param 	[IN] channelmask : channel enable mask for Analog Inspection
* \param 	[IN] offset      : Analog Inspection Offset
* \param 	[IN] mf          : Analog Inspection Multiply Factor 
* \param 	[IN] ami         : Analog Inspection Invert
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetAnalogInspectionMonParams(int handle, uint32_t channelmask, uint32_t offset, CAEN_DGTZ_AnalogMonitorMagnify_t mf, CAEN_DGTZ_AnalogMonitorInspectorInverter_t ami);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetAnalogInspectionMonParams(int handle, uint32_t *channelmask, uint32_t *offset, CAEN_DGTZ_AnalogMonitorMagnify_t *mf, CAEN_DGTZ_AnalogMonitorInspectorInverter_t *ami)
* \brief 	Gets Analog Inspection Monitor parameters from a digitizer
* \param 	[IN]  handle      : digitizer handle
* \param 	[OUT] channelmask : channel enable mask for Analog Inspection
* \param 	[OUT] offset      : Analog Inspection Offset
* \param 	[OUT] mf          : Analog Inspection Multiply Factor
* \param 	[OUT] ami         : Analog Inspection Invert
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetAnalogInspectionMonParams(int handle, uint32_t *channelmask, uint32_t *offset, CAEN_DGTZ_AnalogMonitorMagnify_t *mf, CAEN_DGTZ_AnalogMonitorInspectorInverter_t *ami);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_DisableSlaveTerminatedTransfer(int handle);
* \brief 	Disables BERR as transfer termination signal from slave (digitizer)
*
* \param 	[IN] handle : digitizer handle
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_DisableEventAlignedReadout(int handle);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetEventPackaging(int handle,CAEN_DGTZ_EnaDis_t mode);
* \brief 	Enable or disable the Pack 2.5 mode of V1720/DT5720 Digitizers
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] mode: Enable/Disable the Pack 2,5 mode 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetEventPackaging(int handle,CAEN_DGTZ_EnaDis_t mode);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetEventPackaging(int handle,CAEN_DGTZ_EnaDis_t *mode);
* \brief 	get the information about the Pack 2.5 mode of V1720/DT5720 Digitizers
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] mode: Enable/Disable the Pack 2,5 mode 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetEventPackaging(int handle,CAEN_DGTZ_EnaDis_t *mode);


/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetMaxNumEventsBLT(int handle, uint32_t numEvents)
* \brief 	Sets max event number for each transfer
*
* \param 	[IN] handle    : digitizer handle
* \param 	[IN] numEvents : Maximum Event Number for transfer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetMaxNumEventsBLT(int handle, uint32_t numEvents);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetMaxNumEventsBLT(int handle, uint32_t *numEvents)
* \brief 	Gets the max number of events of each block transfer
*
* \param 	[IN]  handle    : digitizer handle
* \param 	[OUT] numEvents : the munebr of events set.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetMaxNumEventsBLT(int handle, uint32_t *numEvents);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_MallocReadoutBuffer(int handle, char **buffer, *size)
* \brief 	Allocates memory buffer to hold data received from digitizer
*
* \param 	[IN]  handle : digitizer handle
* \param 	[OUT] buffer : the address of the buffer pointer (WARNING: the *buffer MUST be initialized to NULL)
* \param 	[OUT] size   : the size in byte of the buffer allocated
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_MallocReadoutBuffer(int handle, char **buffer, uint32_t *size);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_ReadData(int handle, CAEN_DGTZ_ReadMode_t mode, char *buffer, uint32_t *bufferSize)
* \brief 	Reads data (events) from the digitizer
*
* \param 	[IN]  handle     : digitizer handle
* \param 	[OUT] buffer     : address of the buffer that will store data (acquisition buffer)
* \param 	[OUT] bufferSize : the size of the data stored in the buffer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_ReadData(int handle, CAEN_DGTZ_ReadMode_t mode, char *buffer, uint32_t *bufferSize);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_FreeReadoutBuffer(char **buffer)
* \brief 	Frees memory allocated by the CAEN_DGTZ_MallocReadoutBuffer
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] buffer : address of the buffer (acquisition buffer) returned by CAEN_DGTZ_MallocReadoutBuffer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_FreeReadoutBuffer(char **buffer);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetNumEvents(int handle, char *buffer, uint32_t buffsize, uint32_t *numEvents)
* \brief 	Gets current number of event stored in the acquisition buffer
*
* \param 	[IN] handle     : digitizer handle
* \param 	[IN] buffer     : Address of the acquisition buffer 
* \param 	[IN] bufferSize : Size of the data stored in the acquisition buffer
* \param 	[OUT] numEvents : Number of events stored in the acquisition buffer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetNumEvents(int handle, char *buffer, uint32_t buffsize, uint32_t *numEvents);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetEventInfo(int handle, char *buffer, uint32_t buffsize, int32_t numEvent, CAEN_DGTZ_EventInfo_t *eventInfo, char **EventPtr)
* \brief 	Retrieves the information associated with a specified event
*
* \param 	[IN] handle     : digitizer handle
* \param 	[IN] buffer     : Address of the acquisition buffer 
* \param 	[IN] bufferSize : acquisition buffer size (in samples)
* \param 	[IN] numEvents  : Number of events stored in the acquisition buffer
* \param 	[OUT] eventInfo : Event Info structure containing the information about the specified event
* \param 	[OUT] EventPtr  : Pointer to the requested event in the acquisition buffer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetEventInfo(int handle, char *buffer, uint32_t buffsize, int32_t numEvent, CAEN_DGTZ_EventInfo_t *eventInfo, char **EventPtr);



/**************************************************************************//**
* \fn  		CAEN_DGTZ_DecodeEvent(int handle, char *evtPtr, void **Evt)
* \brief 	Decodes a specified event stored in the acquisition buffer
*
* \param 	[IN]  handle   : digitizer handle
* \param 	[IN]  EventPtr : pointer to the requested event in the acquisition buffer
* \param 	[OUT] Evt      : event structure with the requested event data
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_DecodeEvent(int handle, char *evtPtr, void **Evt);



/**************************************************************************//**
* \fn  		CCAEN_DGTZ_FreeEvent(int handle, void **Evt)
* \brief 	Releases the event returned by the CAEN_DGTZ_DecodeEvent
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] Evt    : event structure that store the decoded event.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_FreeEvent(int handle, void **Evt);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPPParameter(int handle, uint32_t channel, CAEN_DGTZ_DPP_PARAMETER_t Param, uint32_t value);
* \brief 	Set the specified DPP parameter with the specified value.
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] Param: The DPP parameter to set
* \param 	[IN] Value: The value to give to the DPP parameter 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDPPParameter(int handle, uint32_t channel, CAEN_DGTZ_DPP_PARAMETER_t Param, uint32_t value);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetDPPParameter(int handle, uint32_t channel, CAEN_DGTZ_DPP_PARAMETER_t Param, uint32_t *value);
* \brief 	Get the value of a specified DPP parameter
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] Param: The DPP parameter to Get
* \param 	[OUT] Value: The value of the DPP parameter 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDPPParameter(int handle, uint32_t channel, CAEN_DGTZ_DPP_PARAMETER_t Param, uint32_t *value);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPPAcquisitionMode(int handle,CAEN_DGTZ_DPP_AcqMode_t mode, CAEN_DGTZ_DPP_SaveParam_t param);
* \brief 	Set the DPP acquisition mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] mode: The DPP acquisition mode
* \param 	[IN] param: The acquisition data to retrieve in acquisition
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDPPAcquisitionMode(int handle,CAEN_DGTZ_DPP_AcqMode_t mode, CAEN_DGTZ_DPP_SaveParam_t param);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetDPPAcquisitionMode(int handle,CAEN_DGTZ_DPP_AcqMode_t *mode, CAEN_DGTZ_DPP_SaveParam_t *param);
* \brief 	Get the information about the DPP acquisition mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] mode: The DPP acquisition mode
* \param 	[OUT] param: The acquisition data to retrieve in acquisition
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDPPAcquisitionMode(int handle,CAEN_DGTZ_DPP_AcqMode_t *mode, CAEN_DGTZ_DPP_SaveParam_t *param);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPP_TF_VirtualProbe(int handle,CAEN_DGTZ_DPP_VirtualProbe_t vp, CAEN_DGTZ_DPP_TF_VirtuallProbe1_t vp1, CAEN_DGTZ_DPP_TF_VirtuallProbe2_t vp2, CAEN_DGTZ_DPP_TF_DigitalProbe_t dp);
* \brief 	Set the information about the output signal of the TF DPP acquisition mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] vp: The Virtual Probe mode
* \param 	[IN] vp1: The Virtual Probe1 mode
* \param 	[IN] vp2: The Virtual Probe2 mode
* \param 	[IN] dp: The Digital Probe mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDPP_TF_VirtualProbe(int handle,CAEN_DGTZ_DPP_VirtualProbe_t mode, CAEN_DGTZ_DPP_TF_VirtuallProbe1_t vp1, CAEN_DGTZ_DPP_TF_VirtuallProbe2_t vp2, CAEN_DGTZ_DPP_TF_DigitalProbe_t dp);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPP_TF_VirtualProbe(int handle,CAEN_DGTZ_DPP_VirtualProbe_t *vp, CAEN_DGTZ_DPP_TF_VirtuallProbe1_t *vp1, CAEN_DGTZ_DPP_TF_VirtuallProbe2_t *vp2, CAEN_DGTZ_DPP_TF_DigitalProbe_t *dp);
* \brief 	Get the information about the output signal of the TF DPP acquisition mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] vp: The Virtual Probe mode
* \param 	[OUT] vp1: The Virtual Probe1 mode
* \param 	[OUT] vp2: The Virtual Probe2 mode
* \param 	[OUT] dp: The Digital Probe mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDPP_TF_VirtualProbe(int handle,CAEN_DGTZ_DPP_VirtualProbe_t *mode, CAEN_DGTZ_DPP_TF_VirtuallProbe1_t *vp1, CAEN_DGTZ_DPP_TF_VirtuallProbe2_t *vp2, CAEN_DGTZ_DPP_TF_DigitalProbe_t *dp);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPP_CI_VirtualProbe(int handle, CAEN_DGTZ_DPP_VirtualProbe_t vp, CAEN_DGTZ_DPP_TF_VirtuallProbe2_t vp2)
* \brief 	Set the information about the output signals of the CI DPP acquisition mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] vp: The Virtual Probe mode
* \param 	[IN] vp2: The Virtual Probe2 mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/

CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDPP_CI_VirtualProbe(int handle, CAEN_DGTZ_DPP_VirtualProbe_t mode, CAEN_DGTZ_DPP_CI_VirtuallProbe2_t vp2);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetDPP_CI_VirtualProbe(int handle, CAEN_DGTZ_DPP_VirtualProbe_t *vp, CAEN_DGTZ_DPP_TF_VirtuallProbe2_t *vp2)
* \brief 	Set the information about the output signals of the CI DPP acquisition mode.
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] vp: The Virtual Probe mode
* \param 	[OUT] vp2: The Virtual Probe2 mode
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDPP_CI_VirtualProbe(int handle, CAEN_DGTZ_DPP_VirtualProbe_t *mode, CAEN_DGTZ_DPP_CI_VirtuallProbe2_t *vp2);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPP_CI_Coincidence(int handle, CAEN_DGTZ_EnaDis_t Coincidence, uint16_t CoincidanceTime, CAEN_DGTZ_DPP_CI_GPO_SEL_t gpo)
* \brief 	Set the Coincidence mode of the CI DPP Digitizer.
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] mode: Enable/Disable the Coincidence mode 
* \param 	[IN] CoincidanceTime: The time value for the Coincidence
* \param 	[IN] gpo: the General Purpose Output mode to set
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetDPP_CI_Coincidence(int handle, CAEN_DGTZ_EnaDis_t Coincidence, uint16_t CoincidanceTime, CAEN_DGTZ_DPP_CI_GPO_SEL_t gpo);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetDPP_CI_Goincidence(int handle, CAEN_DGTZ_EnaDis_t *Coincidence, uint16_t *CoincidanceTime, CAEN_DGTZ_DPP_CI_GPO_SEL_t *gpo)
* \brief 	Get the Coincidence mode of the CI DPP Digitizer.
*
* \param 	[IN] handle : digitizer handle
* \param 	[OUT] mode: the Coincidence mode status
* \param 	[OUT] CoincidanceTime: The time value for the Coincidence
* \param 	[OUT] gpo: the General Purpose Output mode 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetDPP_CI_Coincidence(int handle, CAEN_DGTZ_EnaDis_t *Coincidence, uint16_t *CoincidanceTime, CAEN_DGTZ_DPP_CI_GPO_SEL_t *gpo);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_AllocateEvent(int handle, void **Evt)
* \brief 	Allocate the memory for the event
*
* \param 	[IN] handle : digitizer handle
* \param 	[IN] Evt: the reference to the Event pointer 
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_AllocateEvent(int handle, void **Evt);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetIOLevel(int handle, CAEN_DGTZ_IOLevel_t level)
* \brief 	Sets the IO Level
*
* \param 	[IN] handle  : digitizer handle
* \param 	[IN] level   : The level to set
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetIOLevel(int handle, CAEN_DGTZ_IOLevel_t level);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetIOLevel(int handle, CAEN_DGTZ_IOLevel_t *level)
* \brief 	Gets the IO Level
*
* \param 	[IN] handle  : digitizer handle
* \param 	[OUT] level  : The IO level of the digitizer
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetIOLevel(int handle, CAEN_DGTZ_IOLevel_t *level);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_SetTriggerPolarity(int handle, uint32_t channel, CAEN_DGTZ_TriggerPolarity_t Polarity)
* \brief 	Sets the trigger polarity of a specified channel
*
* \param 	[IN] handle  : digitizer handle
* \param    [IN] channel : select channel to set the trigger polarity
* \param 	[IN] Polarity   : The polarity to set
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_SetTriggerPolarity(int handle, uint32_t channel, CAEN_DGTZ_TriggerPolarity_t Polarity);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_GetTriggerPolarity(int handle, uint32_t channel, CAEN_DGTZ_TriggerPolarity_t *Polarity)
* \brief 	Gets the trigger polarity of a specified channel
*
* \param 	[IN] handle  : digitizer handle
* \param    [IN] channel : select channel to get the trigger polarity
* \param 	[OUT] Polarity   : The polarity of the specified channel
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_GetTriggerPolarity(int handle, uint32_t channel, CAEN_DGTZ_TriggerPolarity_t *Polarity);

/**************************************************************************//**
* \fn  		CAEN_DGTZ_RearmInterrupt(int handle)
* \brief 	Rearm the Interrupt 
*
* \param 	[IN] handle : digitizer handle
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_DGTZ_ErrorCode CAENDGTZ_API CAEN_DGTZ_RearmInterrupt(int handle);
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

#ifdef __cplusplus
}
#endif

#endif
