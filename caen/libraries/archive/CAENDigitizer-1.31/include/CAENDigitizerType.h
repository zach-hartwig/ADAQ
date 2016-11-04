
/*                                                                         */
/*                                                                         */
/*    Source code written in ANSI C                                        */
/*                                                                         */ 
/*    May      20009:            - Created.                                */
/*                                                                         */
/***************************************************************************/

#ifndef __CAENDIGITIZERTYPE_H
#define __CAENDIGITIZERTYPE_H

#ifdef WIN32
	#include <windows.h>
	#define CAENDGTZ_API __stdcall
#else 
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <stdint.h> 
	#define CAENDGTZ_API
#endif

#ifdef WIN32
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


#define MAX_UINT16_CHANNEL_SIZE				64
#define MAX_UINT8_CHANNEL_SIZE				8
#define MAX_V1724DPP_CHANNEL_SIZE			8
#define MAX_V1720DPP_CHANNEL_SIZE			8
#define MAX_X742_CHANNEL_SIZE				9
#define MAX_X742_GROUP_SIZE					4

/******************************************************************************
* Digitizer Registers Address Map 
******************************************************************************/

#define CAEN_DGTZ_MULTI_EVENT_BUFFER					0x0000

#define CAEN_DGTZ_CHANNEL_ZS_THRESHOLD_BASE_ADDRESS					0x1024
#define CAEN_DGTZ_CHANNEL_ZS_NSAMPLE_BASE_ADDRESS						0x1028
#define CAEN_DGTZ_CHANNEL_THRESHOLD_BASE_ADDRESS						0x1080
#define CAEN_DGTZ_CHANNEL_OV_UND_TRSH_BASE_ADDRESS					0x1084
#define CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS							0x1088
#define CAEN_DGTZ_CHANNEL_AMC_FPGA_FW_BASE_ADDRESS					0x108C
#define CAEN_DGTZ_CHANNEL_BUFFER_OCC_BASE_ADDRESS						0x1094
#define CAEN_DGTZ_CHANNEL_DAC_BASE_ADDRESS									0x1098
#define CAEN_DGTZ_CHANNEL_GROUP_V1740_BASE_ADDRESS					0x10A8
#define CAEN_DGTZ_GROUP_FASTTRG_THR_V1742_BASE_ADDRESS			0x10D4
#define CAEN_DGTZ_GROUP_FASTTRG_DCOFFSET_V1742_BASE_ADDRESS	0x10DC
#define CAEN_DGTZ_DRS4_FREQUENCY_REG												0x10D8

#define CAEN_DGTZ_BROAD_CH_CTRL_ADD						0x8000
#define CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD			0x8004
#define CAEN_DGTZ_BROAD_CH_CLEAR_CTRL_ADD				0x8008
#define CAEN_DGTZ_BROAD_NUM_BLOCK_ADD					0x800C
#define CAEN_DGTZ_CUSTOM_SIZE_REG						0x8020
#define CAEN_DGTZ_DRS4_FREQUENCY_REG_WRITE			0x80D8
#define CAEN_DGTZ_ACQ_CONTROL_ADD						0x8100
#define CAEN_DGTZ_ACQ_STATUS_ADD						0x8104
#define CAEN_DGTZ_SW_TRIGGER_ADD						0x8108
#define CAEN_DGTZ_TRIGGER_SRC_ENABLE_ADD				0x810C
#define CAEN_DGTZ_FP_TRIGGER_OUT_ENABLE_ADD				0x8110
#define CAEN_DGTZ_POST_TRIG_ADD							0x8114				
#define CAEN_DGTZ_FRONT_PANEL_IO_ADD					0x8118			
#define CAEN_DGTZ_FRONT_PANEL_IO_CTRL_ADD				0x811C		
#define CAEN_DGTZ_CH_ENABLE_ADD							0x8120			
#define CAEN_DGTZ_FW_REV_ADD							0x8124			
#define CAEN_DGTZ_DOWNSAMPLE_FACT_ADD					0x8128				
#define CAEN_DGTZ_EVENT_STORED_ADD						0x812C		
#define CAEN_DGTZ_MON_SET_ADD							0x8138				
#define CAEN_DGTZ_BOARD_INFO_ADD						0x8140
#define CAEN_DTGZ_EVENT_SIZE_ADD						0x814C
#define CAEN_DGTZ_MON_MODE_ADD							0x8144			
#define CAEN_DGTZ_ANALOG_MON_ADD						0x8150

