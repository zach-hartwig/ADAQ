/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQSimulationEvent.hh
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

/////////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQSimulationEvent_hh__ 
#define __ADAQSimulationEvent_hh__ 1

#include <TObject.h>
#include <TString.h>

#include <vector>

class ADAQSimulationEvent : public TObject
{
public:
  ADAQSimulationEvent();
  ~ADAQSimulationEvent();
  
  // Method to initialize/reset all member data
  void Initialize();

  // The ID of the detector event
  void SetEventID(Int_t EID) {EventID = EID;}
  Int_t GetEventID() {return EventID;}

  // The ID of the simulation run
  void SetRunID(Int_t RID) {RunID = RID;}
  Int_t GetRunID() {return RunID;}

  // The total ionizing energy deposited
  void SetEnergyDep(Double_t ED) {EnergyDep = ED;}
  Double_t GetEnergyDep() {return EnergyDep;}

  // The number of scintillation/Cerenkov photons created
  void SetPhotonsCreated(Int_t PC) {PhotonsCreated = PC;}
  Int_t GetPhotonsCreated() {return PhotonsCreated;}

  // The number of scintillation/Cerenkov photons detected
  void SetPhotonsDetected(Int_t PD) {PhotonsDetected = PD;}
  Int_t GetPhotonsDetected() {return PhotonsDetected;}

  // Control of vector of scintillation/Cerenkov photon creation time
  void AddPhotonCreationTime(Double_t PCT) {PhotonCreationTime.push_back(PCT);}
  void ClearPhotonCreationTime() {PhotonCreationTime.clear();}
  std::vector<Double_t> GetPhotonCreationTime() {return PhotonCreationTime;}

  // Control of vector of scintillation/Cerenkov photon detection time
  void AddPhotonDetectionTime(Double_t PDT) {PhotonDetectionTime.push_back(PDT);}
  void ClearPhotonDetectionTime() {PhotonDetectionTime.clear();}
  std::vector<Double_t> GetPhotonDetectionTime() {return PhotonDetectionTime;}

  // The position of the vertex (track ID == 0) particle
  void SetVertexPos(Double_t VX, Double_t VY, Double_t VZ)
  {VertexPos[0] = VX; VertexPos[1] = VY; VertexPos[2] = VZ;}

  // The momentum direction of the vertex particle
  void SetVertexMomDir(Double_t PX, Double_t PY, Double_t PZ)
  {VertexMomDir[0] = PX; VertexMomDir[1] = PY; VertexMomDir[2] = PZ;}

  // The kinetic energy of the vertex particle particle
  void SetVertexKE(Double_t VKE) {VertexKE = VKE;}
  Double_t GetVertexKE() {return VertexKE;}

  // A unique integer particle ID of the vertex particle
  void SetVertexPCode(Int_t VPC) {VertexPCode = VPC;}
  Int_t GetVertexPCode() {return VertexPCode;}
  

private:
  // Event metadata
  Int_t EventID, RunID;

  // Generic detector data
  Double_t EnergyDep;

  // Scintillation/Cerenkov photon data
  Int_t PhotonsCreated;
  Int_t PhotonsDetected;
  std::vector<Double_t> PhotonCreationTime;
  std::vector<Double_t> PhotonDetectionTime;

  // Vertex particle (event track ID == 0) data
  Double_t VertexPos[3];
  Double_t VertexMomDir[3];
  Double_t VertexKE;
  Int_t VertexPCode;

  ClassDef(ADAQSimulationEvent, 1);
};

#endif
