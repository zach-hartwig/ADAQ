#ifndef geometryConstruction_hh
#define geometryConstruction_hh 1

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"

#include <vector>
using namespace std;

//#include "ASIMScintillatorSD.hh"

class geometryConstruction : public G4VUserDetectorConstruction
{
  
public:
  geometryConstruction(G4bool); 
  ~geometryConstruction();
  
  G4VPhysicalVolume *Construct();
  void BuildScintillator();

private:
  // Member data for the world
  G4Box *World_S;
  G4LogicalVolume *World_L;
  G4VPhysicalVolume *World_P;
  G4double WorldX, WorldY, WorldZ;

  // Member data for the scintillator
  G4Box *Scintillator_S;
  G4LogicalVolume *Scintillator_L;
  G4VPhysicalVolume *Scintillator_P;
  //  ASIMScintillatorSD *Scintillator_SD;
  G4double ScintillatorX, ScintillatorY, ScintillatorZ;
};

#endif