#define CAEN_DGTZ_VME_CONTROL_ADD						0xEF00				
#define CAEN_DGTZ_VME_STATUS_ADD						0xEF04				
#define CAEN_DGTZ_BOARD_ID_ADD							0xEF08				
#define CAEN_DGTZ_MCST_CBLT_ADD_CTRL_ADD				0xEF0C				
#define CAEN_DGTZ_RELOCATION_ADDRESS_ADD				0xEF10				
#define CAEN_DGTZ_INT_STATUS_ID_ADD						0xEF14				
#define CAEN_DGTZ_INT_EVENT_NUM_ADD						0xEF18				
#define CAEN_DGTZ_BLT_EVENT_NUM_ADD						0xEF1C				
#define CAEN_DGTZ_SCRATCH_ADD							0xEF20				
#define CAEN_DGTZ_SW_RESET_ADD							0xEF24				
#define CAEN_DGTZ_SW_CLEAR_ADD							0xEF28				
#define CAEN_DGTZ_FLASH_EN_ADD							0xEF2C				
#define CAEN_DGTZ_FLASH_DATA_ADD						0xEF30				
#define CAEN_DGTZ_RELOAD_CONFIG_ADD						0xEF34

#define CAEN_DGTZ_ROM_CHKSUM_ADD						0xF000				
#define CAEN_DGTZ_ROM_CHKSUM_LEN_2_ADD					0xF004				
#define CAEN_DGTZ_ROM_CHKSUM_LEN_1_ADD					0xF008				
#define CAEN_DGTZ_ROM_CHKSUM_LEN_0_ADD					0xF00C				
#define CAEN_DGTZ_ROM_CONST_2_ADD						0xF010				
#define CAEN_DGTZ_ROM_CONST_1_ADD						0xF014			
#define CAEN_DGTZ_ROM_CONST_0_ADD						0xF018			
#define CAEN_DGTZ_ROM_C_CODE_ADD						0xF01C			
#define CAEN_DGTZ_ROM_R_CODE_ADD						0xF020			
#define CAEN_DGTZ_ROM_OUI_2_ADD							0xF024				
#define CAEN_DGTZ_ROM_OUI_1_ADD							0xF028			
#define CAEN_DGTZ_ROM_OUI_0_ADD							0xF02C			
#define CAEN_DGTZ_ROM_VERSION_ADD						0xF030				
#define CAEN_DGTZ_ROM_BOARD_ID_2_ADD					0xF034				
#define CAEN_DGTZ_ROM_BOARD_ID_1_ADD					0xF038			
#define CAEN_DGTZ_ROM_BOARD_ID_0_ADD					0xF03C			
#define CAEN_DGTZ_ROM_REVISION_3_ADD					0xF040			
#define CAEN_DGTZ_ROM_REVISION_2_ADD					0xF044				
#define CAEN_DGTZ_ROM_REVISION_1_ADD					0xF048				
#define CAEN_DGTZ_ROM_REVISION_0_ADD					0xF04C				
#define CAEN_DGTZ_ROM_SERIAL_1_ADD						0xF080				
#define CAEN_DGTZ_ROM_SERIAL_0_ADD						0xF084				
#define CAEN_DGTZ_ROM_VCXO_TYPE_ADD						0xF088				


// INDIVIDUAL CHANNEL ADDRESSING
#define CAEN_DGTZ_InputDCOffsetReg_Ch(x)      (0x1098 | ((x)<<8)) ///< Input DC offset adjust Register Indiv. Address 
#define CAEN_DGTZ_ChannelFWRevisionReg_Ch(x)  (0x108C | ((x)<<8)) ///< Channel FW Revision Register Indiv. Address 
#define CAEN_DGTZ_DPP1Reg_Ch(x)               (0x1024 | ((x)<<8)) ///< DPP1 Param Register Indiv. Address
#define CAEN_DGTZ_DPP2Reg_Ch(x)               (0x1028 | ((x)<<8)) ///< DPP2 Param Register Indiv. Address
#define CAEN_DGTZ_DPP3Reg_Ch(x)               (0x102C | ((x)<<8)) ///< DPP3 Param Register Indiv. Address


/*###########################################################################*/
/*
** ErrorCode
*/
/*###########################################################################*/

