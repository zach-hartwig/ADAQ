#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

#include "ASIMReadoutManager.hh"
#include "ASIMReadoutMessenger.hh"


ASIMReadoutMessenger::ASIMReadoutMessenger(ASIMReadoutManager *ARM)
  : theManager(ARM)
{
  asimDirectory = new G4UIdirectory("/ASIM/");
  asimDirectory->SetGuidance("Settings for SWS I/O via persistent data storage in ASIM files");

  asimFileNameCmd = new G4UIcmdWithAString("/ASIM/setFileName",this);
  asimFileNameCmd->SetGuidance("Sets the name of the ROOT file. If a name is not set manually, the");
  asimFileNameCmd->SetGuidance("default file name will be 'ACRONYM_<date>_<time>.root'.");
  asimFileNameCmd->SetParameterName("Choice",false);
  
  asimInitCmd = new G4UIcmdWithoutParameter("/ASIM/init",this);
  asimInitCmd->SetGuidance("Create and initialize the required ASIM objects for persistent storage.");
  asimInitCmd->SetGuidance("Note that this cmd may only be used when existing ASIM objects have been");
  asimInitCmd->SetGuidance("written to disk.");
  asimInitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  asimWriteCmd = new G4UIcmdWithoutParameter("/ASIM/write",this);
  asimWriteCmd->SetGuidance("Writes all ASIM readout objects to disk. Note that this cmd may only be used if");
  asimWriteCmd->SetGuidance("the ASIM objects have already been created and initialized.");
  asimWriteCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  setActiveReadoutCmd = new G4UIcmdWithAnInteger("/ASIM/setActiveReadout", this);
  setActiveReadoutCmd->SetGuidance("Set the readout for which settings will be modified. Use the readout ID to specify.");
  setActiveReadoutCmd->SetParameterName("Choice", false);
  setActiveReadoutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  setEnergyResolutionCmd = new G4UIcmdWithADouble("/ASIM/setEnergyResolution", this);
  setEnergyResolutionCmd->SetGuidance("Set the energy resolution as a percent [%] to be used with the energy");
  setEnergyResolutionCmd->SetGuidance("gaussoam energy broading command, which must be first enabled via the");
  setEnergyResolutionCmd->SetGuidance("/ASIM/enableEnergyBroadening' command");
  setEnergyResolutionCmd->SetParameterName("Choice",false);
  
  setEnergyEvaluationCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/setEnergyEvaluation", this);
  setEnergyEvaluationCmd->SetGuidance("Set the energy at which the desired energy resolution will be computed.");
  setEnergyEvaluationCmd->SetGuidance("enabled via the '/ASIM/enableEnergyBroadening' command");
  setEnergyEvaluationCmd->SetParameterName("Choice",false);
  setEnergyEvaluationCmd->SetUnitCategory("Energy");
  setEnergyEvaluationCmd->SetDefaultUnit("MeV");
  
  setEnergyBroadeningCmd = new G4UIcmdWithABool("/ASIM/setEnergyBroadening", this);
  setEnergyBroadeningCmd->SetGuidance("Enables the ability to broadening the value of energy deposited by a Gaussian function");
  setEnergyBroadeningCmd->SetGuidance("in order to more realistically mimick the detector's response function without the need");
  setEnergyBroadeningCmd->SetGuidance("utilize scintillation photon generation/detection.");
  setEnergyBroadeningCmd->SetParameterName("Choice", false);
  setEnergyBroadeningCmd->SetDefaultValue(false);

  setLowerEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/setLowerEnergyThreshold", this);
  setLowerEnergyThresholdCmd->SetGuidance("Set the lower energy threshold for scoring hits on the detector. Note that energy");
  setLowerEnergyThresholdCmd->SetGuidance("energy thresholding must be enabled via the '/ASIM/enableEnergyThrehold' command");
  setLowerEnergyThresholdCmd->SetParameterName("Choice",false);
  setLowerEnergyThresholdCmd->SetUnitCategory("Energy");
  setLowerEnergyThresholdCmd->SetDefaultUnit("MeV");

  setUpperEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/setUpperEnergyThreshold", this);
  setUpperEnergyThresholdCmd->SetGuidance("Set the upper energy threshold for scoring hits on the detector. Note that energy");
  setUpperEnergyThresholdCmd->SetGuidance("energy tresholding must be enabled via the '/ASIM/enableEnergyThrehold' command");
  setUpperEnergyThresholdCmd->SetParameterName("Choice",false);
  setUpperEnergyThresholdCmd->SetUnitCategory("Energy");
  setUpperEnergyThresholdCmd->SetDefaultUnit("MeV");

  enableEnergyThresholdCmd = new G4UIcmdWithoutParameter("/ASIM/enableEnergyThreshold", this);
  enableEnergyThresholdCmd->SetGuidance("Enables the minimum energy threshold for scoring hits on the detector. The energy");
  enableEnergyThresholdCmd->SetGuidance("may be set via '/ASIM/set{Lower,Upper}EnergyThreshold' command. Note that energy");
  enableEnergyThresholdCmd->SetGuidance("and photon thresholding are mutually exclusive. Setting this will undo the other!");

  setLowerPhotonThresholdCmd = new G4UIcmdWithAnInteger("/ASIM/setLowerPhotonThreshold", this);
  setLowerPhotonThresholdCmd->SetGuidance("Set the lower photon threshold for scoring hits on the detector. Note that photon");
  setLowerPhotonThresholdCmd->SetGuidance("thresholding must be enabled via the '/ASIM/enablePhotonThrehold' command");
  setLowerPhotonThresholdCmd->SetParameterName("Choice",false);

  setUpperPhotonThresholdCmd = new G4UIcmdWithAnInteger("/ASIM/setUpperPhotonThreshold", this);
  setUpperPhotonThresholdCmd->SetGuidance("Set the upper photon threshold for scoring hits on the detector. Note that photon");
  setUpperPhotonThresholdCmd->SetGuidance("tresholding must be enabled via the '/ASIM/enablePhotonThrehold' command");
  setUpperPhotonThresholdCmd->SetParameterName("Choice",false);

  enablePhotonThresholdCmd = new G4UIcmdWithoutParameter("/ASIM/enablePhotonThreshold", this);
  enablePhotonThresholdCmd->SetGuidance("Enables the lower/upper photon thresholds for scoring hits on the detector. The photon");
  enablePhotonThresholdCmd->SetGuidance("thresholds may be set via '/ASIM/set{Lower,Upper}EnergyThreshold' command.");
  enablePhotonThresholdCmd->SetGuidance("Note that energy and photon thresholding are mutually exclusive with energy thresholding.");
  enablePhotonThresholdCmd->SetGuidance("Setting this will undor the other!");

  /*
  setPSDStatusCmd = new G4UIcmdWithABool("/ASIM/setPSDStatus", this);
  setPSDStatusCmd->SetGuidance("Enable/disable pulse shape discrimination for the detectors. This features");
  setPSDStatusCmd->SetGuidance("allows the user to select the particle type for which data will be accumlated");
  setPSDStatusCmd->SetGuidance("throughout the subsequent runs, including run aggregation statistics and writing");
  setPSDStatusCmd->SetGuidance("data to ROOT files.");
  setPSDStatusCmd->SetParameterName("Choice", false);
  setPSDStatusCmd->SetDefaultValue(false);

  setPSDParticleCmd = new G4UIcmdWithAString("/ASIM/setPSDParticle", this);
  setPSDParticleCmd->SetGuidance("Set the particle type that will be allowed through the pulse shape");
  setPSDParticleCmd->SetGuidance("discrimination filter.");
  setPSDParticleCmd->SetParameterName("Choice",false);
  setPSDParticleCmd->SetCandidates("gamma neutron");
  */

  setWaveformStorageCmd = new G4UIcmdWithABool("/ASIM/setWaveformStorage", this);
  setWaveformStorageCmd->SetGuidance("Enable/disable the storage of individual waveforms (e.g. a vector of optical photon");
  setWaveformStorageCmd->SetGuidance("creation/detection times) on disk. Warning: when enabled ASIM file sizes on disk can");
  setWaveformStorageCmd->SetGuidance("quickly become very large");
  setWaveformStorageCmd->SetParameterName("Choice", false);
  setWaveformStorageCmd->SetDefaultValue(false);
}


