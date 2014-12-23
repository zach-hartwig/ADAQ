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
//
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
  void Reset();

  Double_t GetDetectorEfficiency();
  Double_t GetOpticalEfficiency();

  // The ID of the detector event
  void SetRunID(Int_t RID) {RunID = RID;}
  Int_t GetRunID() {return RunID;}

  void SetParticlesIncident(Int_t DI) {ParticlesIncident = DI;}
  void AddToParticlesIncident(Int_t DI) {ParticlesIncident += DI;}
  Int_t GetParticlesIncident() {return ParticlesIncident;}

  void SetParticlesBetweenThresholds(Int_t PBT) {ParticlesBetweenThresholds = PBT;}
  void AddToParticlesBetweenThresholds(Int_t PBT) {ParticlesBetweenThresholds += PBT;}
  Int_t GetParticlesBetweenThresholds() {return ParticlesBetweenThresholds;}

  void SetDetectorLowerThresholdInMeV(Double_t DLT) {DetectorLowerThresholdInMeV = DLT;}
  Double_t GetDetectorLowerThresholdInMeV() {return DetectorLowerThresholdInMeV;}
  
  void SetDetectorUpperThresholdInMeV(Double_t DUT) {DetectorUpperThresholdInMeV = DUT;}
  Double_t GetDetectorUpperThresholdInMeV() {return DetectorUpperThresholdInMeV;}
  
  void SetPhotonsCreated(Int_t PC) {PhotonsCreated = PC;}
  void AddToPhotonsCreated(Int_t PC) {PhotonsCreated += PC;}
  Int_t GetPhotonsCreated() {return PhotonsCreated;}
  
  void SetPhotonsDetected(Int_t PC) {PhotonsDetected = PC;}
  void AddToPhotonsDetected(Int_t PC) {PhotonsDetected += PC;}
  Int_t GetPhotonsDetected() {return PhotonsDetected;}
  
private:
  Int_t RunID;
  
  Int_t ParticlesIncident, ParticlesBetweenThresholds;
  Double_t DetectorLowerThresholdInMeV, DetectorUpperThresholdInMeV;
  
  Int_t PhotonsCreated, PhotonsDetected;
  
  ClassDef(ADAQSimulationRun, 1);
};

#endif
