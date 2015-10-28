/////////////////////////////////////////////////////////////////////////////////
//
// name: ASIMEvent.hh
// date: 20 Oct 15
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ASIMEvent class provides a generic container for the
//       essential event-level data of a particle detector that is
//       modelled within Geant4. The class is designed to provide a
//       straightforward and universal method of persistently storing
//       event-level data for later analysis within the ADAQ
//       framework. This class is designed to be used within the ROOT
//       framework and utilizes ROOT data types to ensure
//       compatibility with post-simulation analysis tools built using
//       ROOT and for portability between platforms.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ASIMEvent_hh__ 
#define __ASIMEvent_hh__ 1

#include <TObject.h>
#include <TString.h>

#include <vector>

class ASIMEvent : public TObject
{
public:
  ASIMEvent();
  ~ASIMEvent();
  
  // Method to initialize/reset all member data
  void Initialize();

  // The ID of the detector event
  void SetEventID(Int_t EID) {EventID = EID;}
  Int_t GetEventID() {return EventID;}

  // The ID of the simulation run
  void SetRunID(Int_t RID) {RunID = RID;}
  Int_t GetRunID() {return RunID;}

  // The total ionizing energy deposited
  void SetEnergyDep(Float_t ED) {EnergyDep = ED;}
  Float_t GetEnergyDep() {return EnergyDep;}

  // The number of scintillation/Cerenkov photons created
  void SetPhotonsCreated(Int_t PC) {PhotonsCreated = PC;}
  void IncrementPhotonsCreated() {PhotonsCreated++;}
  Int_t GetPhotonsCreated() {return PhotonsCreated;}
  
  // The number of scintillation/Cerenkov photons detected
  void SetPhotonsDetected(Int_t PD) {PhotonsDetected = PD;}
  void IncrementPhotonsDetected() {PhotonsDetected++;}
  Int_t GetPhotonsDetected() {return PhotonsDetected;}

  // Control of vector of scintillation/Cerenkov photon creation time
  void AddPhotonCreationTime(Double_t PCT) {PhotonCreationTime.push_back(PCT);}
  void ClearPhotonCreationTime() {PhotonCreationTime.clear();}
  std::vector<Double_t> GetPhotonCreationTime() {return PhotonCreationTime;}

  // Control of vector of scintillation/Cerenkov photon detection time
  void AddPhotonDetectionTime(Double_t PDT) {PhotonDetectionTime.push_back(PDT);}
  void ClearPhotonDetectionTime() {PhotonDetectionTime.clear();}
  std::vector<Double_t> GetPhotonDetectionTime() {return PhotonDetectionTime;}


private:
  // Event metadata
  Int_t EventID, RunID;

  // Generic detector data
  Float_t EnergyDep;

  // Scintillation/Cerenkov photon data
  Int_t PhotonsCreated;
  Int_t PhotonsDetected;
  std::vector<Double_t> PhotonCreationTime;
  std::vector<Double_t> PhotonDetectionTime;

  ClassDef(ASIMEvent, 1);
};

#endif
