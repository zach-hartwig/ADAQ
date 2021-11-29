/************************************************************************

 CAEN SpA - Viareggio
 www.caen.it

 Program: cvupgrade
 Date:    15/02/2010
 Author:  CAEN Computing Division (support.computing@caen.it)

 ------------------------------------------------------------------------
 Description
 ------------------------------------------------------------------------
 This program allows to upgrade the firmware of any CAEN module that
 supports the firmware download through the USB/CONET cable.(except for the
 bridges V1718 and V2718 that use the program 'CAENVMEUpgrade').
 The firmware upgrade consists in writing the configuration file (Altera
 Raw Binary Format) into the flash memory of the board. Usually the flash
 memory contains two images of the configuration file, called 'standard'
 and 'backup'. The 'standard' image only will normally be overwritten ; 
 the new firmware will be loaded by the FPGA after the first power cycle.
 If an error occurs while upgrading, it may happen that the FPGA is not
 properly configured and the board does not respond to the VME access.
 In this case you can restore the old firmware (i.e. the 'backup' image)
 just moving the relevant jumper from STD to BKP and power cycling the
 board. Now you can retry to upgrade the 'standard' image.
 Warning: never upgrade the 'backup' image until you are sure that the
 'standard' works properly.
 This program reads some parameters that define the type of the board
 being upgraded from a file called CVupgrade_params.txt that must be
 in the same directory of CVupgrade. There is one CVupgrade_params file
 for each type of board that can be upgraded.

 ------------------------------------------------------------------------

 ------------------------------------------------------------------------
 Portability
 ------------------------------------------------------------------------
 This program is ANSI C and can be compilated on any platform, except
 for the functions that allow to initialize and access the VME bus.
 For VME Boards, if CAEN's VME bridges (both V1718 and V2718) are used as VME masters,
 a proper installation of the CAENComm library DLL on PC is the only requirement.
 If a different VME Bridge/CPU is used, the CAENComm function calls MUST be reimplemented.

*************************************************************************/
/*------------------------------------------------------------------------
  Modification history:
  ------------------------------------------------------------------------
  Version  | Author | Date       | Changes made
  ------------------------------------------------------------------------
  1.0      | CTIN   | 01.02.2008 | initial version.
  1.1      | LCOL   | 29.05.2008 | Updated WriteFlashPage to account for 
           |        |            | a longer flash erase times 
           |        |            | (increase from 20 to 40 ms).
  1.2      | LCOL   | 25.06.2008 | Added flash enable polarity parameter.         
  1.3      | CTIN   | 26.01.2009 | Extended compatibility to 16, 32 and 64
           |        |            | Mbit flash size;
           |        |            | Replaced fixed wait time (page erasing)
           |        |            | with busy polling;
           |        |            | Add -link and -bdnum command line options
  2.0      | ALUK   | 03.10.2009 | Add the CAENComm as reference library for the 
		   |		|			 | low level access.
  2.1      | ALUK   | 14.02.2010 | Add Keyinfo and KeyWrite functionalities 
		   |        |            |(only Digitizer boards)
  2.2      | NDA	| 15.04.2010 | Added optional command line parameter: modelname
		   |        |            | Added model name (if defined) to the KeyInfo filename
		   |        |            | Changed KeyInfo filename from KeyInfo### to BoardInfo###
		   |        |            | Added main switch -License: this will print (default) or delete ( -delete option)
		   |        |            | the license data
  2.3      | NDA    | 29.07.2010 | Add -Key main switch to set the DPP key file without have to upgrade the firmware
  2.4      | ALUK   | 15.07.2012 | Add -fwrel main switch to get the firmware release of the board
  2.5	   | ALUK   | 04.03.2014 | Add size check and big flash support	
  ------------------------------------------------------------------------

  ------------------------------------------------------------------------*/

#define REVISION  "2.7.0"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "flash.h"

#include "CFASegment.h"

typedef struct UpgradeParams {
  unsigned int			Type  ;
  unsigned int			Link  ;
  unsigned int			BdNum ;
  unsigned long			BaseAddress;  
  unsigned int			FirstPageStd;         // First page of the Standard image
  unsigned int			FirstPageBck;         // First page of the Backup image
  uint64_t				key; 
  unsigned int			image     ;
  unsigned int			noverify  ;
  unsigned int			verifyonly;
  unsigned int			delete    ;
  unsigned int			cfa		  ;			  // CFA mode
  char					ConfFile[1000];
  char					Models[1000];
  char					modelName[50];
  char					family[50];
  char					filepath[1000];
  unsigned int			ROCFwm;
  unsigned int			AMCFwm;
} cvUpgradeParams;

