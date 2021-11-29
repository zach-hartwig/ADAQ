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

//#define MANUAL_BUFFER_SETTING   0
// The following define must be set to the actual number of connected boards
#define MAXNB   1
// NB: the following define MUST specify the ACTUAL max allowed number of board's channels
// it is needed for consistency inside the CAENDigitizer's functions used to allocate the memory
#define MaxNChannels 2

// The following define MUST specify the number of bits used for the energy calculation
#define MAXNBITS 16

/* include some useful functions from file Functions.h
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
int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams)
{
    /* This function uses the CAENDigitizer API functions to perform the digitizer's initial configuration */
    int i, ret = 0;

    /* Reset the digitizer */
    ret |= CAEN_DGTZ_Reset(handle);

    if (ret) {
        printf("ERROR: can't reset the digitizer.\n");
        return -1;
    }

    /* Set the DPP acquisition mode
    This setting affects the modes Mixed and List (see CAEN_DGTZ_DPP_AcqMode_t definition for details)
    CAEN_DGTZ_DPP_SAVE_PARAM_EnergyOnly        Only energy charge (DPP-PSD/DPP-CI v2) is returned
    CAEN_DGTZ_DPP_SAVE_PARAM_TimeOnly        Only time is returned
    CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime    Both charge and time are returned
    CAEN_DGTZ_DPP_SAVE_PARAM_None            No histogram data is returned */
    ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
    
    // Set the digitizer acquisition mode (CAEN_DGTZ_SW_CONTROLLED or CAEN_DGTZ_S_IN_CONTROLLED)
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);

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
            // Set the number of samples for each waveform (you can set different RL for different channels)
            ret |= CAEN_DGTZ_SetRecordLength(handle, Params.RecordLength, i);

            // Set a DC offset to the input signal to adapt it to digitizer's dynamic range
            ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, 0x8000);
            
            // Set the Pre-Trigger size (in samples)
            ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, 80);
            
            // Set the polarity for the given channel (CAEN_DGTZ_PulsePolarityPositive or CAEN_DGTZ_PulsePolarityNegative)
            ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity);
        }
    }

    /* Set the virtual probes

    DPP-PSD for x790 boards can save:
    2 analog waveforms:
        Analog Trace 1: it is always the input signal;
        Analog Trace 2: it can be specified with the VIRTUALPROBE parameter
    4 digital waveforms:
        Digital Trace 1:   it is always the trigger
        Digital Trace 2:   it is always the long gate
        Digital Trace 3/4: they can be specified with the DIGITALPROBE 1 and 2 parameters

    CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE    -> Save only the Input Signal waveform
    CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL      -> Save also the waveform specified in VIRTUALPROBE

    Virtual Probes types for Trace 2:
        CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline         -> Save the Baseline waveform (mean on nsbl parameter)
      ### Virtual Probes only for FW <= 13X.5 ###
        CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Threshold        -> Save the (Baseline - Threshold) waveform. 
    
    Digital Probes types for Digital Trace 3(x790):
      ### Virtual Probes only for FW >= 13X.6 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_ExtTrg       
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_OverThr
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_TrigOut
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_CoincWin
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_PileUp
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_Coincidence

      ### Virtual Probes only for FW <= 13X.5 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_Armed           
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_Trigger         
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_ChargeReady     
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_PileUp          
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_BlOutSafeBand   
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_BlTimeout       
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_CoincidenceMet  
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_Tvaw            


    Digital Probes types for Digital Trace 4(x790):
      ### Virtual Probes only for FW >= 13X.6 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_GateShort
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_TrgVal
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_TrgHO
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_PileUp
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_Coincidence
      ### Virtual Probes only for FW <= 13X.5 ###
        CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_Armed           
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_Trigger        
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_ChargeReady    
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_PileUp          
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_BlOutSafeBand   
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_BlTimeout       
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_CoincidenceMet  
	    CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_Tvaw             */ 
    ret |= CAEN_DGTZ_SetDPP_PSD_VirtualProbe(handle, CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE, CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline, CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_GateLong, CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr);

    if (ret) {
        printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");
        return ret;
    } else {
        return 0;
    }
}

