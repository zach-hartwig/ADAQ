// CaenVMEUpgrade.c : Defines the entry point for the console application.
//
// 06/06/06 - Cambiato l'ordine dei parametri della Init. Link e BdNum erano
//            invertiti quindi non funzionava l'upgrade con piu' di una A2818.

#ifdef LINUX
	#include <inttypes.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

#include "CFASegment.h"

#ifdef LINUX

typedef unsigned char   byte;
typedef int             VARIANT_BOOL;
#define FALSE           (0)
#define TRUE            (-1)

#endif

#include <stdarg.h>
#include <a3818.h>
#include "p30.h"
#include "A3818Upgrade.h"
#include "flash.h"
#include <CAENVMElib.h>

// Define FIRMWARE regions into flash by using their start address (116-bit word address)
#define FIRST_FIRMWARE_PAGE_BASE_WORD_ADDRESS    0x000000
#define SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS   0x200000
#define THIRD_FIRMWARE_PAGE_BASE_WORD_ADDRESS    0x400000
#define FOURTH_FIRMWARE_PAGE_BASE_WORD_ADDRESS   0x600000

#define MAIN_MEM_PAGE_READ          0xD2
#define MAIN_MEM_PAGE_PROG_TH_BUF1  0x82
#define PAGE_ERASE                  0x81

#define FIRST_PAGE_BCK  1408
#define FIRST_PAGE_STD  768
#define FIRST_PAGE_BCK_3718  0x7E00
#define FIRST_PAGE_STD_3718  0x3E00 

#define CF_SIZE         166965

#define BRIDGENAME	((unsigned short)0x38) // only 3718 and 4718 have this register

/* get time in milliseconds */
/*
static long get_time()
{
long time_ms;

#ifdef WIN32
    struct _timeb timebuffer;

    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;

    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif

    return time_ms;
}
*/

void usage(void)
{
		printf("\nCAENBridgeUpgrade V2.2 \n\n");
        printf("Usage: CAENBridgeUpgrade V1718 <VMEDevIndex> [FlashFile] [standard|backup] [-rbf]\n");
        printf("       CAENBridgeUpgrade V2718 <VMEDevIndex> <PCIDevIndex> [FlashFile] [standard|backup] [-rbf]\n");
		printf("       CAENBridgeUpgrade USB_A4818_V2718 <VMEDevIndex> <A4818_PID> [FlashFile] [standard|backup] [-rbf]\n");
        printf("       CAENBridgeUpgrade A2719 <VMEDevIndex> <PCIDevIndex> [FlashFile] [standard|backup] [-rbf]\n");
		printf("       CAENBridgeUpgrade USB_A4818_A2719 <VMEDevIndex> <A4818_PID> [FlashFile] [standard|backup] [-rbf]\n");
        printf("       CAENBridgeUpgrade A2818 <PCIDevIndex> [FlashFile] [standard|backup] [-rbf]\n");
        printf("       CAENBridgeUpgrade A3818 <PCIDevIndex> [FlashFile] [TargetFlashImage (0,1,2,3)] [-rbf]\n");
        printf("\n");
		printf("Or:    CAENBridgeUpgrade V1718 <VMEDevIndex> [FwRevFile] fwrev\n");
        printf("       CAENBridgeUpgrade V2718 <VMEDevIndex> <PCIDevIndex> [FwRevFile] fwrev\n");
		printf("       CAENBridgeUpgrade USB_A4818_V2718 <VMEDevIndex> <A4818_PID> [FwRevFile] fwrev\n");
		printf("       CAENBridgeUpgrade USB_A4818_A2719 <VMEDevIndex> <A4818_PID> [FwRevFile] fwrev\n");
        printf("       CAENBridgeUpgrade A2719 <VMEDevIndex> <PCIDevIndex> [FwRevFile] fwrev\n");
        printf("       CAENBridgeUpgrade A2818 <PCIDevIndex> [FwRevFile] fwrev\n");
        printf("       CAENBridgeUpgrade A3818 <PCIDevIndex> [FwRevFile] fwrev\n");  
}

