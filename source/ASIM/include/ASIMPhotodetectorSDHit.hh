/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMPhotodetectorSDHit.hh
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

#ifndef ASIMPhotodetectorSDHit_hh
#define ASIMPhotodetectorSDHit_hh 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Colour.hh"


class ASIMPhotodetectorSDHit : public G4VHit
{
  
public:
  ASIMPhotodetectorSDHit();
  ~ASIMPhotodetectorSDHit();
  
  ASIMPhotodetectorSDHit(const ASIMPhotodetectorSDHit &right);
  const ASIMPhotodetectorSDHit& operator=(const ASIMPhotodetectorSDHit &right);
  G4int operator==(const ASIMPhotodetectorSDHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();
  
private:
  G4double hitR, hitG, hitB, hitA;
  G4double hitSize;
  
  G4double kineticEnergy, detectionTime;
  G4ThreeVector position;
  
public:
  void SetHitRGBA(double R, double G, double B, double A)
  {hitR = R; hitG = G; hitB = B; hitA = A;}
  
  void SetHitSize(double S) {hitSize = S;}
  
  inline G4double GetKineticEnergy() const {return kineticEnergy;};
  inline void SetKineticEnergy(G4double KE) {kineticEnergy = KE;};
  
  inline G4double GetDetectionTime() const {return detectionTime;};
  inline void SetDetectionTime(G4double DT) {detectionTime = DT;};
  
  inline G4ThreeVector GetPosition() const {return position;};  
  inline void SetPosition(G4ThreeVector pos) {position = pos;};
};


typedef G4THitsCollection<ASIMPhotodetectorSDHit> ASIMPhotodetectorSDHitCollection;
extern G4Allocator<ASIMPhotodetectorSDHit> ASIMPhotodetectorSDHitAllocator;

inline void *ASIMPhotodetectorSDHit::operator new (size_t)
{
  void *aHit;
  aHit = (void *) ASIMPhotodetectorSDHitAllocator.MallocSingle();
  return aHit;
}

inline void ASIMPhotodetectorSDHit::operator delete(void *aHit)
{
  ASIMPhotodetectorSDHitAllocator.FreeSingle((ASIMPhotodetectorSDHit *) aHit);
}

#endif
