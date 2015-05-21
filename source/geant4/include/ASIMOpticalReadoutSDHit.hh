/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMOpticalReadoutSDHit.hh
// date: 21 May 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: ASIMOpticalReadoutSDHit class is intended as a generic Geant4
//       sensitive detector hit class that registers information on
//       detected optical photons on optical readout volumes such as
//       PMTs and SiPMs. The class is used by the complementary class
//       ASIMOpticalReadoutSD
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef ASIMOpticalReadoutSDHit_hh
#define ASIMOpticalReadoutSDHit_hh 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"


class ASIMOpticalReadoutSDHit : public G4VHit
{

public:
  ASIMOpticalReadoutSDHit();
  ~ASIMOpticalReadoutSDHit();
  
  ASIMOpticalReadoutSDHit(const ASIMOpticalReadoutSDHit &right);
  const ASIMOpticalReadoutSDHit& operator=(const ASIMOpticalReadoutSDHit &right);
  G4int operator==(const ASIMOpticalReadoutSDHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();
  
private:
  G4double kineticEnergy;  
  G4ThreeVector position;
  
public:
  inline G4double GetKineticEnergy() const {return kineticEnergy;};
  inline void SetKineticEnergy(G4double KE) {kineticEnergy = KE;};
  
  inline G4ThreeVector GetPosition() const {return position;};  
  inline void SetPosition(G4ThreeVector pos) {position = pos;};
};


typedef G4THitsCollection<ASIMOpticalReadoutSDHit> ASIMOpticalReadoutSDHitCollection;
extern G4Allocator<ASIMOpticalReadoutSDHit> ASIMOpticalReadoutSDHitAllocator;

inline void *ASIMOpticalReadoutSDHit::operator new (size_t)
{
  void *aHit;
  aHit = (void *) ASIMOpticalReadoutSDHitAllocator.MallocSingle();
  return aHit;
}

inline void ASIMOpticalReadoutSDHit::operator delete(void *aHit)
{
  ASIMOpticalReadoutSDHitAllocator.FreeSingle((ASIMOpticalReadoutSDHit *) aHit);
}

#endif
