/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMPhotodetectorSD.hh
// date: 21 May 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMPhotodetectorSD class is intended as a generic Geant4
//       sensitive detector class that should be attached to
//       scintillator readout volumes, such as PMT photocathodes or
//       SiPM surfaces, to handle optical photon detection and readout
//       into the ASIM file format supported by the ADAQ framework.
//       The class makes use of the complementary class
//       ASIMPhotodetectorSDHit.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef ASIMPhotodetectorSD_hh
#define ASIMPhotodetectorSD_hh 1

#include "G4VSensitiveDetector.hh"
#include "G4Colour.hh"

#include "ASIMPhotodetectorSDHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class ASIMPhotodetectorSD : public G4VSensitiveDetector
{
  
public:
  ASIMPhotodetectorSD(G4String);
  ~ASIMPhotodetectorSD();
  
  void InitializeCollections(G4String);
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  G4bool ManualTrigger(const G4Step *);
  
  void EndOfEvent(G4HCofThisEvent*);
  
  // Used by other classes to obtain member data if desired
  ASIMPhotodetectorSDHitCollection *ReturnHitCollection()
  {return hitCollection;}
  
  std::vector<G4String> GetCollectionNameList()
  {return collectionNameList;}
  
  void SetHitRGBA(double R, double G, double B, double A)
  {hitR = R; hitG = G; hitB = B; hitA = A;}
  
  void SetHitSize(G4double HS) {hitSize = HS;}
  
private:
  ASIMPhotodetectorSDHitCollection *hitCollection;
  std::vector<G4String> collectionNameList;

  G4double hitR, hitG, hitB, hitA;
  G4double hitSize;
};

#endif