typedef enum CAEN_DGTZ_ErrorCode {
CAEN_DGTZ_Success						=	 0L,    /* Operation completed successfully             */
CAEN_DGTZ_CommError						=	-1L,    /* Communication error                          */
CAEN_DGTZ_GenericError					=	-2L,    /* Unspecified error                            */
CAEN_DGTZ_InvalidParam					=	-3L,    /* Invalid parameter                            */
CAEN_DGTZ_InvalidLinkType				=	-4L,    /* Invalid Link Type                            */
CAEN_DGTZ_InvalidHandle					=	-5L,    /* Invalid device handle                        */
CAEN_DGTZ_MaxDevicesError				=	-6L,    /* Maximum number of devices exceeded           */
CAEN_DGTZ_BadBoardType					=	-7L,    /* The operation is not allowed on this type of board           */
CAEN_DGTZ_BadInterruptLev				=	-8L,    /* The interrupt level is not allowed			*/
CAEN_DGTZ_BadEventNumber				=	-9L,    /* The event number is bad          				*/
CAEN_DGTZ_ReadDeviceRegisterFail		=   -10L,   /* Unable to read the registry     				*/
CAEN_DGTZ_WriteDeviceRegisterFail		=   -11L,   /* Unable to write into the registry    			*/
CAEN_DGTZ_InvalidChannelNumber			=   -13L,   /* The channel number is invalid     			*/
CAEN_DGTZ_ChannelBusy					=	-14L,   /* The Channel is busy				   			*/
CAEN_DGTZ_FPIOModeInvalid				=	-15L,   /* Invalid FPIO Mode				    			*/
CAEN_DGTZ_WrongAcqMode					=	-16L,   /* Wrong acquisition mode		    			*/
CAEN_DGTZ_FunctionNotAllowed			=	-17L,   /* This function is not allowed for this module	*/
CAEN_DGTZ_Timeout						=	-18L,   /* Communication Timeout			    			*/
CAEN_DGTZ_InvalidBuffer					=	-19L,   /* The buffer is invalid 		    			*/
CAEN_DGTZ_EventNotFound					=	-20L,   /* The event is not found		    			*/
CAEN_DGTZ_InvalidEvent					=	-21L,   /* The vent is invalid			    			*/
CAEN_DGTZ_OutOfMemory					=	-22L,   /* Out of memory					    			*/
CAEN_DGTZ_CalibrationError				=	-23L,   /* Unable to calibrate the board	    			*/
CAEN_DGTZ_DigitizerNotFound				=	-24L,   /* Unable to open the digitizer	    			*/
CAEN_DGTZ_DigitizerAlreadyOpen			=	-25L,   /* The Digitizer is already open					*/	
CAEN_DGTZ_DigitizerNotReady				=	-26L,   /* The Digitizer is not ready to operate			*/	
CAEN_DGTZ_InterruptNotConfigured		= -27L,  /* The Digitizer has not the IRQ configured			*/
CAEN_DGTZ_DigitizerMemoryCorrupted		= -28L, /* The digitizer flash memory is corrupted		*/
CAEN_DGTZ_NotYetImplemented				=	-99L,   /* The function is not yet implemented			*/

}CAEN_DGTZ_ErrorCode; 

#define CAEN_DGTZ_MAX_CHANNEL			8			/*!< \brief The number of channels */


/*###########################################################################*/
/*
** ConnectionType
*/
/*###########################################################################*/

typedef enum CAEN_DGTZ_ConnectionType {
CAEN_DGTZ_USB	    				= 0L,
CAEN_DGTZ_PCI_OpticalLink			= 1L,
CAEN_DGTZ_PCIE_OpticalLink			= 2L,
CAEN_DGTZ_PCIE_EmbeddedDigitizer	= 3L,
} CAEN_DGTZ_ConnectionType;


