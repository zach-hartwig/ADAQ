/*!
        -----------------------------------------------------------------------------

                       --- CAEN SpA - Computing Systems Division ---

        -----------------------------------------------------------------------------

        CAENVMElib.h

        -----------------------------------------------------------------------------

        Created: March 2004

        -----------------------------------------------------------------------------
*/
#ifndef __CAENVMELIB_H
#define __CAENVMELIB_H

#include <stdio.h>
// Rev. 2.5
#ifdef LINUX
	#include <stdint.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#ifdef WIN32
	typedef INT8   int8_t;
	typedef UINT8  uint8_t;
	typedef INT16  int16_t;
	typedef UINT16 uint16_t;
	typedef INT32  int32_t;
	typedef UINT32 uint32_t;
	typedef INT64  int64_t;
	typedef UINT64 uint64_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*
        CAENVME_DecodeError
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Code      : The error code to decode.
        -----------------------------------------------------------------------------
        Returns:
                An string describing the error condition.
        -----------------------------------------------------------------------------
        Description:
                Decode the error.
*/
const char *
#ifdef WIN32
__stdcall
#endif
CAENVME_DecodeError(CVErrorCodes Code);

/*
        CAENVME_API CAENVME_SWRelease
        -----------------------------------------------------------------------------
        Parameters:
                [out] SwRel     : Returns the software release of the library.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                Permits to read the software release of the library.
*/
CAENVME_API
CAENVME_SWRelease(char *SwRel);

/*
        CAENVME_BoardFWRelease.
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] FWRel     : Returns the firmware release of the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                Permits to read the firmware release loaded into the device.
*/
CAENVME_API
CAENVME_BoardFWRelease(int32_t Handle, char *FWRel);

/*
        CAENVME_DriverRelease.
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Rel       : Returns the software release of the device driver
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                Permits to read the software release of the device driver.
*/
CAENVME_API
CAENVME_DriverRelease(int32_t Handle, char *Rel);

/*
        CAENVME_DeviceReset
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                Permits to reset the device.
                Implemented for A2818, A2719, V2718 on Linux platform only 
*/
CAENVME_API
CAENVME_DeviceReset(int32_t dev);


/*
        CAENVME_Init
        -----------------------------------------------------------------------------
        Parameters:
                [in]  BdType    : The model of the bridge (V1718/V2718).
                [in]  Link      : The link number (don't care for V1718).
                [in]  BdNum     : The board number in the link.
                [out] Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function generates an opaque handle to identify a module
                attached to the PC. In the case of V1718 bridge it must be
                specified only the module index (BdNum) because the link is USB.
                In the case of V2718 it must be specified also the link because
                you can have some A2818 optical link inside the PC.
*/
CAENVME_API
CAENVME_Init(CVBoardTypes BdType, short Link, short BdNum, int32_t *Handle);

/*
        CAENVME_End
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                Notifies the library the end of work and free the allocated
                resources.
*/
CAENVME_API
CAENVME_End(int32_t Handle);

/*
        CAENVME_ReadCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [out] Data      : The data read from the VME bus.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a single VME read cycle.
*/
CAENVME_API
CAENVME_ReadCycle(int32_t Handle, uint32_t Address, void *Data,
                  CVAddressModifier AM, CVDataWidth DW);

/*
        CAENVME_RMWCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]     Handle : The handle that identifies the device.
                [in]     Address: The VME bus address.
                [in/out] Data   : The data read and then written to the VME bus.
                [in]     AM     : The address modifier (see CVAddressModifier enum).
                [in]     DW     : The data width.(see CVDataWidth enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a Read-Modify-Write cycle. The Data parameter
                is bidirectional: it is used to write the value to the VME bus and to
                return the value read.
*/
CAENVME_API
CAENVME_RMWCycle(int32_t Handle, uint32_t Address,  void *Data,
                 CVAddressModifier AM, CVDataWidth DW);

