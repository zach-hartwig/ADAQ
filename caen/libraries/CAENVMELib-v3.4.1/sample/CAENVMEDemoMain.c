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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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
	short         ConetNode;
	uint32_t      LinkNumber, Pid;
	int32_t       BHandle;



	if( (argc != 3) && (argc != 4) )
	{
		printf("Usage: CAENVMEDemo V1718 <LinkNum>\n");
		printf("       CAENVMEDemo V2718 <LinkNum> <ConetNode>\n");
		printf("       CAENVMEDemo USB_A4818 <PID> <ConetNode>\n");
		return 1;
	}
	else 
	{
		if( strcmp((char*)argv[1], "V1718") == 0 )
		{
			if( argc == 3 )
			{
				VMEBoard = cvV1718;
				LinkNumber = atoi(argv[2]);
				ConetNode = 0;
				if (CAENVME_Init2(VMEBoard, &LinkNumber, ConetNode, &BHandle) != cvSuccess)
				{
					printf("\n\n Error opening the device\n");
					return 1;
				}
			}
			else
			{
				printf("       CAENVMEDemo V1718 <LinkNum>\n");
				return 1;
			}
		}
		else 
		{
			if( strcmp((char*)argv[1], "V2718") == 0 )
			{
				if( argc == 4 )
				{
					LinkNumber = atoi(argv[2]);
					ConetNode = atoi(argv[3]);
					VMEBoard = cvV2718;
					if (CAENVME_Init2(VMEBoard, &LinkNumber, ConetNode, &BHandle) != cvSuccess)
					{
						printf("\n\n Error opening the device\n");
						return 1;
					}
				}
				else
				{
					printf("       CAENVMEDemo V2718 <LinkNum> <ConetNode>\n");
					return 1;
				}
			}
			else
			{
				if (strcmp((char*)argv[1], "USB_A4818") == 0)
				{
					if (argc == 4)
					{
						Pid = atoi(argv[2]);
						ConetNode = atoi(argv[3]);
						VMEBoard = cvUSB_A4818;
						if (CAENVME_Init2(VMEBoard, &Pid, ConetNode, &BHandle) != cvSuccess)
						{
							printf("\n\n Error opening the device\n");
							return 1;
						}
					}
					else
					{
						printf("       CAENVMEDemo USB_A4818 <PID> <ConetNode>\n");
						return 1;
					}
				}
				else {
					printf("Usage: CAENVMEDemo V1718 <LinkNum>\n");
					printf("       CAENVMEDemo V2718 <LinkNum> <ConetNode>\n");
					printf("       CAENVMEDemo USB_A4818 <PID> <ConetNode>\n");
					return 1;
				}
			}
		}

	}

	con_init() ;

	CaenVmeManual(BHandle,1) ;

	con_end() ;

	CAENVME_End(BHandle);

	return 0;
}

