#ifndef _FLASH_H
#define _FLASH_H

#ifdef _WIN32
#include <windows.h>
#else 
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#endif

#include <CAENVMElib.h>

/*###########################################################################*/
/*
** DEFINITIONS
*/
/*###########################################################################*/
#ifdef WIN32
#define Wait_ms(t)   Sleep(t)
#else
#define Wait_ms(t)   usleep((t)*1000)
#endif

#define CAENCOM_INVALID_FILE_HANDLE		-1
// Opcodes for the flash memory
#define MAIN_MEM_PAGE_READ_CMD          0xD2
#define MAIN_MEM_PAGE_PROG_TH_BUF1_CMD  0x82
#define STATUS_READ_CMD                 0xD7

#define ROM_FLASH_PAGE					0
#define LICENSE_FLASH_PAGE				2
#define KEY_SIZE						64          // Number of bytes for key storage
#define LICENSE_DATA_SIZE				8			// Number of bytes for the licence data
#define MAX_MULTIACCESS_BUFSIZE         2048

// Atmel AT45DB081D specific constants
typedef enum AT45_OPCODES
{
  AT45_PAGE_SIZE                        =  264,    // Number of bytes per page in the target flash
  AT45_IDREG_LENGTH                     =  128,    // Lunghezza (in bytes) del registro ID della flash Atmel
  AT45_MAIN_MEM_PAGE_READ_OPCODE        =  0x00D2,
  AT45_MAIN_MEM_PAGE_PROG_TH_BUF1_OPCODE=  0x0082,
  AT45_READ_SECURITY_REGISTER_OPCODE    =  0x0077,
  AT45_STATUS_READ_CMD_OPCODE           =  0x00D7,
} AT45_OPCODES;

typedef struct CROM_MAP
{   
    uint8_t  crom_chksum;         /* Configuration ROM checksum */
    uint32_t crom_chksum_len;     /* Configuration ROM checksum length */
    uint32_t crom_const;          /* Configuration ROM constant */
    uint8_t  crom_c_code;         /* Configuration ROM c_code */
    uint8_t  crom_r_code;         /* Configuration ROM r_code */
    uint32_t crom_OUI;            /* Configuration ROM Manufacturer identifier (IEEE OUI) */
    uint8_t  crom_version;        /* Configuration ROM Purchased version of the Mod.V1724 */
    uint32_t crom_board_id;       /* Configuration ROM Board identifier \ref CVT_V17XX_ROM_CFG_BOARD_ID */
    uint32_t crom_revision;       /* Configuration ROM Hardware revision identifier */
    uint16_t crom_serial;         /* Configuration ROM Serial number */
    uint8_t  crom_VCXO_type;      /* Configuration ROM VCXO type */
} CROM_MAP;



typedef struct FlashAccess {
  int            Handle;         // CAENComm handle for board access
  unsigned long  RW_Flash;       // Address of the R/W Flash Register
  unsigned long  Sel_Flash;      // Address of the Select Flash Register
  unsigned long  FlashEnable;    // Flash Enable pin polarity
  unsigned int   RegSize;        // Register Data Size (2 = D16, 4 = D32)
  unsigned int   PageSize;       // Flash Page Size
} cvFlashAccess;

typedef enum cvupgrade_ErrorCode {
/* Error Types */
 CvUpgrade_Success		      =  0,     /* Operation completed successfully              	*/
 CvUpgrade_GenericError       = -1,     /* Generic error                        			*/
 CvUpgrade_CAENCommError      = -2,     /* CAENComm function call error        			*/
 CvUpgrade_UnsupportedRegSize = -3,     /* Unsupported Register Size from parameter file	*/
 CvUpgrade_FileAccessError    = -4      /* File access	error */
} cvupgrade_ErrorCode;


int ReadFlashPage (cvFlashAccess *Flash, uint8_t *data, int pagenum);
int WriteFlashPage(cvFlashAccess *Flash, uint8_t *data, int pagenum);
int ReadFlashSecurityReg(cvFlashAccess *Flash, uint8_t *data);

#endif //  _FLASH_H
