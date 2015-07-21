/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                           Copyright (C) 2012-2015                           //
//                 Zachary Seth Hartwig : All rights reserved                  //
//                                                                             //
//      The ADAQ libraries source code is licensed under the GNU GPL v3.0.     //
//      You have the right to modify and/or redistribute this source code      //      
//      under the terms specified in the license, which may be found online    //
//      at http://www.gnu.org/licenses or at $ADAQ/License.md.                 //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQHighVoltageRegisters.hh
// date: 21 Jul 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: This header file contains complete register maps and
//       important register settings for all high voltage units
//       presently supported by the ADAQ libraries. The register
//       addresses and settings are dynamically allocated to
//       ADAQHighVoltage class members upon instantiation of a
//       concrete object with a specific high voltage unit type. Note
//       that "P", "N", and "M" refer to (all channels) positive, (all
//       channels) negative, and standard mixed polarity high voltage.
//       The following high voltage units are presently supported:
//
//       - CAEN "V653X" :  V6533{P,N,M} and V6534{P,N,M} VME boards
//       - CAEN "DT5790" :  DT5790{P,N,M} desktop digitizer+HV unit
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQHighVoltageRegisters_hh__
#define __ADAQHighVoltageRegisters_hh__ 1

// Boost
#include <boost/cstdint.hpp>

//////////////////////////////////////////////////////
// CAEN VME HV boards V6533{P,N,M} and V6534{P,N,M} //
//////////////////////////////////////////////////////

namespace V653X{
  
  // Global register addresses
  const uint32_t VMAX = 0x0050;
  const uint32_t IMAX = 0x0054;
  const uint32_t STATUS = 0x0058;
  const uint32_t FIRMREL = 0x005C;

  // Individual channel register addresses
  
  // Register Address          CH0      CH1      CH2      CH3      CH4      CH5
  const uint32_t VSET[6] = {0x0080,  0x0100,  0x0180,  0x0200,  0x0280,  0x0300}; // Voltage set
  const uint32_t ISET[6] = {0x0084,  0x0104,  0x0184,  0x0204,  0x0284,  0x0304}; // Current set
  const uint32_t VMON[6] = {0x0088,  0x0108,  0x0188,  0x0208,  0x0288,  0x0308}; // Voltage monitor
  const uint32_t IMON[6] = {0x008c,  0x010c,  0x018c,  0x020c,  0x028c,  0x030c}; // Current monitor (max)
  const uint32_t   PW[6] = {0x0090,  0x0110,  0x0190,  0x0210,  0x0290,  0x0310}; // Power 
  const uint32_t  POL[6] = {0x00ac,  0x012c,  0x01ac,  0x022c,  0x02ac,  0x032c}; // Polarity
  const uint32_t TEMP[6] = {0x00b0,  0x0130,  0x01b0,  0x0230,  0x02b0,  0x0330}; // Temperature

  // Register values to control the power state (on/off)
  const uint16_t POWEROFF = 0x00;
  const uint16_t POWERON  = 0x01;
}


/////////////////////////////////////////////
// CAEN desktop digitizer+HV DT5790{P,N,M} //
/////////////////////////////////////////////

// Reference: "CAEN DT5790 & x780 HV Registers Description (Rev. 0)" (05 Dec 14)
//
// Notes:
//
// - Control of DT5790 HV channels is much more limited than VME V653X
//   HV boards, and the individual register names/functions are not
//   perfectly identical. The names/functions of the V653X registers
//   are preserved here to ensure conformity in interacting with V653X
//   and DT5790 HV units via the ADAQHighVoltage class. Unused
//   registers (compared with the V653X above) are marked "(unused)"
//   below and set to 0x0000
//
// - DT5790 registers are all channel-specific (no global settings)
//   and are addressed as "0xZnZZ", where n==2 is HV channel 0 and
//   n==3 is HV channel 1.
//
// - DT5790M has HV channel 0 (positive) and HV channel 1 (negative).

namespace DT5790{

  // Individual channel register addresses.
  
  // Register Address            CH0      CH1
  const uint32_t   VMAX[2] = {0x1230,  0x1330}; // Voltage maximum
  const uint32_t   IMAX[2] = {0x0000,  0x0000}; // Current maximum (unused)
  const uint32_t STATUS[2] = {0x1238,  0x1338}; // Status

  const uint32_t VSET[2] = {0x1220,  0x1320}; // Voltage set
  const uint32_t ISET[2] = {0x1224,  0x1324}; // Current set
  const uint32_t RMPU[2] = {0x1228,  0x1328}; // Ramp voltage up rate
  const uint32_t RMPD[2] = {0x122C,  0x132C}; // Ramp voltage down rate
  const uint32_t VMON[2] = {0x1240,  0x1340}; // Voltage monitor
  const uint32_t IMON[2] = {0x1244,  0x1344}; // Current monitor (max)
  const uint32_t   PW[2] = {0x1234,  0x1334}; // Power
  const uint32_t  POL[2] = {0x0000,  0x0000}; // Polarity (unused)
  const uint32_t TEMP[2] = {0x0000,  0x0000}; // Temperature (unused)
  
  // Register values to control the power state (on/off)
  const uint16_t POWEROFF = 0x00;
  const uint16_t POWERON  = 0x01;
}

#endif
