/******************************************************************************
*
*	CAEN SpA - ATM Division
*	Via Vetraia, 11 - 55049 - Viareggio ITALY
*	+39 0594 388 398 - www.caen.it
*
***************************************************************************//*!
*
*	\file		main_console.c
*	\brief
*	\author		Matteo Bianchini
*	\version
*	\date		2022
*	\copyright	GNU Lesser General Public License
*
******************************************************************************/

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CAENVMElib.h>

#include "Scaler_Demo.h"
#include "Pulser_Demo.h"
#include "main_console.h"
#include "Sys_Func.h"


///Function for board initiation
int SelectBoard(int32_t* p_Handle, int command) {
	//Parameters
	int re = 1;
	//Init parameters
	CVBoardTypes BdType;
	short ConetNode = 0;
	uint32_t LinkNumber_or_PID;
	short optic_reader;

	//temp variables
	short int conn_type;
	char ip[256];



	clr_scr();
	switch (command) {
		//case 0
	case 0:
		return -1;
		break;

		//case V1718
	case 1:

		printf("\n\nBridge: V1718\n");
		printf("\nSelect the Link Number\n");
		scanf("%"SCNd32, &LinkNumber_or_PID);
		BdType = cvV1718;
		re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
		break;

		//case V2718
	case 2:

		printf("\n\nBridge: V2718\n");
		printf("Select the Optical Link Reader\n0- A2818\n1- A3818\n2- A4818\n");
		scanf("%hd", &optic_reader);
		if(optic_reader < 2){
			printf("\nSelect the Link Number\n");
			scanf("%"SCNd32, &LinkNumber_or_PID);
			printf("\nSelect the Conet number in the daisy-chain loop\n");
			scanf("%hd", &ConetNode);
			BdType = cvV2718;
			re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
			break;
		}

		//A4818 case
		else { 
			printf("\nInsert the A4818 PID\n");
			scanf("%"SCNd32, &LinkNumber_or_PID);
			printf("\nSelect the Conet number in the daisy-chain loop\n");
			scanf("%hd", &ConetNode);
			BdType = cvUSB_A4818_V2718_LOCAL;
			re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
			break;
		}
		
		//case V3718
	case 3:

		printf("\n\nBridge: V3718\n");
		printf("\nSelect the Type of Connection:\n0=USB\n1=Optical Link\n");
		scanf("%hd", &conn_type);
		switch (conn_type) {
		case 0:
			printf("\nSelect the Link Number\n");
			scanf("%"SCNd32, &LinkNumber_or_PID);
			BdType = cvUSB_V3718_LOCAL;
			re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
			break;
		case 1:
			printf("Select the Optical Link Reader\n0- A2818\n1- A3818\n2- A4818\n");
			scanf("%hd", &optic_reader);
			//A3818 -A2818 case
			if (optic_reader < 2) {
				printf("\nSelect the Link Number\n");
				scanf("%"SCNd32, &LinkNumber_or_PID);
				printf("\nSelect the Conet Number in the Daisy-Chain Loop\n");
				scanf("%hd", &ConetNode);
				if (optic_reader == 0) BdType = cvPCI_A2818_V3718_LOCAL;
				else BdType = cvPCIE_A3818_V3718_LOCAL;
				re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
				break;
			}
			else {
				printf("\nInsert the A4818 PID\n");
				scanf("%"SCNd32, &LinkNumber_or_PID);
				printf("\nSelect the Conet number in the daisy-chain loop\n");
				scanf("%hd", &ConetNode);
				BdType = cvUSB_A4818_V3718_LOCAL;
				re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
				break;
			}
		default:
			printf("Invalid Parameter\n");
			re = 1;
			break;
		}
		break;

		//case V4718
	case 4:

		printf("\n\nBridge: V4718\n");
		printf("\nSelect the Type of Connection:\n0=USB\n1=Ethernet\n2=Optical Link\n");
		scanf("%hd", &conn_type);
		switch (conn_type) {
		case 0:
			printf("\nInsert the PID\n");
			scanf("%"SCNd32, &LinkNumber_or_PID); // V4718 requires uint32_t
			BdType = cvUSB_V4718_LOCAL;
			re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
			break;
		case 1:
			printf("\nInsert the IP Address\n");
			scanf("%s", ip);
			BdType = cvETH_V4718_LOCAL;
			re = CAENVME_Init2(BdType, ip, ConetNode, p_Handle);
			break;
		case 2:
			printf("Select the Optical Link Reader\n0- A2818\n1- A3818\n2- A4818\n");
			scanf("%hd", &optic_reader);
			//A3818 -A2818 case
			if (optic_reader < 2) {
				printf("\nSelect the Link Number\n");
				scanf("%"SCNd32, &LinkNumber_or_PID);
				printf("\nSelect the Conet Number in the Daisy-Chain Loop\n");
				scanf("%hd", &ConetNode);
				if (optic_reader == 0) BdType = cvPCI_A2818_V4718_LOCAL;
				else BdType = cvPCIE_A3818_V4718_LOCAL;
				re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
				break;
			}
			else {
				printf("\nInsert the A4818 PID\n");
				scanf("%"SCNd32, &LinkNumber_or_PID);
				printf("\nSelect the Conet number in the daisy-chain loop\n");
				scanf("%hd", &ConetNode);
				BdType = cvUSB_A4818_V4718_LOCAL;
				re = CAENVME_Init2(BdType, &LinkNumber_or_PID, ConetNode, p_Handle);
				break;
			}
		default:
			printf("\n\nConnection Option not allowed\n");
			Sleep_(2000);
			re = 1;
			break;
		}
		break;
		//default
	default:
		printf("\n\nOption not allowed\n");
		Sleep_(2000);
		re = 1;
		break;
	}

	return re;
}


