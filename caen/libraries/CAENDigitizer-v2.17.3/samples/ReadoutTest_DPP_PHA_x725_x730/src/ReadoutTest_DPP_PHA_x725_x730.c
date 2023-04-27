/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/

#include <CAENDigitizer.h>

#include <stdio.h>
#include <stdlib.h>

//#define INDIVIDUAL_TRIGGER_INPUTS
// The following define must be set to the actual number of connected boards
#define MAXNB   1
// NB: the following define MUST specify the ACTUAL max allowed number of board's channels
// it is needed for consistency inside the CAENDigitizer's functions used to allocate the memory
#define MaxNChannels 8

// The following define MUST specify the number of bits used for the energy calculation
#define MAXNBITS 15

/* include some useful functions from file Functions.c
you can find this file in the src directory */
#include "Functions.h"

/* ###########################################################################
*  Functions
*  ########################################################################### */

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPPParamsPHA_t DPPParams)
*   \brief   Program the registers of the digitizer with the relevant parameters
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams)
{
    /* This function uses the CAENDigitizer API functions to perform the digitizer's initial configuration */
    int i, ret = 0;

    /* Reset the digitizer */
    ret |= CAEN_DGTZ_Reset(handle);

    if (ret) {
        printf("ERROR: can't reset the digitizer.\n");
        return -1;
    }
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0x01000114);  // Channel Control Reg (indiv trg, seq readout) ??

    /* Set the DPP acquisition mode
    This setting affects the modes Mixed and List (see CAEN_DGTZ_DPP_AcqMode_t definition for details)
    CAEN_DGTZ_DPP_SAVE_PARAM_EnergyOnly        Only energy (DPP-PHA) or charge (DPP-PSD/DPP-CI v2) is returned
    CAEN_DGTZ_DPP_SAVE_PARAM_TimeOnly        Only time is returned
    CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime    Both energy/charge and time are returned
    CAEN_DGTZ_DPP_SAVE_PARAM_None            No histogram data is returned */
    ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
    
    // Set the digitizer acquisition mode (CAEN_DGTZ_SW_CONTROLLED or CAEN_DGTZ_S_IN_CONTROLLED)
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
    
    // Set the number of samples for each waveform
    ret |= CAEN_DGTZ_SetRecordLength(handle, Params.RecordLength);

    // Set the I/O level (CAEN_DGTZ_IOLevel_NIM or CAEN_DGTZ_IOLevel_TTL)
    ret |= CAEN_DGTZ_SetIOLevel(handle, Params.IOlev);

    /* Set the digitizer's behaviour when an external trigger arrives:

    CAEN_DGTZ_TRGMODE_DISABLED: do nothing
    CAEN_DGTZ_TRGMODE_EXTOUT_ONLY: generate the Trigger Output signal
    CAEN_DGTZ_TRGMODE_ACQ_ONLY = generate acquisition trigger
    CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT = generate both Trigger Output and acquisition trigger

    see CAENDigitizer user manual, chapter "Trigger configuration" for details */
    ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);

    // Set the enabled channels
    ret |= CAEN_DGTZ_SetChannelEnableMask(handle, Params.ChannelMask);

    // Set how many events to accumulate in the board memory before being available for readout
    ret |= CAEN_DGTZ_SetDPPEventAggregation(handle, Params.EventAggr, 0);
    
    /* Set the mode used to syncronize the acquisition between different boards.
    In this example the sync is disabled */
    ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    
    // Set the DPP specific parameters for the channels in the given channelMask
    ret |= CAEN_DGTZ_SetDPPParameters(handle, Params.ChannelMask, &DPPParams);
    
    for(i=0; i<MaxNChannels; i++) {
        if (Params.ChannelMask & (1<<i)) {
            // Set a DC offset to the input signal to adapt it to digitizer's dynamic range
            ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, 0x8000);
            
            // Set the Pre-Trigger size (in samples)
            ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, 1000);
            
            // Set the polarity for the given channel (CAEN_DGTZ_PulsePolarityPositive or CAEN_DGTZ_PulsePolarityNegative)
            ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity);
        }
    }

    /* Set the virtual probes settings
    DPP-PHA can save:
    2 analog waveforms:
        the first and the second can be specified with the  ANALOG_TRACE 1 and 2 parameters
        
    2 digital waveforms:
        the first can be specified with the DIGITAL_TRACE_1 parameter
        the second  is always the trigger

    CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE	 -> Save only the ANALOG_TRACE_1 waveform
    CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL      -> Save also the waveform specified in  ANALOG_TRACE_2

    Virtual Probes 1 types:
    CAEN_DGTZ_DPP_VIRTUALPROBE_Input
    CAEN_DGTZ_DPP_VIRTUALPROBE_Delta
    CAEN_DGTZ_DPP_VIRTUALPROBE_Delta2
    CAEN_DGTZ_DPP_VIRTUALPROBE_Trapezoid
    
    Virtual Probes 2 types:
    CAEN_DGTZ_DPP_VIRTUALPROBE_Input
    CAEN_DGTZ_DPP_VIRTUALPROBE_Threshold
    CAEN_DGTZ_DPP_VIRTUALPROBE_TrapezoidReduced
    CAEN_DGTZ_DPP_VIRTUALPROBE_Baseline
    CAEN_DGTZ_DPP_VIRTUALPROBE_None

    Digital Probes types:
    CAEN_DGTZ_DPP_DIGITALPROBE_TRGWin
    CAEN_DGTZ_DPP_DIGITALPROBE_Armed
    CAEN_DGTZ_DPP_DIGITALPROBE_PkRun
    CAEN_DGTZ_DPP_DIGITALPROBE_PileUp
    CAEN_DGTZ_DPP_DIGITALPROBE_Peaking
    CAEN_DGTZ_DPP_DIGITALPROBE_CoincWin
    CAEN_DGTZ_DPP_DIGITALPROBE_BLFreeze
    CAEN_DGTZ_DPP_DIGITALPROBE_TRGHoldoff
	CAEN_DGTZ_DPP_DIGITALPROBE_TRGVal
	CAEN_DGTZ_DPP_DIGITALPROBE_ACQVeto
	CAEN_DGTZ_DPP_DIGITALPROBE_BFMVeto
	CAEN_DGTZ_DPP_DIGITALPROBE_ExtTRG
	CAEN_DGTZ_DPP_DIGITALPROBE_Busy
	CAEN_DGTZ_DPP_DIGITALPROBE_PrgVeto*/

    ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_1, CAEN_DGTZ_DPP_VIRTUALPROBE_Delta2);
    ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_2, CAEN_DGTZ_DPP_VIRTUALPROBE_Input);
    ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, DIGITAL_TRACE_1, CAEN_DGTZ_DPP_DIGITALPROBE_Peaking);

    if (ret) {
        printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");
        return ret;
    } else {
        return 0;
    }
}

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{
    /* The following variable is the type returned from most of CAENDigitizer
    library functions and is used to check if there was an error in function
    execution. For example:
    ret = CAEN_DGTZ_some_function(some_args);
    if(ret) printf("Some error"); */
    CAEN_DGTZ_ErrorCode ret;

    /* Buffers to store the data. The memory must be allocated using the appropriate
    CAENDigitizer API functions (see below), so they must not be initialized here
    NB: you must use the right type for different DPP analysis (in this case PHA) */
    char *buffer = NULL;                                 // readout buffer
    CAEN_DGTZ_DPP_PHA_Event_t       *Events[MaxNChannels];  // events buffer
    CAEN_DGTZ_DPP_PHA_Waveforms_t   *Waveform=NULL;     // waveforms buffer

    /* The following variables will store the digitizer configuration parameters */
    CAEN_DGTZ_DPP_PHA_Params_t DPPParams[MAXNB];
    DigitizerParams_t Params[MAXNB];

    /* Arrays for data analysis */
    uint64_t PrevTime[MAXNB][MaxNChannels];
    uint64_t ExtendedTT[MAXNB][MaxNChannels];
    uint32_t *EHisto[MAXNB][MaxNChannels]; // Energy Histograms 
    int ECnt[MAXNB][MaxNChannels];
    int TrgCnt[MAXNB][MaxNChannels];
    int PurCnt[MAXNB][MaxNChannels];

    /* The following variable will be used to get an handler for the digitizer. The
    handler will be used for most of CAENDigitizer functions to identify the board */
    int handle[MAXNB];

    /* Other variables */
    int i, b, ch, ev;
    int Quit=0;
    int AcqRun = 0;
    uint32_t AllocatedSize, BufferSize;
    int Nb=0;
    int DoSaveWave[MAXNB][MaxNChannels];
    int MajorNumber;
    int BitMask = 0;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    uint32_t NumEvents[MaxNChannels];
    CAEN_DGTZ_BoardInfo_t           BoardInfo;
	uint32_t temp;
    memset(DoSaveWave, 0, MAXNB*MaxNChannels*sizeof(int));
    for (i = 0; i < MAXNBITS; i++)
        BitMask |= 1<<i; /* Create a bit mask based on number of bits of the board */

    /* *************************************************************************************** */
    /* Set Parameters                                                                          */
    /* *************************************************************************************** */
    memset(&Params, 0, MAXNB * sizeof(DigitizerParams_t));
    memset(&DPPParams, 0, MAXNB * sizeof(CAEN_DGTZ_DPP_PHA_Params_t));
    for (b = 0; b < MAXNB; b++) {
        for (ch = 0; ch < MaxNChannels; ch++)
            EHisto[b][ch] = NULL; //set all histograms pointers to NULL (we will allocate them later)

        /****************************\
        * Communication Parameters   *
        \****************************/
        // Direct USB connection
        Params[b].LinkType = CAEN_DGTZ_USB;  // Link Type
        Params[b].VMEBaseAddress = 0;  // For direct USB connection, VMEBaseAddress must be 0

        // Direct optical connection
        //Params[b].LinkType = CAEN_DGTZ_PCI_OpticalLink;  // Link Type
        //Params[b].VMEBaseAddress = 0;  // For direct CONET connection, VMEBaseAddress must be 0

        // Optical connection to A2818 (or A3818) and access to the board with VME bus
        //Params[b].LinkType = CAEN_DGTZ_PCI_OpticalLink;  // Link Type (CAEN_DGTZ_PCIE_OpticalLink for A3818)
        //Params[b].VMEBaseAddress = 0x32100000;  // VME Base Address (only for VME bus access; must be 0 for direct connection (CONET or USB)
        
		
		/* The following is for VME boards connected using A4818 with PID number p, through an V2718 CONET-VME Bridge
        in this case you must set <LikType> = CAEN_DGTZ_USB_A4818_V2718, <LinkNum_OR_A4818_PID> = p, <ConetNode> = 0 
		and <VMEBaseAddress> = <0xXXXXXXXX> (address of the VME board) */
        //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB_A4818_V2718, p, 0, 0xXXXXXXXX, &handle[b]);		
		
		
        // USB connection to V1718 bridge and access to the board with VME bus
        //Params[b].LinkType = CAEN_DGTZ_USB;  // Link Type
        //Params[b].VMEBaseAddress = 0x32100000;  // VME Base Address (only for VME bus access; must be 0 for direct connection (CONET or USB)
        
        Params[b].IOlev = CAEN_DGTZ_IOLevel_NIM;
        /****************************\
        *  Acquisition parameters    *
        \****************************/
        Params[b].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_Mixed;          // CAEN_DGTZ_DPP_ACQ_MODE_List or CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope
        Params[b].RecordLength = 2000;                              // Num of samples of the waveforms (only for Oscilloscope mode)
        Params[b].ChannelMask = 0x01;                               // Channel enable mask
        Params[b].EventAggr = 0;                                   // number of events in one aggregate (0=automatic)
        Params[b].PulsePolarity = CAEN_DGTZ_PulsePolarityNegative; // Pulse Polarity (this parameter can be individual)

        /****************************\
        *      DPP parameters        *
        \****************************/
        for(ch=0; ch<MaxNChannels; ch++) {
			DPPParams[b].thr[ch] = 100;   // Trigger Threshold (in LSB)
			DPPParams[b].k[ch] = 3000;     // Trapezoid Rise Time (ns) 
			DPPParams[b].m[ch] = 900;      // Trapezoid Flat Top  (ns) 
			DPPParams[b].M[ch] = 50000;      // Decay Time Constant (ns) 
			DPPParams[b].ftd[ch] = 500;    // Flat top delay (peaking time) (ns) 
			DPPParams[b].a[ch] = 4;       // Trigger Filter smoothing factor (number of samples to average for RC-CR2 filter) Options: 1; 2; 4; 8; 16; 32
			DPPParams[b].b[ch] = 200;     // Input Signal Rise time (ns) 
			DPPParams[b].trgho[ch] = 1200;  // Trigger Hold Off
			DPPParams[b].nsbl[ch] = 4;     //number of samples for baseline average calculation. Options: 1->16 samples; 2->64 samples; 3->256 samples; 4->1024 samples; 5->4096 samples; 6->16384 samples
			DPPParams[b].nspk[ch] = 0;     //Peak mean (number of samples to average for trapezoid height calculation). Options: 0-> 1 sample; 1->4 samples; 2->16 samples; 3->64 samples
			DPPParams[b].pkho[ch] = 2000;  //peak holdoff (ns)
			DPPParams[b].blho[ch] = 500;   //Baseline holdoff (ns)
			DPPParams[b].enf[ch] = 1.0; // Energy Normalization Factor
			DPPParams[b].decimation[ch] = 0;  //decimation (the input signal samples are averaged within this number of samples): 0 ->disabled; 1->2 samples; 2->4 samples; 3->8 samples
			DPPParams[b].dgain[ch] = 0;    //decimation gain. Options: 0->DigitalGain=1; 1->DigitalGain=2 (only with decimation >= 2samples); 2->DigitalGain=4 (only with decimation >= 4samples); 3->DigitalGain=8( only with decimation = 8samples).
			DPPParams[b].otrej[ch] = 0;
			DPPParams[b].trgwin[ch] = 0;  //Enable Rise time Discrimination. Options: 0->disabled; 1->enabled
			DPPParams[b].twwdt[ch] = 100;  //Rise Time Validation Window (ns)
        }
    }


    /* *************************************************************************************** */
    /* Open the digitizer and read board information                                           */
    /* *************************************************************************************** */
    /* The following function is used to open the digitizer with the given connection parameters
    and get the handler to it */
    for(b=0; b<MAXNB; b++) {
        /* IMPORTANT: The following function identifies the different boards with a system which may change
        for different connection methods (USB, Conet, ecc). Refer to CAENDigitizer user manual for more info.
        Some examples below */
        
        /* The following is for b boards connected via b USB direct links
        in this case you must set Params[b].LinkType = CAEN_DGTZ_USB and Params[b].VMEBaseAddress = 0 */
        ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, b, 0, Params[b].VMEBaseAddress, &handle[b]);

        /* The following is for b boards connected via 1 opticalLink in dasy chain
        in this case you must set Params[b].LinkType = CAEN_DGTZ_PCI_OpticalLink and Params[b].VMEBaseAddress = 0 */
        //ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, b, Params[b].VMEBaseAddress, &handle[b]);

        /* The following is for b boards connected to A2818 (or A3818) via opticalLink (or USB with A1718)
        in this case the boards are accessed throught VME bus, and you must specify the VME address of each board:
        Params[b].LinkType = CAEN_DGTZ_PCI_OpticalLink (CAEN_DGTZ_PCIE_OpticalLink for A3818 or CAEN_DGTZ_USB for A1718)
        Params[0].VMEBaseAddress = <0xXXXXXXXX> (address of first board) 
        Params[1].VMEBaseAddress = <0xYYYYYYYY> (address of second board) 
        etc */
        //ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, 0, Params[b].VMEBaseAddress, &handle[b]);

        if (ret) {
            printf("Can't open digitizer\n");
            goto QuitProgram;    
        }
        
        /* Once we have the handler to the digitizer, we use it to call the other functions */
        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
        if (ret) {
            printf("Can't read board info\n");
            goto QuitProgram;
        }
        printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
        printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

        /* Check firmware revision (only DPP firmwares can be used with this Demo) */
        sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
        if (MajorNumber != V1730_DPP_PHA_CODE) {
            printf("This digitizer has not a DPP-PHA firmware\n");
            goto QuitProgram;
        }
    }

    /* *************************************************************************************** */
    /* Program the digitizer (see function ProgramDigitizer)                                   */
    /* *************************************************************************************** */
    for (b = 0; b < MAXNB; b++) {
        ret = ProgramDigitizer(handle[b], Params[b], DPPParams[b]);
        if (ret) {
            printf("Failed to program the digitizer\n");
            goto QuitProgram;
        }
    }

    /* WARNING: The mallocs MUST be done after the digitizer programming,
    because the following functions needs to know the digitizer configuration
    to allocate the right memory amount */
    /* Allocate memory for the readout buffer */
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0], &buffer, &AllocatedSize);
    /* Allocate memory for the events */
    ret |= CAEN_DGTZ_MallocDPPEvents(handle[0], Events, &AllocatedSize); 
    /* Allocate memory for the waveforms */
    ret |= CAEN_DGTZ_MallocDPPWaveforms(handle[0], &Waveform, &AllocatedSize); 
    if (ret) {
        printf("Can't allocate memory buffers\n");
        goto QuitProgram;    
    }

        
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    // Clear Histograms and counters
    for (b = 0; b < MAXNB; b++) {
        for (ch = 0; ch < MaxNChannels; ch++) {
            EHisto[b][ch] = (uint32_t *)malloc((1 << MAXNBITS) * sizeof(uint32_t));
            memset(EHisto[b][ch], 0, (1 << MAXNBITS) * sizeof(uint32_t));
            TrgCnt[b][ch] = 0;
            ECnt[b][ch] = 0;
            PrevTime[b][ch] = 0;
            ExtendedTT[b][ch] = 0;
            PurCnt[b][ch] = 0;
        }
    }
    PrevRateTime = get_time();
    AcqRun = 0;
    PrintInterface();
    printf("Type a command: ");
    while(!Quit) {
        // Check keyboard
        if(kbhit()) {
            char c;
            c = getch();
            if (c == 'q')  Quit = 1;
            if (c == 't')
                for (b = 0; b < MAXNB; b++)
                    CAEN_DGTZ_SendSWtrigger(handle[b]); // Send a software trigger to each board
            if (c == 'h')
                for (b = 0; b < MAXNB; b++)
                    for (ch = 0; ch < MaxNChannels; ch++)
                        if (ECnt[b][ch] != 0) 
                            SaveHistogram("Histo", b, ch, EHisto[b][ch]);  // Save Histograms to file for each board
            if (c == 'w')
                for (b = 0; b < MAXNB; b++)
                    for (ch = 0; ch < MaxNChannels; ch++)
                        DoSaveWave[b][ch] = 1; // save waveforms to file for each channel for each board (at next trigger)
            if (c == 'r')  {
                for (b = 0; b < MAXNB; b++) {
                    CAEN_DGTZ_SWStopAcquisition(handle[b]); 
                    printf("Restarted\n");
                    CAEN_DGTZ_ClearData(handle[b]);
                    CAEN_DGTZ_SWStartAcquisition(handle[b]);
                }
            }
            if (c == 's')  {
                for (b = 0; b < MAXNB; b++) {
                    // Start Acquisition
                    // NB: the acquisition for each board starts when the following line is executed
                    // so in general the acquisition does NOT starts syncronously for different boards
                    CAEN_DGTZ_SWStartAcquisition(handle[b]);
                    printf("Acquisition Started for Board %d\n", b);
                }
                AcqRun = 1;
            }
            if (c == 'S')  {
                for (b = 0; b < MAXNB; b++) {
                    // Stop Acquisition
                    CAEN_DGTZ_SWStopAcquisition(handle[b]); 
                    printf("Acquisition Stopped for Board %d\n", b);
                }
                AcqRun = 0;
            }
			if (c == 'T') {
				for (b = 0; b < MAXNB; b++) {
					printf("\n");
					for (ch = 0; ch < MaxNChannels; ch++) {
						// Read ADC temperature
						CAEN_DGTZ_ReadTemperature(handle[b], ch, &temp);
						printf("Ch %d  ADC temperature: %d %cC\n", ch, temp, 248);
					}
				}
				printf("Type a command: ");
			}
			if (c == 'C') {
				for (b = 0; b < MAXNB; b++)
					CAEN_DGTZ_Calibrate(handle[b]);
				printf("\nADC calibration ready\n");
				printf("Type a command: ");

			}
        }
        if (!AcqRun) {
            Sleep(10);
            continue;
        }
    
        /* Calculate throughput and trigger rate (every second) */
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime; /* milliseconds */
        if (ElapsedTime > 1000) {
            system(CLEARSCR);
            PrintInterface();
            printf("Readout Rate=%.2f MB\n", (float)Nb/((float)ElapsedTime*1048.576f));
            for(b=0; b<MAXNB; b++) {
                printf("\nBoard %d:\n",b);
                for(i=0; i<MaxNChannels; i++) {
                    if (TrgCnt[b][i]>0)
                        printf("\tCh %d:\tTrgRate=%.2f KHz\tPileUpRate=%.2f%%\n", i, (float)TrgCnt[b][i]/(float)ElapsedTime, (float)PurCnt[b][i]*100/(float)TrgCnt[b][i]);
                    else
                        printf("\tCh %d:\tNo Data\n", i);
                    TrgCnt[b][i]=0;
                    PurCnt[b][i]=0;
                }
            }
            Nb = 0;
            PrevRateTime = CurrentTime;
            printf("\n\n");
        }
        
        /* Read data from the boards */
        for (b = 0; b < MAXNB; b++) {
            /* Read data from the board */
            ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
            if (ret) {
                printf("Readout Error\n");
                goto QuitProgram;    
            }
            if (BufferSize == 0)
                continue;

            Nb += BufferSize;
            //ret = DataConsistencyCheck((uint32_t *)buffer, BufferSize/4);
            ret |= CAEN_DGTZ_GetDPPEvents(handle[b], buffer, BufferSize, Events, NumEvents);
            if (ret) {
                printf("Data Error: %d\n", ret);
                goto QuitProgram;
            }

            /* Analyze data */
            //for(b=0; b<MAXNB; b++) printf("%d now: %d\n", b, Params[b].ChannelMask);
            for (ch = 0; ch < MaxNChannels; ch++) {
                if (!(Params[b].ChannelMask & (1<<ch)))
                    continue;
                
                /* Update Histograms */
                for (ev = 0; ev < NumEvents[ch]; ev++) {
                    TrgCnt[b][ch]++;
                    /* Time Tag */
                    if (Events[ch][ev].TimeTag < PrevTime[b][ch]) 
                        ExtendedTT[b][ch]++;
                    PrevTime[b][ch] = Events[ch][ev].TimeTag;
                    /* Energy */
                    if (Events[ch][ev].Energy > 0) {
                        // Fill the histograms
                        EHisto[b][ch][(Events[ch][ev].Energy)&BitMask]++;
                        ECnt[b][ch]++;
                    } else {  /* PileUp */
                        PurCnt[b][ch]++;
                    }
                    /* Get Waveforms (only from 1st event in the buffer) */
                    if ((Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) && DoSaveWave[b][ch] && (ev == 0)) {
                        int size;
                        int16_t *WaveLine;
                        uint8_t *DigitalWaveLine;
                        CAEN_DGTZ_DecodeDPPWaveforms(handle[b], &Events[ch][ev], Waveform);

                        // Use waveform data here...
                        size = (int)(Waveform->Ns); // Number of samples
                        WaveLine = Waveform->Trace1; // First trace (ANALOG_TRACE_1)
                        SaveWaveform(b, ch, 1, size, WaveLine);

                        WaveLine = Waveform->Trace2; // Second Trace ANALOG_TRACE_2 (if single trace mode, it is a sequence of zeroes)
                        SaveWaveform(b, ch, 2, size, WaveLine);

                        DigitalWaveLine = Waveform->DTrace1; // First Digital Trace (DIGITALPROBE1)
                        SaveDigitalProbe(b, ch, 1, size, DigitalWaveLine);

                        DigitalWaveLine = Waveform->DTrace2; // Second Digital Trace (for DPP-PHA it is ALWAYS Trigger)
                        SaveDigitalProbe(b, ch, 2, size, DigitalWaveLine);
                        DoSaveWave[b][ch] = 0;
                        printf("Waveforms saved to 'Waveform_<board>_<channel>_<trace>.txt'\n");
                    } // loop to save waves        
                } // loop on events
            } // loop on channels
        } // loop on boards
    } // End of readout loop


QuitProgram:
    /* stop the acquisition, close the device and free the buffers */
    for (b =0 ; b < MAXNB; b++) {
        CAEN_DGTZ_SWStopAcquisition(handle[b]);
        CAEN_DGTZ_CloseDigitizer(handle[b]);
        for (ch = 0; ch < MaxNChannels; ch++)
            if (EHisto[b][ch] != NULL)
                free(EHisto[b][ch]);
    }
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_FreeDPPEvents(handle[0], Events);
    CAEN_DGTZ_FreeDPPWaveforms(handle[0], Waveform);
	printf("\nPress a key to quit\n");
	getch();
    return ret;
}
    
