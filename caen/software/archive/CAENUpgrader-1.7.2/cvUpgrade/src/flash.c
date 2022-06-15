#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "flash.h"
#include "CAENComm.h"

static int GetFlashStatus32(cvFlashAccess *Flash, uint8_t *Status)
{
    int res = 0;
    uint32_t reg32;


	// Enable flash
    res |= CAENComm_Write32(Flash->Handle,Flash->Sel_Flash,Flash->FlashEnable);
	// Status read Command
	res |= CAENComm_Write32(Flash->Handle, Flash->RW_Flash, STATUS_READ_CMD);
	// Status read
	res |= CAENComm_Read32(Flash->Handle, Flash->RW_Flash, &reg32);
	// Disable flash 
	res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));
	*Status = (uint8_t)(reg32 & 0xFF);

    return res;

}

static int GetFlashStatus16(cvFlashAccess *Flash, uint8_t *Status)
{
    int res = 0;
    uint16_t reg16;


	// Enable flash
    res |= CAENComm_Write16(Flash->Handle,Flash->Sel_Flash,Flash->FlashEnable);
	// Status read Command
	res |= CAENComm_Write16(Flash->Handle, Flash->RW_Flash, STATUS_READ_CMD);
	// Status read
	res |= CAENComm_Read16(Flash->Handle, Flash->RW_Flash, &reg16);
	// Disable flash 
	res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));
	*Status = (uint8_t)(reg16 & 0xFF);

    return res;

}

// *****************************************************************************
// GetFlashStatus: read the status register of the flash
// Parameters: Status: pointer to the Status variable
// Return: 0 on Success, < 0 on error
// *****************************************************************************
static int GetFlashStatus(cvFlashAccess *Flash, uint8_t *Status) {

  int ret;

  if (Flash->RegSize == 4)
    ret = GetFlashStatus32(Flash, Status);
  else if (Flash->RegSize == 2)
    ret = GetFlashStatus16(Flash, Status); 
  else
    ret = CvUpgrade_FileAccessError; /* Unsupported RegSize */

  return ret;    
    
}


static int WriteFlashPage32(cvFlashAccess *Flash, uint8_t *data, int pagenum)
{
    unsigned int i;
    int res = 0;
    uint8_t stat;
    uint32_t flash_addr;

    int wcnt = 0;

    uint32_t Wbuff[MAX_MULTIACCESS_BUFSIZE];        // Buffers for the Multi Read/Write
    uint32_t Addrs[MAX_MULTIACCESS_BUFSIZE];
    CAENComm_ErrorCode ECs[MAX_MULTIACCESS_BUFSIZE];

	if (Flash->PageSize == 264)  // 4 and 8 Mbit
		flash_addr = (uint32_t)pagenum << 9;
	else if (Flash->PageSize == 528)  // 16 and 32 Mbit
		flash_addr = (uint32_t)pagenum << 10;
	else  // 64 Mbit
		flash_addr = (uint32_t)pagenum << 11;

	// Enable flash 
    res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

    // Opcode
    Wbuff[wcnt] = MAIN_MEM_PAGE_PROG_TH_BUF1_CMD;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
	// Page address
    Wbuff[wcnt] = (flash_addr>>16) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = (flash_addr>>8) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = flash_addr & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;

    // Fill buffer
    for (i=0; i<Flash->PageSize; i++) {
        Addrs[wcnt] = Flash->RW_Flash;
        Wbuff[wcnt] = (uint32_t)data[i];
        ++wcnt;
    }

    // Write Flash Page
    res |= CAENComm_MultiWrite32(Flash->Handle, Addrs, wcnt, Wbuff, ECs);

    // Disable flash 
	res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));

    // wait for Tep (Time of erase and programming)
	do 
		res |= GetFlashStatus(Flash, &stat);
	while (!(stat & 0x80));

    return res;
}

