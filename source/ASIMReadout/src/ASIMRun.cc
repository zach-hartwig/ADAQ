/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMRun.cc
// date: 23 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMRun class provides a generic container for the
//       essential run-level data of a particle detector that is
//       modelled with Monte Carlo particle transport methods. This
//       class is designed to provide a straightforward and universal
//       method of persistently storing run-level data for later
//       analysis within the ADAQ framework. This class is designed to
//       be used within the ROOT framework and utilizes ROOT data
//       types to ensure compatibility with post-simulation analysis
//       tools built using ROOT and for portability between platforms.
//
/////////////////////////////////////////////////////////////////////////////////

#include "ASIMRun.hh"


ASIMRun::ASIMRun()
  : RunID(0), TotalEvents(0),
    ParticlesIncident(0), ParticlesBetweenThresholds(0),
    LowerThresholdInMeV(0.), UpperThresholdInMeV(1.),
    PhotonsCreated(0), PhotonsDetected(0)
{;}


ASIMRun::~ASIMRun()
{;}


void ASIMRun::Reset()
{
  RunID = TotalEvents = 0;
  ParticlesIncident = ParticlesBetweenThresholds = 0;
  PhotonsCreated = PhotonsDetected;
}


Double_t ASIMRun::GetDetectorEfficiency()
{
  if(ParticlesIncident == 0)
    return 0.;
  else
    return (ParticlesBetweenThresholds * 1. / ParticlesIncident);
}


Double_t ASIMRun::GetOpticalEfficiency()
{
  if(PhotonsCreated == 0)
    return 0.;
  else
    return (PhotonsDetected * 1. / PhotonsCreated);
}
