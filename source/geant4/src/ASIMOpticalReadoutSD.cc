/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMOpticalReadoutSD.cc
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

#include "ASIMOpticalReadoutSD.hh"
#include "ASIMOpticalReadoutSDHit.hh"


ASIMOpticalReadoutSD::ASIMOpticalReadoutSD(G4String SDName)
  : G4VSensitiveDetector(SDName)
{
  G4String theCollectionName = SDName + "Collection";

  // A public list that can be accessed from other classes
  // for convenience of obtaining the collection names (mine)
  collectionNameList.push_back(theCollectionName);

  
  // A mandatory protected list only for this class (GEANT4)
  // that must contain name(s) of desired hit collection(s)
  collectionName.insert(collectionNameList.at(0));
}


// Clear the list data
ASIMOpticalReadoutSD::~ASIMOpticalReadoutSD()
{ collectionNameList.clear(); }


// At the beginning of each event, perform the following initializations
void ASIMOpticalReadoutSD::Initialize(G4HCofThisEvent *HCE)
{
  // Ensure HCID is set to <0 for each new event
  static int HCID = -1;
  
  // Create the desired hit collections, using names set in constructor
  hitCollection = new ASIMOpticalReadoutSDHitCollection
    (SensitiveDetectorName,collectionName[0]); 
  
  //  Add the desired hit collections to the mother hit collection of the event
  if(HCID<0){
    HCE -> AddHitsCollection(GetCollectionID(0),hitCollection);
  }
}

// The APD SD is triggered manually from within steppingAction to
// correctly account for "detection" of optical photons at the
// crystal-APD interface.  This requires replacing the default
// triggering method, ASIMOpticalReadoutSD::ProcessHits, with the customized
// ASIMOpticalReadoutSD::ManualTrigger triggering method.

// Default APD triggering: return "false" to nullify method
G4bool ASIMOpticalReadoutSD::ProcessHits(G4Step *, G4TouchableHistory *)
{ return false; }


// Custom APD triggering 
G4bool ASIMOpticalReadoutSD::ManualTrigger(const G4Step *currentStep, G4TouchableHistory *)
{
  G4Track *currentTrack = currentStep -> GetTrack();
  
  // Ensure that currently tracking particle is an optical photon
  if(currentTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;

  // Get the relevant hit information
  kineticEnergy = currentTrack->GetKineticEnergy();
  position = currentTrack->GetPosition();
  
  // Create a new hit and store it in the hit collection
  ASIMOpticalReadoutSDHit *newHit = new ASIMOpticalReadoutSDHit();
  
  newHit->SetKineticEnergy(kineticEnergy);
  newHit->SetPosition(position);

  // Add the hit to the desired hit collection.  If there are >0 hits
  // per event, the will all be stored inside this event's hit collection
  hitCollection->insert(newHit);

  return true;
}


void ASIMOpticalReadoutSD::EndOfEvent(G4HCofThisEvent*)
{;}


