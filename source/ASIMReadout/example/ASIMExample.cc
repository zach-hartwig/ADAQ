/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMExample.cc
// date: 22 Sep 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc:
//
/////////////////////////////////////////////////////////////////////////////////

#include "G4RunManager.hh" 
#include "G4VisManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#include "G4TrajectoryDrawByParticleID.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <ctime>

#include "geometryConstruction.hh"
#include "physicsList.hh"
#include "PGA.hh"
#include "steppingAction.hh"
#include "stackingAction.hh"
#include "eventAction.hh"
#include "runAction.hh"


int main(int argc, char *argv[])
{
  G4bool visualization = false;
  G4bool visQt = false;
  if(argc>1){
    std::string arg1 = argv[1];
    if(arg1=="visOn")
      visualization = true;
    if(arg1=="visQt"){
      visualization = true;
      visQt = true;
    }
  }
  
  if(argc>2){
    std::string arg2 = argv[2];
    if(arg2=="seed")
      CLHEP::HepRandom::setTheSeed(time(0));
  }

  G4RunManager* theRunManager = new G4RunManager;
  
  theRunManager->SetUserInitialization(new geometryConstruction(visualization));
  theRunManager->SetUserInitialization(new physicsList(false, false));
  theRunManager->SetUserAction(new PGA);
  theRunManager->Initialize();
  
  runAction *RunAction = new runAction();
  theRunManager->SetUserAction(RunAction);
  theRunManager->SetUserAction(new steppingAction);
  theRunManager->SetUserAction(new stackingAction(RunAction));
  theRunManager->SetUserAction(new eventAction);
  
  G4UImanager* UI = G4UImanager::GetUIpointer();
  {
    G4String Alias = "/control/alias";
    G4String Exec = "/control/execute";

    UI->ApplyCommand(Alias + " mac " + Exec + " runtime/ASIMExample.mac");

    UI->ApplyCommand("/control/verbose 1");    
    UI->ApplyCommand("/run/verbose 0");
    UI->ApplyCommand("/event/verbose 0");
    UI->ApplyCommand("/hits/verbose 0");
    UI->ApplyCommand("/tracking/verbose 0");
    
    UI->ApplyCommand("/control/execute runtime/ASIMExample.gps");
  }
  
  G4VisManager *visManager = NULL;
  G4TrajectoryDrawByParticleID *colorModel = NULL;
  if(visualization){
    visManager = new G4VisExecutive;
    visManager->Initialize();
    
    // Color particle by particle type
    colorModel = new G4TrajectoryDrawByParticleID;
    colorModel->Set("deuteron", "yellow");
    colorModel->Set("neutron", "cyan");
    colorModel->Set("gamma", "green");
    colorModel->Set("e-", "red");
    colorModel->Set("e+", "blue");
    colorModel->Set("opticalphoton","magenta");
    colorModel->SetDefault("gray");
    visManager->RegisterModel(colorModel);
    visManager->SelectTrajectoryModel(colorModel->Name());
    
    if(visQt){
      G4UIExecutive *theUIExecutive = new G4UIExecutive(argc, argv, "Qt");

      UI->ApplyCommand("/vis/open OGL");
      UI->ApplyCommand("/control/execute runtime/ASIMExample.vis");

      theUIExecutive->SessionStart();
      delete theUIExecutive;
    }
    else{
      UI->ApplyCommand("/vis/open OGLIX");
      UI->ApplyCommand("/control/execute runtime/ASIMExample.vis");
    }
  }

  if(!visQt){
    // Create a decent 'tcsh'-like prompt for tab completion, command
    // history, etc.  Also, style points for cooler prompt
    G4String prompt = "ASIMExample >> ";
    G4int histories = 200;
    G4UItcsh *shell = new G4UItcsh(prompt, histories);
    G4UIsession* session = new G4UIterminal(shell);
    
    // As Gallagher famously said: "STYYYYYYYYYYYYYLE!!"
    G4cout << "\n\n \t ******************************************************************\n"
	   <<     " \t ****                                                          ****\n"
	   <<     " \t **             Welcome to the ASIMExample simulation            **\n\n";
    
    session->SessionStart();

    ////////////////////////
    // Garbage collection //
    ////////////////////////
    
    delete session;
  }
  
  if(visualization){
    delete colorModel;
  }
  
  //if(theRSManager->CheckForOpenASIMFile())
  //theRSManager->WriteASIMFile(true);
  //  delete theRSManager;

  delete theRunManager;

  G4cout << G4endl;
  
  return 0;
}
