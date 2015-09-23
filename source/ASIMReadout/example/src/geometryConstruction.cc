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
  World_X = 5.*cm;
  World_Y = 5.*cm;
  World_Z = 15.*cm;

  // Initialize parameters for the scintillators
  BGO_X = 2.*cm;
  BGO_Y = 2.*cm;
  BGO_Z = 3.*cm;

  NaI_RMin = 0.*cm;
  NaI_RMax = 1.27*cm;
  NaI_Z = 2.54*cm;
  NaIPMT_Z = 0.1*cm;
}


geometryConstruction::~geometryConstruction()
{;}


G4VPhysicalVolume *geometryConstruction::Construct()
{
  ////////////////////////////////////////
  // Create the top-level world volumes //
  ////////////////////////////////////////
    
  World_S = new G4Box("World_S",
		      World_X/2,
		      World_Y/2,
		      World_Z/2);
    
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

  /////////////////////////////////////
  // Create two scintillator volumes //
  /////////////////////////////////////
  
  // A cuboid BGO scintillator
  
  BGO_S = new G4Box("BGO_S",
		    BGO_X/2,
		    BGO_Y/2,
		    BGO_Z/2);
  
  BGO_L = new G4LogicalVolume(BGO_S,
			      G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO"),
			      "BGO_L");
  
  BGO_P = new G4PVPlacement(0,
			    G4ThreeVector(0., 0., -4.*cm),
			    BGO_L,
			    "BGO",
			    World_L,
			    false,
			    0);
  
  G4VisAttributes *BGOVisAtt = new G4VisAttributes(G4Colour(0.3, 1.0, 0.5, 0.6));
  BGOVisAtt->SetForceSolid(true);
  BGO_L->SetVisAttributes(BGOVisAtt);

  // A cylindrical NaI(Tl) scintillator
  
  NaI_S = new G4Tubs("NaI_S",
		     NaI_RMin,
		     NaI_RMax,
		     NaI_Z/2,
		     0.*degree,
		     360.*degree);
  
  NaI_L = new G4LogicalVolume(NaI_S,
			      G4NistManager::Instance()->FindOrBuildMaterial("G4_SODIUM_IODIDE"),
			      "NaI_L");
  
  NaI_P = new G4PVPlacement(0,
			    G4ThreeVector(0., 0., 4.*cm),
			    NaI_L,
			    "NaI",
			    World_L,
			    false,
			    0);
  
  G4VisAttributes *NaIVisAtt = new G4VisAttributes(G4Colour(0.2, 0.6, 1.0, 0.6));
  NaIVisAtt->SetForceSolid(true);
  NaI_L->SetVisAttributes(NaIVisAtt);

  
  NaIPMT_S = new G4Tubs("NaIPMTS",
			NaI_RMin,
			NaI_RMax,
			NaIPMT_Z/2,
			0.*degree,
			360.*degree);
  
  NaIPMT_L = new G4LogicalVolume(NaIPMT_S,
				 G4NistManager::Instance()->FindOrBuildMaterial("G4_SODIUM_IODIDE"),
				 "NaIPMTL");
  
  NaIPMT_P = new G4PVPlacement(0,
			       G4ThreeVector(0., 0., (4.*cm + NaI_Z/2 + NaIPMT_Z/2)),
			       NaIPMT_L,
			       "NaIPMT",
			       World_L,
			       false,
			       0);
  
  G4VisAttributes *NaIPMTVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 1.0));
  NaIPMTVisAtt->SetForceSolid(true);
  NaIPMT_L->SetVisAttributes(NaIPMTVisAtt);

  
  /////////////////////////////////////////////////////////////
  // Specify sensitive detectors (SD) and ASIM readouts (AR) //
  /////////////////////////////////////////////////////////////

  // Get the SD and AR managers
  
  G4SDManager *SDMgr = G4SDManager::GetSDMpointer();
  ASIMReadoutManager *ARMgr = ASIMReadoutManager::GetInstance();

  // The BGO scintillator

  ASIMScintillatorSD *BGO_SD = new ASIMScintillatorSD("BGOSD",
						      new G4Colour(1.0, 1.0, 0.0, 0.4),
						      8); 
  SDMgr->AddNewDetector(BGO_SD);
  BGO_L->SetSensitiveDetector(BGO_SD);
  
  ARMgr->RegisterNewReadout("Demonstrating the readout of a BGO scintillator to an ASIM file",
			    BGO_P);

  // The NaI(Tl) scintillator ...
  
  ASIMScintillatorSD *NaI_SD = new ASIMScintillatorSD("NAISD",
						      new G4Colour(1.0, 0.0, 0.0, 0.4),
						      8); 
  SDMgr->AddNewDetector(NaI_SD);
  NaI_L->SetSensitiveDetector(NaI_SD);

  // ... and the NaI(Tl) photomultiplier tube
  
  ASIMScintillatorSD *NaIPMT_SD = new ASIMScintillatorSD("NAIPMTSD",
							 new G4Colour(1.0, 0.0, 0.0, 0.4),
							 8); 
  SDMgr->AddNewDetector(NaIPMT_SD);
  NaIPMT_L->SetSensitiveDetector(NaIPMT_SD);
  
  ARMgr->RegisterNewReadout("Demonstrating the readout of a NaI(Tl) scintillator to an ASIM file",
			    NaI_P,
			    NaIPMT_P);

  return World_P;
}
