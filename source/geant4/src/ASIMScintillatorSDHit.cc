/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMScintillatorSDHit.cc
// date: 21 May 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMScintillatorSDHit class is intended as a generic Geant4
//       sensitive detector hit class that should be used with the
//       ASIMScintillatorSD. It ensures that the essential information
//       for a scintillator is made available for readout: energy
//       deposition, hit position, hit momentum direction, number of
//       scintillation photons created, and particle type. It also
//       ensures that hits are correctly visualized
//
/////////////////////////////////////////////////////////////////////////////////

#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "ASIMScintillatorSDHit.hh"


G4Allocator<ASIMScintillatorSDHit> ASIMScintillatorSDHitAllocator;


ASIMScintillatorSDHit::ASIMScintillatorSDHit()
  : energyDep(0.), kineticEnergy(0.), position(0), momentumDir(0),
    isOpticalPhoton(false)
{;}


ASIMScintillatorSDHit::~ASIMScintillatorSDHit()
{;}


ASIMScintillatorSDHit::ASIMScintillatorSDHit(const ASIMScintillatorSDHit &right)
  : G4VHit()
{
  energyDep = right.energyDep;
  kineticEnergy = right.kineticEnergy;
  position = right.position;
  momentumDir = right.momentumDir;
  isOpticalPhoton = right.isOpticalPhoton;
  particleDef = right.particleDef;
}


const ASIMScintillatorSDHit& ASIMScintillatorSDHit::operator=(const ASIMScintillatorSDHit &right)
{
  energyDep = right.energyDep;
  kineticEnergy = right.kineticEnergy;
  position = right.position;
  momentumDir = right.momentumDir;
  isOpticalPhoton = right.isOpticalPhoton;
  particleDef = right.particleDef;

  return *this;
}


G4int ASIMScintillatorSDHit::operator==(const ASIMScintillatorSDHit &right) const
{ return (this==&right) ? 1 : 0; }


void ASIMScintillatorSDHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager){
    G4Circle circle(position);
    circle.SetScreenSize(6);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.0, 1.0, 0.0, 1.0);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}


void ASIMScintillatorSDHit::Print()
{;}
