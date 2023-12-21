////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQTemplate.cc
// date: 20 Dec 23 (last updated)
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ADAQTemplate is a simple C++ program intended to provide the
//       foundation for a basic detector waveform acquisition loop
//       using the ADAQ libraries and CAEN digitizers. It provides a
//       tutorial with highly commented code to demonstrate how to
//       program digitizers and read out acquired waveform data, a
//       basic framework that a user can extend into a more mature
//       code for his/her purposes, and a GNUmakefile-based build
//       system foe easy compilation. Dependencies are minimal to
//       enable portability and usability.
//
//       At present, the code supports two types of CAEN firmware:
//       standard and DPP-PSD. It has been tested on x720, x724, and
//       x730 type digitizers as well as the DT5790.
//
// dpnd: The code requires the following software dependencies:
//       -> ADAQ libraries (for control/readout of CAEN digitizers)
//       -> Boost C++ libraries (for threading)
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
  // The AcquisitionManager control all the DAQ functionality,
  // including "arming" the DAQ (setting V6534 and V1720 registers,
  // powering up the V6534 high voltage channels, etc)
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
       << "                          Multithreaded processing will now begin. Creating threads ...\n"
       << endl;

  // Create two Boost threads. The first thread ("Acquisition_thread")
  // will handle waveform acquisition (setup, waveform digitization,
  // persistent storage, shutdown). The Acquisition_thread is
  // configured to accept interrupt signals from the second thread
  // ("Escape_thread"), which allows the user to interface with the
  // code while the acquisiion loop is running in a separate thread
  // (e.g. to manually trigger the digitizer or to terminate
  // acquisition).

  boost::thread Acquisition_thread = boost::thread(&AcquisitionManager::StartAcquisition2,AcquisitionMgr);
  boost::thread Escape_thread = boost::thread(&AcquisitionManager::StopAcquisition,AcquisitionMgr,&Acquisition_thread);
  
  cout << "ADAQTemplate : Joining threads ...\n" 
       << endl;

  // Join the threads. At this point, the "main" thread splits into
  // the Acquisition_thread and the Escape_thread. The main thread
  // will wait until both threads have concluded (which occurs when
  // the user uses the Escape_thread to terminate the waveform
  // digitization)
  
  Acquisition_thread.join();
  Escape_thread.join();

  // The main thread resumes here after the Acquisition_thread and
  // Escape_thread have concluded their business.
  
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
