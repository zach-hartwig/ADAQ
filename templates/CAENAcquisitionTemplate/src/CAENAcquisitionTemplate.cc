////////////////////////////////////////////////////////////////////////////////
//
// name: CAENAcquisitionTemplate.cc
// date: 14 Aug 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: CAENAcquisitionTemplate is a code intended to provide the
//       basic foundation for writing acquisition code that interfaces
//       with the CAEN V1720 board via the V1718 USB/VME module. It
//       provides a mature build system, modular code, and basic
//       layout for rapidply developing acquisition code prototypes
//       and testing suites. 
//
// dpnd: The code requires the following to be installed:
//       -> ADAQ  libraries
//       -> ROOT data analysis framework
//       -> Boost C++ header and libraries
//
////////////////////////////////////////////////////////////////////////////////

// C++ 
#include <iostream>
using namespace std;

// Boost
#include <boost/thread.hpp>

// ADAQ 
#include "AcquisitionManager.hh"


int main(int argc, char *argv[])
{
  if(argc!=1){
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
  // will handle waveform acquisition (V6534/V1720/ROOT setup,
  // waveform digitization, persistent waveform storage in ROOT file,
  // shutdown). The Acquisition_thread is configured to accept
  // interrupt signals from the second thread ("Escape_thread"), which
  // allows the user to terminate the waveform digitization phase and
  // begin the storage/shutdown phase of the acquisition.

  boost::thread Acquisition_thread = boost::thread(&AcquisitionManager::StartAcquisition,AcquisitionMgr);
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
