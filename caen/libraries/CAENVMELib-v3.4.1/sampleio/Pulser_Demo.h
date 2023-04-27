/******************************************************************************
*
*	CAEN SpA - ATM Division
*	Via Vetraia, 11 - 55049 - Viareggio ITALY
*	+39 0594 388 398 - www.caen.it
*
***************************************************************************//*!
*
*	\file		Pulser_Demo.h
*	\brief
*	\author		Matteo Bianchini
*	\version
*	\date		2022
*	\copyright	GNU Lesser General Public License
*
******************************************************************************/

#ifndef PULSER_DEMO_H_
#define PULSER_DEMO_H_

#include <CAENVMElib.h>

//Function for Pulser configuration
int PulserConfig(int32_t Handle, CVPulserSelect* p_Pulser, float* p_pulsertime, int* p_NPulses, CVIOSources* p_IOsource);

//Function for the Output configuration
int OutputConfig(int32_t Handle, CVPulserSelect Pulser, int command);

#endif
