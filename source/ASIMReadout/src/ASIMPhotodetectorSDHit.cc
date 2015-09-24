/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMPhotodetectorSDHit.cc
// date: 21 May 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMPhotodetectorSDHit class is intended as a generic Geant4
//       sensitive detector hit class that registers information on
//       detected optical photons on optical readout volumes such as
//       PMTs and SiPMs. The class is used by the complementary class
//       ASIMPhotodetectorSD
//
/////////////////////////////////////////////////////////////////////////////////


#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "ASIMPhotodetectorSDHit.hh"


G4Allocator<ASIMPhotodetectorSDHit> ASIMPhotodetectorSDHitAllocator;


ASIMPhotodetectorSDHit::ASIMPhotodetectorSDHit()
{;}


ASIMPhotodetectorSDHit::~ASIMPhotodetectorSDHit()
{;}


ASIMPhotodetectorSDHit::ASIMPhotodetectorSDHit(const ASIMPhotodetectorSDHit &right)
  : G4VHit()
{
  kineticEnergy = right.kineticEnergy;
  position = right.position;
}


const ASIMPhotodetectorSDHit& ASIMPhotodetectorSDHit::operator=(const ASIMPhotodetectorSDHit &right)
{
  kineticEnergy = right.kineticEnergy;
  position = right.position;
  return *this;
}


G4int ASIMPhotodetectorSDHit::operator==(const ASIMPhotodetectorSDHit &right) const
{ return (this==&right) ? 1 : 0; }


void ASIMPhotodetectorSDHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
    {
      G4Circle circle(position);
      circle.SetScreenSize(10);
      circle.SetFillStyle(G4Circle::filled);
      G4Colour colour(0.0, 1.0, 1.0, 1.0);
      G4VisAttributes attribs(colour);
      circle.SetVisAttributes(attribs);
      pVVisManager->Draw(circle);
    }
}


void ASIMPhotodetectorSDHit::Print()
{;}
