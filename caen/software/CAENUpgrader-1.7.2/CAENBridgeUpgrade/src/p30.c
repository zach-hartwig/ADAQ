#include "p30.h"
#include "a3818.h"
#include "CAENVMElib.h"

/*******************************************************************/
/* Funzioni per l'accesso alla FLASH BPI */
/*******************************************************************/

/*-------------------------------------------------------------*/
/* offset deve essere allineato a 16 bit (BIT0 non significativo) */
int bpi_flash_read(int32_t dev, uint32_t offset, uint32_t* bpi_data) {
	uint32_t app_data;

	//Imposto indirizzo
	app_data = offset << 1;
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_AD | 0x1000, app_data);

	//Abbasso il CE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x6);

	//Abbasso OE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x2);

	//Rialzo OE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x6);

	//Alzo CE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x7);

	//Sono significativi, per la lettura, solo i 16 bit bassi
	CAENVME_ReadRegister(dev, A3818_BPI_FLASH_DT | 0x1000, &app_data);
	*bpi_data = app_data & 0x0000FFFF;

	return 0;
}

/*-------------------------------------------------------------*/
int bpi_flash_write(int32_t dev, uint32_t offset, uint32_t bpi_data) {
	uint32_t data;

	//Imposto indirizzo
	data = offset << 1;

	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_AD | 0x1000, data);

	//Imposto il dato
	//Per la scrittura i 16 bit da scrivere vanno messi nella parte alta del registro.
	data = bpi_data << 16;
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_DT | 0x1000, data);

	//Abbasso il CE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x6);

	//Abbasso WE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x4);

	//Rialzo WE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x6);

	//Alzo CE
	CAENVME_WriteRegister(dev, A3818_BPI_FLASH_CMD | 0x1000, 0x7);

	return 0;
}

// abilito la BPI flash
void A3818_EnableBPIAccess(int32_t handle) {
	uint32_t data;
	CAENVME_ReadRegister(handle, A3818_DMISCCS | 0x1000, &data);
	data = data | A3818_DMISCCS_SPI_BPI_FLASH_SEL;
	CAENVME_WriteRegister(handle, A3818_DMISCCS | 0x1000, data);
}

// Disabilita l'accesso alla BPI flash.
// L'a3818 può accedere alla sola flash SPI.
void A3818_EnableSPIAccess(int32_t handle) {
	uint32_t data;
	CAENVME_ReadRegister(handle, A3818_DMISCCS | 0x1000, &data);
	data = data & (~A3818_DMISCCS_SPI_BPI_FLASH_SEL);
	CAENVME_WriteRegister(handle, A3818_DMISCCS | 0x1000, data);
}

/****************************************************************************/

// Lettura registro di stato e controllo che la flash sia ready
static uint32_t readFlashStatus(int32_t dev) {

	uint32_t stat;
	bpi_flash_write(dev, 0x0, P30_READ_STATUS);
	bpi_flash_read(dev, 0x0, &stat);

	return stat;

}

/****************************************************************************
 *
 * waitUntilReady
 *
 * Description:
 *
 *    This procedure is called to wait until the flash device status
 *    indicates a ready state or times out.  See the flash device datasheet
 *    for specific details on reading status data.
 *
 * Parameters:
 *
 *    IN      timeout  - timeout value specified as number of times
 *                       to read the status register before giving up.
 *
 * Returns:
 *
 *    uint8_t         - boolean flag indicating whether the ready state was
 *                     reached before the timeout number of reads
 *                     occured.
 *
 * Assumptions:
 *
 *    NONE.
 *
 ***************************************************************************/
static uint8_t waitUntilReady(int32_t dev, uint32_t timeout) {

	while (timeout) {

		if (mIsStatusReady( readFlashStatus(dev) )) {
			return (TRUE);
		}
		timeout--;
	}

	return (FALSE);

}

static int eraseBlock(int32_t dev, uint32_t blockAddress) {

	uint32_t status;

	waitUntilReady(dev, 0xFFFFFFFF);

	// Unlock Command
	bpi_flash_write(dev, blockAddress, P30_BLOCK_LOCK_SETUP);
	bpi_flash_write(dev, blockAddress, P30_BLOCK_UNLOCK);

	//Erase
	bpi_flash_write(dev, blockAddress, P30_BLOCK_ERASE_SETUP);
	bpi_flash_write(dev, blockAddress, P30_BLOCK_ERASE_CONFIRM);

	// Attesa ready della flash
	waitUntilReady(dev, 0xFFFFFFFF);

	status = readFlashStatus(dev);
	bpi_flash_write(dev, 0x0, P30_CLEAR_STATUS);

	// Imposto Read Array Mode
	bpi_flash_write(dev, 0x0, P30_READ_ARRAY);

	return status;
}

/****************************************************************************
 *
 * writeFlashPage
 *
 * Description:
 *
 * Funzioni per la scrittura/lettura di pagine della flash StrataFlash Numonix P30
 * montata sulla A3818.
 * Riferimento: Buffer Program Flowchart (fig. 36) da datasheet
 *    Numonyx Strataflash Embedded memory P30 (306666-12 - Agosto 2008).
 *
 * Parameters:
 *  
 *    IN      dev    - A3818 driver handle
 *
 *    IN      data   - the buffer containing data to be programmed.
 *
 *    IN      wordAddress  - the flash address to be programmed.
 *
 *    IN      numwords - the number of words (ie two bytes) of data contained in the buffer.
 *                                This is a X16 write buffer routine and the part expects a count in # of words
 *
 * Returns:
 *    
 *    int    TBD
 * 
 * Assumptions:
 *
 *    NONE
 *
 ***************************************************************************/