int32_t ProgramHighVoltage(int handle, HighVoltageParams_t HVParams[], uint32_t HVChMask) {
    // IMPORTANT NOTE: on DT5790 boards, HV channels 0/1 are reached through board channels 2/3.
    // we then use an offset of 2 when we act on channels 0/1.
    uint32_t offset = 2;
    uint32_t ch = 0;
    int32_t ret;

    while ((HVChMask >> ch) > 0) {
        if ((HVChMask >> ch) & 0x1) {
            // Convert parameters from HV Units (Volts, uAmpere, etc.)
            // to LSB (value to be written to register).
            uint32_t VSet = HighVoltageUnitsToLSB(&HVParams[ch].VSet);
            uint32_t ISet = HighVoltageUnitsToLSB(&HVParams[ch].ISet);
            uint32_t VMax = HighVoltageUnitsToLSB(&HVParams[ch].VMax);
            uint32_t RampDown = HighVoltageUnitsToLSB(&HVParams[ch].RampDown);
            uint32_t RampUp = HighVoltageUnitsToLSB(&HVParams[ch].RampUp);
            uint32_t chb = ch + offset; // See above IMPORTANT NOTE
            uint32_t mode = (HVParams[ch].PWDownMode == HighVoltage_PWDown_Kill) ? HV_PWDOWN_BITVALUE_KILL : HV_PWDOWN_BITVALUE_RAMP;
            uint32_t PwDownMode;
            
            // Set VSet (0x1n20)
            if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_VSET_ADDR | (chb << 8), VSet)) != CAEN_DGTZ_Success)
                printf("ERROR %d setting VSet. (handle=%d, ch=%u)\n", ret, handle, ch);
            // Set ISet (0x1n24)
            if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_ISET_ADDR | (chb << 8), ISet)) != CAEN_DGTZ_Success)
                printf("ERROR %d setting ISet. (handle=%d, ch=%u)\n", ret, handle, ch);
            // Set VMax (0x1n30)
            if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_VMAX_ADDR | (chb << 8), VMax)) != CAEN_DGTZ_Success)
                printf("ERROR %d setting VMax. (handle=%d, ch=%u)\n", ret, handle, ch);
            // Set RampDown (0x1n2C)
            if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_RAMPDOWN_ADDR | (chb << 8), RampDown)) != CAEN_DGTZ_Success)
                printf("ERROR %d setting RampDown. (handle=%d, ch=%u)\n", ret, handle, ch);
            // Set RampUp (0x1n28)
            if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_RAMPUP_ADDR | (chb << 8), RampUp)) != CAEN_DGTZ_Success)
                printf("ERROR %d setting RampUp. (handle=%d, ch=%u)\n", ret, handle, ch);
            // Set Power Down Mode
            if ((ret = CAEN_DGTZ_ReadRegister(handle, HV_POWER_ADDR | (chb << 8), &PwDownMode)) != CAEN_DGTZ_Success)
                printf("ERROR %d reading address 0x%X. (handle=%d, ch=%u)\n", ret, HV_POWER_ADDR, handle, ch);
            else {
                PwDownMode &= ~(0x1 << HV_REGBIT_PWDOWN); // erase bit HV_REGBIT_PWDOWN
                PwDownMode |= (mode << HV_REGBIT_PWDOWN);
                if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_POWER_ADDR | (chb << 8), PwDownMode)) != CAEN_DGTZ_Success)
                    printf("ERROR %d setting PowerDownMode. (handle=%d, ch=%u)\n", ret, handle, ch);
            }
        }
        ch++;
    }

    return 0;
}