static void Usage() {
        printf("Syntax: cvUpgrade ConfFile ConnType[USB|OPTLINK|USB_A4818] [ConfigOptions][DeviceOptions]\n");
		printf("Syntax: cvUpgrade -KeyInfo ConnType[USB|OPTLINK|USB_A4818] [DeviceOptions]\n");
		printf("Syntax: cvUpgrade -License ConnType[USB|OPTLINK|USB_A4818] [LicenseOptions]\n");
        printf("Syntax: cvUpgrade -Key N ConnType[USB|OPTLINK|USB_A4818] [DeviceOptions]\n\n");
		printf("Syntax: cvUpgrade -fvrel ConnType[USB|OPTLINK|USB_A4818] [DeviceOptions]\n\n");
        printf("Description: write the file 'ConfFile' (Altera Raw Binary Format) into\n");
        printf("the flash memory of board connected by the specified connection type OR\n");
        printf("retrieve the keyinfo data from the board and store it on BoardInfo.dat file OR\n");
		printf("retrieve or delete the license data from the board:\n");
		printf("  to delete the license file you must specify -delete option\n\n");
        printf("ConfigOptions:\n\n");
        printf("-backup: write backup image\n\n");
		printf("-key    N: The key value to enable the DPP firmware\n\n"); 
        printf("-no_verify: disable verify (don't read and compare the flash content)\n\n");
        printf("-verify_only: read the flash content and compare with the ConfFile file\n\n");
		printf("DeviceOptions:\n\n");
        printf("-param filename: allow to specify the file that contain the parameters for the\n");
        printf("                 board that is being upgraded (default is cvUpgrade_params.txt)\n");
		printf("				 With CFA files this option is not required.\n\n");
		printf("-rbf: specifies that the config file to be updated is in RBF format.\n");
        printf("-link   N: when using CONET, it is the optical link number to be used\n");
		printf("-link   N: when using USB_A4818, it is the A4818 PID number\n");
		printf("           when using USB, it is the USB device number to be used (default is 0)\n\n");
        printf("-bdnum  N: select board number in a chainable link (V2718 only)\n\n");
		printf("-VMEbaseaddress   N: The base address of the VME slave board to be upgraded (Hex 32 bit)\n\n");
        printf("-modelname ModelName: specify the model name (e.g. V1724B)\n\n");
        printf("-filepath Folder: specify the folder where store the KeyInfo file\n\n");
		printf("LicenseOptions:\n\n");
        printf("-delete : deletes the license data\n");
}


//******************************************************************************
// ReadConfigurationROM
//******************************************************************************
static int ReadConfigurationROM(cvFlashAccess *Flash, CROM_MAP *crom)
{
  unsigned char data[AT45_PAGE_SIZE];
  int res = 0, d32;

  res = ReadFlashPage(Flash, data, ROM_FLASH_PAGE);

  if (res == 0) {
      crom->crom_chksum     = (uint8_t)(data[0]);
      crom->crom_chksum_len = (uint32_t) ((data[1] << 16) | (data[2] << 8) | data[3]);
      crom->crom_const      = (uint32_t) ((data[4] << 16) | (data[5] << 8) | data[6]);
      crom->crom_c_code     = (uint8_t)(data[7]);
      crom->crom_r_code     = (uint8_t)(data[8]);
      crom->crom_OUI        = (uint32_t) ((data[9] << 16) | (data[10] << 8) | data[11]);
      crom->crom_version    = (uint8_t)(data[12]);
      crom->crom_board_id   = (uint32_t) ((data[13] << 16) | (data[14] << 8) | data[15]);
      crom->crom_revision   = (uint32_t) ( (data[16] << 24) | (data[17] << 16) | (data[18] << 8) | data[19]);
      crom->crom_serial     = (uint16_t) ((data[32] << 8) | (data[33]));
      crom->crom_VCXO_type  = (uint8_t)(data[34]);
  }
  CAENComm_Read32(Flash->Handle,0xF050, &d32);
  crom->crom_flash_type = (uint8_t)d32;
  return res;
}