static int writeFlashPage(int32_t dev, uint32_t *data, int wordAddress,
		int numWords) {
	int i;

	// La modalit� di scrittura con buffer ha un massimo di 32 word a 16 bit
	// che possono essere scritte.
	if (numWords > 32)
		return -1;

	// Imposto Read Array Mode
	bpi_flash_write(dev, 0x0, P30_READ_ARRAY);

	// Status check
	waitUntilReady(dev, 0xFFFFFFFF);

	bpi_flash_write(dev, wordAddress, P30_BUFFERED_PROGRAM);
	bpi_flash_write(dev, wordAddress, numWords - 1); // N=16 words da scrivere (bisogna scrivere N-1)

	for (i = wordAddress; i < (wordAddress + numWords); ++i) {
		bpi_flash_write(dev, i, data[i - wordAddress]);
	}

	bpi_flash_write(dev, wordAddress, P30_BUFFERED_PROGRAM_CONFIRM);

	// Status check
	waitUntilReady(dev, 0xFFFFFFFF);

	// Imposto Read Array Mode
	bpi_flash_write(dev, 0x0, P30_READ_ARRAY);

	return 0;
}

int readFlash(int32_t dev, uint32_t *data, int address, int length) {
	int flash_addr;
	int i;

	// Imposto Read Array Mode
	bpi_flash_write(dev, 0x0, P30_READ_ARRAY);

	flash_addr = address;
	for (i = 0; i < length; i++) {
		bpi_flash_read(dev, flash_addr + i, &data[i]);
	}

	return 0;
}

unsigned int verifyFlash(uint32_t handle, uint32_t *buffer,
		uint32_t bufferLength, uint32_t startWordAddress) {
	int finish, i;
	unsigned int verify_err;
	int bp, wc, pp;
	uint32_t bpi_rd[32]; // 16-bit word read from flash

	wc = 0; // byte counter


	bp = 0; // byte pointer in the page
	finish = 0; // it goes high when eof is found
	verify_err = 0; // number of errors during verify

	pp = 0;

	while (wc < bufferLength) {

#ifdef PRINT_POINTS
		if (((wc % (32 * 1024)) == 0) || finish) {
			printf(".");
			fflush(stdout);
		}
#endif                             
		readFlash(handle, bpi_rd, (pp * 32) + startWordAddress, 32); // read page

		for (i = 0; i < 32; i++) { // verify page
			if (bpi_rd[i] != buffer[wc + i]) {
				printf("page = %d; bpi_rd[%d]=%04X, buffer[%d]=%04X  \n", pp,
						i, bpi_rd[i], wc + i, buffer[wc + i]);
				printf("\n\n Writing or Verify error at page %d - %d\n", pp, i);
				++verify_err;
			}
		}
		bp = 0;
		pp++;
		wc += 32;
	} // end of while loop


	return verify_err;

}

// L'immagine del firmware dell'A3818 � di circa 1716 KB
int eraseFirmware(uint32_t handle, uint32_t baseAddress, uint32_t fwcopy) {
	int i;

	if (fwcopy == 0) {
		// Cancella i primi 4 blocchi da 32KB = 128KB
		for (i = 0; i < 4; i++) {
			eraseBlock(handle, baseAddress + (i * (16 * 1024)));
#ifdef PRINT_POINTS

			printf(".");
			fflush(stdout);
#endif             
		}
		// Cancella 13 blocchi da 128KB
		for (i = 4; i < 18; i++) {
			eraseBlock(handle, baseAddress + ((i - 3) * (64 * 1024)));
#ifdef PRINT_POINTS             
			printf(".");
			fflush(stdout);
#endif             
		}
	} else {
		// Cancella 15 blocchi da 128KB = cancella 15*128K = 1920KB
		for (i = 0; i < 14; i++) {
			eraseBlock(handle, baseAddress + (i * (64 * 1024)));
#ifdef PRINT_POINTS             
			printf(".");
			fflush(stdout);
#endif             
		}
	}

	// Ritorna in modalit� Read Array Mode
	bpi_flash_write(handle, 0x0, P30_READ_ARRAY);
	return 0;
}

void writeFlash(uint32_t handle, uint32_t *buffer, uint32_t bufferLength,
		uint32_t startWordAddress) {

	int wc, pp;

	wc = 0;
	pp = 0;

#ifdef PRINT_POINTS        
	printf(".");
	fflush(stdout);
#endif        
	while (wc < bufferLength) {
#ifdef PRINT_POINTS        	
		if (((wc % (32 * 1024)) == 0)) {
			printf(".");
			fflush(stdout);
		}
#endif                      
		writeFlashPage(handle, buffer + (pp * 32),
				startWordAddress + (pp * 32), 32);

		pp++;
		wc += 32;
	} // end of while loop

}
