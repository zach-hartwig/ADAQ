// G4
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

// ASIM
#include "ASIMReadoutManager.hh"
#include "ASIMScintillatorSD.hh"
#include "ASIMPhotodetectorSD.hh"

// ASIMExample
#include "geometryConstruction.hh"
#include "NaIOpticalData.hh"


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

  BuildMaterials();
}


geometryConstruction::~geometryConstruction()
{;}


void geometryConstruction::BuildMaterials()
{
  G4NistManager *NISTMgr = G4NistManager::Instance();

  // Define conversion factor for wavelength (nm) to energy (eV)
  const G4double nm2eV = 1239.583 * eV * nm;

  
  ////////////////////////////////////////////
  // BGO scintillator (no optical included) //
  ////////////////////////////////////////////

  BGO = NISTMgr->FindOrBuildMaterial("G4_BGO");


  /////////////////////////////////////////////////////
  // NaI(Tl) scintillator (optical physics included) //
  /////////////////////////////////////////////////////
  
  NaI = G4NistManager::Instance()->FindOrBuildMaterial("G4_SODIUM_IODIDE");
  G4MaterialPropertiesTable *NaI_MPT = new G4MaterialPropertiesTable();
  
  // Create the optical properties of NaI(Tl) necessary for full
  // optical physics simulation and attach via a G4MPT object. See the
  // "NaIOpticalData.hh" header for optical properties specified here.

  // Transform the wavelength spectrum into an energy spectrum
  for(G4int i=0; i<NaI_entries; i++)
    NaI_eSpectrum[i] = nm2eV / NaI_eWavelength[i];      

  NaI_MPT->AddConstProperty("FASTSCINTILLATIONRISETIME", NaI_fastRiseTimeC);
  NaI_MPT->AddConstProperty("FASTTIMECONSTANT", NaI_fastDecayTimeC);
  NaI_MPT->AddConstProperty("SCINTILLATIONYIELD", NaI_yield);
  NaI_MPT->AddConstProperty("YIELDRATIO",NaI_yRatio);
  NaI_MPT->AddConstProperty("RESOLUTIONSCALE",NaI_rScale);
  
  NaI_MPT->AddProperty("RINDEX", NaI_rIndexSpectrum, NaI_rIndex, NaI_rIndexEntries);
  NaI_MPT->AddProperty("ABSLENGTH", NaI_absLengthSpectrum, NaI_absLength, NaI_absLengthEntries);
  NaI_MPT->AddProperty("FASTCOMPONENT", NaI_eSpectrum, NaI_eProb, NaI_entries);
  
  NaI->SetMaterialPropertiesTable(NaI_MPT);
  NaI->GetIonisation()->SetMeanExcitationEnergy(NaI_meanIonisE);


  //////////////////////////////////////////////////////
  // Bialkali photocathode (optical physics included) //
  //////////////////////////////////////////////////////

  Bialkali = new G4Material("Bialkali", 1.*g/cm3, 3);
  Bialkali->AddElement(G4NistManager::Instance()->FindOrBuildElement(37), 1); // Rubidium
  Bialkali->AddElement(G4NistManager::Instance()->FindOrBuildElement(55), 1); // Cesium
  Bialkali->AddElement(G4NistManager::Instance()->FindOrBuildElement(51), 1); // Antimony

  G4MaterialPropertiesTable *Bialkali_MPT = new G4MaterialPropertiesTable();

  const G4int nEntries = 27;
    
  G4double photonWavelength[nEntries] = {298.3*nm, 302.0*nm, 304.5*nm, 309.6*nm, 313.1*nm,
					 313.3*nm, 316.5*nm, 320.2*nm, 329.4*nm, 343.8*nm,
					 363.9*nm, 391.0*nm, 416.6*nm, 442.3*nm, 467.0*nm,
					 488.8*nm, 502.8*nm, 525.8*nm, 541.5*nm, 556.2*nm,
					 569.0*nm, 581.4*nm, 592.7*nm, 602.6*nm, 612.9*nm,
					 621.7*nm, 630.1*nm};
  
  G4double photonEnergy[nEntries];
  G4double reflectivity[nEntries];
  G4double refractiveIndex[nEntries];
  
  for(G4int i=0; i<nEntries; i++){
    photonEnergy[i] = nm2eV/photonWavelength[i];
    reflectivity[i] = 0.;
    refractiveIndex[i] = 4.7;
  }
  
  G4double efficiency[nEntries] = { 2.28*perCent,  3.98*perCent,  5.16*perCent,  6.70*perCent,  8.28*perCent,
				    9.89*perCent, 10.22*perCent, 11.68*perCent, 15.07*perCent, 19.22*perCent,
				   22.86*perCent, 24.35*perCent, 23.96*perCent, 22.13*perCent, 19.57*perCent,
				   16.58*perCent, 14.94*perCent, 11.13*perCent,  8.95*perCent,  7.09*perCent,
				    5.62*perCent,  4.36*perCent,  3.38*perCent,  2.63*perCent,  2.02*perCent,
				    1.56*perCent,  1.21*perCent };

  Bialkali_MPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, nEntries);
  Bialkali_MPT->AddProperty("EFFICIENCY", photonEnergy, efficiency, nEntries);
  Bialkali_MPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
  
  Bialkali->SetMaterialPropertiesTable(Bialkali_MPT);


  ////////////
  // Vacuum //
  ////////////

  Vacuum = NISTMgr->FindOrBuildMaterial("G4_Galactic");
}


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
				Vacuum,
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


  BuildBGOScintillator();
  BuildNaIScintillator();
  BuildReadouts();

  return World_P;
}