/* Digitizers Model */
typedef enum
{
	CAEN_DGTZ_V1724			=0L,						/*!< \brief The board is V1724  */
	CAEN_DGTZ_V1721			=1L,						/*!< \brief The board is V1721  */
	CAEN_DGTZ_V1731			=2L,						/*!< \brief The board is V1731  */
	CAEN_DGTZ_V1720			=3L,						/*!< \brief The board is V1720  */
	CAEN_DGTZ_V1740			=4L,						/*!< \brief The board is V1740  */
	CAEN_DGTZ_V1751			=5L,						/*!< \brief The board is V1751  */
	CAEN_DGTZ_DT5724		=6L,					    /*!< \brief The board is DT5724 */
	CAEN_DGTZ_DT5721		=7L,					    /*!< \brief The board is DT5721 */
	CAEN_DGTZ_DT5731		=8L,					    /*!< \brief The board is DT5731 */
	CAEN_DGTZ_DT5720		=9L,					    /*!< \brief The board is DT5720 */
	CAEN_DGTZ_DT5740		=10L,					/*!< \brief The board is DT5740 */
	CAEN_DGTZ_DT5751		=11L,					/*!< \brief The board is DT5751 */
	CAEN_DGTZ_N6724		    =12L,					/*!< \brief The board is N6724  */
	CAEN_DGTZ_N6721		    =13L,					/*!< \brief The board is N6721  */
	CAEN_DGTZ_N6731		    =14L,					/*!< \brief The board is N6731  */
	CAEN_DGTZ_N6720		    =15L,					/*!< \brief The board is N6720  */
	CAEN_DGTZ_N6740		    =16L,					/*!< \brief The board is N6740  */
	CAEN_DGTZ_N6751		    =17L,					/*!< \brief The board is N6751  */
	CAEN_DGTZ_DT5742		=18L,
	CAEN_DGTZ_N6742			=19L,
	CAEN_DGTZ_V1742			=20L,
} CAEN_DGTZ_BoardModel_t;

typedef enum {
    CAEN_DGTZ_VME64_FORM_FACTOR   = 0L,
    CAEN_DGTZ_VME64X_FORM_FACTOR  = 1L,
    CAEN_DGTZ_DESKTOP_FORM_FACTOR = 2L,
    CAEN_DGTZ_NIM_FORM_FACTOR     = 3L,
} CAEN_DGTZ_BoardFormFactor_t;

typedef enum {
    CAEN_DGTZ_XX724_FAMILY_CODE  = 0L,
    CAEN_DGTZ_XX721_FAMILY_CODE  = 1L,
    CAEN_DGTZ_XX731_FAMILY_CODE  = 2L,
    CAEN_DGTZ_XX720_FAMILY_CODE  = 3L,
    CAEN_DGTZ_XX740_FAMILY_CODE  = 4L,
    CAEN_DGTZ_XX751_FAMILY_CODE  = 5L,
    CAEN_DGTZ_XX742_FAMILY_CODE  =  6L, 
} CAEN_DGTZ_BoardFamilyCode_t;

typedef enum CAEN_DGTZ_DPP_PARAMETER
{
	CAEN_DGTZ_DPP_Param_m					= 0L,
	CAEN_DGTZ_DPP_Param_M					= 1L,
	CAEN_DGTZ_DPP_Param_Delta1				= 2L,
	CAEN_DGTZ_DPP_Param_a					= 3L,
	CAEN_DGTZ_DPP_Param_b					= 4L,
	CAEN_DGTZ_DPP_Param_NSBaseline		= 5L,
	CAEN_DGTZ_DPP_Param_shf					= 6L,
	CAEN_DGTZ_DPP_Param_k					= 7L,
	CAEN_DGTZ_DPP_Param_NSPeakMean	= 8L,
	CAEN_DGTZ_DPP_Param_FlatTopDelay	= 9L,
	CAEN_DGTZ_DPP_Param_Decimation		= 10L,
	CAEN_DGTZ_DPP_Param_TrgThreshold	= 11L,
	CAEN_DGTZ_DPP_Param_TrgWinOffset	= 12L,
	CAEN_DGTZ_DPP_Param_TrgWinWidth	= 13L,
	CAEN_DGTZ_DPP_Param_DigitalGain		= 14L,
	CAEN_DGTZ_DPP_Param_GateWidth		= 15L,
	CAEN_DGTZ_DPP_Param_PreGate			= 16L,
	CAEN_DGTZ_DPP_Param_HoldOffTime		= 17L,
	CAEN_DGTZ_DPP_Param_BslThreshold	= 18L,
	CAEN_DGTZ_DPP_Param_NoFlatTime		= 19L,
	CAEN_DGTZ_DPP_Param_GateMode		= 20L,
	CAEN_DGTZ_DPP_Param_InvertInput		= 21L,
} CAEN_DGTZ_DPP_PARAMETER_t;