//******************************************************************************
// ReadLicense
//******************************************************************************
static int ReadLicense(cvFlashAccess *Flash, uint8_t licenseData[LICENSE_DATA_SIZE])
{
  unsigned char data[AT45_PAGE_SIZE];
  int page_num;
  int res = 0;
  if (Flash->FlashEnable == 0x02) page_num = LICENSE_FLASH_PAGE -1; else page_num = LICENSE_FLASH_PAGE;
  if( (res = ReadFlashPage(Flash, data, page_num))!= 0) {
	  return res;
  }
  memcpy( licenseData, data, LICENSE_DATA_SIZE); 
  return 0;
}


//******************************************************************************
// EraseLicense
//******************************************************************************
static int EraseLicense(cvFlashAccess *Flash)
{
  unsigned char data[AT45_PAGE_SIZE];
  int page_num;
  if (Flash->FlashEnable == 0x02) page_num = LICENSE_FLASH_PAGE -1; else page_num = LICENSE_FLASH_PAGE;
  memset( data, 0xff, sizeof( data));
  return WriteFlashPage(Flash, data, page_num);
}

//******************************************************************************
// PrintCROM2File
//******************************************************************************
static void PrintCROM2File(FILE *fp, const CROM_MAP *map) {
    fprintf(fp,"Checksum        = %X\n", map->crom_chksum);
    fprintf(fp,"Checksum Length = %X\n", map->crom_chksum_len);
    fprintf(fp,"Constant field  = %X\n", map->crom_const);
    fprintf(fp,"C Code          = %X\n", map->crom_c_code);
    fprintf(fp,"R Code          = %X\n", map->crom_r_code);
    fprintf(fp,"OUI             = %X\n", map->crom_OUI);
    fprintf(fp,"Version         = %X\n", map->crom_version);
    fprintf(fp,"Board ID        = %X\n", map->crom_board_id);
    fprintf(fp,"PCB Revision    = %d\n", map->crom_revision);
    fprintf(fp,"Serial Number   = %d\n", map->crom_serial);
    fprintf(fp,"VCXO Type ID    = %d\n", map->crom_VCXO_type);
	fprintf(fp,"Flash Type      = %d\n", map->crom_flash_type);
}

//******************************************************************************
// KeyInfo
//******************************************************************************
static int KeyInfo(cvUpgradeParams *Config, cvFlashAccess *Flash) {
	int i;
	unsigned char security_vme[AT45_IDREG_LENGTH];
	// unsigned long vboard_base_address;
	char KeyFilename[200];
	FILE *cf = NULL;
	int res;

	CROM_MAP rom;

    /* initialize the connection */
    res = CAENComm_OpenDevice(Config->Type, Config->Link, Config->BdNum,  Config->BaseAddress, &Flash->Handle);
    if (res != CAENComm_Success) {
        printf("Cannot open the Device!\n");
        return(CvUpgrade_FileAccessError);
    }
   
	if (ReadFlashSecurityReg(Flash, security_vme)) {
      printf("Error while reading on-board flash memory!\n");
	  return(CvUpgrade_FileAccessError);
	}
	if (ReadConfigurationROM(Flash, &rom)) {
      printf("Error while reading on-board flash memory!\n");
	  return(CvUpgrade_FileAccessError);
	}
	if( strlen( Config->modelName)) {
		sprintf(KeyFilename, "%s/BoardInfo-%s-%d.dat", Config->filepath, Config->modelName, rom.crom_serial);
	} else {
		sprintf(KeyFilename, "%s/BoardInfo-%d.dat", Config->filepath, rom.crom_serial);
	}
    cf = fopen(KeyFilename,"w");
	if(cf == 0) {
      printf("Cannot open %s file! Exiting ...\n", KeyFilename);
      return(CvUpgrade_FileAccessError);
	}

    printf("Writing output file %s\n", KeyFilename);
	for(i=0; i < 64; i++) {
	  fprintf(cf, "%02X", security_vme[64+i]);
	  fprintf(cf, (i < 63) ? ":" : "\n");
	}
	PrintCROM2File(cf, &rom);

	CAENComm_CloseDevice(Flash->Handle);
	Flash->Handle= CAENCOM_INVALID_FILE_HANDLE;

	printf("\n");
	printf("Program exits successfully.\n");
    fclose(cf);
	return 0;
}

