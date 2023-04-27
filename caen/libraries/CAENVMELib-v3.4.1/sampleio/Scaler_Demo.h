/******************************************************************************
*
*	CAEN SpA - ATM Division
*	Via Vetraia, 11 - 55049 - Viareggio ITALY
*	+39 0594 388 398 - www.caen.it
*
***************************************************************************//*!
*
*	\file		Scaled_Demo.h
*	\brief
*	\author		Matteo Bianchini
*	\version
*	\date		2022
*	\copyright	GNU Lesser General Public License
*
******************************************************************************/

#ifndef SCALER_DEMO_H_
#define SCALER_DEMO_H_

#include <CAENVMElib.h>

//V3718 - V4718 Scaler Setup
int V3718_ScalerSetup(uint32_t handle);

//V1718 - V2718 Scaler Setup
int V1718_ScalerSetup(uint32_t handle);

#endif
