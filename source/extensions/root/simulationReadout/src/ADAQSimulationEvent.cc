/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQSimulationEvent.cc
// date: 23 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ADAQSimulationEvent C++class provides a generic container
//       for the essential event-level data of a particle detector
//       that is modelled with Monte Carlo particle transport
//       methods. This class is designed to provide a straightforward
//       and universal method of persistently storing event-level data
//       for later analysis within the ADAQ framework. This class is
//       designed to be used within the ROOT framework and utilizes
//       ROOT data types to ensure compatibility with post-simulation
//       analysis tools built using ROOT and for portability between
//       platforms. 
//
/////////////////////////////////////////////////////////////////////////////////


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


