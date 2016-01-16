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
  
  selectReadoutCmd = new G4UIcmdWithAnInteger("/ASIM/readout/select", this);
  selectReadoutCmd->SetGuidance("Selectthe readout for which settings will be applied. Use the readout ID to specify.");
  selectReadoutCmd->SetParameterName("Choice", false);
  selectReadoutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Enable/disable the readout
  
  setReadoutEnabledCmd = new G4UIcmdWithABool("/ASIM/readout/setReadoutEnabled", this);
  setReadoutEnabledCmd->SetGuidance("Enable ('true') / disable ('false') the selected readout.");
  setReadoutEnabledCmd->SetParameterName("Choice", false);
  setReadoutEnabledCmd->SetDefaultValue(false);

  // Energy broadening
  
  setEnergyBroadeningCmd = new G4UIcmdWithABool("/ASIM/readout/setEnergyBroadening", this);
  setEnergyBroadeningCmd->SetGuidance("Enable gaussian broadening of energy deposition in the selected readout");
  setEnergyBroadeningCmd->SetParameterName("Choice", false);
  setEnergyBroadeningCmd->SetDefaultValue(false);

  setEnergyResolutionCmd = new G4UIcmdWithADouble("/ASIM/readout/setEnergyResolution", this);
  setEnergyResolutionCmd->SetGuidance("Set the resolution as a percent [%] to use in energy broadening for the");
  setEnergyResolutionCmd->SetGuidance("selected readout. Resolution is defined as FWHM / Mean for a gaussian.");
  setEnergyResolutionCmd->SetParameterName("Choice",false);
  
  setEnergyEvaluationCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/readout/setEnergyEvaluation", this);
  setEnergyEvaluationCmd->SetGuidance("Set the energy at which the energy resolution will be computed for the");
  setEnergyEvaluationCmd->SetGuidance("selected readout.");
  setEnergyEvaluationCmd->SetParameterName("Choice",false);
  setEnergyEvaluationCmd->SetUnitCategory("Energy");
  setEnergyEvaluationCmd->SetDefaultUnit("MeV");

  // Thresholds
  
  setThresholdTypeCmd = new G4UIcmdWithAString("/ASIM/readout/setThresholdType",this);
  setThresholdTypeCmd->SetGuidance("Set the selected readout threshold type: 'energy' uses energy deposition in the readout volume;");
  setThresholdTypeCmd->SetGuidance("'photon' uses total number of optical photons detected by the photodetector.");
  setThresholdTypeCmd->SetParameterName("Choice",false);
  setThresholdTypeCmd->SetCandidates("energy photon");
  
  setLowerEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/readout/setLowerEnergyThreshold", this);
  setLowerEnergyThresholdCmd->SetGuidance("Set the lower threshold for the selected readout triggering when using energy thresholds");
  setLowerEnergyThresholdCmd->SetParameterName("Choice",false);
  setLowerEnergyThresholdCmd->SetUnitCategory("Energy");
  setLowerEnergyThresholdCmd->SetDefaultUnit("MeV");

  setUpperEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/ASIM/readout/setUpperEnergyThreshold", this);
  setUpperEnergyThresholdCmd->SetGuidance("Set the upper threshold for the selected readout triggering when using energy thresholds");
  setUpperEnergyThresholdCmd->SetParameterName("Choice",false);
  setUpperEnergyThresholdCmd->SetUnitCategory("Energy");
  setUpperEnergyThresholdCmd->SetDefaultUnit("MeV");

  setLowerPhotonThresholdCmd = new G4UIcmdWithAnInteger("/ASIM/readout/setLowerPhotonThreshold", this);
  setLowerPhotonThresholdCmd->SetGuidance("Set the lower threshold for the selected readout triggering when using photon thresholds");
  setLowerPhotonThresholdCmd->SetParameterName("Choice",false);
  
  setUpperPhotonThresholdCmd = new G4UIcmdWithAnInteger("/ASIM/readout/setUpperPhotonThreshold", this);
  setUpperPhotonThresholdCmd->SetGuidance("Set the upper threshold for the selected readout triggering when using photon thresholds");
  setUpperPhotonThresholdCmd->SetParameterName("Choice",false);
  
  // Enable/disable readout of full waveforms
  
  setWaveformStorageCmd = new G4UIcmdWithABool("/ASIM/readout/setWaveformStorage", this);
  setWaveformStorageCmd->SetGuidance("Enable/disable the storage of individual 'waveforms' (vector of optical photon creation/detection");
  setWaveformStorageCmd->SetGuidance("times) for the selected readout. Warning: ASIM file sizes on disk can quickly become very large!");
  setWaveformStorageCmd->SetParameterName("Choice", false);
  setWaveformStorageCmd->SetDefaultValue(false);

    // Enable/disable the readout

  selectArrayCmd = new G4UIcmdWithAnInteger("/ASIM/array/select", this);
  selectArrayCmd->SetGuidance("Select teh array for which settings will be applied. Use the array ID to specify.");
  selectArrayCmd->SetParameterName("Choice", false);
  selectArrayCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  setArrayEnabledCmd = new G4UIcmdWithABool("/ASIM/array/setEnabled", this);
  setArrayEnabledCmd->SetGuidance("Enable ('true') / disable ('false') the presently selected array.");
  setArrayEnabledCmd->SetParameterName("Choice", false);
  setArrayEnabledCmd->SetDefaultValue(false);

  setArrayThresholdTypeCmd = new G4UIcmdWithAString("/ASIM/array/setThresholdType",this);
  setArrayThresholdTypeCmd->SetGuidance("Set the selected array threshold type: 'energy' uses energy deposition in the readout volume;");
  setArrayThresholdTypeCmd->SetGuidance("'photon' uses total number of optical photons detected by the photodetector.");
  setArrayThresholdTypeCmd->SetParameterName("Choice",false);
  setArrayThresholdTypeCmd->SetCandidates("energy photon");

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
  delete setArrayThresholdTypeCmd;
  delete setArrayEnabledCmd;
  delete selectArrayCmd;
  
  delete setWaveformStorageCmd;
  delete setUpperPhotonThresholdCmd;
  delete setLowerPhotonThresholdCmd;
  delete setUpperEnergyThresholdCmd;
  delete setLowerEnergyThresholdCmd;
  delete setThresholdTypeCmd;
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

  // Thresholds

  if(cmd == setThresholdTypeCmd)
    theManager->SetThresholdType(newValue);

  if(cmd == setLowerEnergyThresholdCmd)
    theManager->SetLowerEnergyThreshold(setLowerEnergyThresholdCmd->GetNewDoubleValue(newValue));
  
  if(cmd == setUpperEnergyThresholdCmd)
    theManager->SetUpperEnergyThreshold(setUpperEnergyThresholdCmd->GetNewDoubleValue(newValue));

  if(cmd == setUpperPhotonThresholdCmd)
    theManager->SetUpperPhotonThreshold(setUpperPhotonThresholdCmd->GetNewIntValue(newValue));
  
  // Waveform storage
  
  if(cmd == setWaveformStorageCmd)
    theManager->SetWaveformStorage(setWaveformStorageCmd->GetNewBoolValue(newValue));

  
  ////////////////////
  // Array commands //
  ////////////////////

  if(cmd == selectArrayCmd)
    theManager->SelectArray(selectArrayCmd->GetNewIntValue(newValue));
  
  if(cmd == setArrayEnabledCmd)
    theManager->SetArrayEnabled(setArrayEnabledCmd->GetNewBoolValue(newValue));
  
  if(cmd == setArrayThresholdTypeCmd)
    theManager->SetArrayThresholdType(newValue);
  
  if(cmd == setArrayLowerEnergyThresholdCmd)
    theManager->SetArrayLowerEnergyThreshold(setArrayLowerEnergyThresholdCmd->GetNewDoubleValue(newValue));
  
  if(cmd == setArrayUpperEnergyThresholdCmd)
    theManager->SetArrayUpperEnergyThreshold(setArrayUpperEnergyThresholdCmd->GetNewDoubleValue(newValue));
}
