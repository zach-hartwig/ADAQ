#ifndef _P30_STRATAFLASH_
#define _P30_STRATAFLASH_

#ifdef LINUX
	#include <inttypes.h>
	#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>


#ifdef WIN32
#include <windows.h>
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

#ifndef WIN32
#define FALSE           (0)
#define TRUE            (-1)
#endif
// Numonyx P30 Command set
#define P30_READ_ARRAY               0x00FF
#define P30_READ_ID                  0x0090
#define P30_BUFFERED_PROGRAM         0x00E8
#define P30_BUFFERED_PROGRAM_CONFIRM 0x00D0
#define P30_BLOCK_LOCK_SETUP         0x0060
#define P30_BLOCK_UNLOCK             0x00D0
#define P30_BLOCK_ERASE_SETUP        0x0020
#define P30_BLOCK_ERASE_CONFIRM      0x00D0
#define P30_READ_CFI                 0x0098
#define P30_READ_STATUS              0x0070
#define P30_CLEAR_STATUS             0x0050
  
  
/* Status Register Values */
#define P30_STAT_BLOCK_LOCKED         0x0002
#define P30_STAT_PROGRAM_SUSPENDED    0x0004
#define P30_STAT_VPP_LOW              0x0008
#define P30_STAT_PROGRAM_ERROR        0x0010
#define P30_STAT_ERASE_ERROR          0x0020
#define P30_STAT_ERASE_SUSPENDED      0x0040
#define P30_STAT_STATUS_READY         0x0080

/* P30 Flash parameters */  
#define TOTAL_NUMBLOCKS        0x102
#define MAIN_BLOCK_NUMBYTES    0x20000         /* 64 K-Word block size */
#define PARM_BLOCK_NUMBYTES    0x8000          /* 16 K-Word block size */

#define TOP_PARTITION_BASE     0xFF0000        /* address */
#define BOTTOM_PARTITION_BASE  0x0000          /* address */

#define TOP_BLOCK_LOWER        0xFF
#define TOP_BLOCK_UPPER        0x102

#define BOTTOM_BLOCK_LOWER     0x0
#define BOTTOM_BLOCK_UPPER     0x3


#define TOTAL_SIZE            0x2000000
#define BLOCK_MASK            0xFFFF

#define PRINT_POINTS          1

/* A3818 reg. address */
#define A3818_BPI_FLASH_AD              (0x20)  // R/W - BPI_FLASH TEST
#define A3818_BPI_FLASH_DT              (0x24)  // R/W
#define A3818_BPI_FLASH_CMD             (0x28)  // R/W
#define A3818_DMISCCS                   (0x08)  // Device Miscellaneous Control Status
#       define  A3818_DMISCCS_SPI_BPI_FLASH_SEL (1 << 4) //  BPI/SPI FLASH access select (default 0 = SPI FLASH)


#define mIsStatusReady(A) ( ( A & P30_STAT_STATUS_READY ) == P30_STAT_STATUS_READY )
  
/*******************************************************************/
/* Funzioni per l'accesso alla FLASH BPI */
/*******************************************************************/
// abilita la BPI flash
void A3818_EnableBPIAccess(int32_t handle);

// Disabilita l'accesso alla BPI flash.
void A3818_EnableSPIAccess(int32_t handle);

/*-------------------------------------------------------------*/
/* offset deve essere allineato a 16 bit (BIT0 non significativo) */
int bpi_flash_read(int32_t dev, uint32_t offset, uint32_t* bpi_data);
/*-------------------------------------------------------------*/
int bpi_flash_write(int32_t dev, uint32_t offset, uint32_t bpi_data);
/*-------------------------------------------------------------*/


int readFlash(int32_t dev, uint32_t *data, int address, int length);
unsigned int verifyFlash(uint32_t handle,uint32_t *buffer, uint32_t bufferLength, uint32_t startWordAddress);
int eraseFirmware(uint32_t handle, uint32_t baseAddress, uint32_t fwcopy);
void writeFlash(uint32_t handle, uint32_t *buffer, uint32_t bufferLength, uint32_t startWordAddress);

#endif   // _P30_STRATAFLASH_
