#include <stdio.h>
#include "CAENDigitizer.h"

#include "keyb.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAXNB 1 /* Number of connected boards */

int checkCommand() {
	int c = 0;
	if(!kbhit())
			return 0;

	c = getch();
	switch (c) {
		case 's': 
			return 9;
			break;
		case 'k':
			return 1;
			break;
		case 'q':
			return 2;
			break;
	}
	return 0;
}

int main(int argc, char* argv[])
{
    /* The following variable is the type returned from most of CAENDigitizer
    library functions and is used to check if there was an error in function
    execution. For example:
    ret = CAEN_DGTZ_some_function(some_args);
    if(ret) printf("Some error"); */
	CAEN_DGTZ_ErrorCode ret;

    /* The following variable will be used to get an handler for the digitizer. The
    handler will be used for most of CAENDigitizer functions to identify the board */
	int	handle[MAXNB];

    CAEN_DGTZ_BoardInfo_t BoardInfo;
	CAEN_DGTZ_EventInfo_t eventInfo;
	CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
	char *buffer = NULL;
	int MajorNumber;
	int i,b;
	int c = 0, count[MAXNB];
	char * evtptr = NULL;
	uint32_t size,bsize;
	uint32_t numEvents;
	i = sizeof(CAEN_DGTZ_TriggerMode_t);

    for(b=0; b<MAXNB; b++){
        /* IMPORTANT: The following function identifies the different boards with a system which may change
        for different connection methods (USB, Conet, ecc). Refer to CAENDigitizer user manual for more info.
        brief:
            CAEN_DGTZ_OpenDigitizer(<LikType>, <LinkNum_OR_A4818_PID>, <ConetNode>, <VMEBaseAddress>, <*handler>);
        Some examples below */
        
        /* The following is for b boards connected via b USB direct links
        in this case you must set <LikType> = CAEN_DGTZ_USB and <ConetNode> = <VMEBaseAddress> = 0 */
        //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, b, 0, 0, &handle[b]);

        /* The following is for b boards connected via 1 opticalLink in dasy chain
        in this case you must set <LikType> = CAEN_DGTZ_PCI_OpticalLink and <LinkNum> = <VMEBaseAddress> = 0 */
        //ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, b, Params[b].VMEBaseAddress, &handle[b]);

		/* The following is for VME boards connected using A4818 with PID number p, through an V2718 CONET-VME Bridge
        in this case you must set <LikType> = CAEN_DGTZ_USB_A4818_V2718, <LinkNum_OR_A4818_PID> = p, <ConetNode> = 0 
		and <VMEBaseAddress> = <0xXXXXXXXX> (address of the VME board) */
        //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB_A4818_V2718, p, 0, 0xXXXXXXXX, &handle[b]);

        /* The following is for b boards connected to A2818 (or A3818) via opticalLink (or USB with A1718)
        in this case the boards are accessed throught VME bus, and you must specify the VME address of each board:
        <LikType> = CAEN_DGTZ_PCI_OpticalLink (CAEN_DGTZ_PCIE_OpticalLink for A3818 or CAEN_DGTZ_USB for A1718)
        <LinkNum> must be the bridge identifier
        <ConetNode> must be the port identifier in case of A2818 or A3818 (or 0 in case of A1718)
        <VMEBaseAddress>[0] = <0xXXXXXXXX> (address of first board) 
        <VMEBaseAddress>[1] = <0xYYYYYYYY> (address of second board) 
        ...
        <VMEBaseAddress>[b-1] = <0xZZZZZZZZ> (address of last board)
        See the manual for details */
        ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,0,&handle[b]);
        if(ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer\n");
            goto QuitProgram;
        }
        /* Once we have the handler to the digitizer, we use it to call the other functions */
        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
        printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
        printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
	    
		// Check firmware revision (DPP firmwares cannot be used with this demo */
		sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
		if (MajorNumber >= 128) {
			printf("This digitizer has a DPP firmware!\n");
			goto QuitProgram;
		}

        ret = CAEN_DGTZ_Reset(handle[b]);                                               /* Reset Digitizer */
	    ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);                                 /* Get Board Info */
	    ret = CAEN_DGTZ_SetRecordLength(handle[b],4096);                                /* Set the lenght of each waveform (in samples) */
	    ret = CAEN_DGTZ_SetChannelEnableMask(handle[b],1);                              /* Enable channel 0 */
	    ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle[b],0,32768);                  /* Set selfTrigger threshold */
	    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY,1);  /* Set trigger on channel 0 to be ACQ_ONLY */
	    ret = CAEN_DGTZ_SetSWTriggerMode(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY);         /* Set the behaviour when a SW tirgger arrives */
	    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle[b],3);                                /* Set the max number of events to transfer in a sigle readout */
        ret = CAEN_DGTZ_SetAcquisitionMode(handle[b],CAEN_DGTZ_SW_CONTROLLED);          /* Set the acquisition mode */
        if(ret != CAEN_DGTZ_Success) {
            printf("Errors during Digitizer Configuration.\n");
            goto QuitProgram;
        }
    }
	printf("\n\nPress 's' to start the acquisition\n");
    printf("Press 'k' to stop  the acquisition\n");
    printf("Press 'q' to quit  the application\n\n");
    while (1) {
		c = checkCommand();
		if (c == 9) break;
		if (c == 2) return;
		Sleep(100);
    }
    /* Malloc Readout Buffer.
    NOTE1: The mallocs must be done AFTER digitizer's configuration!
    NOTE2: In this example we use the same buffer, for every board. We
    Use the first board to allocate the buffer, so if the configuration
    is different for different boards (or you use different board models), may be
    that the size to allocate must be different for each one. */
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0],&buffer,&size);

    
    for(b=0; b<MAXNB; b++)
            /* Start Acquisition
            NB: the acquisition for each board starts when the following line is executed
            so in general the acquisition does NOT starts syncronously for different boards */
            ret = CAEN_DGTZ_SWStartAcquisition(handle[b]);

    // Start acquisition loop
	while(1) {
        for(b=0; b<MAXNB; b++) {
		    ret = CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a SW Trigger */

		    ret = CAEN_DGTZ_ReadData(handle[b],CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); /* Read the buffer from the digitizer */

            /* The buffer red from the digitizer is used in the other functions to get the event data
            The following function returns the number of events in the buffer */
		    ret = CAEN_DGTZ_GetNumEvents(handle[b],buffer,bsize,&numEvents);

		    printf(".");
		    count[b] +=numEvents;
		    for (i=0;i<numEvents;i++) {
                /* Get the Infos and pointer to the event */
			    ret = CAEN_DGTZ_GetEventInfo(handle[b],buffer,bsize,i,&eventInfo,&evtptr);

                /* Decode the event to get the data */
			    ret = CAEN_DGTZ_DecodeEvent(handle[b],evtptr,&Evt);
			    //*************************************
			    // Event Elaboration
			    //*************************************
			    ret = CAEN_DGTZ_FreeEvent(handle[b],&Evt);
		    }
		    c = checkCommand();
		    if (c == 1) goto Continue;
		    if (c == 2) goto Continue;
        } // end of loop on boards
    } // end of readout loop

Continue:
    for(b=0; b<MAXNB; b++)
        printf("\nBoard %d: Retrieved %d Events\n",b, count[b]);
    goto QuitProgram;

/* Quit program routine */
QuitProgram:
    // Free the buffers and close the digitizers
	ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    for(b=0; b<MAXNB; b++)
        ret = CAEN_DGTZ_CloseDigitizer(handle[b]);
	printf("Press 'Enter' key to exit\n");
	c = getchar();
	return 0;
}