typedef enum
{
	CAEN_DGTZ_FPIO_MODES_GPIO			= 0L,			/*!< General purpose IO */
	CAEN_DGTZ_FPIO_MODES_PROGIO			= 1L,			/*!< Programmed IO */
	CAEN_DGTZ_FPIO_MODES_PATTERN		= 2L,			/*!< Pattern mode */
} CAEN_DGTZ_FrontPanelIOModes;

typedef enum
{
	CAEN_DGTZ_TRGMODE_DISABLED				= 0L,
	CAEN_DGTZ_TRGMODE_EXTOUT_ONLY			= 2L,
	CAEN_DGTZ_TRGMODE_ACQ_ONLY				= 1L,
	CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT		= 3L,
}CAEN_DGTZ_TriggerMode_t;

typedef enum {
	CAEN_DGTZ_TRIGGER					 = 0L,
	CAEN_DGTZ_FASTTRG_ALL				 = 1L,
	CAEN_DGTZ_FASTTRG_ACCEPTED	 		 = 2L,
	CAEN_DGTZ_BUSY					     = 3L,
	CAEN_DGTZ_CLK_OUT					 = 4L,
	CAEN_DGTZ_RUN					     = 5L,
	CAEN_DGTZ_TRGPULSE					 = 6L,	
	CAEN_DGTZ_OVERTHRESHOLD		         =	7L,					
} CAEN_DGTZ_OutputSignalMode_t;

typedef enum
{
	CAEN_DGTZ_ZS_NO						= 0L,
	CAEN_DGTZ_ZS_INT					= 1L,
	CAEN_DGTZ_ZS_ZLE					= 2L,
	CAEN_DGTZ_ZS_AMP					= 3L,
} CAEN_DGTZ_ZS_Mode_t;

typedef enum
{
	CAEN_DGTZ_ENABLE					= 1L,
	CAEN_DGTZ_DISABLE					= 0L,
} CAEN_DGTZ_EnaDis_t;

typedef enum 
{
	CAEN_DGTZ_ZS_FINE					= 0L,
	CAEN_DGTZ_ZS_COARSE					= 1L,
}CAEN_DGTZ_ThresholdWeight_t;			


typedef enum
{
	CAEN_DGTZ_SW_CONTROLLED				= 0L,
	CAEN_DGTZ_S_IN_CONTROLLED			= 1L,
}CAEN_DGTZ_AcqMode_t;

typedef enum
{
	CAEN_DGTZ_AM_TRIGGER_MAJORITY		= 0L,
	CAEN_DGTZ_AM_TEST					= 1L,
	CAEN_DGTZ_AM_ANALOG_INSPECTION		= 2L,
	CAEN_DGTZ_AM_BUFFER_OCCUPANCY		= 3L,
	CAEN_DGTZ_AM_VOLTAGE_LEVEL			= 4L,
}CAEN_DGTZ_AnalogMonitorOutputMode_t;

typedef enum 
{
	CAEN_DGTZ_AM_MAGNIFY_1X				= 0L,
	CAEN_DGTZ_AM_MAGNIFY_2X				= 1L,
	CAEN_DGTZ_AM_MAGNIFY_4X				= 2L,
	CAEN_DGTZ_AM_MAGNIFY_8X				= 3L,
}CAEN_DGTZ_AnalogMonitorMagnify_t;			


typedef enum 
{
	CAEN_DGTZ_AM_INSPECTORINVERTER_P_1X	 = 0L,
	CAEN_DGTZ_AM_INSPECTORINVERTER_N_1X  = 1L,
}CAEN_DGTZ_AnalogMonitorInspectorInverter_t;

typedef enum 
{
	CAEN_DGTZ_IRQ_MODE_RORA				 = 0L,
	CAEN_DGTZ_IRQ_MODE_ROAK				 = 1L,
}CAEN_DGTZ_IRQMode_t;

typedef enum
{
	CAEN_DGTZ_IRQ_DISABLED				= 0L,
	CAEN_DGTZ_IRQ_ENABLED_OPTICAL		= 1L,
	CAEN_DGTZ_IRQ_ENABLED_VME_RORA		= 1L,
	CAEN_DGTZ_IRQ_ENABLED_VME_ROAK		= 2L,
} CAEN_DGTZ_IRQState_t;

