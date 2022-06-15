#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "flash.h"
#include "CAENVMElib.h"

static int GetFlashStatus32(cvFlashAccess *Flash, uint8_t *Status)
{
    int res = 0;
    uint32_t reg32;


	// Enable flash
    res |= CAENVME_WriteRegister(Flash->Handle,Flash->Sel_Flash,Flash->FlashEnable);
	// Status read Command
	reg32 = STATUS_READ_CMD;
	res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, reg32);
	// Status read
	res |= CAENVME_ReadRegister(Flash->Handle, Flash->RW_Flash, &reg32);
	// Disable flash 
	res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, !Flash->FlashEnable);
	*Status = (uint8_t)(reg32 & 0xFF);

    return res;

}


// *****************************************************************************
// GetFlashStatus: read the status register of the flash
// Parameters: Status: pointer to the Status variable
// Return: 0 on Success, < 0 on error
// *****************************************************************************
static int GetFlashStatus(cvFlashAccess *Flash, uint8_t *Status) {

  int ret;

    ret = GetFlashStatus32(Flash, Status);

  return ret;    
    
}


static int WriteFlashPage32(cvFlashAccess *Flash, uint8_t *data, int pagenum)
{
    unsigned int i;
    int res = 0;
    uint8_t stat;
    uint32_t flash_addr;
	uint32_t reg32;

	if (Flash->PageSize == 264)  // 4 and 8 Mbit
		flash_addr = (uint32_t)pagenum << 9;
	else if (Flash->PageSize == 528)  // 16 and 32 Mbit
		flash_addr = (uint32_t)pagenum << 10;
	else  // 64 Mbit
		flash_addr = (uint32_t)pagenum << 11;

	reg32 = !Flash->FlashEnable; 
	res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, reg32);
	// Enable flash 
    res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

    // Opcode
    res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, MAIN_MEM_PAGE_PROG_TH_BUF1_CMD); 

	// Page address

    res |= CAENVME_WriteRegister(Flash->Handle,Flash->RW_Flash, (flash_addr>>16) & 0xFF); 

    res |= CAENVME_WriteRegister(Flash->Handle,Flash->RW_Flash, (flash_addr>>8) & 0xFF); 

    res |= CAENVME_WriteRegister(Flash->Handle,Flash->RW_Flash, flash_addr & 0xFF); 

    // Fill buffer
    for (i=0; i<Flash->PageSize; i++) {
		res |= CAENVME_WriteRegister(Flash->Handle,Flash->RW_Flash, data[i]); 
    }

    // Disable flash 
   reg32 = !Flash->FlashEnable; 
	res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, reg32);

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
    uint32_t Rbuff;
	uint32_t reg32;
    int  rcnt = 0;

	if (Flash->PageSize == 264)       // 4 and 8 Mbit
		flash_addr = (uint32_t)pagenum << 9;
	else if (Flash->PageSize == 528)  // 16 and 32 Mbit
		flash_addr = (uint32_t)pagenum << 10;
	else                              // 64 Mbit
		flash_addr = (uint32_t)pagenum << 11;

    reg32 = !Flash->FlashEnable; 
	res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, reg32);    
	// Enable flash
    res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

	// Opcode

   res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, MAIN_MEM_PAGE_READ_CMD); 
	// Page address

    res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, (flash_addr>>16) & 0xFF); 

    res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, (flash_addr>>8) & 0xFF); 

    res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, flash_addr & 0xFF); 
	// additional don't care bytes
    for (i=0; i<4; i++) {
		res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, 0);  
    }

    // Read Flash Page
    rcnt = 0;
    for (i=0; i<Flash->PageSize; i++) {
		CAENVME_ReadRegister(Flash->Handle, Flash->RW_Flash, &Rbuff);
		data[i] = (uint8_t) Rbuff;
    }

	// Disable flash 
    reg32 = !Flash->FlashEnable; 
	res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, reg32);

    return res;
}


static int ReadFlashSecurityReg32(cvFlashAccess *Flash, uint8_t *data)
{
  int i;
  int res = 0;
  uint32_t data32[2048];
  uint32_t reg32;

  reg32 = !Flash->FlashEnable;
  res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, reg32);
  // enable flash (NCS = 0)
  res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, Flash->FlashEnable);

  // write opcode
  reg32 = AT45_READ_SECURITY_REGISTER_OPCODE;
  res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, reg32);
  // additional don't care bytes
  reg32 = 0;
  for (i=0; i<3; i++)
      res |= CAENVME_WriteRegister(Flash->Handle, Flash->RW_Flash, reg32);

  // read flash page
  for (i=0; i<AT45_IDREG_LENGTH; i++) 
      res |= CAENVME_ReadRegister(Flash->Handle, Flash->RW_Flash,(data32+i));

  // disable flash (NCS = 1)
  reg32 = !Flash->FlashEnable;
  res |= CAENVME_WriteRegister(Flash->Handle, Flash->Sel_Flash, reg32);
  for (i=0; i<AT45_IDREG_LENGTH; i++) 
	  data[i] = (uint8_t)data32[i];

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

    ret = WriteFlashPage32(Flash, data, pagenum);

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


    ret = ReadFlashPage32(Flash, data, pagenum);
 
  return ret;    
    
}


// *****************************************************************************
// ReadFlashSecurityReg
// *****************************************************************************
int ReadFlashSecurityReg(cvFlashAccess *Flash, uint8_t *data) {

  int ret;

    ret = ReadFlashSecurityReg32(Flash, data);
  return ret;    
    
}
