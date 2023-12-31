////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQTemplate.cc
// date: 31 Dec 23 (last updated)
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQTemplate is a relatively simple C++ program intended to
//       provide a basic code framework for digitized data acquisition
//       using the ADAQ libraries and CAEN digitizers. It essentially
//       functions as a tutorial with commented code to demonstrate
//       how to first program digitizers and then read out waveform
//       data during acquisition. It is also intended to be copied and
//       extended by a user into a more mature data acquisition
//       framework intended to satisfy more complex needs. The code
//       provides a reasonable directory structure for doing so as
//       well as a GNUmakefile build system to integrate dependencies
//       and control compilation. Dependencies are minimal to maximize
//       portability.
//
//       At present, the code supports two types of CAEN firmware:
//       standard (STD) and pulse shape discrimination (PSD). It has
//       been developed and tested on x720, x724, and x730 type
//       digitizers as well as the DT5790.
//
// dpnd: The code requires the following software dependencies:
//       -> ADAQ libraries (to enhance control/readout of CAEN digitizers)
//       -> Boost C++ libraries (for threading)
//
// 2run: Ensure appropriate CAEN driver is installed, the USB/Optical
//       cord is connected between the CAEN hardware and PC, and the
//       CAEN hardware is powered on! Then:
//
//       $ make
//       $ ./bin/ADAQTemplate
//
////////////////////////////////////////////////////////////////////////////////

// C++ 
#include <iostream>
using namespace std;

// Boost
#include <boost/thread.hpp>

// ADAQTemplate
#include "AcquisitionManager.hh"


int main(int argc, char *argv[])
{
  if(argc != 1){
    cout << "\nADAQTemplate : Error! No command line arguments are allowed!\n"
	 << endl;
    return -42;
  }
  
  ////////////////////////////////////////////
  // Create an "acquisition manager" object //
  ////////////////////////////////////////////

  AcquisitionManager *AcquisitionMgr = new AcquisitionManager;


  /////////////////////////////////////
  // Arm the data acquisition system //
  /////////////////////////////////////

  cout << "\nADAQTemplate : Arming the DAQ ...\n"
       << endl;
  
  AcquisitionMgr->Arm();


  ///////////////////////////
  // Start the acquisition //
  ///////////////////////////

  cout << "ADAQTemplate : Starting the waveform acquisition phase ...\n"
       << "               Multithreaded processing will now begin. Creating threads ...\n"
       << endl;

  // Create two Boost threads. The first thread ("Acquisition_thread")
  // will contain the acquisition loop, which reads out waveform data
  // from the digitizer and operates on it. The Acquisition_thread is
  // configured to accept control signals from the second thread
  // ("Control_thread"), which allows the user to interface with the
  // code while the acquisiion loop is running in a separate thread

  boost::thread Acquisition_thread = boost::thread(&AcquisitionManager::RunAcquisitionLoop, AcquisitionMgr);
  boost::thread Control_thread = boost::thread(&AcquisitionManager::RunControlLoop, AcquisitionMgr, &Acquisition_thread);
  
  cout << "ADAQTemplate : Joining threads ...\n" 
       << endl;

  // Join the threads. At this point, the "main" thread splits into
  // the Acquisition_thread and the Control_thread. The main thread
  // waits until both threads have concluded, which occurs when the
  // user uses the Control_thread to terminate the acquisition loop
  
  Acquisition_thread.join();
  Control_thread.join();
  
  // The main thread resumes here after the Acquisition_thread and
  // Control_thread have concluded their business.
  
  cout << "ADAQTemplate : All worker threads have completed!\n"
       << endl;
  

  ////////////////////////////////////////
  // Disarm the data acquisition system //
  ////////////////////////////////////////
  
  cout << "ADAQTemplate : Disarming the DAQ ...\n"
       << endl;
  
  AcquisitionMgr->Disarm();
  
  delete AcquisitionMgr;

  return 0;
}