static int WriteFlashPage16(cvFlashAccess *Flash, uint8_t *data, int pagenum)
{
    unsigned int i;
    int res = 0;
    uint8_t stat;
    uint32_t flash_addr;

    int wcnt = 0;

    uint16_t Wbuff[MAX_MULTIACCESS_BUFSIZE];        // Buffers for the Multi Read/Write
    uint32_t Addrs[MAX_MULTIACCESS_BUFSIZE];
    CAENComm_ErrorCode ECs[MAX_MULTIACCESS_BUFSIZE];

	if (Flash->PageSize == 264)  // 4 and 8 Mbit
		flash_addr = (uint32_t)pagenum << 9;
	else if (Flash->PageSize == 528)  // 16 and 32 Mbit
		flash_addr = (uint32_t)pagenum << 10;
	else  // 64 Mbit
		flash_addr = (uint32_t)pagenum << 11;

	// Enable flash 
    res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

    // Opcode
    Wbuff[wcnt] = MAIN_MEM_PAGE_PROG_TH_BUF1_CMD;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
	// Page address
    Wbuff[wcnt] = (flash_addr>>16) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = (flash_addr>>8) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = flash_addr & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;

    // Fill buffer
    for (i=0; i<Flash->PageSize; i++) {
        Addrs[wcnt] = Flash->RW_Flash;
        Wbuff[wcnt] = (uint32_t)data[i];
        ++wcnt;
    }

    // Write Flash Page
    res |= CAENComm_MultiWrite16(Flash->Handle, Addrs, wcnt, Wbuff, ECs);

    // Disable flash 
	res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));

	// wait for Tep (Time of erase and programming)
	do 
		res |= GetFlashStatus(Flash, &stat);
	while (!(stat & 0x80));

    return res;
}


static int ReadFlashPage32(cvFlashAccess *Flash, uint8_t *data, int pagenum)
{
    unsigned int i;
    int res = 0;
    uint32_t flash_addr;
    uint32_t Wbuff[MAX_MULTIACCESS_BUFSIZE];        // Buffers for the Multi Read/Write
    uint32_t Addrs[MAX_MULTIACCESS_BUFSIZE];
    uint32_t Rbuff[MAX_MULTIACCESS_BUFSIZE];
    CAENComm_ErrorCode ECs[MAX_MULTIACCESS_BUFSIZE];

    int wcnt = 0, rcnt = 0;

	if (Flash->PageSize == 264)       // 4 and 8 Mbit
		flash_addr = (uint32_t)pagenum << 9;
	else if (Flash->PageSize == 528)  // 16 and 32 Mbit
		flash_addr = (uint32_t)pagenum << 10;
	else                              // 64 Mbit
		flash_addr = (uint32_t)pagenum << 11;
    
	// Enable flash
    res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

	// Opcode
    Wbuff[wcnt] = MAIN_MEM_PAGE_READ_CMD;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
	// Page address
    Wbuff[wcnt] = (flash_addr>>16) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = (flash_addr>>8) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = flash_addr & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
	// additional don't care bytes
    for (i=0; i<4; i++) {
        Wbuff[wcnt] = 0;
        Addrs[wcnt] = Flash->RW_Flash;
        ++wcnt;
    }

    res |= CAENComm_MultiWrite32(Flash->Handle, Addrs, wcnt, Wbuff, ECs);

    // Read Flash Page
    rcnt = 0;
    for (i=0; i<Flash->PageSize; i++) {
        Addrs[rcnt] = Flash->RW_Flash;
        ++rcnt;
    }
    res |= CAENComm_MultiRead32(Flash->Handle, Addrs, Flash->PageSize, Rbuff, ECs);

	// Disable flash 
    res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));

	// transfer the 32bit data buffer to the 8 bit buffer
	for (i=0; i<Flash->PageSize; i++)
		data[i] = (uint8_t)Rbuff[i];

    return res;
}


static int ReadFlashPage16(cvFlashAccess *Flash, uint8_t *data, int pagenum)
{
    unsigned int i;
    int res = 0;
    uint32_t flash_addr;
    uint16_t Wbuff[MAX_MULTIACCESS_BUFSIZE];        // Buffers for the Multi Read/Write
    uint32_t Addrs[MAX_MULTIACCESS_BUFSIZE];
    uint16_t Rbuff[MAX_MULTIACCESS_BUFSIZE];
    CAENComm_ErrorCode ECs[MAX_MULTIACCESS_BUFSIZE];

    int wcnt = 0, rcnt = 0;

	if (Flash->PageSize == 264)       // 4 and 8 Mbit
		flash_addr = (uint32_t)pagenum << 9;
	else if (Flash->PageSize == 528)  // 16 and 32 Mbit
		flash_addr = (uint32_t)pagenum << 10;
	else                              // 64 Mbit
		flash_addr = (uint32_t)pagenum << 11;
    
	// Enable flash
    res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

	// Opcode
    Wbuff[wcnt] = MAIN_MEM_PAGE_READ_CMD;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
	// Page address
    Wbuff[wcnt] = (flash_addr>>16) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = (flash_addr>>8) & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
    Wbuff[wcnt] = flash_addr & 0xFF;
    Addrs[wcnt] = Flash->RW_Flash;
    ++wcnt;
	// additional don't care bytes
    for (i=0; i<4; i++) {
        Wbuff[wcnt] = 0;
        Addrs[wcnt] = Flash->RW_Flash;
        ++wcnt;
    }

    // Send opcodes to flash
    res |= CAENComm_MultiWrite16(Flash->Handle, Addrs, wcnt, Wbuff, ECs);

    // Read Flash Page
    rcnt = 0;
    for (i=0; i<Flash->PageSize; i++) {
        Addrs[rcnt] = Flash->RW_Flash;
        ++rcnt;
    }
    res |= CAENComm_MultiRead16(Flash->Handle, Addrs, Flash->PageSize, Rbuff, ECs);

	// Disable flash 
    res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));

	// transfer the 32bit data buffer to the 8 bit buffer
	for (i=0; i<Flash->PageSize; i++)
		data[i] = (uint8_t)Rbuff[i];

    return res;
}