/*
        CAENVME_WriteCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  Data      : The data written to the VME bus.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a single VME write cycle.
*/
CAENVME_API
CAENVME_WriteCycle(int32_t Handle, uint32_t Address, void *Data,
                   CVAddressModifier AM, CVDataWidth DW);

/*
        CAENVME_MultiRead (Ver. 2.2)
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Addrs     : The VME bus addresses.
                [out] Buffer    : The data read from the VME bus.
                [in]  NCycles   : The number of read cycles to perform.
                [in]  AMs       : The address modifiers (see CVAddressModifier enum).
                [in]  DWs       : The data widths.(see CVDataWidth enum).
                [out] ECs       : The error codes relaive to each cycle.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a block of single VME read cycles.
*/
CAENVME_API
CAENVME_MultiRead(int32_t Handle, uint32_t *Addrs, uint32_t *Buffer,
        int NCycles, CVAddressModifier *AMs, CVDataWidth *DWs, CVErrorCodes *ECs);

/*
        CAENVME_MultiWrite (Ver. 2.2)
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Addrs     : The VME bus addresses.
                [in]  Buffer    : The data to write to the VME bus.
                [in]  NCycles   : The number of read cycles to perform.
                [in]  AMs       : The address modifiers (see CVAddressModifier enum).
                [in]  DWs       : The data widths.(see CVDataWidth enum).
                [out] ECs       : The error codes relaive to each cycle.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a block of single VME write cycles.
*/
CAENVME_API
CAENVME_MultiWrite(int32_t Handle, uint32_t *Addrs, uint32_t *Buffer,
        int NCycles, CVAddressModifier *AMs, CVDataWidth *DWs, CVErrorCodes *ECs);

/*
        CAENVME_BLTReadCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [out] Buffer    : The data read from the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME block transfer read cycle. It can be used to
                perform MBLT transfers using 64 bit data width.
*/
CAENVME_API
CAENVME_BLTReadCycle(int32_t Handle, uint32_t Address, void *Buffer,
                     int Size, CVAddressModifier AM, CVDataWidth DW, int *count);

/*
        Ver. 2.3 - New function

        CAENVME_FIFOBLTReadCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [out] Buffer    : The data read from the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME block transfer read cycle. It can be used to
                perform MBLT transfers using 64 bit data width. The Address is not 
                incremented on the VMEBus during the cycle. 
*/
CAENVME_API
CAENVME_FIFOBLTReadCycle(int32_t Handle, uint32_t Address, void *Buffer,
                         int Size, CVAddressModifier AM, CVDataWidth DW, int *count);

/*
        CAENVME_MBLTReadCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [out] Buffer    : The data read from the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME multiplexed block transfer read cycle.
*/
CAENVME_API
CAENVME_MBLTReadCycle(int32_t Handle, uint32_t Address, void *Buffer,
                      int Size, CVAddressModifier AM, int *count);

/*
        Ver. 2.3 - New function
        
        CAENVME_FIFOMBLTReadCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [out] Buffer    : The data read from the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME multiplexed block transfer read cycle.
                The Address is not incremented on the VMEBus during the cycle. 
*/
CAENVME_API
CAENVME_FIFOMBLTReadCycle(int32_t Handle, uint32_t Address, void *Buffer,
                          int Size, CVAddressModifier AM, int *count);

/*
        CAENVME_BLTWriteCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  Buffer    : The data to be written to the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME block transfer write cycle.
*/
CAENVME_API
CAENVME_BLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
                      int size, CVAddressModifier AM, CVDataWidth DW, int *count);

/*
        Ver. 2.3 - New function

        CAENVME_FIFOBLTWriteCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  Buffer    : The data to be written to the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME block transfer write cycle. The address
                is not incremented during the cycle.
*/
CAENVME_API
CAENVME_FIFOBLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
                      int size, CVAddressModifier AM, CVDataWidth DW, int *count);