void geometryConstruction::BuildBGOScintillator()
{
  BGO_S = new G4Box("BGO_S",
		    BGO_X/2,
		    BGO_Y/2,
		    BGO_Z/2);
  
  BGO_L = new G4LogicalVolume(BGO_S,
			      BGO,
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
}


void geometryConstruction::BuildNaIScintillator()
{
  ///////////////////////
  // Build NaI volumes //
  ///////////////////////
  //
  // First we build the physical volumes for the NaI(TL), which
  // includes the cylindrical NaI(Tl) scintillator and a cylindrical
  // bialkali PMT photocathode mockup
  
  NaI_S = new G4Tubs("NaI_S",
		     NaI_RMin,
		     NaI_RMax,
		     NaI_Z/2,
		     0.*degree,
		     360.*degree);
  
  NaI_L = new G4LogicalVolume(NaI_S,
			      NaI,
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
				 Bialkali,
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


  ////////////////////////////////
  // Build NaI optical surfaces //
  ////////////////////////////////
  //
  // Second we create the optical surfaces that are necessary for full
  // simulation of the optical properties of this "detector". There
  // are three types of optical surfaces used:
  //
  // 0. Reflective surfaces: These surfaces are wrapped around 2 of 3
  //    liquid scintillator sides and the window sides to reflect
  //    optical photons back into the scintillator. The user may
  //    choose between data-driven (LUT: look-up-tables) or
  //    parameterized surface properties specified by the user.
  // 
  // 1. Transmission surfaces: This surface is placed between the
  //    scintillator liquid and the borofloat window to enable
  //    physically correct light transmission
  //
  // 2. Detection surface: This surface is placed at 1 end of the
  //    EJ301 scintillator cylinder abutting the SiPM readout
  //    device. It's purpose is to "detect" optical photons.


  // A note on dielectric-metal interface parameters: A REFLECTIVITY
  // value of "X" sets the prob. that optical photons will be
  // reflected (=X) by the metal and absorbed by the metal (=1-X). An
  // EFFICIENCY value of "Y" set the prob. that the absorbed optical
  // photon will be detected (=Y) and not detected (=1-Y). "Detected"
  // here implies that - in the real world - the optical photon
  // produces a photoelectron in the metal that goes on to produce a
  // detectable voltage from the readout device.
  
  G4OpticalSurface *ReflectiveWrap_OS = new G4OpticalSurface("ReflectiveWrap_OS");
  ReflectiveWrap_OS->SetType(dielectric_metal);
  ReflectiveWrap_OS->SetModel(unified);
  ReflectiveWrap_OS->SetFinish(polished);
  
  // Optical photon energy spectrum
  G4double ReflectiveSpct[2] = {0.1*eV, 15.*eV};
  
  // Corresponding probability of reflection
  G4double ReflectiveRefl[2] = {0.98, 0.98};
  
  // Corresponding probability of producing a photoelectron
  // (irrelevant since this surface only performs reflection)
  G4double ReflectiveEffe[2] = {0., 0.};
  
  G4MaterialPropertiesTable *ReflectiveMPT = new G4MaterialPropertiesTable();
  ReflectiveMPT->AddProperty("REFLECTIVITY", ReflectiveSpct, ReflectiveRefl, 2);
  ReflectiveMPT->AddProperty("EFFICIENCY", ReflectiveSpct, ReflectiveEffe, 2);
  ReflectiveWrap_OS->SetMaterialPropertiesTable(ReflectiveMPT);
  
  new G4LogicalBorderSurface("NaIReflectiveWrap_LBS",
			     NaI_P,
			     World_P,
			     ReflectiveWrap_OS);

  new G4LogicalBorderSurface("NaIPMTReflectiveWrap_LBS",
			     NaIPMT_P,
			     World_P,
			     ReflectiveWrap_OS);
  
  ////////////////////
  // Detection surface 
  
  G4OpticalSurface *DetectionWrap_OS = new G4OpticalSurface("DetectionWrap_OS");
  DetectionWrap_OS->SetType(dielectric_metal);
  DetectionWrap_OS->SetModel(unified);
  DetectionWrap_OS->SetFinish(polished);
  DetectionWrap_OS->SetMaterialPropertiesTable(Bialkali->GetMaterialPropertiesTable());
  
  new G4LogicalBorderSurface("DetectionWrap_LBS", 
			     NaI_P,
			     NaIPMT_P,
			     DetectionWrap_OS);
}


void geometryConstruction::BuildReadouts()
{
  
  /////////////////////////////////////////////////////////////
  // Specify sensitive detectors (SD) and ASIM readouts (AR) //
  /////////////////////////////////////////////////////////////

  // Get the SD and AR managers
  
  G4SDManager *SDMgr = G4SDManager::GetSDMpointer();
  ASIMReadoutManager *ARMgr = ASIMReadoutManager::GetInstance();

  // The BGO scintillator

  ASIMScintillatorSD *BGO_SD = new ASIMScintillatorSD("BGO-Detector");
  BGO_SD->SetHitRGBA(1.0, 0.0, 0.0, 0.4);
  BGO_SD->SetHitSize(8);

  SDMgr->AddNewDetector(BGO_SD);
  BGO_L->SetSensitiveDetector(BGO_SD);
  
  ARMgr->RegisterNewReadout("Demonstrating the readout of a BGO scintillator to an ASIM file",
			    BGO_P);

  // The NaI(Tl) scintillator ...
  
  ASIMScintillatorSD *NaI_SD = new ASIMScintillatorSD("NaI(Tl)-Detector");
  NaI_SD->SetHitRGBA(1.0, 1.0, 0.0, 0.4);
  NaI_SD->SetHitSize(8);

  SDMgr->AddNewDetector(NaI_SD);
  NaI_L->SetSensitiveDetector(NaI_SD);

  // ... and the NaI(Tl) photomultiplier tube
  
  ASIMPhotodetectorSD *NaIPMT_SD = new ASIMPhotodetectorSD("NaI(Tl)-PMT");
  NaIPMT_SD->SetHitRGBA(0.0, 1.0, 1.0, 1.0);
  NaIPMT_SD->SetHitSize(4);

  SDMgr->AddNewDetector(NaIPMT_SD);
  NaIPMT_L->SetSensitiveDetector(NaIPMT_SD);
  
  ARMgr->RegisterNewReadout("Demonstrating the readout of a NaI(Tl)-PMT detector to an ASIM file",
			    NaI_P,
			    NaIPMT_P);
}