typedef enum 
{
	CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT		= 0L,
	CAEN_DGTZ_SLAVE_TERMINATED_READOUT_2eVME 	= 1L,
	CAEN_DGTZ_SLAVE_TERMINATED_READOUT_2eSST 	= 2L,
	CAEN_DGTZ_POLLING_MBLT					 					= 3L,
	CAEN_DGTZ_POLLING_2eVME										= 4L,
	CAEN_DGTZ_POLLING_2eSST										= 5L,
} CAEN_DGTZ_ReadMode_t;

typedef enum
{
	CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope			= 0L,
	CAEN_DGTZ_DPP_ACQ_MODE_List						= 1L,
	CAEN_DGTZ_DPP_ACQ_MODE_Mixed					= 2L,
} CAEN_DGTZ_DPP_AcqMode_t;

typedef enum
{
	CAEN_DGTZ_DPP_CI_GPO_Gate					= 0L,
	CAEN_DGTZ_DPP_CI_GPO_Discri				= 1L,
	CAEN_DGTZ_DPP_CI_GPO_Coincidence		= 2L,
} CAEN_DGTZ_DPP_CI_GPO_SEL_t;

typedef enum {
	CAEN_DGTZ_DPP_Channel_0					= 0L,
	CAEN_DGTZ_DPP_Channel_1					= 1L,
	CAEN_DGTZ_DPP_Channel_2					= 2L,
	CAEN_DGTZ_DPP_Channel_3					= 3L,
	CAEN_DGTZ_DPP_Channel_4					= 4L,
	CAEN_DGTZ_DPP_Channel_5					= 5L,
	CAEN_DGTZ_DPP_Channel_6					= 6L,
	CAEN_DGTZ_DPP_Channel_7					= 7L,
	CAEN_DGTZ_DPP_Channel_ALL				= 255L,
} CAEN_DGTZ_DPP_Channel_t;

typedef enum
{
	CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE		= 0L,
	CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL			= 1L,
} CAEN_DGTZ_DPP_VirtualProbe_t;

typedef enum
{
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_trgKln		= 0L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_Armed		= 1L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_PkRun		= 2L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_PkAbort		= 3L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_Peaking		= 4L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_PkHoldOff	= 5L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_Flat			= 6L,
	CAEN_DGTZ_DPP_TF_DIGITAL_PROBE_trgHoldOff	= 7L,
} CAEN_DGTZ_DPP_TF_DigitalProbe_t;

typedef enum
{
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE1_Input		= 0L,
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE1_Delta		= 1L,
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE1_Delta2		= 2L,
	CAEN_DGTZ_DPP_TF_VirtuallProbe1_trapezoid	= 3L,
} CAEN_DGTZ_DPP_TF_VirtuallProbe1_t;

typedef enum
{
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE2_Input		= 0L,
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE2_S3			= 1L,
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE2_DigitalCombo	= 2L,
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE2_trapBaseline	= 3L,
	CAEN_DGTZ_DPP_TF_VIRTUALPROBE2_None			= 4L,
} CAEN_DGTZ_DPP_TF_VirtuallProbe2_t;

typedef enum 
{
	CAEN_DGTZ_DPP_CI_VIRTUALPROBE2_Delta		= 0L,
	CAEN_DGTZ_DPP_CI_VIRTUALPROBE2_Baseline		= 1L,	
} CAEN_DGTZ_DPP_CI_VirtuallProbe2_t;

typedef enum
{
	CAEN_DGTZ_DPP_SAVE_PARAM_EnergyOnly		= 0L,
	CAEN_DGTZ_DPP_SAVE_PARAM_TimeOnly		= 1L,
	CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime	= 2L,
	CAEN_DGTZ_DPP_SAVE_PARAM_ChargeAndTime  = 4L,
	CAEN_DGTZ_DPP_SAVE_PARAM_None			= 3L,
} CAEN_DGTZ_DPP_SaveParam_t;

typedef enum {
	CAEN_DGTZ_IOLevel_NIM		= 0L,
	CAEN_DGTZ_IOLevel_TTL		= 1L,
} CAEN_DGTZ_IOLevel_t;

typedef enum {
	CAEN_DGTZ_DRS4_5GHz		= 0L,
	CAEN_DGTZ_DRS4_2_5GHz	= 1L,
	CAEN_DGTZ_DRS4_1GHz		= 2L,
} CAEN_DGTZ_DRS4Frequency_t;


