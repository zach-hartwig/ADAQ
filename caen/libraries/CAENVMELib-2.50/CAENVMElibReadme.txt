
        ----------------------------------------------------------------------

                    --- CAEN SpA - Computing Systems Division --- 

        ----------------------------------------------------------------------
        
        CAENVMElib Library Readme file
        
        ----------------------------------------------------------------------

        Package for Linux 

        June 2015


 The complete documentation can be found in the User's Manual on CAEN's web
 site at: http://www.caen.it.


 Content
 -------

 CAENVMELibReadme.txt       : This file.

 CAENVMELibReleaseNotes.txt : Release Notes of the last software release.

 lib                        : Directory containing the library binary file
                              and an install script .
							  
 labview-6.0                : Directory containing the LabView library of VIs.							  

 sample                     : Directory containing a sample program with
                              source code.

 include                    : Directory containing the relevant header files.


 System Requirements
 -------------------

 - CAEN V1718 USB-VME Bridge and/or CAEN V2718 PCI-VME Bridge
 - Linux glibc version 2.12 or greater

 Installation notes
 ------------------

  - Login as root

  - Copy the needed files on your work directory

To install the dynamic library:

  - Go to the library directory

  - Execute: sh install (for 32bit installation)
  
  - Execute: sh install_x64 (for 64bit installation)

 The installation copies and installs the library in /usr/lib.

