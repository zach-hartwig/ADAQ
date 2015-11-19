/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMScintillatorSDHit.hh
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
//       ensures that hits are correctly visualized.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef ASIMScintillatorSDHit_hh
#define ASIMScintillatorSDHit_hh 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Colour.hh"


class ASIMScintillatorSDHit : public G4VHit
{

public:
  ASIMScintillatorSDHit();
  ~ASIMScintillatorSDHit();
  
  ASIMScintillatorSDHit(const ASIMScintillatorSDHit &right);
  const ASIMScintillatorSDHit& operator=(const ASIMScintillatorSDHit &right);
  G4int operator==(const ASIMScintillatorSDHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();
  
private:
  G4double hitR, hitG, hitB, hitA;
  G4double hitSize;
  
  G4double energyDep, kineticEnergy, creationTime;
  G4ThreeVector position, momentumDir;
  G4bool isOpticalPhoton;
  G4ParticleDefinition *particleDef;
  
public:
  void SetHitRGBA(double R, double G, double B, double A)
  {hitR = R; hitG = G; hitB = B; hitA = A;}
  
  void SetHitSize(double S) {hitSize = S;}
  
  inline G4double GetEnergyDep() const {return energyDep;}
  inline void SetEnergyDep(G4double eDep) {energyDep = eDep;}
  
  inline G4double GetKineticEnergy() const {return kineticEnergy;}
  inline void SetKineticEnergy(G4double kE) {kineticEnergy = kE;}

  inline G4double GetCreationTime() const {return creationTime;}
  inline void SetCreationTime(G4double CT) {creationTime = CT;}

  inline G4ThreeVector GetPosition() const {return position;}
  inline void SetPosition(G4ThreeVector p) {position = p;}

  inline G4ThreeVector GetMomentumDir() const {return momentumDir;}
  inline void SetMomentumDir(G4ThreeVector mD) {momentumDir = mD;}

  inline G4bool GetIsOpticalPhoton() const {return isOpticalPhoton;}
  inline void SetIsOpticalPhoton(G4bool isOP) {isOpticalPhoton = isOP;}
  
  inline G4ParticleDefinition *GetParticleDef() const {return particleDef;}
  inline void SetParticleDef(G4ParticleDefinition *pD) {particleDef = pD;}
};


typedef G4THitsCollection<ASIMScintillatorSDHit> ASIMScintillatorSDHitCollection;
extern G4Allocator<ASIMScintillatorSDHit> ASIMScintillatorSDHitAllocator;


inline void *ASIMScintillatorSDHit::operator new (size_t)
{
  void *aHit;
  aHit = (void *) ASIMScintillatorSDHitAllocator.MallocSingle();
  return aHit;
}


inline void ASIMScintillatorSDHit::operator delete(void *aHit)
{ ASIMScintillatorSDHitAllocator.FreeSingle((ASIMScintillatorSDHit *) aHit); }

#endif
