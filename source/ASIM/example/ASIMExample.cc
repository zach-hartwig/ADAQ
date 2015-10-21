/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMExample.cc
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMExample is a Geant4 simulation that demonstrates the use
//       of the ASIM libraries to handle the readout of detector data
//       and persistent storage to disk. The ASIM libraries provide a
//       simple but powerful way to handle detector simulation and
//       data analysis in Geant4. An unlimited number of scintillator
//       and (optionally) associated photodetector volumes can be
//       specified by the user for readout into a standardized ROOT
//       file known as an ASIM file. The ASIM file can then be
//       analyzed offline, either using the ADAQAnalysis tool or the
//       user's own ROOT scripts.
//
/////////////////////////////////////////////////////////////////////////////////

// Geant4
#include "G4RunManager.hh" 
#include "G4VisManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisExecutive.hh"
#include "G4TrajectoryDrawByParticleID.hh"
#include "G4UIExecutive.hh"

// C++
#include <ctime>

// ASIM
#include <ASIMReadoutManager.hh>
#ifdef MPI_ENABLED
#include "MPIManager.hh"
#endif

// ASIMExample
#include "geometryConstruction.hh"
#include "physicsList.hh"
#include "PGA.hh"
#include "steppingAction.hh"
#include "stackingAction.hh"
#include "eventAction.hh"
#include "runAction.hh"


int main(int argc, char *argv[])
{
  ////////////////////////////////
  // Parse command line options //
  ////////////////////////////////

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

  G4bool UseNeutronHPPhysics = false;
  if(argc>2){
    std::string arg2 = argv[2];
    if(arg2=="hp")
      UseNeutronHPPhysics = true;
  }
 
  G4bool UseOpticalPhysics = false;
  if(argc>3){
    std::string arg3 = argv[3];
    if(arg3=="optical")
      UseOpticalPhysics = true;
  }
  
  if(argc>4){
    std::string arg2 = argv[4];
    if(arg2=="seed")
      CLHEP::HepRandom::setTheSeed(time(0));
  }

  
  ////////////////////////////////////
  // Sequential/parallel processing //
  ////////////////////////////////////
  
  G4bool useParallelProcessing = false;

#ifdef MPI_ENABLED
  useParallelProcessing = true;

  // Declare faux command line arguments to pass to MPI; use static to
  // ensure that only one copy is created to prevent memory issues
  static const G4int argcMPI = 2;
  static char *argvMPI[argcMPI];
  argvMPI[0] = argv[0]; // binary name
  argvMPI[1] = (char *)"slaveForum/Slave"; // slave file base name
  
  MPIManager *theMPIManager= new MPIManager(argcMPI, argvMPI);
#endif


  /////////////////////////////////////////////////
  // Initialize the singleton ASIMStorageManager //
  /////////////////////////////////////////////////

  ASIMReadoutManager *theReadoutManager = new ASIMReadoutManager;
  if(useParallelProcessing)
    theReadoutManager->EnableParallelMode();


  //////////////////////////////////////////////////
  // Initialize mandatory and option user classes //
  //////////////////////////////////////////////////

  G4RunManager* theRunManager = new G4RunManager;
  
  theRunManager->SetUserInitialization(new geometryConstruction);
  theRunManager->SetUserInitialization(new physicsList(UseNeutronHPPhysics,
						       UseOpticalPhysics));
  theRunManager->SetUserAction(new PGA);
  theRunManager->Initialize();
  
  theRunManager->SetUserAction(new runAction);
  theRunManager->SetUserAction(new steppingAction);
  theRunManager->SetUserAction(new stackingAction);
  theRunManager->SetUserAction(new eventAction);

  ///////////////////////////////////
  // Initialize the user interface //
  ///////////////////////////////////
 
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

  /////////////////////////
  // Parallel processing //
  /////////////////////////

  if(useParallelProcessing){

    G4String macroCmd = "/control/execute runtime/ASIMExample.mpi.mac";
    UI->ApplyCommand(macroCmd);

#ifdef MPI_ENABLED    
    delete theMPIManager;
#endif
    
  }
  
  
  ///////////////////////////
  // Sequential processing //
  ///////////////////////////
  else{

    ///////////////////////////////////////////////////////////////
    // Optionally initialize standard OpenGL or Qt visualziation //
    ///////////////////////////////////////////////////////////////

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
	     <<     " \t ******                                                      ******\n"
	     <<     " \t ****           Welcome to the ASIMExample simulation          ****\n"
	     <<     " \t **                                                              **\n"
	     <<     " \t ******************************************************************\n"
	     << G4endl;
    
      session->SessionStart();
    

      ////////////////////////
      // Garbage collection //
      ////////////////////////
      
      delete session;
    }
  
    if(visualization){
      delete colorModel;
    }
  }
  
  if(theReadoutManager->CheckForOpenASIMFile())
    theReadoutManager->WriteASIMFile(true);
  delete theReadoutManager;

  delete theRunManager;

  G4cout << G4endl;
  
  return 0;
}
