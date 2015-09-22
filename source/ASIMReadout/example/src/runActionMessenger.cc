#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "runAction.hh"
#include "runActionMessenger.hh"


runActionMessenger::runActionMessenger(runAction *RA)
  : RunAction(RA)
{;}


runActionMessenger::~runActionMessenger()
{;}


void runActionMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{;}
