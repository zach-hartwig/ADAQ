/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMRun.hh
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMRun class provides a generic container for the
//       essential run-level data of a particle detector that is
//       modelled within Geant4. The class is designed to provide a
//       straightforward and universal method of persistently storing
//       run-level data for later analysis within the ADAQ
//       framework. This class is designed to be used within the ROOT
//       framework and utilizes ROOT data types to ensure
//       compatibility with post-simulation analysis tools built using
//       ROOT and for portability between platforms.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ASIMRun_hh__ 
#define __ASIMRun_hh__ 1

#include <TObject.h>
#include <TString.h>

#include <vector>

class ASIMRun : public TObject
{
public:
  ASIMRun();
  ~ASIMRun();
  
  // Method to initialize/reset all member data
  void Reset();

  Double_t GetDetectorEfficiency();
  Double_t GetOpticalEfficiency();

  // The ID of the detector event
  void SetRunID(Int_t RID) {RunID = RID;}
  Int_t GetRunID() {return RunID;}

  void SetTotalEvents(Int_t TE) {TotalEvents  = TE;}
  Int_t GetTotalEvents() {return TotalEvents;}

  void SetParticlesIncident(Int_t DI) {ParticlesIncident = DI;}
  void AddToParticlesIncident(Int_t DI) {ParticlesIncident += DI;}
  Int_t GetParticlesIncident() {return ParticlesIncident;}

  void SetParticlesBetweenThresholds(Int_t PBT) {ParticlesBetweenThresholds = PBT;}
  void AddToParticlesBetweenThresholds(Int_t PBT) {ParticlesBetweenThresholds += PBT;}
  Int_t GetParticlesBetweenThresholds() {return ParticlesBetweenThresholds;}

  void SetLowerThresholdInMeV(Double_t LT) {LowerThresholdInMeV = LT;}
  Double_t GetLowerThresholdInMeV() {return LowerThresholdInMeV;}
  
  void SetUpperThresholdInMeV(Double_t UT) {UpperThresholdInMeV = UT;}
  Double_t GetUpperThresholdInMeV() {return UpperThresholdInMeV;}
  
  void SetPhotonsCreated(Int_t PC) {PhotonsCreated = PC;}
  void AddToPhotonsCreated(Int_t PC) {PhotonsCreated += PC;}
  Int_t GetPhotonsCreated() {return PhotonsCreated;}
  
  void SetPhotonsDetected(Int_t PC) {PhotonsDetected = PC;}
  void AddToPhotonsDetected(Int_t PC) {PhotonsDetected += PC;}
  Int_t GetPhotonsDetected() {return PhotonsDetected;}
  
private:
  Int_t RunID, TotalEvents;
  Int_t ParticlesIncident, ParticlesBetweenThresholds;
  Double_t LowerThresholdInMeV, UpperThresholdInMeV;
  Int_t PhotonsCreated, PhotonsDetected;
  
  ClassDef(ASIMRun, 1);
};

#endif
