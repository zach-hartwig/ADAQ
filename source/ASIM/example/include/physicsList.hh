#ifndef physicsList_hh
#define physicsList_hh 1

#include "G4VModularPhysicsList.hh"

class G4OpticalPhysics;

class physicsList: public G4VModularPhysicsList
{
public:

  physicsList(G4bool, G4bool);
  ~physicsList();

  void ConstructParticle();
  void ConstructPhysics();
  void SetCuts();

  G4VPhysicsConstructor *GetDecayPhysics() const {return decayPhysics;}
  G4OpticalPhysics *GetOpticalPhysics() const {return opticalPhysics;}
  
private:
  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  G4double cutForProton;

  G4bool useNeutronHP, useScintillation;

  G4VPhysicsConstructor *decayPhysics;
  std::vector<G4VPhysicsConstructor *> EMPhysicsList;
  std::vector<G4VPhysicsConstructor *> hadronPhysicsList;
  G4OpticalPhysics *opticalPhysics;
};

#endif
