/*
-----------------------------------------------------------------------------

--- CAEN SpA - Computing Systems Division --- 

-----------------------------------------------------------------------------

Name		:	CAENVMEDemoMain.c

Project		:	CaenVmeDemo

Description :	Example program for V1718 & V2718 control.

---------------------------
Date		:	November 2004
Release		:	1.0
Author		:	C.Landi

---------------------------
Date		:	August 2006
Release		:	1.1
Author		:	NDA
Description :	64 bit porting (CAENVMElib rev >= 2.5)

-----------------------------------------------------------------------------


-----------------------------------------------------------------------------
*/


#include <stdlib.h>
#include <stdio.h>
//#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "CAENVMElib.h"
#include "console.h"


// -----------------------------------------------------------------------------

extern void CaenVmeManual(long, short) ; 

// -----------------------------------------------------------------------------



/*
-----------------------------------------------------------------------------

Main program

-----------------------------------------------------------------------------
*/


int main(int argc, char **argv) 

{
	CVBoardTypes  VMEBoard;
	short         Link;
	short         Device;
	short		  Pid;
	int32_t       BHandle,BHandle1;



	if( (argc != 3) && (argc != 4) )
	{
		printf("Usage: CAENVMEDemo V1718 <VMEDevice>\n");
		printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
		printf("       CAENVMEDemo USB_A4818 <PID>\n");
		return 1;
	}
	else 
	{
		if( strcmp((char*)argv[1], "V1718") == 0 )
		{
			if( argc == 3 )
			{
				VMEBoard = cvV1718;
				Device = atoi((const char*)argv[2]);
				Link = 0;
				if (CAENVME_Init(VMEBoard, Link, Device, &BHandle) != cvSuccess)
				{
					printf("\n\n Error opening the device\n");
					return 1;
				}
			}
			else
			{
				printf("       CAENVMEDemo V1718 <VMEDevice>\n");
				return 1;
			}
		}
		else 
		{
			if( strcmp((char*)argv[1], "V2718") == 0 )
			{
				if( argc == 4 )
				{
					Device = atoi((const char*) argv[2]);
					Link = atoi((const char*) argv[3]);
					VMEBoard = cvV2718;	
					if (CAENVME_Init(VMEBoard, Link, Device, &BHandle) != cvSuccess)
					{
						printf("\n\n Error opening the device\n");
						return 1;
					}
				}
				else
				{
					printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
					return 1;
				}
			}
			else
			{
				if (strcmp((char*)argv[1], "USB_A4818") == 0)
				{
					if (argc == 3)
					{
						Pid = atoi((const char*)argv[2]);
						VMEBoard = cvUSB_A4818;
						if (CAENVME_Init(VMEBoard, 0, Pid, &BHandle) != cvSuccess)
						{
							printf("\n\n Error opening the device\n");
							return 1;
						}
					}
					else
					{
						printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
						return 1;
					}
				}
				else {
					printf("Usage: CAENVMEDemo V1718 <VMEDevice>\n");
					printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
					return 1;
				}
			}
		}

	}

	{
		int ret;
		uint32_t *Addrs =(uint32_t*) malloc(64 * sizeof(int));
		uint32_t *Buffer = (uint32_t*) malloc(64 * sizeof(int));
		CVAddressModifier *Ams = (CVAddressModifier*) malloc(64 * sizeof(CVAddressModifier));
		CVDataWidth *DWs = (CVDataWidth*) malloc(64 * sizeof(CVDataWidth));
		CVErrorCodes *ECs = (CVErrorCodes*) malloc(64 * sizeof(CVErrorCodes));
		int i;

		for (i=0; i<64;i++) {
			Addrs[i] = 0x22228000 + (i << 16);
			Ams[i] = cvA32_U_DATA;
			DWs[i] = cvD32;
		}
		ret = CAENVME_MultiRead(BHandle,Addrs, Buffer,64,Ams,DWs,ECs);
		if (ret != cvSuccess) 
			return;
	}
	con_init() ;

	// CAENVME_SetFIFOMode(BHandle,1) ;

	CaenVmeManual(BHandle,1) ;

	con_end() ;

	CAENVME_End(BHandle);
	return 0;
}