CVBoardTypes DecodeDev(char *arg)
{
        if(( strcmp(arg, "V1718") == 0 )|| ( strcmp(arg, "v1718") == 0 )) return cvV1718;
        if(( strcmp(arg, "V2718") == 0 )|| ( strcmp(arg, "v2718") == 0 )) return cvV2718;
        if(( strcmp(arg, "A2719") == 0 )|| ( strcmp(arg, "a2719") == 0 )) return cvA2719;
        if(( strcmp(arg, "A2818") == 0 )|| ( strcmp(arg, "a2818") == 0 )) return cvA2818;
        if(( strcmp(arg, "A3818") == 0 )|| ( strcmp(arg, "a3818") == 0 )) return cvA3818;
		if ((strcmp(arg, "USB_A4818_V2718") == 0) || (strcmp(arg, "usb_a4818_V2718") == 0)) return cvUSB_A4818_V2718_LOCAL;
		if ((strcmp(arg, "USB_A4818_A2719") == 0) || (strcmp(arg, "usb_a4818_a2719") == 0)) return cvUSB_A4818_A2719_LOCAL;
        return -1;
}

int validateModel(FILE* fin, CVBoardTypes model, char** fwdata, int* fwsize) {
	//FILE* fin = fopen(fname, "rb");
	while (!feof(fin)) {
		int last;
		CFASegmentPtr segm = newSegment(fin, &last);
		if (segm == NULL) {
			fclose(fin);
			return -2;
		}
		if (getSegmentType(segm) == CFA_SEGMENT_TYPE_2) {
			CFASegmentType2* t2s = toType2(segm);
			int i;
			for (i = 0; i < t2s->numModels; i++) {
				if (model == t2s->models[i]->modelId) {
					*fwdata = malloc(t2s->fwSizeBytes);
					memcpy(*fwdata, t2s->fwData, t2s->fwSizeBytes);
					*fwsize = t2s->fwSizeBytes;
					deleteSegment(segm);
					return 0;
				}
			}
		}
		deleteSegment(segm);
		if (last) break;
	}
	return -1;
}

