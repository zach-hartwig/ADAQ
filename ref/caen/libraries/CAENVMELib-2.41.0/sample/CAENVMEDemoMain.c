/*
    -----------------------------------------------------------------------------

                   --- CAEN SpA - Computing Systems Division --- 

    -----------------------------------------------------------------------------

    Name		:	CAENVMEDemoMain.c

    Project		:	CaenVmeDemo

    Description :	Example program for V1718 & V2718 control.

    Date		:	November 2004
    Release		:	1.0
    Author		:	C.Landi

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

#include "CAENVMElib.h"
#include "console.h"


// -----------------------------------------------------------------------------

extern  void CaenVmeManual(long, short) ; 

// -----------------------------------------------------------------------------



/*
    -----------------------------------------------------------------------------

      Main program

    -----------------------------------------------------------------------------
*/


int main(int argc, void *argv[]) 

{
CVBoardTypes  VMEBoard;
short         Link;
short         Device;
int32_t       BHandle;



if( (argc != 3) && (argc != 4) )
    {
    printf("Usage: CAENVMEDemo V1718 <VMEDevice>\n");
    printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
    exit(1);
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
            }
        else
            {
            printf("       CAENVMEDemo V1718 <VMEDevice>\n");
            exit(1);
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
                }
            else
                {
                printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
                exit(1);
                }
            }
        else
            {
            printf("Usage: CAENVMEDemo V1718 <VMEDevice>\n");
            printf("       CAENVMEDemo V2718 <VMEDevice> <VMELink>\n");
            exit(1);
            }
        }

    }

// Initialize the Board

if( CAENVME_Init(VMEBoard, Link, Device, &BHandle) != cvSuccess ) 
    {
    printf("\n\n Error opening the device\n");
    exit(1);
    }


con_init() ;

// CAENVME_SetFIFOMode(BHandle,1) ;
	
CaenVmeManual(BHandle,1) ;

con_end() ;

CAENVME_End(BHandle);
}

