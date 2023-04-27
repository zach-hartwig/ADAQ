/*
|--------------------------------------------------------|
|					  SCALER DEMO						 |
|				    CAEN ATM DIVISION					 |
|				    MATTEO BIANCHINI					 |
|--------------------------------------------------------|
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "CAENVMElib.h"
#include "Scaler_Demo.h"
#include "Sys_Func.h"


//V3718 - V4718 Scaler Setup
int V3718_ScalerSetup(uint32_t handle) {

	short ScalerMode;
	int re=0;
	uint32_t input_source = 0x0, start_source = 0x0, clear_source = 0x0, gate_source = 0x0, scaler_data = 0;
	uint16_t dwell_time, maxhit_count;
	CVContinuosRun continuous_run;
	bool is_gate_open=0, scaler_started=0;
	

	clr_scr();
	printf("SCALER PARAMETER SELECTION SECTION\n");
	//Scaler Mode Selection
	printf("Select the Scaler mode\n\nGate Mode = 0\nD-Well Time Mode = 1\nMaxHits Mode = 2\n");
	scanf("%hd", &ScalerMode);
	clr_scr();
	//Input Source Selection
	printf("\n\nSelect the Input Source (see definition in CAENVMETypes.h)\n\ncvSourceIN0 = 0x2\ncvSourceIN1 = 0x3\ncvSourceDTACK = 0x6\ncvSourceBERR = 0x7\ncvSourceDS = 0x4\ncvSourceAS = 0x5\ncvSourceCoinc = 0xA\ncvSourceINOR = 0xB\n");
	scanf("%"SCNx32, &input_source);
	
	//Start Source Selection
	printf("\n\nSelect the Start Source (see definition in CAENVMETypes.h)\n\ncvSourceIN0 = 0x2\ncvSourceIN1 = 0x3\ncvSourceDTACK = 0x6\ncvSourceBERR = 0x7\ncvSourceDS = 0x4\ncvSourceAS = 0x5\ncvSourceSW = 0x8\ncvSourceFP_Button = 0x9\ncvSourceCoinc = 0xA\ncvSourceINOR = 0xB\n");
	scanf("%"SCNx32, &start_source);

	//Clear Source Selection
	printf("\n\nSelect the Clear Source (see definition in CAENVMETypes.h)\n\ncvSourceIN0 = 0x2\ncvSourceIN1 = 0x3\ncvSourceDTACK = 0x6\ncvSourceBERR = 0x7\ncvSourceDS = 0x4\ncvSourceAS = 0x5\ncvSourceSW = 0x8\ncvSourceFP_Button = 0x9\ncvSourceCoinc = 0xA\ncvSourceINOR = 0xB\n");
	scanf("%"SCNx32, &clear_source);

	

	//Device Configuration
	re = CAENVME_SetScaler_Mode(handle, ScalerMode);

	re = CAENVME_SetScaler_InputSource(handle, input_source);

	re = CAENVME_SetScaler_StartSource(handle, start_source);

	re = CAENVME_SetScaler_ClearSource(handle, clear_source);


	
	//Switch on ScalerMode
	switch (ScalerMode) {
	//Gate Mode
	case 0:
		printf("\n\nSelect the Gate Source (see definition in CAENVMETypes.h)\n\ncvSourceIN0 = 0x2\ncvSourceIN1 = 0x3\ncvSourceDTACK = 0x6\ncvSourceBERR = 0x7\ncvSourceDS = 0x4\ncvSourceAS = 0x5\ncvSourceSW = 0x8\ncvSourceFP_Button = 0x9\ncvSourceCoinc = 0xA\ncvSourceINOR = 0xB\n");
		scanf("%"SCNx32, &gate_source);
		re = CAENVME_SetScaler_GateSource(handle,gate_source);
		break;
	//D-Well Time Mode
	case 1:
		printf("\n\nSelect the D-Well Time\n");
		scanf("%"SCNx16, &dwell_time);
		re = CAENVME_SetScaler_DWellTime(handle, dwell_time);
		break;
	//MaxHits Mode
	case 2:
		printf("\n\nSelect the MaxHit Count\n");
		scanf("%"SCNx16, &maxhit_count);
		re = CAENVME_SetScaler_MaxHits(handle, maxhit_count);
		break;
	//Invalid Parameter Case
	default:
		printf("\nInvalid parameter\n");
		Sleep_(1000);
		return 1;
	}


	//Run Mode Selection
	printf("\n\nSelect the Run Mode\n\nContinuous Run = 0\nSingle Run = 1\n");
	scanf("%X", &continuous_run);

	re = CAENVME_SetScaler_ContinuousRun(handle, continuous_run);

	clr_scr();
	printf("\nScaler set\n");
	Sleep_(1000);
	clr_scr();

	
	printf("Scaler Acquisition\n\n");
	if (start_source == 0x8) printf("Press [s] to Start/Stop the Scaler;");
	if (clear_source == 0x8) printf(" Press [r] to Reset;");
	if (gate_source == 0x8) printf(" Press [g] to Start/Stop the Gate;");
	printf(" Press [q] to Quit;\n");

	while (1) {
		
		if (kbhit_()) {
			if (scaler_started == 0 && getch_() == 's') {
				re = CAENVME_SetScaler_SWStart(handle);
				printf("Scaler Started\n");
				scaler_started = 1;
			}
			if (clear_source == 0x8 && getch_() == 'r') {
				re = CAENVME_SetScaler_SWReset(handle);
				printf("Scaler Reset\n");
			}
			if (scaler_started == 1 && getch_() == 's') {
				re = CAENVME_SetScaler_SWStop(handle);
				printf("Scaler Stop\n");
				scaler_started = 0;
				if (gate_source != 0x8) {
					re = CAENVME_ReadRegister(handle, 0x32, &scaler_data);
					if (re != 0) printf("Whoooops, something goes wrong\n");
					printf("Scaler Data = %"PRId32"\n", scaler_data);
				}
			}
			if (gate_source == 0x8 && getch_() == 'g') {
				if (is_gate_open == 0) {
					re = CAENVME_SetScaler_SWOpenGate(handle);
					if (re == 0) {
						is_gate_open = 1;
						printf("Gate Opened\n");
					}
				}
				else {
					re = CAENVME_SetScaler_SWCloseGate(handle);
					if (re == 0) {
						is_gate_open = 0;
						re = CAENVME_ReadRegister(handle, 0x32, &scaler_data);
						if (re != 0) printf("Whoooops, something goes wrong\n");
						printf("Gate Closed\n");
						printf("Scaler Data = %"PRId32"\n", scaler_data);
					}
				}
			}
			if (getch_() == 'q') {
				if (scaler_started == 1) re = CAENVME_SetScaler_SWStop(handle);
				printf("Quit\n");
				break;
			}
		}
	}
	

	return re;
}



//V1718 - V2718 Scaler Setup 
int V1718_ScalerSetup(uint32_t handle) {
	
	int re = 0;
	
	printf("to do\n");
	Sleep_(1000);

	return re;
}