/*
        CAENVME_MBLTWriteCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  Buffer    : The data to be written to the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME multiplexed block transfer write cycle.
*/
CAENVME_API
CAENVME_MBLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
                       int size, CVAddressModifier AM, int *count);

/*
        Ver. 2.3 - New function
        
        CAENVME_FIFOMBLTWriteCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  Buffer    : The data to be written to the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [out] count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME multiplexed block transfer write cycle.
                The address is not incremented during the cycle.
*/
CAENVME_API
CAENVME_FIFOMBLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
                           int size, CVAddressModifier AM, int *count);

/*
        CAENVME_ADOCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME address only cycle. It can be used to
                perform MBLT transfers using 64 bit data width.
*/
CAENVME_API
CAENVME_ADOCycle(int32_t Handle, uint32_t Address, CVAddressModifier AM);

/*
        CAENVME_ADOHCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME address only with handshake cycle.
*/
CAENVME_API
CAENVME_ADOHCycle(int32_t Handle, uint32_t Address, CVAddressModifier AM);

/*
        CAENVME_IACKCycle
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Level     : The IRQ level to acknowledge (see CVIRQLevels enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a VME interrupt acknowledge cycle.
*/
CAENVME_API
CAENVME_IACKCycle(int32_t Handle, CVIRQLevels Level, void *Vector, CVDataWidth DW);

/*
        CAENVME_IRQCheck
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Mask      : A bit-mask indicating the active IRQ lines.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function returns a bit mask indicating the active IRQ lines.
*/
CAENVME_API
CAENVME_IRQCheck(int32_t Handle, CAEN_BYTE *Mask);

/*
        CAENVME_IRQEnable
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Mask      : A bit-mask indicating the IRQ lines.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function enables the IRQ lines specified by Mask.
*/
CAENVME_API
CAENVME_IRQEnable(int32_t Handle, uint32_t Mask);

/*
        CAENVME_IRQDisable
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Mask      : A bit-mask indicating the IRQ lines.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function disables the IRQ lines specified by Mask.
*/
CAENVME_API
CAENVME_IRQDisable(int32_t Handle, uint32_t Mask);

/*
        CAENVME_IRQWait
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Mask      : A bit-mask indicating the IRQ lines.
                [in]  Timeout   : Timeout in milliseconds.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function wait the IRQ lines specified by Mask until one of
                them raise or timeout expires.
*/
CAENVME_API
CAENVME_IRQWait(int32_t Handle, uint32_t Mask, uint32_t Timeout);

/*
        CAENVME_SetPulserConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  PulSel    : The pulser to configure (see CVPulserSelect enum).
                [in]  Period    : The period of the pulse in time units.
                [in]  Width     : The width of the pulse in time units.
                [in]  Unit      : The time unit for the pulser configuration (see
                                  CVTimeUnits enum).
                [in]  PulseNo   : The number of pulses to generate (0 = infinite).
                [in]  Start     : The source signal to start the pulse burst (see
                                  CVIOSources enum).
                [in]  Reset     : The source signal to stop the pulse burst (see
                                  CVIOSources enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to configure the pulsers. All the timing parameters
                are expressed in the time units specified. The start signal source can be
                one of: front panel button or software (cvManualSW), input signal 0
                (cvInputSrc0),input signal 1 (cvInputSrc1) or input coincidence
                (cvCoincidence). The reset signal source can be: front panel button or
                software (cvManualSW) or, for pulser A the input signal 0 (cvInputSrc0),
                for pulser B the input signal 1 (cvInputSrc1).
*/
CAENVME_API
CAENVME_SetPulserConf(int32_t Handle, CVPulserSelect PulSel, unsigned char Period,
                      unsigned char Width, CVTimeUnits Unit, unsigned char PulseNo,
                      CVIOSources Start, CVIOSources Reset);

