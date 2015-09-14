## The ADAQ Libraries Source Code Change Log

name: Changelog.md  
date: 26 Aug 15 (last updated)  
auth: Zach Hartwig  
mail: hartwig@psfc.mit.edu


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



