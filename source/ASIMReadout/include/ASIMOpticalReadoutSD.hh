/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMOpticalReadoutSD.hh
// date: 21 May 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMOpticalReadoutSD class is intended as a generic Geant4
//       sensitive detector class that should be attached to
//       scintillator readout volumes, such as PMT photocathodes or
//       SiPM surfaces, to handle optical photon detection and readout
//       into the ASIM file format supported by the ADAQ framework.
//       The class makes use of the complementary class
//       ASIMOpticalReadoutSDHit.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef ASIMOpticalReadoutSD_hh
#define ASIMOpticalReadoutSD_hh 1

#include "G4VSensitiveDetector.hh"

#include "ASIMOpticalReadoutSDHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class ASIMOpticalReadoutSD : public G4VSensitiveDetector
{
  
public:
  ASIMOpticalReadoutSD(G4String name);
  ~ASIMOpticalReadoutSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  G4bool ManualTrigger(const G4Step *, G4TouchableHistory *);
  G4bool ProcessHits_constStep(const G4Step*,G4TouchableHistory*);
  
  void EndOfEvent(G4HCofThisEvent*);
  
  // Used by other classes to obtain member data if desired
  ASIMOpticalReadoutSDHitCollection *ReturnHitCollection()
  {return hitCollection;}
  
  std::vector<G4String> GetCollectionNameList()
  {return collectionNameList;}
  
  
  // All the desired hit collections.  Each collections will require a
  // name to be pushed into the collectionName vector in the
  // constructor of ASIMOpticalReadoutSD
private:
  ASIMOpticalReadoutSDHitCollection *hitCollection;

public:
  G4double kineticEnergy;
  G4ThreeVector position;
  
  std::vector<G4String> collectionNameList;
};




#endif

