#include <string.h>
#include "CAENDigitizer.h"

#define MAX_X742_CHANNELS		0x08
#define MAX_X742_CHANNEL_SIZE				9
#define FLASH(n)          (0x10D0 | ( n << 8))  // base address of the flash memory (first byte)
#define SEL_FLASH(n)      (0x10CC | ( n << 8))  // flash enable/disable 
#define STATUS(n)         (0x1088 | ( n << 8))  // status register
#define MAIN_MEM_PAGE_READ          0x00D2      
#define MAIN_MEM_PAGE_PROG_TH_BUF1  0x0082   

typedef struct {
	int16_t	 	cell[MAX_X742_CHANNELS+1][1024];
	int8_t	 	nsample[MAX_X742_CHANNELS+1][1024];
	float		time[1024];
} DataCorrection_t;

int32_t LoadCorrectionTables(int handle, DataCorrection_t *Table, uint8_t group, uint32_t frequency);
void ApplyDataCorrection(DataCorrection_t* CTable, CAEN_DGTZ_DRS4Frequency_t frequency, int CorrectionLevelMask, CAEN_DGTZ_X742_GROUP_t *data);