ASIMReadoutMessenger::~ASIMReadoutMessenger()
{
  delete setWaveformStorageCmd;
  //delete setPSDParticleCmd;
  //delete setPSDStatusCmd;
  delete enablePhotonThresholdCmd;
  delete setUpperPhotonThresholdCmd;
  delete setLowerPhotonThresholdCmd;
  delete enableEnergyThresholdCmd;
  delete setUpperEnergyThresholdCmd;
  delete setLowerEnergyThresholdCmd;
  delete setEnergyBroadeningCmd;
  delete setEnergyEvaluationCmd;
  delete setEnergyResolutionCmd;
  delete setActiveReadoutCmd;
  delete asimWriteCmd;
  delete asimInitCmd;
  delete asimFileNameCmd;
  delete asimDirectory;
}


void ASIMReadoutMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if(cmd == asimFileNameCmd)
    theManager->SetFileName(newValue);

  if(cmd == asimInitCmd)
    theManager->InitializeASIMFile();
  
  if(cmd == asimWriteCmd)
    theManager->WriteASIMFile();

  // Set the active readout via readout ID

  if(cmd == setActiveReadoutCmd)
    theManager->SetActiveReadout(setActiveReadoutCmd->GetNewIntValue(newValue));

  // Energy broadening 

  if(cmd == setEnergyBroadeningCmd)
    theManager->SetEnergyBroadeningStatus(setEnergyBroadeningCmd->GetNewBoolValue(newValue));
  
  if(cmd == setEnergyResolutionCmd)
    theManager->SetEnergyResolution(setEnergyResolutionCmd->GetNewDoubleValue(newValue));

  if(cmd == setEnergyEvaluationCmd)
    theManager->SetEnergyEvaluation(setEnergyEvaluationCmd->GetNewDoubleValue(newValue));

  // Energy thresholds

  if(cmd == setLowerEnergyThresholdCmd)
    theManager->SetLowerEnergyThreshold(setLowerEnergyThresholdCmd->GetNewDoubleValue(newValue));
  
  if(cmd == setUpperEnergyThresholdCmd)
    theManager->SetUpperEnergyThreshold(setUpperEnergyThresholdCmd->GetNewDoubleValue(newValue));
  
  if(cmd == enableEnergyThresholdCmd)
    theManager->EnableEnergyThresholds();

  // Photon thresholds
  
  if(cmd == setLowerPhotonThresholdCmd)
    theManager->SetLowerPhotonThreshold(setLowerPhotonThresholdCmd->GetNewIntValue(newValue));

  if(cmd == setUpperPhotonThresholdCmd)
    theManager->SetUpperPhotonThreshold(setUpperPhotonThresholdCmd->GetNewIntValue(newValue));
  
  if(cmd == enablePhotonThresholdCmd)
    theManager->EnablePhotonThresholds();

  /*
  if(cmd == setPSDStatusCmd)
    theManager->SetDetectorPSDStatus(setPSDStatusCmd->GetNewBoolValue(newValue));
  
  if(cmd == setPSDParticleCmd)
    theManager->SetDetectorPSDParticle(newValue);
  */

  if(cmd == setWaveformStorageCmd)
    theManager->SetWaveformStorage(setWaveformStorageCmd->GetNewBoolValue(newValue));
}
