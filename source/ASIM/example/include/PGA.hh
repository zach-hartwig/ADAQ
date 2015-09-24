#ifndef PGA_hh
#define PGA_hh 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Event.hh"

#include "G4GeneralParticleSource.hh"

class PGA : public G4VUserPrimaryGeneratorAction
{
public:
  PGA();
  ~PGA();
  
  G4GeneralParticleSource *GetSource() {return TheSource;}

  void GeneratePrimaries(G4Event *anEvent);

private:
  G4GeneralParticleSource *TheSource;
};

#endif