static int ReadFlashSecurityReg32(cvFlashAccess *Flash, uint8_t *data)
{
  int i;
  int res = 0;
  uint32_t data32[2048];

  // enable flash (NCS = 0)
  res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);  

  // write opcode
  res |= CAENComm_Write32(Flash->Handle, Flash->RW_Flash, AT45_READ_SECURITY_REGISTER_OPCODE);
  // additional don't care bytes
  for (i=0; i<3; i++)
      res |= CAENComm_Write32(Flash->Handle, Flash->RW_Flash, 0); 

  // read flash page
  for (i=0; i<AT45_IDREG_LENGTH; i++) 
      res |= CAENComm_Read32(Flash->Handle, Flash->RW_Flash,(data32+i));

  // disable flash (NCS = 1)
  res |= CAENComm_Write32(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));
  for (i=0; i<AT45_IDREG_LENGTH; i++) 
	  data[i] = (uint8_t)data32[i];

  return res;
}

static int ReadFlashSecurityReg16(cvFlashAccess *Flash, uint8_t *data)
{
  int i;
  int res = 0;
  uint16_t data16[2048];

  // enable flash (NCS = 0)
  res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, 0);  

  // write opcode
  res |= CAENComm_Write16(Flash->Handle, Flash->RW_Flash, AT45_READ_SECURITY_REGISTER_OPCODE);
  // additional don't care bytes
  for (i=0; i<3; i++)
      res |= CAENComm_Write16(Flash->Handle, Flash->RW_Flash, Flash->FlashEnable); 

  // read flash page
  for (i=0; i<AT45_IDREG_LENGTH; i++) 
      res |= CAENComm_Read16(Flash->Handle, Flash->RW_Flash,(data16+i));

  // disable flash (NCS = 1)
  res |= CAENComm_Write16(Flash->Handle, Flash->Sel_Flash, !(Flash->FlashEnable & 0x01));
  for (i=0; i<AT45_IDREG_LENGTH; i++) 
	  data[i] = (uint8_t)data16[i];

  return res;
}



// *****************************************************************************
// WriteFlashPage: write one page of the flash memory
// Parameters: data: pointer to the data buffer
//             pagenum: flash page number
// Return: 0 on Success, < 0 on error
// *****************************************************************************
int WriteFlashPage(cvFlashAccess *Flash, uint8_t *data, int pagenum) {

  int ret;

  if (Flash->RegSize == 4)
    ret = WriteFlashPage32(Flash, data, pagenum);
  else if (Flash->RegSize == 2)
    ret = WriteFlashPage16(Flash, data, pagenum); 
  else
    ret = CvUpgrade_FileAccessError; /* Unsupported RegSize */

  return ret;    
    
}

// *****************************************************************************
// ReadFlashPage: read one page of the flash memory
// Parameters: data: pointer to the data buffer
//             pagenum: flash page number
// Return: 0 on Success, < 0 on error
// *****************************************************************************
int ReadFlashPage(cvFlashAccess *Flash, uint8_t *data, int pagenum) {

  int ret;

  if (Flash->RegSize == 4)
    ret = ReadFlashPage32(Flash, data, pagenum);
  else if (Flash->RegSize == 2)
    ret = ReadFlashPage16(Flash, data, pagenum); 
  else
    ret = CvUpgrade_FileAccessError; /* Unsupported RegSize */

  return ret;    
    
}


// *****************************************************************************
// ReadFlashSecurityReg
// *****************************************************************************
int ReadFlashSecurityReg(cvFlashAccess *Flash, uint8_t *data) {

  int ret;

  if (Flash->RegSize == 4)
    ret = ReadFlashSecurityReg32(Flash, data);
  else if (Flash->RegSize == 2)
    ret = ReadFlashSecurityReg16(Flash, data); 
  else
    ret = CvUpgrade_FileAccessError; /* Unsupported RegSize */

  return ret;    
    
}