static int GetFWRel(cvUpgradeParams *Config, cvFlashAccess *Flash) {
	/* initialize the connection */
	int ret,d32,i;
	unsigned char x,y;
	int chNum = 2;
	uint16_t BuildHexCode,d16;
	if( CAENComm_OpenDevice(Config->Type, Config->Link, Config->BdNum,  Config->BaseAddress, &Flash->Handle)!= CAENComm_Success){
        printf("Cannot open the Device!\n");
        return(CvUpgrade_FileAccessError);
    }

	if (strcmp(Config->Models,"DIGITIZERS") == 0) { 
		// DIGITIZERS
		if ((ret = CAENComm_Read32(Flash->Handle,Config->ROCFwm, &d32)) != CAENComm_Success) {
			printf("unable to find ROC FPGA  release\n");
			return -1;
		}
		y = (uint8_t) (d32 & 0xff);
		x = (uint8_t) ((d32 & 0xff00)>> 8);
		BuildHexCode = (uint16_t) ((d32 & 0xffff0000)>> 16);
		printf("ROC FPGA Release is %02d.%02d - Build %04X\n",x,y, BuildHexCode);

		for (i = 0; i< chNum; i++) {
			if ((ret = CAENComm_Read32(Flash->Handle, Config->AMCFwm | (i<<8), &d32)) == CAENComm_Success) {
				if ( (uint16_t)((d32 & 0xffff0000)>> 16) != 0x0000 ) break; // HACK this is for x780 support! AMC Build must be != 0000
			}
		}
		if (i == (int32_t)chNum) {
			printf("unable to find AMC FPGA release\n");
			return -1;
		}
		y = (uint8_t) (d32 & 0xff);
		x = (uint8_t) ((d32 & 0xff00)>> 8);
		BuildHexCode = (uint16_t) ((d32 & 0xffff0000)>> 16);
		printf("AMC FPGA Release is %02d.%02d - Build %04X",x,y, BuildHexCode);
	}
	else {
		if (strcmp(Config->Models,"V1190,V1290") == 0) {
			if ((ret = CAENComm_Read16(Flash->Handle,Config->ROCFwm, &d16)) != CAENComm_Success) {
				printf("unable to find firmware release\n");
				return -1;
			}
			y = (uint8_t) (d16 & 0xff);
			x = (uint8_t) ((d16 & 0xff00)>> 8);
			printf("Firmware Release is %02d.%02d\n",x,y);
		}
		else {
			if ((ret = CAENComm_Read32(Flash->Handle,Config->ROCFwm, &d32)) != CAENComm_Success) {
				printf("unable to find firmware release\n");
				return -1;
			}
			y = (uint8_t) (d32 & 0xff);
			x = (uint8_t) ((d32 & 0xff00)>> 8);
			printf("Firmware Release is  %02d.%02d\n",x,y);
		}
	}
	CAENComm_CloseDevice(Flash->Handle);
	Flash->Handle= CAENCOM_INVALID_FILE_HANDLE;

	printf("\n");
	printf("Program exits successfully.\n");
	fflush(stdout);
	return 0;
}	

//******************************************************************************
// License
//******************************************************************************
static int License(cvUpgradeParams *Config, cvFlashAccess *Flash) {

	/* initialize the connection */
	if( CAENComm_OpenDevice(Config->Type, Config->Link, Config->BdNum,  Config->BaseAddress, &Flash->Handle)!= CAENComm_Success){
        printf("Cannot open the Device!\n");
        return(CvUpgrade_FileAccessError);
    }

	if( Config->delete) {
		// delete the license data
		if( EraseLicense( Flash)) {
			printf("Error deleting license data! Exiting ...\n");
			return(CvUpgrade_FileAccessError);
		}
	} else {
		uint8_t licenseData[LICENSE_DATA_SIZE];
		if( ReadLicense( Flash, licenseData)) {
			printf("Error reading license data! Exiting ...\n");
			return(CvUpgrade_FileAccessError);
		}
		// print the license data
		// check if a valid license
		printf("\n");
		if( ( licenseData[0]!= 0x00) || ( licenseData[1]!= 0x04)) {
			printf("No valid license found or No license required!");
		} else {
			int i;
			for( i= 0; i< LICENSE_DATA_SIZE; i++) {
				printf( "%02X ", licenseData[i]);
			}
		}
	}
	CAENComm_CloseDevice(Flash->Handle);
	Flash->Handle= CAENCOM_INVALID_FILE_HANDLE;

	printf("\n");
	printf("Program exits successfully.\n");
	fflush(stdout);
	return 0;
}

