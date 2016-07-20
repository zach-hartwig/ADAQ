////////////////////////////////////////////////////////////////////////////////
//
// name: CAENAcquisitionTemplate.cc
// date: 15 Jul 16
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: CAENAcquisitionTemplate is a code intended to provide the
//       basic foundation for writing acquisition code using CAEN
//       digitizers. It provides a mature build system, modular code,
//       and basic layout for rapidly developing acquisition code,
//       prototypng, and testing suites. The idea is that the user
//       should copy this directory wholesale into a new directory,
//       completely rename everything as they fit, and start to
//       modify/augment the code as they fit.
//
// dpnd: The code requires the following to be installed:
//       -> ADAQ libraries
//       -> ROOT data analysis framework
//       -> Boost C++ header and libraries
//
////////////////////////////////////////////////////////////////////////////////

// C++ 
#include <iostream>
using namespace std;

// Boost
#include <boost/thread.hpp>

// CAENAcquisitionTemplate
#include "AcquisitionManager.hh"


int main(int argc, char *argv[])
{
  if(argc != 1){
    cout << "\nCAENAcquisitionTemplate : Error! No command line arguments are allowed!\n"
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

  cout << "\nCAENAcquisitionTemplate : Arming the DAQ ...\n"
       << endl;

  AcquisitionMgr->Arm();


  ///////////////////////////
  // Start the acquisition //
  ///////////////////////////

  cout << "CAENAcquisitionTemplate : Starting the waveform acquisition phase ...\n"
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
  
  cout << "CAENAcquisitionTemplate : Joining threads ...\n" 
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
  
  cout << "CAENAcquisitionTemplate : All worker threads have completed!\n"
       << endl;
  

  ////////////////////////////////////////
  // Disarm the data acquisition system //
  ////////////////////////////////////////
  
  cout << "CAENAcquisitionTemplate : Disarming the DAQ ...\n"
       << endl;
  
  AcquisitionMgr->Disarm();
  
  delete AcquisitionMgr;

  return 0;
}
