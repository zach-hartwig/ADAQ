/******************************************************************************
*
* CAEN SpA - Software Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//*!
*
* \file     CAENComm.h
* \brief       CAENComm - Communication Layer
* \author   Carlo Tintori & Alberto Lucchesi (support.computing@caen.it)
* \date     2009
*
* This library provides the low level functions for the access to the devices.
******************************************************************************/

#ifndef __CAENCOMMLIB_H
#define __CAENCOMMLIB_H

#include <stdint.h>

#define _CAEN_COMM_STR_HELPER(S)		#S
#define CAEN_COMM_STR(S)				_CAEN_COMM_STR_HELPER(S)
#define CAEN_COMM_DEPRECATED_MSG(V, R)	"Deprecated since " #V ". Use " #R

#define CAEN_COMM_VERSION_MAJOR			1																																//!< Major version
#define CAEN_COMM_VERSION_MINOR			6																																//!< Minor version
#define CAEN_COMM_VERSION_PATCH			0																																//!< Patch version
#define CAEN_COMM_VERSION				CAEN_COMM_STR(CAEN_COMM_VERSION_MAJOR) "." CAEN_COMM_STR(CAEN_COMM_VERSION_MINOR) "." CAEN_COMM_STR(CAEN_COMM_VERSION_PATCH)	//!< The version string as Major.Minor.Patch
#define CAEN_COMM_VERSION_NUMBER		((CAEN_COMM_VERSION_MAJOR) * 10000 + (CAEN_COMM_VERSION_MINOR) * 100 + (CAEN_COMM_VERSION_PATCH))								//!< The version number: for example version 1.2.3 gives 10203

#ifdef _WIN32
#define CAEN_COMM_API					__stdcall
#define CAEN_COMM_DLLAPI
#define CAEN_COMM_DEPRECATED(V, R, ...)	__declspec(deprecated(CAEN_COMM_DEPRECATED_MSG(V, R))) __VA_ARGS__
#else
#define CAEN_COMM_API
#define CAEN_COMM_DLLAPI				__attribute__((visibility("default")))
#define CAEN_COMM_DEPRECATED(V, R, ...)	__VA_ARGS__ __attribute__((deprecated(CAEN_COMM_DEPRECATED_MSG(V, R))))
#endif

/** Link Types */
typedef enum CAEN_Comm_ConnectionType {
	CAENComm_USB					= 0,
	CAENComm_OpticalLink			= 1,
	CAENComm_USB_A4818_V2718		= 2,
	CAENComm_USB_A4818_V3718		= 3,
	CAENComm_USB_A4818_V4718		= 4,
	CAENComm_USB_A4818				= 5,
	CAENComm_ETH_V4718				= 6,
	CAENComm_USB_V4718				= 7,
} CAENComm_ConnectionType;

/** Error Types */
typedef enum CAENComm_ErrorCode {
	CAENComm_Success				=  0,		//!< Operation completed successfully
	CAENComm_VMEBusError			= -1,		//!< VME bus error during the cycle
	CAENComm_CommError				= -2,		//!< Communication error
	CAENComm_GenericError			= -3,		//!< Unspecified error
	CAENComm_InvalidParam			= -4,		//!< Invalid parameter
	CAENComm_InvalidLinkType		= -5,		//!< Invalid Link Type
	CAENComm_InvalidHandler			= -6,		//!< Invalid device handler
	CAENComm_CommTimeout			= -7,		//!< Communication Timeout
	CAENComm_DeviceNotFound			= -8,		//!< Unable to Open the requested Device
	CAENComm_MaxDevicesError		= -9,		//!< Maximum number of devices exceeded
	CAENComm_DeviceAlreadyOpen		= -10,		//!< The device is already opened
	CAENComm_NotSupported			= -11,		//!< Not supported function
	CAENComm_UnusedBridge			= -12,		//!< There aren't board controlled by that CAEN Bridge
	CAENComm_Terminated				= -13,		//!< Communication terminated by the Device
	CAENComm_UnsupportedBaseAddress	= -14,		//!< The Base Address is not supported, it's a Desktop device?
} CAENComm_ErrorCode;

typedef enum CAENCOMM_INFO {
	CAENComm_PCI_Board_SN			= 0,		//!< s/n of the PCI/PCIe board
	CAENComm_PCI_Board_FwRel		= 1,		//!< Firmware Release fo the PCI/PCIe board
	CAENComm_VME_Bridge_SN			= 2,		//!< s/n of the VME bridge
	CAENComm_VME_Bridge_FwRel1		= 3,		//!< Firmware Release for the VME bridge
	CAENComm_VME_Bridge_FwRel2		= 4,		//!< Firmware Release for the optical chipset inside the VME bridge (V2718 only)
	CAENComm_VMELIB_handle			= 5,		//!< The VMELib handle used by CAENCOMM
} CAENCOMM_INFO;

