/******************************************************************************
*
* CAEN SpA - Software Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* 
* Name      CAENComm.h
* Project   CAENComm - Communication Layer
* Authors   Carlo Tintori & Alberto Lucchesi (support.computing@caen.it)
* Version   1.0
* Date      2009/09/23
*
* This library provides the low level functions for the access to the devices.
* By default, this library is based on the CAENVMElib (CAEN VME bridges V1718
* and V2718) and the CAENVMElink (USB and CONET).
******************************************************************************/

#ifndef __CAENCOMMLIB_H
#define __CAENCOMMLIB_H

#ifdef _WIN32
#include <windows.h>
#else 
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#endif


#ifdef _WIN32
#define STDCALL __stdcall
#else
#define STDCALL
#endif


/*###########################################################################*/
/*
** DEFINITIONS
*/
/*###########################################################################*/
/* variable types */
#ifdef _WIN32
	#ifndef int8_t
        #define int8_t  INT8
    #endif
    #ifndef int16_t
        #define int16_t INT16
    #endif
    #ifndef int32_t
        #define int32_t INT32
    #endif
    #ifndef int64_t
        #define int64_t INT64
    #endif
    #ifndef uint8_t
        #define uint8_t  UINT8
    #endif
    #ifndef uint16_t
        #define uint16_t UINT16
    #endif
    #ifndef uint32_t
        #define uint32_t UINT32
    #endif
    #ifndef uint64_t
        #define uint64_t UINT64
    #endif	
#endif


/* Link Types */
typedef enum CAEN_Comm_ConnectionType {
 CAENComm_USB				= 0,
 CAENComm_PCI_OpticalLink	= 1,
 CAENComm_PCIE_OpticalLink	= 2,
 CAENComm_PCIE				= 3,
} CAENComm_ConnectionType;

typedef enum CAENComm_ErrorCode {
/* Error Types */
 CAENComm_Success		      =  0,     /* Operation completed successfully             			*/
 CAENComm_VMEBusError         = -1,     /* VME bus error during the cycle               			*/
 CAENComm_CommError           = -2,     /* Communication error                          			*/
 CAENComm_GenericError        = -3,     /* Unspecified error                            			*/
 CAENComm_InvalidParam        = -4,     /* Invalid parameter                            			*/
 CAENComm_InvalidLinkType     = -5,     /* Invalid Link Type                            			*/
 CAENComm_InvalidHandler      = -6,     /* Invalid device handler                       			*/
 CAENComm_CommTimeout		  = -7,     /* Communication Timeout						   			*/
 CAENComm_DeviceNotFound      = -8,     /* Unable to Open the requested Device          			*/
 CAENComm_MaxDevicesError     = -9,     /* Maximum number of devices exceeded        		    */
 CAENComm_DeviceAlreadyOpen   = -10,    /* The device is already opened							*/
 CAENComm_NotSupported		  = -11,    /* Not supported function								*/
 CAENComm_UnusedBridge	      = -12,    /* There aren't board controlled by that CAEN Bridge		*/
 CAENComm_Terminated		  = -13,    /* Communication terminated by the Device				*/
} CAENComm_ErrorCode;

typedef enum CAENCOMM_INFO {
		CAENComm_PCI_Board_SN		= 0,		/* s/n of the PCI/PCIe board	*/
		CAENComm_PCI_Board_FwRel	= 1,		/* Firmware Release fo the PCI/PCIe board	*/
		CAENComm_VME_Bridge_SN		= 2,		/* s/n of the VME bridge */
		CAENComm_VME_Bridge_FwRel1	= 3,		/* Firmware Release for the VME bridge */
		CAENComm_VME_Bridge_FwRel2	= 4,		/* Firmware Release for the optical chipset inside the VME bridge (V2718 only)*/
} CAENCOMM_INFO;


typedef enum IRQLevels {
        IRQ1 = 0x01,        /* Interrupt level 1                            */
        IRQ2 = 0x02,        /* Interrupt level 2                            */
        IRQ3 = 0x04,        /* Interrupt level 3                            */
        IRQ4 = 0x08,        /* Interrupt level 4                            */
        IRQ5 = 0x10,        /* Interrupt level 5                            */
        IRQ6 = 0x20,        /* Interrupt level 6                            */
        IRQ7 = 0x40         /* Interrupt level 7                            */
} IRQLevels;


/*##########################################################################*/
/*
** COMMUNICATION LAYER
*/
/*##########################################################################*/