int main(int argc, char *argv[])
{
        FILE          *stream =NULL;
		char*		  fwdata;
		int			  fwsize;
        CVBoardTypes  Board;
        short         Link;
        short         Device;
        int32_t       BHandle;
        int           fwcopy;
		int		fwread;
		int		cfa = 1;
		int	    fi;
        int		TargetFlashImage = 0;
        int           finish, i, verify_err,ret;
        int           bp, bcnt, pp;
        unsigned char pdw[264], pdr[264], c;
		unsigned int  flashPageSize = 264;
        cvFlashAccess	flash;
		uint32_t value = 0;
        // char           FWRel[20];

        if( argc < 4 ) {
                usage();
                exit(1);
        }

        fwcopy = 0;
		fwread = 0;
		cfa = 0;
		fi = 0;

        switch( (Board = DecodeDev(argv[1])) ) {
        case cvV1718:
        case cvA2818:
                if( argc >= 5 ) {
                        if( strcmp((char*)argv[4], "standard" ) == 0 ) {
                                fwcopy = 0;
                        } else {
                                if( strcmp((char*)argv[4], "backup") == 0 ) {
                                        fwcopy = 1;
                                } else if ( strcmp((char*)argv[4], "fwrev") == 0 ) {
										fwread = 1;
								} else {
                                        usage();
                                        exit(1);
                                }
                        }
						if (argc >= 6 && (strcmp(argv[5], "-rbf") == 0)) {
							cfa = 0;
						}
                }
                if( Board == cvV1718 ) {
						flash.Sel_Flash = 0x04; // FLENA 
						flash.RW_Flash = 0x03;
                        Device = atoi((const char*)argv[2]);
                        Link = 0;
                } else {
						flash.Sel_Flash = 0x14; 
						flash.RW_Flash = 0x10;
                        Device = 0;
                        Link = atoi((const char*)argv[2]);
                }
                if(!fwread && (stream = fopen((const char*)argv[3], "rb" )) == NULL ) {
                        printf( "File %s not found\n",(char *)argv[3]);
                        exit(1);
                }
				if (!fwread)
					printf("\n\nLoading file %s\n", argv[3]);
                break;
        case cvV2718:
        case cvA2719:
		case cvUSB_A4818_V2718_LOCAL:
		case cvUSB_A4818_A2719_LOCAL:
                if( argc < 5 ) {
                        usage();
                        exit(1);
                }
                if( argc >= 6 ) {
                        if( strcmp((char*)argv[5], "standard") == 0 ) {
                                fwcopy = 0;
                        } else {
                                if( strcmp((char*)argv[5], "backup") == 0 ) {
                                        fwcopy = 1;
                                } else if ( strcmp((char*)argv[5], "fwrev") == 0 ) {
										fwread = 1;
								} else {
                                        usage();
                                        exit(1);
                                }
                        }
						if (argc >= 7 && strcmp(argv[6], "-rbf") == 0) {
							cfa = 0;
						}
                }
                Device = atoi((const char*) argv[3]);
                Link = atoi((const char*) argv[2]);
                if(!fwread && (stream = fopen((const char*)argv[4], "rb" )) == NULL ) {
                        printf( "File %s not found\n",argv[4]);
                        exit(1);
                }
                if ((Board == cvA2719) || (Board == cvUSB_A4818_A2719_LOCAL)) {
                	flash.Sel_Flash = 0x03;
                	flash.RW_Flash = 0x02;
				}else {
					flash.Sel_Flash = 0x04;
					flash.RW_Flash = 0x03;
				}
				if (!fwread) 
					printf("\n\nLoading file %s\n", argv[4]);
                break;
        case cvA3818:
			if( argc >= 5 ) {
						if (strcmp((char*)argv[4], "fwrev")==0) fwread = 1;
						else TargetFlashImage = atoi((const char*)argv[4]);
						if (argc >= 6 && strcmp(argv[5], "-rbf") == 0) {
							cfa = 0;
						}
                }
                Device = 0;
                Link = atoi((const char*)argv[2]);
                if(!fwread && (stream = fopen((const char*)argv[3], "rb" )) == NULL ) {
                        printf( "File %s not found\n",argv[3]);
                        exit(1);
                }
				if (!fwread)
					printf("\n\nLoading file %s\n", argv[3]);
                break;
        default:
            usage();
            exit(1);
        }

		if (cfa) {
			printf("CFA mode\n");
			if (validateModel(stream, Board, &fwdata, &fwsize) < 0) {
				fclose(stream);
				printf("\n\nCannot find a compatible firmware in the CFA file\n");
				exit(99);
			}
		}

        // initialize the Board
        if( CAENVME_Init(Board, Link, Device, &BHandle) != cvSuccess ) {
                printf("\n\nError opening the device\n");
                exit(1);
        }

		if (fwread) {
			char fwrev[100];
			int ret; // major, minor;
			if ((Board == cvV2718) || (Board == cvA2719) || (Board == cvUSB_A4818_V2718_LOCAL) || (Board == cvUSB_A4818_A2719_LOCAL)) {
				stream = fopen((const char*)argv[4], "w" );
			}
			else {
				stream = fopen((const char*)argv[3], "w" );
			}
			memset(fwrev,0,100);
			ret = CAENVME_BoardFWRelease(BHandle,fwrev);
			if (ret) {
				printf("\n\nError reading firmware revision\n");
				CAENVME_End(BHandle);
				exit(1);
			}
			//major = fwrev >> 8;
           // minor = fwrev & 0xFF;
			//fprintf(stream,"Firmware revision = %d.%02d",major,minor);
			fprintf(stream,"Firmware revision = %s",fwrev);
			fclose(stream);
			printf("\n\nFirmware revision read successfully\n");
			CAENVME_End(BHandle);
			exit(0);
		}

        if (Board == cvA3818) {
			if (!cfa) ret = A3818UpgradeFromFile(BHandle,stream,TargetFlashImage,A3818_UPGRADE_FULL);
			else ret = A3818UpgradeFromMem(BHandle,fwdata,fwsize,TargetFlashImage,A3818_UPGRADE_FULL);
			CAENVME_End(BHandle);
			exit(0);
        }
        
        bcnt = 0;                // byte counter

		ret = CAENVME_ReadRegister(BHandle, BRIDGENAME, &value);
		if (ret != cvSuccess) return ret;
		if (value == 0x3718) {
			flashPageSize = 256;
			if (fwcopy == 0)
				pp = FIRST_PAGE_STD_3718;   // page pointer
			else
				pp = FIRST_PAGE_BCK_3718;
		}
		else {
			if (fwcopy == 0)
				pp = FIRST_PAGE_STD;   // page pointer
			else
				pp = FIRST_PAGE_BCK;
		}
        bp = 0;                  // byte pointer in the page
        finish = 0;              // it goes high when eof is found
        verify_err = 0;          // number of errors during verify
		
		flash.Handle = BHandle;
		flash.RegSize = 4;
		flash.FlashEnable = 1;
		flash.PageSize = flashPageSize;

		if ((Board == cvA2719) || (Board == cvUSB_A4818_A2719_LOCAL)) {
					uint32_t reg32;
					int res = 0;

					reg32 = !flash.FlashEnable;
					res |= CAENVME_WriteRegister(flash.Handle, flash.Sel_Flash, reg32);
					// Enable flash
					res |= CAENVME_WriteRegister(flash.Handle, flash.Sel_Flash, flash.FlashEnable);

					// Opcode
					res |= CAENVME_WriteRegister(flash.Handle, flash.RW_Flash, MAIN_MEM_PAGE_PROG_TH_BUF1_CMD);

					// Disable flash
				   reg32 = !flash.FlashEnable;
					res |= CAENVME_WriteRegister(flash.Handle, flash.Sel_Flash, reg32);
				}

        while( !finish ) {
                if (!cfa) c = (unsigned char)fgetc(stream);  // read one byte from file
				else c = fwdata[fi++];
                // mirror byte (lsb becomes msb)
				if (value != 0x3718) {
					pdw[bp] = 0;
					for (i = 0; i < 8; i++)
						if (c & (1 << i))
							pdw[bp] = pdw[bp] | (0x80 >> i);
					bp++;
				}
				else {
					pdw[bp] = c;
					bp++;
				}
                bcnt++;
                if( !cfa && feof(stream) ) finish = 1;
				else if (cfa && fi >= fwsize) finish = 1;
                //write and verify a page
                if( (bp == flashPageSize) || finish ) {
                        printf(".");
                        fflush(stdout);
                        if ((Board == cvV1718) || (Board == cvV2718) || (Board == cvA2818) || (Board == cvA2719) || (Board == cvUSB_A4818_V2718_LOCAL) || (Board == cvUSB_A4818_A2719_LOCAL)) {
                            CAENVME_WriteFlashPage(BHandle, pdw, pp);  // write page
                            CAENVME_ReadFlashPage(BHandle, pdr, pp);    // read page
                        }
                        else {	
                          WriteFlashPage(&flash, pdw, pp);  // write page
						  ReadFlashPage(&flash, pdr, pp);
							           
			}
                        for( i = 0; i < bp; i++ ) {               // verify page
                                if( pdr[i] != pdw[i] ) {
                                	printf("%x = %x\n",pdr[i],pdw[i]);
                                        printf("\n\n Writing or Verify error at page %d - %d\n", pp, i);
                                        verify_err = 1;
                                        finish = 1;
                                        i = bp;
                                }
            }
                        bp = 0;
                        pp++;
                }
        }  // end of while loop


        fclose(stream);

        CAENVME_End(BHandle);

        if( verify_err > 0 ) {
                printf("\n\n%d errors found during verify!\n",verify_err);
                printf("Firmware upgrade not done. Move jumper to BCK to restore\n\n");
				return 1;
        } else {
                printf("\n\nFirmware updated without errors. Written %d bytes\n",bcnt);
                printf("Power cycle to load the new firmware\n\n");
				return 0;
        }
}

