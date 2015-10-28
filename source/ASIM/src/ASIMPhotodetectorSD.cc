/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMPhotodetectorSD.cc
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

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"
#include "G4THitsMap.hh"
#include "G4SDManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"

#include "ASIMPhotodetectorSD.hh"
#include "ASIMPhotodetectorSDHit.hh"


ASIMPhotodetectorSD::ASIMPhotodetectorSD(G4String name)
  : G4VSensitiveDetector(name),
    hitR(0.), hitG(0.), hitB(1.), hitA(0.5), hitSize(5)
{ InitializeCollections(name); }
    

void ASIMPhotodetectorSD::InitializeCollections(G4String name)
{
  G4String theCollectionName = name + "Collection";

  // A public list that can be accessed from other classes
  // for convenience of obtaining the collection names (mine)
  collectionNameList.push_back(theCollectionName);

  
  // A mandatory protected list only for this class (GEANT4)
  // that must contain name(s) of desired hit collection(s)
  collectionName.insert(collectionNameList.at(0));
}


// Clear the list data
ASIMPhotodetectorSD::~ASIMPhotodetectorSD()
{ collectionNameList.clear(); }


// At the beginning of each event, perform the following initializations
void ASIMPhotodetectorSD::Initialize(G4HCofThisEvent *HCE)
{
  // Ensure HCID is set to <0 for each new event
  static int HCID = -1;
  
  // Create the desired hit collections, using names set in constructor
  hitCollection = new ASIMPhotodetectorSDHitCollection
    (SensitiveDetectorName,collectionName[0]); 
  
  //  Add the desired hit collections to the mother hit collection of the event
  if(HCID<0){
    HCE -> AddHitsCollection(GetCollectionID(0),hitCollection);
  }
}


// The standard G4VSensitiveDetector scoring method is empty for the
// ASIMPhotodetectorSD since only optical photon hits are of interest
G4bool ASIMPhotodetectorSD::ProcessHits(G4Step *CurrentStep, G4TouchableHistory *)
{
  if(CurrentStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    return true;
  else
    return false;
}


// This method handles scoring of optical photon hits on the volume
// with which the ASIMPhotodetectorSD class is associated. This method
// is called by ASIMReadoutManager::HandleOpticalPhotonReadout() in
// order to correctly add only detected photons to the hit collection.
G4bool ASIMPhotodetectorSD::ManualTrigger(const G4Step *currentStep)
{
  G4Track *currentTrack = currentStep -> GetTrack();
  
  // Ensure that currently tracking particle is an optical photon
  if(currentTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;

  // Create a new hit and store it in the hit collection
  ASIMPhotodetectorSDHit *newHit = new ASIMPhotodetectorSDHit;
  newHit->SetHitRGBA(hitR, hitG, hitB, hitA);
  newHit->SetHitSize(hitSize);
  
  // Get the relevant hit information
  G4double kineticEnergy = currentTrack->GetKineticEnergy();
  G4double detectionTime = currentTrack->GetGlobalTime();
  G4ThreeVector position = currentTrack->GetPosition();
  
  // Set the quantities to the SD hit class
  newHit->SetKineticEnergy(kineticEnergy);
  newHit->SetDetectionTime(detectionTime);
  newHit->SetPosition(position);

  // Insert this hit into the SD hit collection
  hitCollection->insert(newHit);
  
  return true;
}


void ASIMPhotodetectorSD::EndOfEvent(G4HCofThisEvent*)
{;}