typedef enum IRQLevels {
	IRQ1							= 0x01,		//!< Interrupt level 1
	IRQ2							= 0x02,		//!< Interrupt level 2
	IRQ3							= 0x04,		//!< Interrupt level 3
	IRQ4							= 0x08,		//!< Interrupt level 4
	IRQ5							= 0x10,		//!< Interrupt level 5
	IRQ6							= 0x20,		//!< Interrupt level 6
	IRQ7							= 0x40,		//!< Interrupt level 7
} IRQLevels;

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
* \brief      Decode error code
*
* \param[in]  ErrCode: Error code.
* \param[out] ErrMsg: string with the error message.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_DecodeError(int ErrCode, char *ErrMsg);

/**************************************************************************//**
* \brief      Open the device
* \deprecated Kept for backward compatibility. Newer implementations should use CAENComm_OpenDevice2().
*
* \param[in]  LinkType: the link used by the device.
* \param[in]  LinkNum: The link number.
* \param[in]  ConetNode: The board number in the link (where meaningful).
* \param[in]  VMEBaseAddress: The VME base address of the board.
* \param[out] handle: device handler.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DEPRECATED(1.6.0, CAENComm_OpenDevice2,

CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_OpenDevice(CAENComm_ConnectionType LinkType, int LinkNum, int ConetNode, uint32_t VMEBaseAddress, int *handle)

);

/**************************************************************************//**
* \brief      Open the device
*
* \param[in]  LinkType: the link used by the device.
* \param[in]  arg: A pointer to value representing the link. The requested pointed
*                  type depends on the type of connection specified by LinkType:
*                  - a string (as `const char*`) if LinkType is CAENComm_ETH_V4718;
*                  - an integer (as `const uint32_t*`) in all other cases.
* \param[in]  ConetNode: The board number in the link (where meaningful).
* \param[in]  VMEBaseAddress: The VME base address of the board.
* \param[out] handle: device handler.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_OpenDevice2(CAENComm_ConnectionType LinkType, const void* arg, int ConetNode, uint32_t VMEBaseAddress, int* handle);

/**************************************************************************//**
* \brief      Close the device
*
* \param[in]  handle: device handler.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_CloseDevice(int handle);

/**************************************************************************//**
* \brief      Write a 32 bit register of the device
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offset.
* \param[in]  Data: new register content to write into the device.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_Write32(int handle, uint32_t Address, uint32_t Data);

/**************************************************************************//**
* \brief      Write a 16 bit register of the device
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offset.
* \param[in]  Data: new register content to write into the device.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_Write16(int handle, uint32_t Address, uint16_t Data);

/**************************************************************************//**
* \brief      Read a 32 bit data from the specified register.
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offset.
* \param[out] Data: The data read from the device.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_Read32(int handle, uint32_t Address, uint32_t *Data);

/**************************************************************************//**
* \brief      Read a 16 bit data from the specified register.
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offset.
* \param[out] Data: The data read from the device.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_Read16(int handle, uint32_t Address, uint16_t *Data);

/**************************************************************************//**
* \brief      The function performs a block of single 32bit ReadRegister.
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offsets.
* \param[in]  nCycles: the number of read to perform.
* \param[out] data: The datas read from the device.
* \param[out] ErrorCode: The error codes relaive to each cycle.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_MultiRead32(int handle, const uint32_t *Address, int nCycles, uint32_t *data, CAENComm_ErrorCode *ErrorCode);

/**************************************************************************//**
* \brief      The function performs a block of single 16bit ReadRegister.
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offsets.
* \param[in]  nCycles: the number of read to perform.
* \param[out] data: The datas read from the device.
* \param[out] ErrorCode: The error codes relaive to each cycle.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_MultiRead16(int handle, const uint32_t *Address, int nCycles, uint16_t *data, CAENComm_ErrorCode *ErrorCode);

/**************************************************************************//**
* \brief      The function performs a block of single 16bit WriteRegister.
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offsets.
* \param[in]  nCycles: the number of read to perform.
* \param[in]  data: The datas to write to the device.
* \param[out] ErrorCode: The error codes relaive to each cycle.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_MultiWrite16(int handle, const uint32_t *Address, int nCycles, const uint16_t *data, CAENComm_ErrorCode *ErrorCode);

/**************************************************************************//**
* \brief      The function performs a block of single 16bit WriteRegister.
*
* \param[in]  handle: device handler.
* \param[in]  Address: register address offsets.
* \param[in]  nCycles: the number of read to perform.
* \param[in]  data: The datas to write to the device.
* \param[out] ErrorCode: The error codes relaive to each cycle.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_MultiWrite32(int handle, const uint32_t *Address, int nCycles, const uint32_t *data, CAENComm_ErrorCode *ErrorCode);

/**************************************************************************//**
* \brief      Read a block of data from the device using a BLT (32 bit) cycle
*
* \param[in]  handle: device handler.
* \param[in]  Address: data space starting address.
* \param[out] Buff: pointer to the data buffer.
* \param[in]  BltSize: size of the Block Read Cycle (in longwords).
* \param[out] nw: number of longwords (32 bit) actually read from the device.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_BLTRead(int handle, uint32_t Address, uint32_t *Buff, int BltSize, int *nw);

/**************************************************************************//**
* \brief      Read a block of data from the device using an MBLT (64 bit) cycle
*
* \param[in]  handle: device handler.
* \param[in]  Address: data space starting address.
* \param[out] Buff: pointer to the data buffer.
* \param[in]  BltSize: size of the Block Read Cycle (in longwords).
* \param[out] nw: number of longwords (32 bit) actually read from the device.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_MBLTRead(int handle, uint32_t Address, uint32_t *Buff, int BltSize, int *nw);

/**************************************************************************//**
* \brief      The function returns a bit mask indicating the active VME IRQ lines.
*
* \param[in]  VMEhandle: CAENVME device handler.
* \param[out] Mask: A bit-mask indicating the VME IRQ lines.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_VMEIRQCheck(int VMEhandle, uint8_t *Mask);

/**************************************************************************//**
* \brief      The function disables the IRQ lines.
*
* \param[in]  handle: device handler.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_IRQDisable(int handle);

/**************************************************************************//**
* \brief      The function enaables the IRQ lines.
*
* \param[in]  handle: device handler.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_IRQEnable(int handle);

/**************************************************************************//**
* \brief      The function performs a 16 bit VME interrupt acknowledge cycle.
*
* \param[in]  VMEhandle: CAENVME device handler.
* \param[in]  Level: The VME IRQ level to acknowledge (see IRQLevels enum).
* \param[out] BoardID: the Id of the Board that reased the interrupt.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_VMEIACKCycle16(int VMEhandle, IRQLevels Level, int *BoardID);

/**************************************************************************//**
* \brief      The function performs a 32 bit VME interrupt acknowledge cycle.
*
* \param[in]  VMEhandle: CAENVME device handler.
* \param[in]  Level: The VME IRQ level to acknowledge (see IRQLevels enum).
* \param[out] BoardID: the Id of the Board that reased the interrupt.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_VMEIACKCycle32(int VMEhandle, IRQLevels Level, int *BoardID);

/**************************************************************************//**
* \brief      The function performs a 16 bit VME interrupt acknowledge cycle.
*
* \param[in]  handle: device handler.
* \param[in]  Level: The VME IRQ level to acknowledge (see IRQLevels enum).
* \param[out] BoardID: the Id of the Board that reased the interrupt.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_IACKCycle(int handle, IRQLevels Level, int *BoardID);

/**************************************************************************//**
* \brief      The function wait the IRQ until one of them raise or timeout expires.
* \warning    This function can be used ONLY on board NOT controlled by CAEN VME Bridges
*
* \param[in]  handle: device handler.
* \param[in]  Timeout: Timeout in milliseconds.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_IRQWait(int handle, uint32_t Timeout);

/**************************************************************************//**
* \brief      The function wait the IRQ until one of them raise or timeout expires.
*
* \param[in]  LinkType: the link used to connect to the CAEN VME Bridge.
* \param[in]  LinkNum: The link number.
* \param[in]  ConetNode: The CAEN VME Bridge number in the link.
* \param[in]  Timeout: Timeout in milliseconds.
* \param[in]  IRQMask: A bit-mask indicating the IRQ lines.
* \param[out] VMEHandle: The CAEN Bridhe handle to use in VMEIRQCheck and VMEIACKCycle.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_VMEIRQWait(CAENComm_ConnectionType LinkType, int LinkNum, int ConetNode, uint8_t IRQMask, uint32_t Timeout, int *VMEHandle);

/**************************************************************************//**
* \brief      The function returns information about serial number or firmware release of the device.
*
* \param[in]  handle: device handler.
* \param[in]  info: The interested info (see CAENCOMM_INFO enum).
* \param[out] data: an array (user defined to 30 byte) with the requested info for
*                  all the CAENCOMM_INFO entry except for 'CAENComm_VMELIB_handle' that 
*                  requires a pointer to an uint32_t value.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_Info(int handle, CAENCOMM_INFO info, void *data);

/**************************************************************************//**
* \brief      Returns the Software Release of the library
*
* \param[out] SwRel: the Software Release of the library.
* \return     0 = Success; negative numbers are error codes
******************************************************************************/
CAEN_COMM_DLLAPI CAENComm_ErrorCode CAEN_COMM_API CAENComm_SWRelease(char *SwRel);

#ifdef __cplusplus
}
#endif

#endif