/*
        CAENVME_SetScalerConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Limit     : The counter limit for the scaler.
                [in]  AutoReset : Enable/disable the counter auto reset.
                [in]  Hit       : The source signal for the signal to count (see
                                  CVIOSources enum).
                [in]  Gate      : The source signal for the gate (see CVIOSources enum).
                [in]  Reset     : The source signal to stop the counter (see
                                  CVIOSources enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to configure the scaler. Limit range is 0 - 1024
                (10 bit). The hit signal source can be: input signal 0 (cvInputSrc0)
                or input coincidence (cvCoincidence). The gate signal source can be:
                front panel button or software (cvManualSW) or input signal 1
                (cvInputSrc1). The reset signal source can be: front panel button or
                software (cvManualSW) or input signal 1 (cvInputSrc1).
*/
CAENVME_API
CAENVME_SetScalerConf(int32_t Handle, short Limit, short AutoReset,
                      CVIOSources Hit, CVIOSources Gate, CVIOSources Reset);

/*
        CAENVME_SetOutputConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  OutSel    : The output line to configure (see CVOutputSelect
                                  enum).
                [in]  OutPol    : The output line polarity (see CVIOPolarity enum).
                [in]  LEDPol    : The output LED polarity (see CVLEDPolarity enum).
                [in]  Source    : The source signal to propagate to the output line
                                  (see CVIOSources enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to configure the output lines of the module. It
                can be specified the polarity for the line and for the LED. The
                output line source depends on the line as figured out by the
                following table:

                +-----------------------------------------------------------+
                !           S O U R C E      S E L E C T I O N              !
                +--------------+---------------+---------------+------------+
                ! cvVMESignals ! cvCoincidence ! cvMiscSignals ! cvManualSW !
        +---+---+--------------+---------------+---------------+------------+
        !   ! 0 !      DS      ! Input Coinc.  !   Pulser A    ! Manual/SW  !
        ! O +---+--------------+---------------+---------------+------------+
        ! U ! 1 !      AS      ! Input Coinc.  !   Pulser A    ! Manual/SW  !
        ! T +---+--------------+---------------+---------------+------------+
        ! P ! 2 !    DTACK     ! Input Coinc.  !   Pulser B    ! Manual/SW  !
        ! U +---+--------------+---------------+---------------+------------+
        ! T ! 3 !     BERR     ! Input Coinc.  !   Pulser B    ! Manual/SW  !
        !   +---+--------------+---------------+---------------+------------+
        !   ! 4 !     LMON     ! Input Coinc.  !  Scaler end   ! Manual/SW  !
        +---+---+--------------+---------------+---------------+------------+
*/
CAENVME_API
CAENVME_SetOutputConf(int32_t Handle, CVOutputSelect OutSel, CVIOPolarity OutPol,
                      CVLEDPolarity LEDPol, CVIOSources Source);

/*
        CAENVME_SetInputConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  InSel     : The input line to configure (see CVInputSelect enum).
                [in]  InPol     : The input line polarity (see CVIOPolarity enum).
                [in]  LEDPol    : The output LED polarity (see CVLEDPolarity enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to configure the input lines of the module. It
                ca be specified the polarity for the line and for the LED.
*/
CAENVME_API
CAENVME_SetInputConf(int32_t Handle, CVInputSelect InSel, CVIOPolarity InPol,
                     CVLEDPolarity LEDPol);

/*
        CAENVME_GetPulserConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  PulSel    : The pulser to configure (see CVPulserSelect enum).
                [out] Period    : The period of the pulse in time units.
                [out] Width     : The width of the pulse in time units.
                [out] Unit      : The time unit for the pulser configuration (see
                                  CVTimeUnits enum).
                [out] PulseNo   : The number of pulses to generate (0 = infinite).
                [out] Start     : The source signal to start the pulse burst (see
                                  CVIOSources enum).
                [out] Reset     : The source signal to stop the pulse burst (see
                                  CVIOSources enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to read the configuration of the pulsers.
*/
CAENVME_API
CAENVME_GetPulserConf(int32_t Handle, CVPulserSelect PulSel, unsigned char *Period,
                      unsigned char *Width, CVTimeUnits *Unit, unsigned char *PulseNo,
                      CVIOSources *Start, CVIOSources *Reset);

