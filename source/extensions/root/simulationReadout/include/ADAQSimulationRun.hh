/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQSimulationRun.hh
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

/////////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQSimulationRun_hh__ 
#define __ADAQSimulationRun_hh__ 1

#include <TObject.h>
#include <TString.h>

#include <vector>

class ADAQSimulationRun : public TObject
{
public:
  ADAQSimulationRun();
  ~ADAQSimulationRun();
  
  // Method to initialize/reset all member data
  void Initialize();

  // The ID of the detector event
  void SetRunID(Int_t RID) {RunID = RID;}
  Int_t GetRunID() {return RunID;}

private:
  // Run metadata
  Int_t RunID;
  
  ClassDef(ADAQSimulationRun, 1);
};

#endif
