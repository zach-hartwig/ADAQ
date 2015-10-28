/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMScintillatorSD.hh
// date: 21 May 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMScintillatorSD class is intended as a generic Geant4
//       sensitive detector class that should be attached to
//       scintillator volumes to handle data readout into the ASIM
//       file format supported by the ADAQ framework. It ensures that
//       the essential information for a scintillator is made
//       available for readout: energy deposition, hit
//       position/momentum, scintillation photons created, and
//       particle type. The class should be used with the
//       ASIMScintillatorSDHit class.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef ASIMScintillatorSD_hh
#define ASIMScintillatorSD_hh 1

#include "G4VSensitiveDetector.hh"
#include "G4Colour.hh"

#include "ASIMScintillatorSDHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class ASIMScintillatorSD : public G4VSensitiveDetector
{
  
public:
  ASIMScintillatorSD(G4String);
  ~ASIMScintillatorSD();

  void InitializeCollections(G4String);
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  G4bool ManualTrigger(const G4Track *);
  void EndOfEvent(G4HCofThisEvent*);

  std::vector<G4String> GetCollectionNameList()
  {return collectionNameList;}

  void SetHitRGBA(double R, double G, double B, double A)
  {hitR = R; hitG = G; hitB = B; hitA = A;}
  
  void SetHitSize(G4double HS) {hitSize = HS;}
  
private:
  ASIMScintillatorSDHitCollection *hitCollection;
  std::vector<G4String> collectionNameList;

  G4double hitR, hitG, hitB, hitA;
  G4double hitSize;
};

#endif

