#include "PGA.hh"


PGA::PGA()
{
  // Create the particle source of type acroGPS. As documented
  // elsewhere, acroGPS is based on the G4GeneralParticleSource module
  // that is included in the Geant4 distribution. It has been custom
  // modified for the addition of certain features (time
  // distributions, nuclear reaction distributions). The acroGPS class
  // is also responsible for the creation of the GPS macros for
  // certain particle sources (RFQ D+ ions, for example), which will
  // only be built if an ACRONYM sequential build has been executed.
  
  TheSource = new G4GeneralParticleSource();
}


PGA::~PGA()
{ delete TheSource; }


void PGA::GeneratePrimaries(G4Event *currentEvent)
{ TheSource->GeneratePrimaryVertex(currentEvent); }
