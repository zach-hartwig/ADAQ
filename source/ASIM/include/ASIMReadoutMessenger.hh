#ifndef ASIMReadoutMessenger_hh
#define ASIMReadoutMessenger_hh 1

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class ASIMReadoutManager;

class ASIMReadoutMessenger : public G4UImessenger
{
  
public:
  ASIMReadoutMessenger(ASIMReadoutManager *);
  ~ASIMReadoutMessenger();
  
  void SetNewValue(G4UIcommand *, G4String);
  
private:
  ASIMReadoutManager *theManager;
  
  G4UIdirectory *asimDir;
  G4UIdirectory *asimFileDir;
  G4UIdirectory *asimControlDir;
  G4UIdirectory *asimReadoutDir;
  G4UIdirectory *asimArrayDir;

  // File commands

  G4UIcmdWithAString *asimFileNameCmd;
  G4UIcmdWithoutParameter *asimInitCmd;
  G4UIcmdWithoutParameter *asimWriteCmd;

  // Readout commands
  
  G4UIcmdWithAnInteger *selectReadoutCmd;
  G4UIcmdWithABool *setReadoutEnabledCmd;
  G4UIcmdWithABool *setEnergyBroadeningCmd;
  G4UIcmdWithADouble *setEnergyResolutionCmd;
  G4UIcmdWithADoubleAndUnit *setEnergyEvaluationCmd;
  G4UIcmdWithAString *setThresholdTypeCmd;
  G4UIcmdWithADoubleAndUnit *setLowerEnergyThresholdCmd;
  G4UIcmdWithADoubleAndUnit *setUpperEnergyThresholdCmd;
  G4UIcmdWithAnInteger *setLowerPhotonThresholdCmd;
  G4UIcmdWithAnInteger *setUpperPhotonThresholdCmd;
  G4UIcmdWithABool *setWaveformStorageCmd;

  // Array commands
  G4UIcmdWithAnInteger *selectArrayCmd;
  G4UIcmdWithABool *setArrayEnabledCmd;
  G4UIcmdWithAString *setArrayThresholdTypeCmd;
  G4UIcmdWithADoubleAndUnit *setArrayLowerEnergyThresholdCmd;
  G4UIcmdWithADoubleAndUnit *setArrayUpperEnergyThresholdCmd;
};

#endif
