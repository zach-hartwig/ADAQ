
        ----------------------------------------------------------------------

                    --- CAEN SpA - Computing Systems Division --- 

        ----------------------------------------------------------------------
        
        CAENComm Library Readme file
        
        ----------------------------------------------------------------------

        Package for Linux kernels 2.4, 2.6

        July 2010


 The complete documentation can be found in the User's Manual on CAEN's web
 site at: http://www.caen.it.


 Content
 -------

 Readme.txt       : This file.

 ReleaseNotes.txt : Release Notes of the last software release.

 lib              : Directory containing the library binary file
                   and an install script.
                   
 include          : Directory containing the relevant header files.


 System Requirements
 -------------------

 - Linux kernel Rel. 2.4 or 2.6 with gnu C/C++ compiler
 - CAENVMELib library version 2.12 or above.


 Installation notes
 ------------------

  - Login as root

  - Copy the needed files on your work directory

To install the dynamic library:

  - Go to the library directory

  - Execute: 
      sh install               to install the 32bit version of the library compiled with glibc version 2.5
      sh install_x64           to install the 64bit version of the library compiled with glibc version 2.5

 The installation copies and installs the library in /usr/lib.

