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
  : hitColour(new G4Colour(1.0, 0.0, 0.0, 1.0)), hitSize(5),
    kineticEnergy(0.), detectionTime(0.), position(0)
{;}


ASIMPhotodetectorSDHit::ASIMPhotodetectorSDHit(G4Colour *colour, G4double size)
  : hitColour(colour), hitSize(size),
    kineticEnergy(0.), detectionTime(0.), position(0)
{;}


ASIMPhotodetectorSDHit::~ASIMPhotodetectorSDHit()
{;}


ASIMPhotodetectorSDHit::ASIMPhotodetectorSDHit(const ASIMPhotodetectorSDHit &right)
  : G4VHit()
{
  kineticEnergy = right.kineticEnergy;
  detectionTime = right.detectionTime;
  position = right.position;
}


const ASIMPhotodetectorSDHit& ASIMPhotodetectorSDHit::operator=(const ASIMPhotodetectorSDHit &right)
{
  kineticEnergy = right.kineticEnergy;
  detectionTime = right.detectionTime;
  position = right.position;
  return *this;
}


G4int ASIMPhotodetectorSDHit::operator==(const ASIMPhotodetectorSDHit &right) const
{ return (this==&right) ? 1 : 0; }


void ASIMPhotodetectorSDHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager){
    
    G4Colour hitColor(hitColour->GetRed(),
		      hitColour->GetGreen(),
		      hitColour->GetBlue(),
		      hitColour->GetAlpha());

    G4VisAttributes attribs(hitColor);
    
    G4Circle circle(position);
    circle.SetScreenSize(hitSize);
    circle.SetFillStyle(G4Circle::filled);
    circle.SetVisAttributes(attribs);
    
    pVVisManager->Draw(circle);
  }
}


void ASIMPhotodetectorSDHit::Print()
{;}
