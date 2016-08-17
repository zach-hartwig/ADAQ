## The ADAQ Libraries Source Code Change Log

name: Changelog.md  
date: 17 Aug 16 (last updated)  
auth: Zach Hartwig  
mail: hartwig@psfc.mit.edu


## Version 1.8 Series

### 1.8.2

 - Implementing ability to use gated TTL/NIM acquisition on/off via
   S-IN / GPI connector. This fixes issues that seem to be caused when
   trying to use CAEN methods to activate these acquisition modes.


### 1.8.1

 - Implementing basic FPGA buffer check for CAEN DPP-PSD firmware
   (binary state of "full" or "Not full" is determined)

 - Implementing CAENDigitzer temperature and calibration functions for
   x725 and x730 family of digitizers in the ADAQControl library

 - Correcting major bug for HV device settings to correctly enable
   full settings and operation for all channels on HV device
   

### 1.8.0

 - Major upgrade to the ASIM library: new capabilities to readout
   registered readouts as arrays and with optional coincidence mode,
   lower/upper thresholds

 - Updates to ASIM example simulations

 - Cleanup of makefiles, depracated code


## Version 1.6 Series

### 1.6.0

 - Upgraded all libraries for compatibility with ROOT6 (v6.06.00);
   ADAQ v1.4.3 is last tagged version compatible with ROOT5 (v5.34.00)

 - Adding C++11 compiler flags to all three libraries. Necessary for
   compiling ASIM libraries against newest Geant4 (v10.02)
   
 - Upgraded BuildLibraries.sh script to handle sequential and parallel
   builds of the ASIM libraries

 - Minor updates to libraries, Makefiles, and setup.sh script


## Version 1.4 Series

### 1.4.3

 - Removed MPIManager header include from ASIMStorageManager class;
   enables ADAQAnalysis to compile on non-Geant4 systems

 - Upgraded CAENDigitizer library to 2.6.8; upgraded all board
   firmware version

 - Cleanup of compilation warnings in ASIM libraries
 

### 1.4.2

 - Bug fix in ADAQControl library preventing connection to high
   voltage units with non-zero USB link numbers

 - Bug fix in ASIM libaries removing depracated "file open" checks
   that were preventing run-level aggregation when ASIM files were
   intentionally not open

 - Updated CAENUpgraderGUI to most-recent version (1.5.2)


### 1.4.1

 - Bug fix in ASIM libraries to enable linking against libASIMStorage
   for systems without Geant4 or libASIMReadoutManager

 - Bug fix to enable ASIM libraries to handle multiple ASIM files
   (with option of multiple runs per file) within one Geant4 session

 - Bug fix to prevent massive memory leak with ASIM*SD/SDHit classes

 - Updated ASIMExample Geant4 simulation for compatibility with ASIM
   library bug fixes above.


### 1.4.0

 - Releasing production version of ASIM library to enable
   comprehensive, easy integration of ADAQ framework with Geant4
   simulation. Enables readout of simulated deetector data into ASIM 
   files that can be analyzed with the powerfulADAQAnalysis tool.

 - Releasing example Geant4 simulation (ASIMExample) to show how to
   integrate ASIM libraries into a user's simulation

 - Enabled the CAEN V1725 digitizer within the ADAQControl
   library. Initial testing demonstrated full compatibility

 - Updated CAENUSBdrvB driver to v1.5.1; updated CAENDigitizer library
   to 2.6.7. Both are most up-to-date.

 - Update to all libraries Makefiles and the BuildLibraries.sh script
   to ensure correct building on basic Linux platforms


## Version 1.2 Series

### 1.2.1  

 - Fixed time stamp readout type from a signed 32-bit integer to a 64-bit
   unsigned integer. Now correctly stores long-duration time stamps

 - Adding channel-specific event aggregation settings for DPP-PSD

 - Enabling setting of high voltage maximum for devices that have
   software rather than hardware VMAX limits (e.g. DT5790X)

 - Automatically set software high voltage max for DT5790X devices
 
### 1.2.0

 - Implemented ability to control digitizer and high voltage functions
   of the new CAEN DT5790 desktop unit

 - Fully implemented ability to use CAEN DPP-PSD, including
   ADAQDigitizer methods as well as support for readout to ADAQ files
   via the the ADAQReadout library

 - Upgrade CAENMVE to version 2.50; Upgrade CAENUpgrade tool to 1.5.1


## Version 1.0 Series

### 1.0.1

 - Update CAENDigitizer to version 2.6.5; ADAQDigitizer wrappers updated to match

 - Added CAEN DPP-PSD firwmare functions to ADAQDigitizer class

 - Large update to the ASIMReadout classes for Geant4 interface

 - Makefile overhauled for more friendly and generic builds out-of-the-box

 - Cleaned up about 20 compiler warnings for ADAQControl library

 - Updating CAEN documentation, firmware, and software

 - Adding a Bash script to automate building and installing all three
   ADAQ libraries