//******************************************************************************
// WriteKey
//******************************************************************************
int WriteKey(cvUpgradeParams *Config, cvFlashAccess *Flash)
{
    unsigned int err=0, res, handleToClose= 0; 
	unsigned int page_num;
	unsigned char key_page[528];
    int k;    

	if ((Config->key != 0) && 
		((!strncmp(Config->Models,"DIGITIZERS",1000)) || 
		(!strncmp(Config->Models,"V1495",1000)))) {
			if( Flash->Handle== CAENCOM_INVALID_FILE_HANDLE) {
				res = CAENComm_OpenDevice(Config->Type, Config->Link, Config->BdNum,  Config->BaseAddress, &Flash->Handle);
				if (res != CAENComm_Success) {
					printf("Cannot open the Device!\n");
					return(CvUpgrade_FileAccessError);
				}
				handleToClose= 1;
			}

			if (Flash->FlashEnable == 0x02) page_num = LICENSE_FLASH_PAGE -1; else page_num = LICENSE_FLASH_PAGE; 
			ReadFlashPage(Flash, key_page, page_num);

			memset(key_page, 0, LICENSE_DATA_SIZE);
			for (k = LICENSE_DATA_SIZE- 1; k > -1; k--) {
				key_page[LICENSE_DATA_SIZE- 1- k] = (unsigned char) ((Config->key >> (k * 8)) & 0xFF);
			}
			if (Flash->FlashEnable == 0x02) page_num = LICENSE_FLASH_PAGE -1; else page_num = LICENSE_FLASH_PAGE; 
			WriteFlashPage(Flash, key_page, page_num);
			if( handleToClose) {
				CAENComm_CloseDevice(Flash->Handle);
				Wait_ms(1000);
				Flash->Handle= CAENCOM_INVALID_FILE_HANDLE;
			}
            printf("\nKey written successfully\n");
	fflush(stdout);
	}
    return err;
}

//******************************************************************************
// validateModel
//******************************************************************************
int validateModel(cvUpgradeParams *config, cvFlashAccess* flash, char** fwData, int* fwSize) {
	FILE* fin = fopen(config->ConfFile, "rb");
	while (!feof(fin)) {
		int last;
		CFASegmentPtr segm = newSegment(fin, &last);
		if (segm == NULL) {
			fclose(fin);
			return -2;
		}
		if (getSegmentType(segm) == CFA_SEGMENT_TYPE_1) {
			CFASegmentType1* t1s = toType1(segm);
			int i, j;
			for (i = 0; i < t1s->numModels; i++) {
				for (j = 0; j<t1s->models[i]->numChecks; j++) {
					uint32_t address = t1s->models[i]->registerChecks[j].address;
					uint32_t value;
					if (t1s->accessType == 2) CAENComm_Read16(flash->Handle, address, (uint16_t*)&value);
					else {
						CAENComm_Read32(flash->Handle, address, &value);
					}
					if (value != t1s->models[i]->registerChecks[j].value) break;
				}
				if (j == t1s->models[i]->numChecks) { /* this means that all the checks match */
					uint32_t valueF;
					CAENComm_Read32(flash->Handle, 0xF050, &valueF); // ACK: questa cosa non va bene fatta così va ripensato il tutto con il nuovo Upgrader
					if (valueF * 2 == t1s->flashEnLevel) {
						flash->PageSize = t1s->flashPageSize;
						flash->RegSize = t1s->accessType;
						flash->Sel_Flash = t1s->flashCsAddress;
						flash->RW_Flash = t1s->flashCwAddress;
						flash->FlashEnable = t1s->flashEnLevel;
						config->FirstPageStd = t1s->flashStdAddress;
						config->FirstPageBck = t1s->flashBckAddress;

						*fwData = malloc(t1s->fwSizeBytes);
						memcpy(*fwData, t1s->fwData, t1s->fwSizeBytes);
						*fwSize = t1s->fwSizeBytes;

						sprintf(config->Models, "id %d", t1s->models[i]->modelId);

						deleteSegment(segm);
						fclose(fin);
						return 0;
					}
				}
			}
		}
		deleteSegment(segm);
		if (last) break;
	}
	fclose(fin);
	return -1;
}



