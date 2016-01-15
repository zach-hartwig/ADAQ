#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

#include "ASIMReadoutManager.hh"
#include "ASIMReadoutMessenger.hh"


ASIMReadoutMessenger::ASIMReadoutMessenger(ASIMReadoutManager *ARMgr)
  : theManager(ARMgr)
{
  //////////////////////
  // ASIM directories //
  //////////////////////
  
  asimDir = new G4UIdirectory("/ASIM/");
  asimDir->SetGuidance("Settings to control ASIM data storage");

  asimFileDir = new G4UIdirectory("/ASIM/file/");
  asimFileDir->SetGuidance("Settings to control ASIM files");

  asimControlDir = new G4UIdirectory("/ASIM/control/");
  asimControlDir->SetGuidance("Settings to control ASIM readout behavior");

  asimReadoutDir = new G4UIdirectory("/ASIM/readout/");
  asimReadoutDir->SetGuidance("Settings to control individual ASIM readouts");

  asimArrayDir = new G4UIdirectory("/ASIM/array/");
  asimArrayDir->SetGuidance("Settings to control ASIM array readout behavior");

  
  ///////////////////
  // File aommands //
  ///////////////////

  asimFileNameCmd = new G4UIcmdWithAString("/ASIM/file/setName",this);
  asimFileNameCmd->SetGuidance("Set the name of the ASIM file. The name of the file should end in the");
  asimFileNameCmd->SetGuidance("the '.asim.root' extension to be properly recognized.");
  asimFileNameCmd->SetParameterName("Choice",false);
  
  asimInitCmd = new G4UIcmdWithoutParameter("/ASIM/file/init",this);
  asimInitCmd->SetGuidance("Initialize the ASIM file in preparations for receiving readout data.");
  asimInitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  asimWriteCmd = new G4UIcmdWithoutParameter("/ASIM/file/write",this);
  asimWriteCmd->SetGuidance("Write the ASIM file containing all readout data to disk");
  asimWriteCmd->AvailableForStates(G4State_PreInit, G4State_Idle);


  //////////////////////
  // Control commands //
  //////////////////////


  //////////////////////
  // Readout commands //
  //////////////////////

  // Command to select the readout as the 'active' readout that will
  // have its values set by use of following commands
  
  selectReadoutCmd = new G4UIcmdWithAnInteger("/ASIM/readout/selectReadout", this);
  selectReadoutCmd->SetGuidance("Set the readout for which settings will be modified. Use the readout ID to specify.");
  selectReadoutCmd->SetParameterName("Choice", false);
  selectReadoutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Enable/disable the readout
  
  setReadoutEnabledCmd = new G4UIcmdWithABool("/ASIM/readout/setReadoutEnabled", this);
  setReadoutEnabledCmd->SetGuidance("Enable/disable the presently active readout.");
  setReadoutEnabledCmd->SetParameterName("Choice", false);
  setReadoutEnabledCmd->SetDefaultValue(false);

  // Energy broadening
  
  setEnergyBroadeningCmd = new G4UIcmdWithABool("/ASIM/readout/setEnergyBroadening", this);
  setEnergyBroadeningCmd->SetGuidance("Enable the ability to broadening the value of energy deposited by a Gaussian function");
  setEnergyBroadeningCmd->SetGuidance("in order to more realistically mimick the detector's response function without the need");
  setEnergyBroadeningCmd->SetGuidance("utilize scintillation photon generation/detection.");
  setEnergyBroadeningCmd->SetParameterName("Choice", false);
  setEnergyBroadeningCmd->SetDefaultValue(false);

  setEnergyResolutionCmd = new G4UIcmdWithADouble("/ASIM/readout/setEnergyResolution", this);
  setEnergyResolutionCmd->SetGuidance("Set the energy resolution as a percent [%] to be used with the energy");
  setEnergyResolutionCmd->SetGuidance("gaussoam energy broading command, which must be first enabled via the");
  setEnergyResolutionCmd->SetGuidance("/ASIM/readout/setEnergyBroadening' command");
  setEnergyResolutionCmd->SetParameterName("Choice",false);
  
  setEnergyEvaluationCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/readout/setEnergyEvaluation", this);
  setEnergyEvaluationCmd->SetGuidance("Set the energy at which the desired energy resolution will be computed.");
  setEnergyEvaluationCmd->SetGuidance("enabled via the '/ASIM/readout/setEnergyBroadening' command");
  setEnergyEvaluationCmd->SetParameterName("Choice",false);
  setEnergyEvaluationCmd->SetUnitCategory("Energy");
  setEnergyEvaluationCmd->SetDefaultUnit("MeV");

  // Energy threshold
  
  setLowerEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/readout/setLowerEnergyThreshold", this);
  setLowerEnergyThresholdCmd->SetGuidance("Set the lower energy threshold for scoring hits on the detector. Note that energy");
  setLowerEnergyThresholdCmd->SetGuidance("energy thresholding must be enabled via the '/ASIM/readout/enableEnergyThrehold' command");
  setLowerEnergyThresholdCmd->SetParameterName("Choice",false);
  setLowerEnergyThresholdCmd->SetUnitCategory("Energy");
  setLowerEnergyThresholdCmd->SetDefaultUnit("MeV");

  setUpperEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/readout/setUpperEnergyThreshold", this);
  setUpperEnergyThresholdCmd->SetGuidance("Set the upper energy threshold for scoring hits on the detector. Note that energy");
  setUpperEnergyThresholdCmd->SetGuidance("energy tresholding must be enabled via the '/ASIM/readout/enableEnergyThrehold' command");
  setUpperEnergyThresholdCmd->SetParameterName("Choice",false);
  setUpperEnergyThresholdCmd->SetUnitCategory("Energy");
  setUpperEnergyThresholdCmd->SetDefaultUnit("MeV");

  enableEnergyThresholdCmd = new G4UIcmdWithoutParameter("/ASIM/readout/enableEnergyThreshold", this);
  enableEnergyThresholdCmd->SetGuidance("Enable the minimum energy threshold for scoring hits on the detector. The energy");
  enableEnergyThresholdCmd->SetGuidance("may be set via '/ASIM/readout/set{Lower,Upper}EnergyThreshold' command. Note that energy");
  enableEnergyThresholdCmd->SetGuidance("and photon thresholding are mutually exclusive. Setting this will undo the other!");

  // Photon threshold

  enablePhotonThresholdCmd = new G4UIcmdWithoutParameter("/ASIM/readout/enablePhotonThreshold", this);
  enablePhotonThresholdCmd->SetGuidance("Enable the lower/upper photon thresholds for scoring hits on the detector. The photon");
  enablePhotonThresholdCmd->SetGuidance("thresholds may be set via '/ASIM/readout/set{Lower,Upper}EnergyThreshold' command.");
  enablePhotonThresholdCmd->SetGuidance("Note that energy and photon thresholding are mutually exclusive with energy thresholding.");
  enablePhotonThresholdCmd->SetGuidance("Setting this will undor the other!");

  setLowerPhotonThresholdCmd = new G4UIcmdWithAnInteger("/ASIM/readout/setLowerPhotonThreshold", this);
  setLowerPhotonThresholdCmd->SetGuidance("Set the lower photon threshold for scoring hits on the detector. Note that photon");
  setLowerPhotonThresholdCmd->SetGuidance("thresholding must be enabled via the '/ASIM/readout/enablePhotonThrehold' command");
  setLowerPhotonThresholdCmd->SetParameterName("Choice",false);

  setUpperPhotonThresholdCmd = new G4UIcmdWithAnInteger("/ASIM/readout/setUpperPhotonThreshold", this);
  setUpperPhotonThresholdCmd->SetGuidance("Set the upper photon threshold for scoring hits on the detector. Note that photon");
  setUpperPhotonThresholdCmd->SetGuidance("tresholding must be enabled via the '/ASIM/readout/enablePhotonThrehold' command");
  setUpperPhotonThresholdCmd->SetParameterName("Choice",false);

  // Enable/disable readout of full waveforms
  
  setWaveformStorageCmd = new G4UIcmdWithABool("/ASIM/readout/setWaveformStorage", this);
  setWaveformStorageCmd->SetGuidance("Enable/disable the storage of individual waveforms (e.g. a vector of optical photon");
  setWaveformStorageCmd->SetGuidance("creation/detection times) on disk. Warning: when setd ASIM file sizes on disk can");
  setWaveformStorageCmd->SetGuidance("quickly become very large");
  setWaveformStorageCmd->SetParameterName("Choice", false);
  setWaveformStorageCmd->SetDefaultValue(false);

    // Enable/disable the readout

  selectArrayCmd = new G4UIcmdWithAnInteger("/ASIM/array/select", this);
  selectArrayCmd->SetGuidance("Set the array for which settings will be modified. Use the array ID to specify.");
  selectArrayCmd->SetParameterName("Choice", false);
  selectArrayCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  setArrayEnabledCmd = new G4UIcmdWithABool("/ASIM/array/setEnabled", this);
  setArrayEnabledCmd->SetGuidance("Enable/disable the presently active readout.");
  setArrayEnabledCmd->SetParameterName("Choice", false);
  setArrayEnabledCmd->SetDefaultValue(false);

  enableArrayEnergyThresholdCmd = new G4UIcmdWithoutParameter("/ASIM/array/enableEnergyThreshold", this);
  enableArrayEnergyThresholdCmd->SetGuidance("Enable the minimum energy threshold for scoring hits on the array. The energy");
  enableArrayEnergyThresholdCmd->SetGuidance("may be set via '/ASIM/array/set{Lower,Upper}EnergyThreshold' command. Note that energy");
  enableArrayEnergyThresholdCmd->SetGuidance("and photon thresholding are mutually exclusive. Setting this will undo the other!");
  
  setArrayLowerEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/array/setLowerEnergyThreshold", this);
  setArrayLowerEnergyThresholdCmd->SetGuidance("Set the lower energy threshold for scoring hits on the array. Note that energy");
  setArrayLowerEnergyThresholdCmd->SetGuidance("energy thresholding must be enabled via the '/ASIM/array/enableEnergyThrehold' command");
  setArrayLowerEnergyThresholdCmd->SetParameterName("Choice",false);
  setArrayLowerEnergyThresholdCmd->SetUnitCategory("Energy");
  setArrayLowerEnergyThresholdCmd->SetDefaultUnit("MeV");

  setArrayUpperEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/array/setUpperEnergyThreshold", this);
  setArrayUpperEnergyThresholdCmd->SetGuidance("Set the upper energy threshold for scoring hits on the array. Note that energy");
  setArrayUpperEnergyThresholdCmd->SetGuidance("energy tresholding must be enabled via the '/ASIM/array/enableEnergyThrehold' command");
  setArrayUpperEnergyThresholdCmd->SetParameterName("Choice",false);
  setArrayUpperEnergyThresholdCmd->SetUnitCategory("Energy");
  setArrayUpperEnergyThresholdCmd->SetDefaultUnit("MeV");
}