/*
        CAENVME_GetScalerConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Limit     : The counter limit for the scaler.
                [out] AutoReset : The auto reset configuration.
                [out] Hit       : The source signal for the signal to count (see
                                  CVIOSources enum).
                [out] Gate      : The source signal for the gate (see CVIOSources enum).
                [out] Reset     : The source signal to stop the counter (see
                                  CVIOSources enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to read the configuration of the scaler.
*/
CAENVME_API
CAENVME_GetScalerConf(int32_t Handle, short *Limit, short *AutoReset,
                      CVIOSources *Hit, CVIOSources *Gate, CVIOSources *Reset);


/*
        CAENVME_SetOutputConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  OutSel    : The output line to configure (see CVOutputSelect enum).
                [out] OutPol    : The output line polarity (see CVIOPolarity enum).
                [out] LEDPol    : The output LED polarity (see CVLEDPolarity enum).
                [out] Source    : The source signal to propagate to the output line (see
                                  CVIOSources enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to read the configuration of the output lines.
*/
CAENVME_API
CAENVME_GetOutputConf(int32_t Handle, CVOutputSelect OutSel, CVIOPolarity *OutPol,
                      CVLEDPolarity *LEDPol, CVIOSources *Source);

/*
        CAENVME_SetInputConf
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  InSel     : The input line to configure (see CVInputSelect enum).
                [out] InPol     : The input line polarity (see CVIOPolarity enum).
                [out] LEDPol    : The output LED polarity (see CVLEDPolarity enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to read the configuration of the input lines.
*/
CAENVME_API
CAENVME_GetInputConf(int32_t Handle, CVInputSelect InSel, CVIOPolarity *InPol,
                     CVLEDPolarity *LEDPol);

/*
        CAENVME_ReadRegister
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Reg       : The internal register to read (see CVRegisters enum).
                [out] Data      : The data read from the module.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to read all internal registers.
*/
CAENVME_API
CAENVME_ReadRegister(int32_t Handle, CVRegisters Reg, unsigned int *Data);

/*
        CAENVME_WriteRegister
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Reg       : The internal register to read (see CVRegisters enum).
                [in]  Data      : The data to be written to the module.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function permits to write to all internal registers.
*/
CAENVME_API
CAENVME_WriteRegister(int32_t Handle, CVRegisters Reg, unsigned int Data);

/*
        CAENVME_SetOutputRegister
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Mask      : The lines to be set.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the lines specified. Refer the CVOutputRegisterBits
                enum to compose and decoding the bit mask.
*/
CAENVME_API
CAENVME_SetOutputRegister(int32_t Handle, unsigned short Mask);

/*
        CAENVME_ClearOutputRegister
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Mask      : The IRQ lines to be cleared.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function clears the lines specified. Refer the CVOutputRegisterBits
                enum to compose and decoding the bit mask.
*/
CAENVME_API
CAENVME_ClearOutputRegister(int32_t Handle, unsigned short Mask);

/*
        CAENVME_PulseOutputRegister
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Mask      : The lines to be pulsed.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function produces a pulse with the lines specified by setting and
                then clearing them. Refer the CVOutputRegisterBits enum to compose and
                decoding the bit mask.
*/
CAENVME_API
CAENVME_PulseOutputRegister(int32_t Handle, unsigned short Mask);

/*
        CAENVME_ReadDisplay
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The values read from the module (see CVDisplay enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function reads the VME data display on the front panel of the module.
                Refer to the CVDisplay data type definition and comments to decode the
                value returned.
*/
CAENVME_API
CAENVME_ReadDisplay(int32_t Handle, CVDisplay *Value);