//******************************************************************************
// cvUpgrade
//******************************************************************************
int cvUpgrade(cvUpgradeParams *Config, cvFlashAccess *Flash)
{
    unsigned int i, j, page, pa,  NumPages, err=0, done;    
    unsigned int CFsize,pll = 0;
    unsigned char c, *CFdata = NULL;
    unsigned char pdr[2048];
	unsigned int AvailableFlashSpace;
    int res;

    FILE *cf;

	/* initialize the connection */
    res = CAENComm_OpenDevice(Config->Type, Config->Link, Config->BdNum,  Config->BaseAddress, &Flash->Handle);
    if (res != CAENComm_Success) {
        printf("Cannot open the Device!\n");
		return(CvUpgrade_CAENCommError);
    }


    // ************************************************
    // Open Binary Configuration File
    // ************************************************
	if (!Config->cfa) {
		// open and read the configuration file
		cf = fopen(Config->ConfFile,"rb");
		if(cf == 0) {
			printf("Can't open file %s\n",Config->ConfFile);
			Usage();
			CAENComm_CloseDevice(Flash->Handle);
			return(CvUpgrade_FileAccessError);
		}
		// calculate the size
		fseek (cf, 0, SEEK_END);
		CFsize = ftell (cf);
		fseek (cf, 0, SEEK_SET);
		if ( (CFdata = (unsigned char *)malloc(CFsize+Flash->PageSize)) == NULL ) {
			printf("Can't allocate %d bytes\n",CFsize);
			CAENComm_CloseDevice(Flash->Handle);
			return(-3);
		}

		for(i=0; i<CFsize; i++) {
			// read one byte from file and mirror it (lsb becomes msb)
			c = (unsigned char)fgetc(cf);
			CFdata[i]=0;
			for(j=0; j<8; j++)
				CFdata[i] |= ( (c >> (7-j)) & 1) << j;
		}
		fclose(cf);

	} else {
		printf("CFA Mode. Reading archive.\n");
		if (validateModel(Config, Flash, &CFdata, &CFsize)) {
			printf("Error: no compatible firmware found in archive!\n");
			CAENComm_CloseDevice(Flash->Handle);
			return -99;
		}
		printf("Compatible firmware found in archive.\n");

		for (i=0; i<CFsize; i++) {
			char c = CFdata[i];
			CFdata[i] = 0;
			for (j=0; j<8; j++)
				CFdata[i] |= ( (c >> (7-j)) & 1) << j;
		}
	}


	if (Config->FirstPageBck < Config->FirstPageStd) AvailableFlashSpace = Config->FirstPageStd - Config->FirstPageBck; else AvailableFlashSpace =  Config->FirstPageBck - Config->FirstPageStd;
    AvailableFlashSpace = AvailableFlashSpace * Flash->PageSize;
	if (Config->FirstPageBck == Config->FirstPageStd) {
		pll = 1;
		AvailableFlashSpace = Flash->PageSize;
	}
	/*if (CFsize > AvailableFlashSpace) {
		printf("\nCommunication Error: the firmware file is bigger that the available digitizer memory\n");
        CAENComm_CloseDevice(Flash->Handle);
		return -99;
	}*/

	NumPages = (CFsize % Flash->PageSize == 0) ? (CFsize / Flash->PageSize) : (CFsize / Flash->PageSize) + 1;

	printf("Board Types: %s\n", Config->Models);
	if (NumPages == 1) {
			 printf("Overwriting image of the PLL with %s\n", Config->ConfFile);
			 pa = Config->FirstPageStd;
	}
	else {
		if (Config->image == 0) {
			if(!Config->verifyonly)
			  printf("Overwriting Standard image of the firmware with %s\n", Config->ConfFile);
			else
				printf("Verifying Standard image of the firmware with %s\n", Config->ConfFile);
			pa = Config->FirstPageStd;
		}
		else {
			if(!Config->verifyonly)
				printf("Overwriting Backup image of the firmware with %s\n", Config->ConfFile);
			else
				printf("Verifying Backup image of the firmware with %s\n", Config->ConfFile);
			pa = Config->FirstPageBck;
		}
	}
    printf("0%% Done\n");
    done = 10;

    // ************************************************
    // Start for loop
    // ************************************************
	if (pll) {
		if (ReadFlashPage(Flash, pdr, 0) < 0) {
			printf("\nCommunication Error: the board at Base Address %08X does not respond\n", (unsigned int) (Config->BaseAddress));
			err = 1;
		}
		else {
			 for(i=0; i<CFsize; i++) {
				if (Flash->PageSize == 264) {
					pdr[i] = CFdata[i];
				}
				else {
				 pdr[i+264] = CFdata[i];
				}
			 }
			 if (WriteFlashPage(Flash, pdr, pa) < 0) {
				printf("\nCommunication Error: the board at Base Address %08X does not respond\n", (unsigned int) (Config->BaseAddress));
				err = 1;
			 }
			 printf("100%% Done\n");
		}
	}
	else {
		for(page=0; page < NumPages; page++)  {
			if(!Config->verifyonly) {
				// Write Page
				if (WriteFlashPage(Flash, CFdata + page*Flash->PageSize, pa + page) < 0) {
					printf("\nCommunication Error: the board at Base Address %08X does not respond\n", (unsigned int) (Config->BaseAddress));
					err = 1;
					break;
					}
			}

			if(!Config->noverify) {
				// Read Page
				if (ReadFlashPage(Flash, pdr, pa + page) < 0) {
					printf("\nCommunication Error: the board at Base Address %08X does not respond\n", (unsigned int) (Config->BaseAddress));
					err = 1;
					break;
				}
				// Verify Page
				for(i=0; (i<Flash->PageSize) && ((page*Flash->PageSize+i) < CFsize); i++)  {
					if(pdr[i] != CFdata[page*Flash->PageSize + i])  {
						printf("\nFlash verify error (byte %d of page %d)!\n", i, pa + page);
						if ((Config->image == 0) && !Config->verifyonly)
							printf("The STD image can be corrupted! \nMove the jumper to Backup image and cycle the power\n");
						err = 1;
						break;
					}
				}
			}
			if (err)
				break;

			if (page == (NumPages-1)) {
				printf("100%% Done\n");
				fflush(stdout);
			} else if ( page >= (NumPages*done/100) ) {
				printf("%d%% Done\n", done);
				fflush(stdout);
				done += 10;
			}
		}  // end of for loop
	}
    if(!err) {
        if (Config->verifyonly) {
            printf("\nFirmware verified successfully. Read %d bytes\n", CFsize);
        } else {
            printf("\nFirmware updated successfully. Written %d bytes\n", CFsize);
            printf("The new firmware will be loaded after a power cycle\n");
        }
    }

    if (CFdata != NULL) 
        free(CFdata);

	if( !err) {
		err= WriteKey( Config, Flash);
	}
    CAENComm_CloseDevice(Flash->Handle);
	Flash->Handle= CAENCOM_INVALID_FILE_HANDLE;
    Wait_ms(1000);
	fflush(stdout);
 
    return err;

}