int32_t ToggleHVON(int32_t handle, uint32_t HVChMask) {
    // To toggle HV powerON we need to toggle bit 0 of 'HV_POWER_ADDR',
    // so we read it, toggle the bit and write it again.
    // IMPORTANT NOTE: on DT5790 boards, HV channels 0/1 are reached through board channels 2/3.
    // we then use an offset of 2 when we act on channels 0/1.
    uint32_t offset = 2;
    uint32_t ch = 0;
    int32_t ret;

    while ((HVChMask >> ch) > 0) {
        if ((HVChMask >> ch) & 0x1) {
            uint32_t u32;
            uint32_t chb = ch + offset; // See above IMPORTANT NOTE
			ch++;
            if ((ret = CAEN_DGTZ_ReadRegister(handle, HV_POWER_ADDR | (chb << 8), &u32)) != CAEN_DGTZ_Success)
                printf("ERROR %d reading address 0x%X. (handle=%d, ch=%u)\n", ret, HV_POWER_ADDR, handle, ch);
            else {
                // bit 0 represents power status, so we toggle it.
                uint32_t isOn = (u32 & 0x1);
				if (isOn == 1) { // set bit 0 to 0 (will turn channel off).
					u32 &= ~(0x1);
					printf("Switching HV channel %d OFF. ", ch - 1);
				}
				else {// set bit 0 to 1 (will turn channel on).
					u32 |= 0x1;
					printf("Switching HV channel %d ON. ", ch - 1);
				}
                // write the value again to 'HV_POWER_ADDR'.
                if ((ret = CAEN_DGTZ_WriteRegister(handle, HV_POWER_ADDR | (chb << 8), u32)) != CAEN_DGTZ_Success)
                    printf("ERROR %d toggling power on status. (handle=%d, ch=%u)\n", ret, handle, ch);
            }
        }
    }

    return 0;
}

