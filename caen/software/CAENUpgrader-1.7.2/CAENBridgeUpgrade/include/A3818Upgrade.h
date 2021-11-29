#ifndef _A3818UPGRADE_H_
#define _A3818UPGRADE_H_

#ifdef LINUX
#include <inttypes.h>
#include <sys/time.h>
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

#include <stdarg.h>
#include <CAENVMElib.h>

#include <time.h>


#include "p30.h"
#include <a3818.h>

#ifdef LINUX

typedef long            LONG;
typedef unsigned long   ULONG;
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef unsigned char  *PUCHAR;
typedef unsigned int    BOOL;

#endif


typedef enum A3818Upgrade_Mode {
/* Error Types */
 A3818_UPGRADE_FULL               =  0,     
 A3818_UPGRADE_ERASE_ONLY         =  1,    
 A3818_UPGRADE_VERIFY_ONLY        =  2,	
 } A3818Upgrade_Mode;

 
// Expected BIN file size for A3818 with Virtex5 xc5vlx50t
#define BITSTREAM_BYTES 1756544

// Define FIRMWARE regions into flash by using their start address (116-bit word address)
#define FIRST_FIRMWARE_PAGE_BASE_WORD_ADDRESS    0x000000
#define SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS   0x200000
#define THIRD_FIRMWARE_PAGE_BASE_WORD_ADDRESS    0x400000
#define FOURTH_FIRMWARE_PAGE_BASE_WORD_ADDRESS   0x600000


int A3818UpgradeFromFile(int32_t A3818_handle, FILE * binfile, int fwcopy, A3818Upgrade_Mode mode);
int A3818UpgradeFromMem(int32_t A3818_handle, char* fwdata, int fwsize, int fwcopy, A3818Upgrade_Mode mode);

#endif // _A3818UPGRADE_H_