typedef enum {
	CAEN_DGTZ_TriggerOnRisingEdge		= 0L,
	CAEN_DGTZ_TriggerOnFallingEdge		= 1L,
} CAEN_DGTZ_TriggerPolarity_t;

typedef struct {
	char						ModelName[12];
	uint32_t					Model;
	uint32_t					Channels;
	uint32_t					FormFactor;
	uint32_t					FamilyCode;
	char						ROC_FirmwareRel[20];
	char						AMC_FirmwareRel[20];
	uint32_t					SerialNumber;
	uint32_t					PCB_Revision;
	uint32_t					ADC_NBits;
} CAEN_DGTZ_BoardInfo_t;

typedef struct 
{
	uint32_t			 EventSize;
	uint32_t			 BoardId;
	uint32_t			 Pattern;
	uint32_t			 ChannelMask;
	uint32_t			 EventCounter;
	uint32_t			 TriggerTimeTag;
} CAEN_DGTZ_EventInfo_t;

typedef struct 
{
	uint32_t				 ChSize[MAX_X742_CHANNEL_SIZE]; 		  // the number of samples stored in DataChannel array  
	float					 *DataChannel[MAX_X742_CHANNEL_SIZE];  // the array of ChSize samples
	uint32_t				 TriggerTimeTag;
	uint16_t				 StartIndexCell;
} CAEN_DGTZ_X742_GROUP_t;

typedef struct 
{
	uint32_t			ChSize[MAX_UINT16_CHANNEL_SIZE]; // the number of samples stored in DataChannel array  
	uint16_t			*DataChannel[MAX_UINT16_CHANNEL_SIZE]; // the array of ChSize samples
} CAEN_DGTZ_UINT16_EVENT_t;

typedef struct 
{
	uint32_t			ChSize[MAX_UINT8_CHANNEL_SIZE]; // the number of samples stored in DataChannel array  
	uint8_t				*DataChannel[MAX_UINT8_CHANNEL_SIZE];  // the array of ChSize samples
} CAEN_DGTZ_UINT8_EVENT_t;

typedef struct 
{
	uint8_t					GrPresent[MAX_X742_GROUP_SIZE]; // If the group has data the value is 1 otherwise is 0  
	CAEN_DGTZ_X742_GROUP_t	DataGroup[MAX_X742_GROUP_SIZE]; // the array of ChSize samples
} CAEN_DGTZ_X742_EVENT_t;

typedef struct 
{
	uint32_t			ChTraceSize[MAX_V1724DPP_CHANNEL_SIZE];
	uint32_t			ChTimeTagSize[MAX_V1724DPP_CHANNEL_SIZE];
	uint16_t			*DataTraceA[MAX_V1724DPP_CHANNEL_SIZE];
	uint16_t			*DataTraceB[MAX_V1724DPP_CHANNEL_SIZE];
	uint8_t				*DigitalTraceC[MAX_V1724DPP_CHANNEL_SIZE];
	uint8_t				*DigitalTraceD[MAX_V1724DPP_CHANNEL_SIZE];
	uint32_t			*TimeTag[MAX_V1724DPP_CHANNEL_SIZE];
	uint16_t			*Energy[MAX_V1724DPP_CHANNEL_SIZE];
} CAEN_DGTZ_V1724DPP_TF_EVENT_t;

typedef struct 
{
	uint32_t			ChInputSize[MAX_V1720DPP_CHANNEL_SIZE];
	uint16_t			*Input[MAX_V1720DPP_CHANNEL_SIZE];
	uint16_t			*VirtualProbe[MAX_V1720DPP_CHANNEL_SIZE];
	uint8_t			*Flat[MAX_V1720DPP_CHANNEL_SIZE];
	uint8_t			*HoldOff[MAX_V1720DPP_CHANNEL_SIZE];
	uint8_t			*OverThreshold[MAX_V1720DPP_CHANNEL_SIZE];
	uint8_t			*Gate[MAX_V1720DPP_CHANNEL_SIZE];
	uint32_t			ChTimeTagSize[MAX_V1720DPP_CHANNEL_SIZE];
	uint32_t			*TimeTag[MAX_V1720DPP_CHANNEL_SIZE];
	int32_t			*Charge[MAX_V1720DPP_CHANNEL_SIZE];
} CAEN_DGTZ_V1720DPP_CI_EVENT_t;
#endif

