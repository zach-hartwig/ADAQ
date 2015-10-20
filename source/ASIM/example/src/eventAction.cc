#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "Randomize.hh"

#include <ctime>

#ifdef MPI_ENABLED
#include "MPIManager.hh"
#endif
#include "ASIMReadoutManager.hh"

#include "eventAction.hh"


eventAction::eventAction()
  : eventInfoFreq(50), runID(0),
    runTime(0.), prevRunTime(0.), eventsPerSec(0.), totalEventsToRun(0.), timeToFinish(0.)
{ 
  EventActionMessenger = new eventActionMessenger(this); 
}


eventAction::~eventAction()
{ delete EventActionMessenger; }


void eventAction::BeginOfEventAction(const G4Event *currentEvent)
{
  G4int event = currentEvent->GetEventID();
  
  if(event==0){
    G4cout << "\n\n"
	   << "\n***************************************************************************\n"
	   <<   "****  G4 STATUS: Tracking Events!  ****************************************\n***" 
	   << G4endl;

    totalEventsToRun = 
      G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
  }
  else if(event % eventInfoFreq == 0){
    
    G4RunManager *runMgr = G4RunManager::GetRunManager();
    
    // Account for the fact that the process clock runs continously
    // across successive runs by subtracting the previous run time
    // from the total process time to get the current run time
    if(runMgr->GetCurrentRun()->GetRunID()!=runID){
      prevRunTime = clock()*1.0/CLOCKS_PER_SEC;
      runID++;
    }
    
    // Calculate the rate [particles tracked / s] and the estimated
    // time to completion of the present run [m,s]
    runTime = clock()*1.0/CLOCKS_PER_SEC - prevRunTime;
    eventsPerSec = event*1.0/runTime;  // [s]
    timeToFinish = (totalEventsToRun-event)/eventsPerSec; // [s]
    
    // Output the event variables in scientific notation using
    // std::stringstreams to avoid screwing up G4cout formatting
    std::stringstream eventSS;
    eventSS.precision(3);
    eventSS << std::scientific << (double)event;

    std::stringstream tEventSS;
    tEventSS.precision(3);
    tEventSS << std::scientific << totalEventsToRun;

    G4cout << "\r**  Event [" << eventSS.str() << "/" << tEventSS.str() << "]    "
	   << std::setprecision(4)
	   << "Rate [" << eventsPerSec << "]    " 
	   << std::setprecision(2)
	   << "Time2Finish [" 
	   << ((int)timeToFinish)/3600  << "h " 
	   << ((int)timeToFinish%3600)/60 << "m " 
	   << ((int)timeToFinish%3600)%60 << "s]" 
	   << std::setprecision(6) << std::flush;
  }
}


void eventAction::EndOfEventAction(const G4Event *currentEvent)
{
  if( currentEvent->GetHCofThisEvent() )
    ASIMReadoutManager::GetInstance()->ReadoutEvent(currentEvent);
}