/*
        CAENVME_SetArbiterType
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Value     : The type of VME bus arbitration to implement (see
                                  CVArbiterTypes enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the behaviour of the VME bus arbiter on the module.
*/
CAENVME_API
CAENVME_SetArbiterType(int32_t Handle, CVArbiterTypes Value);

/*
        CAENVME_SetRequesterType
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Value     : The type of VME bus requester to implement (see
                                  CVRequesterTypes enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the behaviour of the VME bus requester on the module.
*/
CAENVME_API
CAENVME_SetRequesterType(int32_t Handle, CVRequesterTypes Value);

/*
        CAENVME_SetReleaseType
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Value     : The type of VME bus release policy to implement (see
                                  CVReleaseTypes enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the release policy of the VME bus on the module.
*/
CAENVME_API
CAENVME_SetReleaseType(int32_t Handle, CVReleaseTypes Value);

/*
        CAENVME_SetBusReqLevel
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Value     : The type of VME bus requester priority level to set
                                  (see CVBusReqLevels enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the specified VME bus requester priority level on
                the module.
*/
CAENVME_API
CAENVME_SetBusReqLevel(int32_t Handle, CVBusReqLevels Value);

/*
        CAENVME_SetTimeout
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Value     : The value of VME bus timeout to set (see
                                  CVVMETimeouts enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the specified VME bus timeout on the module.
*/
CAENVME_API
CAENVME_SetTimeout(int32_t Handle, CVVMETimeouts Value);

/*
        CAENVME_SetLocationMonitor
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The addres to be monitored.
                [in]  Write     : Flag to specify read or write cycle types.
                [in]  Lword     : Flag to specify long word cycle type.
                [in]  Iack      : Flag to specify interrupt acknowledge cycle type.

        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the Location Monitor.
*/
CAENVME_API
CAENVME_SetLocationMonitor(int32_t Handle, uint32_t Address, CVAddressModifier Am,
                           short Write, short Lword, short Iack);
/*
        CAENVME_SetFIFOMode
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Value     : Enable/disable the FIFO mode.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function enables/disables the auto increment of the VME addresses
                during the block transfer cycles. With the FIFO mode enabled the
                addresses are not incremented.
*/
CAENVME_API
CAENVME_SetFIFOMode(int32_t Handle, short Value);

/*
        CAENVME_GetArbiterType
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The type of VME bus arbitration implemented (see
                                  CVArbiterTypes enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function get the type of VME bus arbiter implemented on the module.
*/
CAENVME_API
CAENVME_GetArbiterType(int32_t Handle, CVArbiterTypes *Value);

/*
        CAENVME_GetRequesterType
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The type of VME bus requester implemented (see
                                  CVRequesterTypes enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function get the type of VME bus requester implemented on the module.
*/
CAENVME_API
CAENVME_GetRequesterType(int32_t Handle, CVRequesterTypes *Value);

/*
        CAENVME_GetReleaseType
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The type of VME bus release policy implemented (see
                                  CVReleaseTypes enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function get the type of VME bus release implemented on the module.
*/
CAENVME_API
CAENVME_GetReleaseType(int32_t Handle, CVReleaseTypes *Value);

/*
        CAENVME_GetBusReqLevel
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The type of VME bus requester priority level (see
                                  CVBusReqLevels enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function reads the specified VME bus requester priority level on
                the module.
*/
CAENVME_API
CAENVME_GetBusReqLevel(int32_t Handle, CVBusReqLevels *Value);

/*
        CAENVME_GetTimeout
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The value of VME bus timeout (see CVVMETimeouts enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function sets the specified VME bus timeout on the module.
*/
CAENVME_API
CAENVME_GetTimeout(int32_t Handle, CVVMETimeouts *Value);

