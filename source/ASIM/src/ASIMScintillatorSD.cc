/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMScintillatorSD.cc
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

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4THitsMap.hh"
#include "G4SDManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

#include "ASIMScintillatorSD.hh"
#include "ASIMScintillatorSDHit.hh"


ASIMScintillatorSD::ASIMScintillatorSD(G4String name)
  : G4VSensitiveDetector(name),
    hitR(1.), hitG(0.), hitB(0.), hitA(0.5), hitSize(5)
{ InitializeCollections(name); }


void ASIMScintillatorSD::InitializeCollections(G4String name)
{
  G4String theCollectionName = name + "Collection";
  
  // A public list that can be accessed from other classes
  // for convenience of obtaining the collection names (mine)
  collectionNameList.push_back(theCollectionName);
  
  // A mandatory protected list only for this class (Geant4)
  // that must contain name(s) of desired hit collection(s)
  collectionName.insert(collectionNameList.at(0));
}


ASIMScintillatorSD::~ASIMScintillatorSD()
{ collectionNameList.clear(); }


void ASIMScintillatorSD::Initialize(G4HCofThisEvent *HCE)
{
  static int HCID = -1;
  
  hitCollection = new ASIMScintillatorSDHitCollection(SensitiveDetectorName,
						      collectionName[0]); 
  
  if(HCID<0){
    HCE->AddHitsCollection(GetCollectionID(0),
			   hitCollection);
  }
}


G4bool ASIMScintillatorSD::ProcessHits(G4Step *currentStep, G4TouchableHistory *)
{
  G4Track *currentTrack = currentStep->GetTrack();
  
  G4ParticleDefinition *particleDef = currentTrack->GetDefinition();
  
  // Ensure that optical photons are excluded from registering hits
  if(particleDef == G4OpticalPhoton::OpticalPhotonDefinition())
    return false;
  
  // Handle all other valid particles
  else{
    ASIMScintillatorSDHit *newHit = new ASIMScintillatorSDHit;
    newHit->SetHitRGBA(hitR, hitG, hitB, hitA);
    newHit->SetHitSize(hitSize);
    
    // Obtain the quantities from the step/track objects
    G4double energyDep = currentStep->GetTotalEnergyDeposit() * currentTrack->GetWeight();
    G4double kineticEnergy = currentTrack->GetKineticEnergy();
    G4ThreeVector position = currentTrack->GetPosition();
    G4ThreeVector momentumDir = currentTrack->GetMomentumDirection();
      
    // Set the quantities to the SD hit class
    newHit->SetEnergyDep(energyDep);
    newHit->SetKineticEnergy(kineticEnergy);
    newHit->SetPosition(position);
    newHit->SetMomentumDir(momentumDir);
    newHit->SetIsOpticalPhoton(false);
    newHit->SetParticleDef(particleDef);

    // Insert the SD hit into the SD hit collection
    hitCollection->insert(newHit);
    
    return true;
  }
}


// This method handles scoring of optical photons that are created in
// the volume to which the ASIMScintillatorSD object is attached. The
// method is called by ASIMReadoutManager::HandleOpticalPhotonCreation()
// in order to correctly add created photons at birth to the hit collection
G4bool ASIMScintillatorSD::ManualTrigger(const G4Track *currentTrack)
{
  G4ParticleDefinition *particleDef = currentTrack->GetDefinition();
  
  // Ensure that currently tracking particle is an optical photon
  if(particleDef == G4OpticalPhoton::OpticalPhotonDefinition()){
    
    ASIMScintillatorSDHit *newHit = new ASIMScintillatorSDHit;
    newHit->SetHitRGBA(hitR, hitG, hitB, hitA);
    newHit->SetHitSize(hitSize);
    
    // Get the relevant hit information
    G4double kineticEnergy = currentTrack->GetKineticEnergy();
    G4double creationTime = currentTrack->GetGlobalTime();
    G4ThreeVector position = currentTrack->GetPosition();
    
    // Set the quantities to the SD hit class. Note that the
    // IsOpticalPhoton flag is necessary to set since the
    // ASIMScintillatorSD class is also responsible for scoring other
    // particle types as well as optical photons.
    
    newHit->SetKineticEnergy(kineticEnergy);
    newHit->SetCreationTime(creationTime);
    newHit->SetPosition(position);
    newHit->SetIsOpticalPhoton(true);
    newHit->SetParticleDef(particleDef);
    
    // Insert the hit into the SD hit collection
    hitCollection->insert(newHit);
    
    return true;
  }
  
  // Handle all other particles
  else
    return false;
}


void ASIMScintillatorSD::EndOfEvent(G4HCofThisEvent *)
{;}
