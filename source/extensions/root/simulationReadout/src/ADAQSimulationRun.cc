/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQSimulationRun.cc
// date: 23 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ADAQSimulationRun C++ class provides a generic container
//       for the essential run-level data of a particle detector that
//       is modelled with Monte Carlo particle transport methods. This
//       class is designed to provide a straightforward and universal
//       method of persistently storing run-level data for later
//       analysis within the ADAQ framework. This class is designed to
//       be used within the ROOT framework and utilizes ROOT data
//       types to ensure compatibility with post-simulation analysis
//       tools built using ROOT and for portability between platforms.
//
/////////////////////////////////////////////////////////////////////////////////

#include "ADAQSimulationRun.hh"


ADAQSimulationRun::ADAQSimulationRun()
  : RunID(0), 
    ParticlesIncident(0), ParticlesBetweenThresholds(0),
    DetectorLowerThresholdInMeV(0.), DetectorUpperThresholdInMeV(1000000.),
    PhotonsCreated(0), PhotonsDetected(0)
{;}


ADAQSimulationRun::~ADAQSimulationRun()
{;}


void ADAQSimulationRun::Reset()
{
  ParticlesIncident = ParticlesBetweenThresholds = 0;
  PhotonsCreated = PhotonsDetected;
}


Double_t ADAQSimulationRun::GetDetectorEfficiency()
{
  if(ParticlesIncident == 0)
    return 0.;
  else
    return (ParticlesBetweenThresholds * 1.0 / ParticlesIncident);
}


Double_t ADAQSimulationRun::GetOpticalEfficiency()
{
  if(PhotonsCreated == 0)
    return 0.;
  else
    return (PhotonsDetected * 1.0 / PhotonsCreated);
}
