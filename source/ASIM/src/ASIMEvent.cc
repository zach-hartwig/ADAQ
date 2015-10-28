/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMEvent.cc
// date: 23 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMEvent class provides a generic container for the
//       essential event-level data of a particle detector that is
//       modelled with Monte Carlo particle transport methods. This
//       class is designed to provide a straightforward and universal
//       method of persistently storing event-level data for later
//       analysis within the ADAQ framework. This class is designed to
//       be used within the ROOT framework and utilizes ROOT data
//       types to ensure compatibility with post-simulation analysis
//       tools built using ROOT and for portability between platforms.
//
/////////////////////////////////////////////////////////////////////////////////

#include "ASIMEvent.hh"

ASIMEvent::ASIMEvent()
{ Initialize(); }


ASIMEvent::~ASIMEvent()
{;}


void ASIMEvent::Initialize()
{
  EventID = 0;
  RunID = 0;
  EnergyDep = 0.;
  PhotonsCreated = 0;
  PhotonsDetected = 0;
  PhotonCreationTime.clear();
  PhotonDetectionTime.clear();
}
