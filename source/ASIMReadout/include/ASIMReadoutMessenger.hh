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
  
  G4UIdirectory *asimDirectory;
  
  G4UIcmdWithAString *asimFileNameCmd;
  G4UIcmdWithoutParameter *asimInitCmd;
  G4UIcmdWithoutParameter *asimWriteCmd;

  G4UIcmdWithAnInteger *setActiveReadoutCmd;
  
  G4UIcmdWithADoubleAndUnit *setLowerEnergyThresholdCmd;
  G4UIcmdWithADoubleAndUnit *setUpperEnergyThresholdCmd;
  G4UIcmdWithoutParameter *enableEnergyThresholdCmd;

  G4UIcmdWithAnInteger *setLowerPhotonThresholdCmd;
  G4UIcmdWithAnInteger *setUpperPhotonThresholdCmd;
  G4UIcmdWithoutParameter *enablePhotonThresholdCmd;

  G4UIcmdWithABool *setPSDStatusCmd;
  G4UIcmdWithAString *setPSDParticleCmd;
  
  G4UIcmdWithADouble *setEnergyResolutionCmd;
  G4UIcmdWithADoubleAndUnit *setEnergyEvaluationCmd;
  G4UIcmdWithABool *setEnergyBroadeningCmd;
};

#endif
