    -----------------------------------------------------------------------------

                   --- CAEN SpA - Computing Systems Division --- 

    -----------------------------------------------------------------------------

    CAENDigitizer Library Rel. 2.17.3
    
    Installation and Use Instructions

    -----------------------------------------------------------------------------

    Jan 2023

    -----------------------------------------------------------------------------

  
 The complete documentation can be found in the CAENDigitizer Library Manual
 available on this CD or on CAEN's Web Site at http://www.caen.it.


 Content 
 -------

 Readme.txt       : This file.

 ReleaseNotes.txt : Release Notes of the last software release.
 
 lib              : Directory containing the library binary file
                   and an install script.
 
 include          : Directory containing the relevant header files.
 
 samples          : Directory with five folders containing the source
                  code of examples that show how acquire with this
                  library (one example for Standard Firmware and one
                  for each DPP type) and a folder containig the
                  algorithms for x742 boards offline corrections.
 

 System Requirements
 -------------------
 - glibc version 2.11.1 or above
 - CAENComm library version 1.0.2 or above.


 Installation notes
 ------------------
 
  - Go to the lib directory

  - Execute: 
      sh install               to install the 32bit version of the library 
      sh install_x64           to install the 64bit version of the library 
      sh install_arm64         to install the arm 64bit version of the library 