/*
        CAENVME_GetFIFOMode
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Value     : The FIFO mode read setting.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function read if the auto increment of the VME addresses during the
                block transfer cycles is enabled (0) or disabled (!=0).
*/
CAENVME_API
CAENVME_GetFIFOMode(int32_t Handle, short *Value);

/*
        CAENVME_SystemReset
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function performs a system reset on the module.
*/
CAENVME_API
CAENVME_SystemReset(int32_t Handle);

/*
        CAENVME_ResetScalerCount
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function resets the counter of the scaler..
*/
CAENVME_API
CAENVME_ResetScalerCount(int32_t Handle);

/*
        CAENVME_EnableScalerGate
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function enables the gate of the scaler.
*/
CAENVME_API
CAENVME_EnableScalerGate(int32_t Handle);

/*
        CAENVME_DisableScalerGate
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function disables the gate of the scaler.
*/
CAENVME_API
CAENVME_DisableScalerGate(int32_t Handle);

/*
        CAENVME_StartPulser
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  PulSel    : The pulser to configure (see CVPulserSelect enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function starts the generation of pulse burst if the specified
                pulser is configured for manual/software operation.
*/
CAENVME_API
CAENVME_StartPulser(int32_t Handle, CVPulserSelect PulSel);

/*
        CAENVME_StopPulser
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  PulSel    : The pulser to configure (see CVPulserSelect enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function stops the generation of pulse burst if the specified
                pulser is configured for manual/software operation.
*/
CAENVME_API
CAENVME_StopPulser(int32_t Handle, CVPulserSelect PulSel);

/*
        CAENVME_WriteFlashPage
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Data      : The data to write.
                [in]  PageNum   : The flash page number to write.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function writes the data into the specified flash page.
*/
CAENVME_API
CAENVME_WriteFlashPage(int32_t Handle, unsigned char *Data, int PageNum);

/*
        CAENVME_ReadFlashPage
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Data      : The data to write.
                [in]  PageNum   : The flash page number to write.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function reads the data from the specified flash page.
*/
CAENVME_API
CAENVME_ReadFlashPage(int32_t Handle, unsigned char *Data, int PageNum);

/*
        CAENVME_EraseFlashPage
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  PageNum   : The flash page number to write.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function erases the specified flash page.
*/
CAENVME_API
CAENVME_EraseFlashPage(int32_t Handle, int Pagenum);

#ifdef LINUX
/*
        CAENVME_BLTReadAsync
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [in]  Address   : The VME bus address.
                [out] Buffer    : The data read from the VME bus.
                [in]  Size      : The size of the transfer in bytes.
                [in]  AM        : The address modifier (see CVAddressModifier enum).
                [in]  DW        : The data width.(see CVDataWidth enum).
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function starts a VME block transfer read cycle. It can be used
                to perform MBLT transfers using 64 bit data width.
                This function cannot be used with the V1718 USB bridge.
                This function can be used only on Linux platforms.
                Take care to call the CAENVME_BLTReadWait function before any other
                call to a CAENVMElib function with the same handle.
*/
CAENVME_API
CAENVME_BLTReadAsync(int32_t Handle, uint32_t Address, void *Buffer,
                     int Size, CVAddressModifier AM, CVDataWidth DW);

/*
        CAENVME_BLTReadWait
        -----------------------------------------------------------------------------
        Parameters:
                [in]  Handle    : The handle that identifies the device.
                [out] Count     : The number of bytes transferred.
        -----------------------------------------------------------------------------
        Returns:
                An error code about the execution of the function.
        -----------------------------------------------------------------------------
        Description:
                The function wait for the completion of a VME block transfer read
                cycle started with the CAENVME_BLTReadAsync function call.
                This function cannot be used with the V1718 USB bridge.
                This function can be used only on Linux platforms.
*/
CAENVME_API
CAENVME_BLTReadWait(int32_t Handle, int *Count);

#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __CAENVMELIB_H
