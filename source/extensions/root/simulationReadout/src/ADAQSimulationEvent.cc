#include "ADAQSimulationEvent.hh"

ADAQSimulationEvent::ADAQSimulationEvent()
{ Initialize(); }


ADAQSimulationEvent::~ADAQSimulationEvent()
{;}


void ADAQSimulationEvent::Initialize()
{
  EventID = RunID = 0;
  EnergyDep = 0.;
  PhotonsCreated = PhotonsDetected = 0;
  PhotonCreationTime.clear();
  PhotonDetectionTime.clear();
  VertexPos[0] = VertexPos[1] = VertexPos[2] = 0.;
  VertexMomDir[0] = VertexMomDir[1] = VertexMomDir[2] = 0.;
  VertexKE = 0.;
  VertexPCode = 0;
}


