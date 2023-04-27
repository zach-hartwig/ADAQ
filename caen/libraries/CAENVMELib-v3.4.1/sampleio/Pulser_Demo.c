/******************************************************************************
*
*	CAEN SpA - ATM Division
*	Via Vetraia, 11 - 55049 - Viareggio ITALY
*	+39 0594 388 398 - www.caen.it
*
***************************************************************************//*!
*
*	\file		Pulser_Demo.c
*	\brief
*	\author		Matteo Bianchini
*	\version
*	\date		2022
*	\copyright	GNU Lesser General Public License
*
******************************************************************************/

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <CAENVMElib.h>

#include "Scaler_Demo.h"
#include "Pulser_Demo.h"
#include "Sys_Func.h"


//Function for Pulser configuration
int PulserConfig(int32_t Handle, CVPulserSelect* p_Pulser, float* p_pulsertime, int* p_NPulses, CVIOSources* p_IOsource) {

	//Pulser parametes
	int TimeUnits, re;
	unsigned char Period;
	unsigned char Width;

	clr_scr();
	//Pulser selection
	
	printf("PULSER PARAMETER SELECTION SECTION\n");
	printf("\nSelect the Pulser (0=A/1=B)\n");
	scanf("%d", p_Pulser);
	if ((*p_Pulser != cvPulserA) & (*p_Pulser != cvPulserB)) {
		printf("value not allowed\n");
		return 1;
	}
	
	printf("\nSelect the type of signal to start/reset the pulse burst (insert an integer, see definition of CVIOSources in CAENVMEtypes.h)\ncvManualSW = 0\ncvInputSrc0 = 1\ncvInputSrc1 = 2\ncvCoincidence = 3\ncvVMESignals = 4\n");
	scanf("%d", p_IOsource);
	if ((*p_IOsource < 0) || (*p_IOsource > 4)) {
		printf("value not allowed\n");
		return 1;
	}



	//Pulser parameters selection
	printf("\nSelect the Pulser time unit (insert an integer, see definition of CVIOSources in CAENVMEtypes.h)\ncvUnit25ns = 0\ncvUnit1600ns = 1 (valid only for V1718 and V2718)\ncvUnit410us = 2 (valid only for V1718 and V2718)\ncvUnit104ms = 3 (valid only for V1718 and V2718)\ncvUnit25us = 4\n");
	scanf("%d", &TimeUnits);
	if ((TimeUnits < 0) || (TimeUnits > 4)) {
		printf("value not allowed\n");
		return 1;
	}
	printf("\nSelect the Pulser period (in time units)\n");
	scanf("%hhd", &Period);
	printf("\nSelect the Pulser width (in time units)\n");
	scanf("%hhd", &Width);
	printf("\nSelect the number of pulses (0=infinite)\n");
	scanf("%d", p_NPulses);

	if (*p_NPulses > UCHAR_MAX || *p_NPulses < 0) {
		printf("value not allowed\n");
		return 1;
	}

	if (*p_NPulses == 0) {
		printf("\nSelect the number of time duration of the pulses in ms(0=infinite)\n");
		scanf("%f", p_pulsertime);
	}

	re = CAENVME_SetPulserConf(Handle, *p_Pulser, Period, Width, TimeUnits, (unsigned char)*p_NPulses, *p_IOsource, *p_IOsource);
	if ( re!= cvSuccess) {
		printf("\n\nError setting the pulser\n");
		return 1;
	}

	clr_scr();
	printf("\n\nPulser set\n");
	Sleep_(1000);

	return 0;
}



//Function for the Output configuration
int OutputConfig(int32_t Handle, CVPulserSelect Pulser, int command) {

	//Output setting parameters
	CVOutputSelect Out;
	CVIOSources Output_Source;
	CVIOPolarity PulsePolarity;
	//Some variables
	int re=0;

	clr_scr();


	switch(command){
	//V1718-V2718 case
	case 1:
	case 2:
		Output_Source = cvMiscSignals;
		break;
	//V3718-V4718 case
	case 3:
	case 4:
		switch (Pulser) {
		case cvPulserA:
			Output_Source = cvPulserV3718A;
			break;
		case cvPulserB:
			Output_Source = cvPulserV3718B;
			break;
		default:
			printf("\n \nValue of the pulser not allowed\n");
			return 1;
		}
		break;

	default:
		return 1;
	}

	printf("\nSelect the pulses polarity\n0 = Direct\n1 = Inverted\n");
	scanf("%d", &PulsePolarity);

	if(command > 2){
		printf("\nSelect the Pulser Out\nOut0 = 0 \nOut1 = 1 \nOut2 = 2 \nOut3 = 3\n");
			}
	else if (command < 3) {
		printf("\nSelect the Output Source\nSee the CAENVMELib User Manual for more details\n");
	}
	scanf("%d", &Out);
	//Set the output
	re = CAENVME_SetOutputConf(Handle, Out, PulsePolarity, cvActiveHigh, Output_Source);
	if (re != cvSuccess) {
		printf("\n\nError setting the output %d\n", re);
		return 1;
	}
	clr_scr();
	//For V1718 and V2718
	if (command < 3) printf("\nPulser A on Output 0 and 1\n\nPulser B on Output 2 and 3\n");
	//For V3718 and V4718
	else printf("\nPulser %d on Out%d\n", Pulser, Out);

	Sleep_(1000);

	return 0;
}
