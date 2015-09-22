#ifndef eventAction_hh
#define eventAction_hh 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include <fstream>

//#include "rootStorageManager.hh"
#include "eventActionMessenger.hh"


class eventAction : public G4UserEventAction
{
public:
  eventAction();
  ~eventAction();

  void BeginOfEventAction(const G4Event *);
  void EndOfEventAction(const G4Event *);

  // Method to set the frequency that event ID is output to screen
  void SetEventInfoFreq(G4int n) {eventInfoFreq = n;};
  
private:
  G4int eventInfoFreq;

  G4int runID;
  G4double runTime, prevRunTime, eventsPerSec, totalEventsToRun, timeToFinish;

  eventActionMessenger *EventActionMessenger;
};

#endif