//******************************************************************************
// MAIN
//******************************************************************************
int main(int argc,char *argv[])
{
    int err=0;
	int key = 0;
	int license = 0;
	int fwrel = 0;
	int writeKey= 0;
    cvUpgradeParams Params;
    cvFlashAccess   FlashInfo;
    char ParamFile[1000] = "cvUpgrade_params.txt";
	int argIndex= 1;

    FILE *bdf;

    printf("\n");
    printf("********************************************************\n");
    printf(" CAEN SpA - Front-End Division                          \n");
    printf("--------------------------------------------------------\n");
    printf(" CAEN Board Firmware Upgrade                              \n");
    printf(" Version %s                                             \n", REVISION);
    printf("********************************************************\n\n");

    // Check command arguments (must be at least 2)
    if (argc < 3)  {
        Usage();
        return(-1);
    }
    // Inizialize defaults 
	FlashInfo.Handle= CAENCOM_INVALID_FILE_HANDLE;
    FlashInfo.FlashEnable = 0;
    Params.Link = 0;
    Params.BdNum = 0;
    Params.image = 0; // Default = standard
	Params.noverify = 0;
	Params.verifyonly = 0;
    Params.BaseAddress = 0;
    Params.key = 0;
	*Params.modelName= '\0';
	Params.delete = 0;
	Params.cfa = 1;

    strstr(Params.ConfFile, "cvUpgrade_params.txt");
	if (strcmp(argv[argIndex],"-KeyInfo") == 0 ) {
		key = 1;
	} else if (strcmp(argv[argIndex],"-License") == 0 ) {
		license = 1;
	} else if (strcmp(argv[argIndex],"-Key") == 0 ) {
		sscanf(argv[++argIndex], "%llx", &Params.key); 
		writeKey = 1;
	} else if (strcmp(argv[argIndex],"-fwrel") == 0 ) {
		fwrel = 1;
	} else {
      		sprintf(Params.ConfFile, argv[argIndex]);
	}
	argIndex++;
    if (strcmp(argv[argIndex],"USB") == 0) Params.Type = CAENComm_USB;
	if (strcmp(argv[argIndex],"OPTLINK") == 0) Params.Type = CAENComm_OpticalLink;
	if (strcmp(argv[argIndex],"USB_A4818") == 0) Params.Type = CAENComm_USB_A4818;
	argIndex++;
    for (; argIndex<argc; argIndex++) {
        if ( strcmp(argv[argIndex],"-backup") == 0 )
            Params.image = 1;
        if ( strcmp(argv[argIndex],"-no_verify") == 0 )
            Params.noverify = 1;
        if ( strcmp(argv[argIndex],"-verify_only") == 0 )
            Params.verifyonly = 1;        
        if ( strcmp(argv[argIndex],"-param") == 0 )
            sprintf(ParamFile, "%s", argv[++argIndex]);
		if ( strcmp(argv[argIndex],"-rbf") == 0 )
			Params.cfa = 0;
		if ( strcmp(argv[argIndex],"-VMEbaseaddress") == 0 )
			sscanf(argv[++argIndex], "%x", (unsigned int *) (&Params.BaseAddress));
		if ( strcmp(argv[argIndex],"-key") == 0 )
			sscanf(argv[++argIndex], "%llx", &Params.key); 
        if ( strcmp(argv[argIndex],"-link") == 0 )
            Params.Link = atoi(argv[++argIndex]);  
        if ( strcmp(argv[argIndex],"-bdnum") == 0 )
            Params.BdNum = atoi(argv[++argIndex]);  
		if ( strcmp(argv[argIndex],"-modelname") == 0 ) {
			strncpy( Params.modelName, argv[++argIndex], sizeof( Params.modelName));
			Params.modelName[ sizeof( Params.modelName)- 1]= '\0';
		}
		if ( strcmp(argv[argIndex],"-modelfamily") == 0 ) {
			strncpy( Params.family, argv[++argIndex], sizeof( Params.family));
			Params.family[ sizeof( Params.family)- 1]= '\0';
		}
		if ( strcmp(argv[argIndex],"-filepath") == 0 ) {
			strncpy( Params.filepath, argv[++argIndex], sizeof( Params.filepath));
			Params.filepath[ sizeof( Params.filepath)- 1]= '\0';
		}
        if ( strcmp(argv[argIndex],"-delete") == 0 )
            Params.delete = 1;
    }

    // open the board descriptor file
	if ((!Params.cfa) || (fwrel)  || (license) || (key) | (writeKey)) {
		bdf = fopen(ParamFile,"r");
		if(bdf == 0) {
			printf("Can't open file %s\n", ParamFile);
			Usage();
			return (-2); 
		}

		fscanf(bdf, "%s", Params.Models);
		fscanf(bdf, "%x", (unsigned int *) (&FlashInfo.Sel_Flash));
		fscanf(bdf, "%x", (unsigned int *) (&FlashInfo.RW_Flash));
		fscanf(bdf, "%d", &FlashInfo.RegSize);
		fscanf(bdf, "%d", &FlashInfo.PageSize);
		fscanf(bdf, "%d", &Params.FirstPageStd);
		fscanf(bdf, "%d", &Params.FirstPageBck);
		if (!feof(bdf)) {
			int tmp;
			if (fscanf(bdf, "%d", &tmp) > 0)
				FlashInfo.FlashEnable = tmp;
		}
		fscanf(bdf, "%x", &Params.ROCFwm);
		fscanf(bdf, "%x", &Params.AMCFwm);
		fclose(bdf);
	}

    // Call cvUpgrade function
	//if (key == 0)
 //     err = cvUpgrade(&Params, &FlashInfo);
	//else {
	//	if ((!strncmp(Params.Models,"DIGITIZERS",1000)) || 
	//		(!strncmp(Params.Models,"V1495",1000)) ) {
	//			err = KeyInfo(&Params, &FlashInfo);
	//	}
	//}
	if (key ) {
		if ((!strncmp(Params.Models,"DIGITIZERS",1000)) || 
			(!strncmp(Params.Models,"V1495",1000)) ) {
				err = KeyInfo(&Params, &FlashInfo);
		}
	} else if (license ) {
		if ((!strncmp(Params.Models,"DIGITIZERS",1000)) || 
			(!strncmp(Params.Models,"V1495",1000)) ) {
				err = License(&Params, &FlashInfo);
		}
	} else if (writeKey ) {
		err = WriteKey(&Params, &FlashInfo);
	} else if (fwrel ) {
		err = GetFWRel(&Params, &FlashInfo);
	} else {
      err = cvUpgrade(&Params, &FlashInfo);
	  if (err) return err;
	}
    if (err)
        return (CvUpgrade_GenericError);
    else{
	fflush(stdout);
		_Exit(0);
    }
}


