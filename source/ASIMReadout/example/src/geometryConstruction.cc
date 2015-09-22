#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "ASIMScintillatorSD.hh"
#include "ASIMReadoutManager.hh"

#include "geometryConstruction.hh"


geometryConstruction::geometryConstruction()
{
  // Parameters for the world volume
  WorldX = 5.*cm;
  WorldY = 5.*cm;
  WorldZ = 10.*cm;

  // Initialize parameters for the core scintillator
  ScintillatorX = 1.*cm;
  ScintillatorY = 1.*cm;
  ScintillatorZ = 3.*cm;
}


geometryConstruction::~geometryConstruction()
{;}


G4VPhysicalVolume *geometryConstruction::Construct()
{
  /////////////////////////////////////
  // Create the top-level world volumes

  // The simulation world
    
  World_S = new G4Box("World_S",
		      WorldX/2,
		      WorldY/2,
		      WorldZ/2);
    
  World_L = new G4LogicalVolume(World_S, 
				G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
				"World_L");
    
  World_P = new G4PVPlacement(0, 
			      G4ThreeVector(), 
			      World_L, 
			      "TheWorld",
			      0,
			      false,
			      0);
  
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetVisibility(true);
  World_L->SetVisAttributes(WorldVisAtt);
  
  // The scintillator
  
  Scintillator_S = new G4Box("Scintillator_S",
			     ScintillatorX/2,
			     ScintillatorY/2,
			     ScintillatorZ/2);
  
  Scintillator_L = new G4LogicalVolume(Scintillator_S,
				       G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO"),
				       "Scintillator_L");
  
  Scintillator_P = new G4PVPlacement(0,
				     G4ThreeVector(0., 0., 0.),
				     Scintillator_L,
				     "TheScintillator",
				     World_L,
				     false,
				     0);
  
  G4VisAttributes *ScintillatorVisAtt = new G4VisAttributes(G4Colour(0.3, 1.0, 0.5, 0.7));
  ScintillatorVisAtt->SetForceSolid(true);
  Scintillator_L->SetVisAttributes(ScintillatorVisAtt);
  
  ASIMScintillatorSD *Scintillator_SD = new ASIMScintillatorSD("ScintillatorSD",
							       new G4Colour(1.0, 1.0, 0.0, 0.4),
							       8); 
  G4SDManager::GetSDMpointer()->AddNewDetector(Scintillator_SD);
  
  ASIMReadoutManager::GetInstance()->RegisterReadout("ScintillatorSD",
						     "ASIMExample demonstration of scintillator readout");
  
  Scintillator_L->SetSensitiveDetector(Scintillator_SD);
  
  return World_P;
}