//main
int main() {
	//Variables
	int re;
	short command, bridge_mode;
	//Connection parameters
	uint32_t Handle;

	//Pulser parameters
	CVPulserSelect Pulser;
	float pulsertime;
	int NPulses;
	CVIOSources IOsource;

	clr_scr();
	//BRIDGE CONNECTION
	printf("Welcome to CAEN Bridge Pulser Demo\nSelect the Bridge:\n");
	printf("1- V1718\n");
	printf("2- V2718\n");
	printf("3- V3718\n");
	printf("4- V4718\n");
	printf("0- Exit\n\n\n");
	scanf("%hd", &command);


	//Connection
	re = SelectBoard(&Handle, command);
	clr_scr();
	if (re == -1) {
		printf("\n\nQuit\n");
		Sleep_(1000);
		return 0;
	}
	if (re != 0) {
		printf("\nError Connetting to the Bridge\n");
		Sleep_(3000);
		return re;
	}
	else printf("\nBridge Connected\n");
	Sleep_(3000);

	while (1) {

		clr_scr();
		printf("Select the Bridge function\n0 = Pulser\n1 = Scaler\n2 = Quit\n");
		scanf("%hd", &bridge_mode);

		switch (bridge_mode) {
		case(0):
			//Pulser Config
			if (PulserConfig(Handle, &Pulser, &pulsertime, &NPulses, &IOsource) != 0) {
				return 1;
			}

			//Output Config
			if (OutputConfig(Handle, Pulser, command) != 0) {
				return 1;
			}


			if (IOsource == cvManualSW) {
				//PULSER SOFTWARE START
				clr_scr();
				printf("Press [s] to start the Pulser\n");
				while (1) {
					if (kbhit_() != 0) {
						if (getch_() == 's') {
								re = CAENVME_StartPulser(Handle, Pulser);
								if (re != cvSuccess) printf("\n \n Error starting the pulser\n");
								break;
						}
					}
				}
				printf("Pulser started\n");

				//PULSER SOFTWARE STOP
				if (NPulses == 0) {
					if (pulsertime != 0) {
						Sleep_((int)pulsertime);
						if (CAENVME_StopPulser(Handle, Pulser) != cvSuccess) {
							printf("\n \n Error stopping the pulser\n");
							return 1;
						}
					}
					else if (pulsertime == 0) {
						printf("\nPress [s] to Stop the Pulser\n");
						while (1) {
							if (kbhit_()!=0) {
								if (getch_() == 's') {
									re = CAENVME_StopPulser(Handle, Pulser);
									break;
								}
								else printf("Invalid command\n");
							}
						}
					}
					printf("Pulser stopped\n");
					Sleep_(500);
				}
			}
			break;
		case(1):
			if (command > 2) {
				re = V3718_ScalerSetup(Handle);
				if (re != cvSuccess) printf("Error %d setting the scaler", re);
				Sleep_(1000);
			}
			else {
				re = V1718_ScalerSetup(Handle);
				if (re != cvSuccess) printf("Error %d setting the scaler", re);
			}
			break;
		case(2):
			//END
			CAENVME_End(Handle);
			return 0;
		default:
			printf("Invalid Parameter\n");
			CAENVME_End(Handle);
			return 1;
		}
	}

	
	CAENVME_End(Handle);
	return 0;
}