int32_t PrintHVMonitor(int32_t handle, uint32_t HVChMask) {
    // IMPORTANT NOTE: on DT5790 boards, HV channels 0/1 are reached through board channels 2/3.
    // we then use an offset of 2 when we act on channels 0/1.
    uint32_t offset = 2;
    uint32_t ch = 0;
    int32_t ret;

    while ((HVChMask >> ch) > 0) {
        if ((HVChMask >> ch) & 0x1) {
            uint32_t u32;
            uint32_t chb = ch + offset; // See above IMPORTANT NOTE
            double VMonRes = 0.1; // XX790 boards have a VMon resolution of 0.1 Volts / LSB.
            double IMonRes = 0.05; // XX790 boards have a VMon resolution of 0.1 Volts / LSB.
			ch++;
            // Read and print VMon.
            if ((ret = CAEN_DGTZ_ReadRegister(handle, HV_VMON_ADDR | (chb << 8), &u32)) != CAEN_DGTZ_Success)
                printf("ERROR %d reading address 0x%X. (handle=%d, ch=%u)\n", ret, HV_VMON_ADDR, handle, ch);
			printf("\n CH %d: VMon = %.1f V;", ch - 1, u32 * VMonRes);

            // Read and print IMon
            if ((ret = CAEN_DGTZ_ReadRegister(handle, HV_IMON_ADDR | (chb << 8), &u32)) != CAEN_DGTZ_Success)
                printf("\nERROR %d reading address 0x%X. (handle=%d, ch=%u)\n", ret, HV_IMON_ADDR, handle, ch);
            printf(" IMon = %.1f uA; ", u32 * IMonRes);
        }
    }

    return 0;
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
    NB: you must use the right type for different DPP analysis (in this case PSD) */
    char *buffer = NULL;                                    // readout buffer
    CAEN_DGTZ_DPP_PSD_Event_t       *Events[MaxNChannels];  // events buffer
    CAEN_DGTZ_DPP_PSD_Waveforms_t   *Waveform=NULL;         // waveforms buffer

    /* The following variables will store the digitizer configuration parameters */
    CAEN_DGTZ_DPP_PSD_Params_t DPPParams[MAXNB];
    DigitizerParams_t Params[MAXNB];
    HighVoltageParams_t HVParams[HV_MAXCHANNELS];

    /* Arrays for data analysis */
    uint64_t PrevTime[MAXNB][MaxNChannels];
    uint64_t ExtendedTT[MAXNB][MaxNChannels];
    uint32_t *EHistoShort[MAXNB][MaxNChannels];   // Energy Histograms for short gate charge integration
    uint32_t *EHistoLong[MAXNB][MaxNChannels];    // Energy Histograms for long gate charge integration
    float *EHistoRatio[MAXNB][MaxNChannels];      // Energy Histograms for ratio Long/Short
    int ECnt[MAXNB][MaxNChannels];                // Number-of-Entries Counter for Energy Histograms short and long gate
    int TrgCnt[MAXNB][MaxNChannels];

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

    memset(DoSaveWave, 0, MAXNB*MaxNChannels*sizeof(int));
    for (i=0; i<MAXNBITS; i++)
        BitMask |= 1<<i; /* Create a bit mask based on number of bits of the board */

    /* *************************************************************************************** */
    /* Set Parameters                                                                          */
    /* *************************************************************************************** */
    memset(&Params, 0, MAXNB*sizeof(DigitizerParams_t));
    memset(&DPPParams, 0, MAXNB*sizeof(CAEN_DGTZ_DPP_PSD_Params_t));
    for(b=0; b<MAXNB; b++) {
        for(ch=0; ch<MaxNChannels; ch++) {
            EHistoShort[b][ch] = NULL; // Set all histograms pointers to NULL (we will allocate them later)
            EHistoLong[b][ch] = NULL;
            EHistoRatio[b][ch] = NULL;
        }

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
        //Params[b].LinkType = CAEN_DGTZ_USB;  // Link Type (CAEN_DGTZ_PCIE_OpticalLink for A3818)
        //Params[b].VMEBaseAddress = 0x32110000;  // VME Base Address (only for VME bus access; must be 0 for direct connection (CONET or USB)
        
        Params[b].IOlev = CAEN_DGTZ_IOLevel_TTL;
        /****************************\
        *  Acquisition parameters    *
        \****************************/
        Params[b].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_Mixed;          // CAEN_DGTZ_DPP_ACQ_MODE_List or CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope
        Params[b].RecordLength = 12;                              // Num of samples of the waveforms (only for Oscilloscope mode Ns =RecordLength * 8)
        Params[b].ChannelMask = 0xF;                               // Channel enable mask
        Params[b].EventAggr = 17554;                                  // number of events in one aggregate (0=automatic)
        Params[b].PulsePolarity = CAEN_DGTZ_PulsePolarityNegative; // Pulse Polarity (this parameter can be individual)

        /****************************\
        *      DPP parameters        *
        \****************************/
        for(ch=0; ch<MaxNChannels; ch++) {
            DPPParams[b].thr[ch] = 50;        // Trigger Threshold
            /* The following parameter is used to specifiy the number of samples for the baseline averaging:
            0 -> absolute Bl
            1 -> 8samp
            2 -> 32samp
            3 -> 128samp */
            DPPParams[b].nsbl[ch] = 1;
            DPPParams[b].lgate[ch] = 32;    // Long Gate Width (N*4ns)
            DPPParams[b].sgate[ch] = 24;    // Short Gate Width (N*4ns)
            DPPParams[b].pgate[ch] = 8;     // Pre Gate Width (N*4ns)
            /* Self Trigger Mode:
            0 -> Disabled
            1 -> Enabled */
            DPPParams[b].selft[ch] = 1;
            // Trigger configuration:
            // CAEN_DGTZ_DPP_TriggerConfig_Peak       -> trigger on peak. NOTE: Only for FW <= 13X.5
            // CAEN_DGTZ_DPP_TriggerConfig_Threshold  -> trigger on threshold */
            DPPParams[b].trgc[ch] = CAEN_DGTZ_DPP_TriggerConfig_Threshold;
            /* Trigger Validation Acquisition Window */
            DPPParams[b].tvaw[ch] = 50;
            /* Charge sensibility: 0->40fc/LSB; 1->160fc/LSB; 2->640fc/LSB; 3->2,56pc/LSB */
            DPPParams[b].csens[ch] = 0;
        }
		/*The following parameters could be set individually for each channel but in this demo they are set in broadcast mode*/
        /* Pile-Up rejection Mode
        CAEN_DGTZ_DPP_PSD_PUR_DetectOnly -> Only Detect Pile-Up
        CAEN_DGTZ_DPP_PSD_PUR_Enabled -> Reject Pile-Up */
        DPPParams[b].purh = CAEN_DGTZ_DPP_PSD_PUR_DetectOnly;
        DPPParams[b].purgap = 100;  // Purity Gap
        DPPParams[b].blthr = 3;     // Baseline Threshold
        DPPParams[b].bltmo = 100;   // Baseline Timeout (no more existent)
        DPPParams[b].trgho = 8;     // Trigger HoldOff
    }
    // Load HV Parameters and informations with default values for XX790.
    for (ch = 0; ch < HV_MAXCHANNELS; ch++)
        FillHVParameterDT5790(&HVParams[ch]);

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

        // Check firmware revision (only DPP firmware can be used with this Demo) */
        sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
        if (MajorNumber != 131 && MajorNumber != 132 && MajorNumber != 136) {
            printf("This digitizer has not a DPP-PSD firmware\n");
            goto QuitProgram;
        }

        // Set HighVoltage configuration (only if the board is a XX790 board).
        if (BoardInfo.FamilyCode == CAEN_DGTZ_XX790_FAMILY_CODE) {
            int32_t res = ProgramHighVoltage(handle[b], HVParams, (0x1 << HV_MAXCHANNELS) - 1);
            if (res) {
                printf("Error setting HV values\n");
                goto QuitProgram;
            }
        }
    }

    /* *************************************************************************************** */
    /* Program the digitizer (see function ProgramDigitizer)                                   */
    /* *************************************************************************************** */
    for(b=0; b<MAXNB; b++) {
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
    for(b=0; b<MAXNB; b++) {
        for(ch=0; ch<MaxNChannels; ch++) {
            // Allocate Memory for Histos and set them to 0
            EHistoShort[b][ch] = (uint32_t *)malloc( (1<<MAXNBITS)*sizeof(uint32_t) );
            memset(EHistoShort[b][ch], 0, (1<<MAXNBITS)*sizeof(uint32_t));
            EHistoLong[b][ch] = (uint32_t *)malloc( (1<<MAXNBITS)*sizeof(uint32_t) );
            memset(EHistoLong[b][ch], 0, (1<<MAXNBITS)*sizeof(uint32_t));
            EHistoRatio[b][ch] = (float *)malloc( (1<<MAXNBITS)*sizeof(float) );
            memset(EHistoRatio[b][ch], 0, (1<<MAXNBITS)*sizeof(float));
            TrgCnt[b][ch] = 0;
            ECnt[b][ch] = 0;
            PrevTime[b][ch] = 0;
            ExtendedTT[b][ch] = 0;
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
            if (c == 'q')
                Quit = 1;
            if (c == 't')
                for(b=0; b<MAXNB; b++)
                    CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a software trigger to each board */
            if (c == 'h')
                for (b = 0; b < MAXNB; b++)
                    for (ch = 0; ch < MaxNChannels; ch++)
                        if (ECnt[b][ch] != 0) {
                            /* Save Histograms to file for each board and channel */
                            SaveHistogram("HistoShort", b, ch, EHistoShort[b][ch]);  
                            SaveHistogram("HistoLong", b, ch, EHistoLong[b][ch]);
                        }
            if (c == 'w')
                for(b=0; b<MAXNB; b++)
                    for(ch=0; ch<MaxNChannels; ch++)
                        DoSaveWave[b][ch] = 1; /* save waveforms to file for each channel for each board (at next trigger) */
            if (c == 'r')  {
                for(b=0; b<MAXNB; b++) {
                    CAEN_DGTZ_SWStopAcquisition(handle[b]); 
                    printf("Restarted\n");
                    CAEN_DGTZ_ClearData(handle[b]);
                    CAEN_DGTZ_SWStartAcquisition(handle[b]);
                }
            }
            if (c == 's')  {
                for(b=0; b<MAXNB; b++) {
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
            if (c == 'o') {
                for (b = 0; b < MAXNB; b++) {
          
                    ToggleHVON(handle[b], (0x1 << HV_MAXCHANNELS) - 1);
                }
            }
            if (c == 'm') {
                for (b = 0; b < MAXNB; b++) {
                    
                    PrintHVMonitor(handle[b], (0x1 << HV_MAXCHANNELS) - 1);
                }
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
                        printf("\tCh %d:\tTrgRate=%.2f KHz\t%\n", b*8+i, (float)TrgCnt[b][i]/(float)ElapsedTime);
                    else
                        printf("\tCh %d:\tNo Data\n", i);
                    TrgCnt[b][i]=0;
                }
            }
            Nb = 0;
            PrevRateTime = CurrentTime;
            printf("\n\n");
        }
        
        /* Read data from the boards */
        for(b=0; b<MAXNB; b++) {
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
            for(ch=0; ch<MaxNChannels; ch++) {
                if (!(Params[b].ChannelMask & (1<<ch)))
                    continue;
                
                /* Update Histograms */
                for(ev=0; ev<NumEvents[ch]; ev++) {
                    TrgCnt[b][ch]++;
                    /* Time Tag */
                    if (Events[ch][ev].TimeTag < PrevTime[b][ch]) 
                        ExtendedTT[b][ch]++;
                    PrevTime[b][ch] = Events[ch][ev].TimeTag;
                    /* Energy */
                    if ( (Events[ch][ev].ChargeLong > 0) && (Events[ch][ev].ChargeShort > 0)) {
                            // Fill the histograms
                            EHistoShort[b][ch][(Events[ch][ev].ChargeShort) & BitMask]++;
                            EHistoLong[b][ch][(Events[ch][ev].ChargeLong) & BitMask]++;
                            ECnt[b][ch]++;
                    }

                    /* Get Waveforms (only from 1st event in the buffer) */
                    if ((Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) && DoSaveWave[b][ch] && (ev == 0)) {
                        int size;
                        int16_t *WaveLine;
                        uint8_t *DigitalWaveLine;
                        CAEN_DGTZ_DecodeDPPWaveforms(handle[b], &Events[ch][ev], Waveform);

                        // Use waveform data here...
                        size = (int)(Waveform->Ns); // Number of samples
                        WaveLine = Waveform->Trace1; // First trace (for DPP-PSD it is ALWAYS the Input Signal)
                        SaveWaveform(b, ch, 1, size, WaveLine);

                        WaveLine = Waveform->Trace2; // Second Trace (if single trace mode, it is a sequence of zeroes)
                        SaveWaveform(b, ch, 2, size, WaveLine);
                        DoSaveWave[b][ch] = 0;

                        DigitalWaveLine = Waveform->DTrace1; // First Digital Trace (Gate Short)
                        SaveDigitalProbe(b, ch, 1, size, DigitalWaveLine);
                        DoSaveWave[b][ch] = 0;

                        DigitalWaveLine = Waveform->DTrace2; // Second Digital Trace (Gate Long)
                        SaveDigitalProbe(b, ch, 2, size, DigitalWaveLine);
                        DoSaveWave[b][ch] = 0;

                        DigitalWaveLine = Waveform->DTrace3; // Third Digital Trace (DIGITALPROBE1 set with CAEN_DGTZ_SetDPP_PSD_VirtualProbe)
                        SaveDigitalProbe(b, ch, 3, size, DigitalWaveLine);
                        DoSaveWave[b][ch] = 0;

                        DigitalWaveLine = Waveform->DTrace4; // Fourth Digital Trace (DIGITALPROBE2 set with CAEN_DGTZ_SetDPP_PSD_VirtualProbe)
                        SaveDigitalProbe(b, ch, 4, size, DigitalWaveLine);
                        DoSaveWave[b][ch] = 0;
                        printf("Waveforms saved to 'Waveform_<board>_<channel>_<trace>.txt'\n");
                    } // loop to save waves        
                } // loop on events
            } // loop on channels
        } // loop on boards
    } // End of readout loop


QuitProgram:
    /* stop the acquisition, close the device and free the buffers */
    for(b=0; b<MAXNB; b++) {
        CAEN_DGTZ_SWStopAcquisition(handle[b]);
        CAEN_DGTZ_CloseDigitizer(handle[b]);
        for (ch=0; ch<MaxNChannels; ch++) {
            free(EHistoShort[b][ch]);
            free(EHistoLong[b][ch]);
            free(EHistoRatio[b][ch]);
        }
    }
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_FreeDPPEvents(handle[0], Events);
    CAEN_DGTZ_FreeDPPWaveforms(handle[0], Waveform);
    return ret;
}
    