ASIMReadoutMessenger::~ASIMReadoutMessenger()
{
  delete setArrayUpperEnergyThresholdCmd;
  delete setArrayLowerEnergyThresholdCmd;
  delete setArrayEnabledCmd;
  delete selectArrayCmd;
  
  delete setWaveformStorageCmd;
  delete enablePhotonThresholdCmd;
  delete setUpperPhotonThresholdCmd;
  delete setLowerPhotonThresholdCmd;
  delete enableEnergyThresholdCmd;
  delete setUpperEnergyThresholdCmd;
  delete setLowerEnergyThresholdCmd;
  delete setEnergyBroadeningCmd;
  delete setEnergyEvaluationCmd;
  delete setEnergyResolutionCmd;
  delete setReadoutEnabledCmd;
  delete selectReadoutCmd;

  delete asimWriteCmd;
  delete asimInitCmd;
  delete asimFileNameCmd;

  delete asimReadoutDir;
  delete asimControlDir;
  delete asimFileDir;
  delete asimDir;
}


void ASIMReadoutMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  ///////////////////
  // File commands //
  ///////////////////
  
  if(cmd == asimFileNameCmd)
    theManager->SetFileName(newValue);

  if(cmd == asimInitCmd)
    theManager->InitializeASIMFile();
  
  if(cmd == asimWriteCmd)
    theManager->WriteASIMFile();


  //////////////////////
  // Readout commands //
  //////////////////////

  // Select the active readout via readout ID

  if(cmd == selectReadoutCmd)
    theManager->SelectReadout(selectReadoutCmd->GetNewIntValue(newValue));
  
  // Enable/disable the readout
  
  if(cmd == setReadoutEnabledCmd)
    theManager->SetReadoutEnabled(setReadoutEnabledCmd->GetNewBoolValue(newValue));
  
  // Energy broadening 

  if(cmd == setEnergyBroadeningCmd)
    theManager->SetEnergyBroadening(setEnergyBroadeningCmd->GetNewBoolValue(newValue));
  
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

  // Waveform storage
  
  if(cmd == setWaveformStorageCmd)
    theManager->SetWaveformStorage(setWaveformStorageCmd->GetNewBoolValue(newValue));

  
  ////////////////////
  // Array commands //
  ////////////////////

  if(cmd == selectArrayCmd)
    theManager->SelectArray(selectArrayCmd->GetNewIntValue(newValue));

  if(cmd == setArrayEnabledCmd)
    theManager->SetArrayEnabled(selectArrayCmd->GetNewBoolValue(newValue));

  if(cmd == enableArrayEnergyThresholdCmd)
    theManager->EnableArrayEnergyThresholds();

  if(cmd == setArrayLowerEnergyThreshold)
    theManager->SetArrayLowerEnergyThreshold(setArrayLowerEnergyThreshold->GetNewDoubleValue(newValue));
  
  if(cmd == setArrayUpperEnergyThreshold)
    theManager->SetArrayUpperEnergyThreshold(setArrayUpperEnergyThreshold->GetNewDoubleValue(newValue));
}