/**************************************************************************//**
* \fn      void CAENComm_DecodeError(int ErrCode, char *ErrMsg)
* \brief   Decode error code
*
* \param   [IN]  ErrCode: Error code
* \param   [OUT] ErrMsg: string with the error message
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_DecodeError(int ErrCode, char *ErrMsg);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_OpenDevice(CAENComm_DeviceDescr dd, int *handle);
* \brief   Open the device
*
* \param	[IN] LinkType: the link used by the device(CAENComm_USB|CAENComm_PCI_OpticalLink|CAENComm_PCIE_OpticalLink|CAENComm_PCIE)
* \param	[IN] LinkNum: The link number
* \param	[IN] ConetNode: The board number in the link.
* \param	[IN] VMEBaseAddress: The VME base address of the board
* \param	[OUT] handle: device handler
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_OpenDevice(CAENComm_ConnectionType LinkType, int LinkNum, int ConetNode, uint32_t VMEBaseAddress, int *handle);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_CloseDevice(int handle);
* \brief   Close the device
*
* \param   [IN]  handle: device handler
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_CloseDevice(int handle);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_Write32(int handle, uint32_t Address, uint32_t *Data)
* \brief   Write a 32 bit register of the device
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offset
* \param   [IN]  Data: new register content to write into the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_Write32(int handle, uint32_t Address, uint32_t Data);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_Write16(int handle, uint32_t Address, uint16_t *Data)
* \brief   Write a 16 bit register of the device
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offset
* \param   [IN]  Data: new register content to write into the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_Write16(int handle, uint32_t Address, uint16_t Data);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_Read32(int handle, uint32_t Address, uint32_t *Data)
* \brief   Read a 32 bit data from the specified register.
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offset
* \param   [OUT] Data: The data read from the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_Read32(int handle, uint32_t Address, uint32_t *Data);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_Read16(int handle, uint32_t Address, uint16_t *Data)
* \brief   Read a 16 bit data from the specified register.
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offset
* \param   [OUT]  Data: The data read from the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_Read16(int handle, uint32_t Address, uint16_t *Data);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_MultiRead32(int handle, uint32_t Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode)
* \brief   The function performs a block of single 32bit ReadRegister.
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offsets
* \param   [IN]  nCycles: the number of read to perform
* \param   [OUT] data: The datas read from the device
* \param   [OUT] ErrorCode: The error codes relaive to each cycle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_MultiRead32(int handle, uint32_t *Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_MultiRead16(int handle, uint32_t *Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode)
* \brief   The function performs a block of single 16bit ReadRegister.
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offsets
* \param   [IN]  nCycles: the number of read to perform
* \param   [OUT] data: The datas read from the device
* \param   [OUT] ErrorCode: The error codes relaive to each cycle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_MultiRead16(int handle, uint32_t *Address, int nCycles, uint16_t *data, CAENComm_ErrorCode *ErrorCode);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_MultiWrite16(int handle, uint32_t *Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode)
* \brief   The function performs a block of single 16bit WriteRegister.
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offsets
* \param   [IN]  nCycles: the number of read to perform
* \param   [IN]  data: The datas to write to the device
* \param   [OUT] ErrorCode: The error codes relaive to each cycle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_MultiWrite16(int handle, uint32_t *Address, int nCycles, uint16_t *data, CAENComm_ErrorCode *ErrorCode);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_MultiWrite32(int handle, uint32_t *Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode)
* \brief   The function performs a block of single 16bit WriteRegister.
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: register address offsets
* \param   [IN]  nCycles: the number of read to perform
* \param   [IN]  data: The datas to write to the device
* \param   [OUT] ErrorCode: The error codes relaive to each cycle.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_MultiWrite32(int handle, uint32_t *Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_BLTRead(int handle, uint32_t Address, uint32_t *Buff, int BltSize, int *nw)
* \brief   Read a block of data from the device using a BLT (32 bit) cycle
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: data space starting address
* \param   [IN]  BltSize: size of the Block Read Cycle (in bytes)
* \param   [OUT] buff: pointer to the data buffer
* \param   [OUT] nw: number of longwords (32 bit) actually read from the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_BLTRead(int handle, uint32_t Address, uint32_t *Buff, int BltSize, int *nw);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_MBLTRead(int handle, uint32_t Address, uint32_t *Buff, int BltSize, int *nw)
* \brief   Read a block of data from the device using an MBLT (64 bit) cycle
*
* \param   [IN]  handle: device handler
* \param   [IN]  Address: data space starting address
* \param   [IN]  BltSize: size of the Block Read Cycle (in bytes)
* \param   [OUT] buff: pointer to the data buffer
* \param   [OUT] nw: number of longwords (32 bit) actually read from the device
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_MBLTRead(int handle, uint32_t Address, uint32_t *Buff, int BltSize, int *nw);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_VMEIRQCheck(int handle, uint8_t *Mask)
* \brief   The function returns a bit mask indicating the active VME IRQ lines.

* \param   [IN]  handle: device handler
* \param   [OUT]  Mask: A bit-mask indicating the VME IRQ lines.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_VMEIRQCheck(int VMEhandle, uint8_t *Mask);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_IRQDisable(int handle)
* \brief   The function disables the IRQ lines.

* \param   [IN]  handle: device handler
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_IRQDisable(int handle);

/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_IRQEnable(int handle)
* \brief   The function enaables the IRQ lines.

* \param   [IN]  handle: device handler
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_IRQEnable(int handle);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_VMEIACKCycle16(int VMEhandle, IRQLevels Level, int *BoardID)
* \brief   The function performs a 16 bit VME interrupt acknowledge cycle .

* \param   [IN]  handle: device handler
* \param   [IN]  Level: The VME IRQ level to acknowledge (see IRQLevels enum).
* \param   [OUT] BoardID: the Id of the Board that reased the interrupt.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_VMEIACKCycle16(int VMEhandle, IRQLevels Level, int *BoardID);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_VMEIACKCycle32(int VMEhandle, IRQLevels Level, int *BoardID)
* \brief   The function performs a 32 bit VME interrupt acknowledge cycle.

* \param   [IN]  handle: device handler
* \param   [IN]  Level: The VME IRQ level to acknowledge (see IRQLevels enum).
* \param   [OUT] BoardID: the Id of the Board that reased the interrupt.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_VMEIACKCycle32(int VMEhandle, IRQLevels Level, int *BoardID);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_IACKCycle(int handle, IRQLevels Level, int *BoardID)
* \brief   The function performs a 16 bit VME interrupt acknowledge cycle .

* \param   [IN]  handle: device handler
* \param   [IN]  Level: The VME IRQ level to acknowledge (see IRQLevels enum).
* \param   [OUT] BoardID: the Id of the Board that reased the interrupt.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_IACKCycle(int handle, IRQLevels Level, int *BoardID);

/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_IRQWait(int handle, uint32_t Timeout)
* \brief   The function wait the IRQ until one of them raise or timeout expires.
		   This function can be used ONLY on board NOT controlled by CAEN VME Bridges 

* \param   [IN]  handle: device handler
* \param   [IN]  Timeout: Timeout in milliseconds.
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_IRQWait(int handle, uint32_t Timeout);

/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_VMEIRQWait(int LinkType, int LinkNum, int ConetNode, uint8_t IRQMask, uint32_t Timeout, int *VMEHandle)
* \brief   The function wait the IRQ until one of them raise or timeout expires.

* \param   [IN] LinkType: the link used to connect to the CAEN VME Bridge(CAENComm_USB|CAENComm_PCI_OpticalLink|CAENComm_PCIE_OpticalLink|CAENComm_PCIE)
* \param   [IN] LinkNum: The link number
* \param   [IN] ConetNode: The CAEN VME Bridge number in the link.
* \param   [IN] Timeout: Timeout in milliseconds.
* \param   [IN] IRQMask: A bit-mask indicating the IRQ lines
* \param   [OUT] VMEHandle: The CAEN Bridhe handle to use in VMEIRQCheck and VMEIACKCycle
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_VMEIRQWait(CAENComm_ConnectionType LinkType, int LinkNum, int ConetNode, uint8_t IRQMask, uint32_t Timeout,int *VMEHandle);

/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_Info(int handle, CAENCOMM_INFO info, char *data)
* \brief   The function returns information about serial number or firmware release of the device.

* \param   [IN]  handle: device handler
* \param   [IN]  info: The interested info (see CAENCOMM_INFO enum).
* \param   [OUT]  data: an array (user defined to 30 byte) with the requested info
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_Info(int handle, CAENCOMM_INFO info, char *data);


/**************************************************************************//**
* \fn      CAENComm_ErrorCode STDCALL CAENComm_SWRelease(char *SwRel)
* \brief   Returns the Software Release of the library
*
* \param   [OUT] SwRel: the Software Release of the library
* \return  0 = Success; negative numbers are error codes
******************************************************************************/
CAENComm_ErrorCode STDCALL CAENComm_SWRelease(char *SwRel);
#endif
