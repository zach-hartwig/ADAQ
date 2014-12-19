/////////////////////////////////////////////////////////////////////////////////
//
// name: ADAQEvent.hh
// date: 19 Dec 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: The ADAQEvent C++ class provides a generic container for the
//       essential event-level data of a particle detector that is
//       modelled with Monte Carlo particle transport methods. This
//       class is designed to provide a straightforward and universal
//       method of persistently storing event-level data for later
//       analysis within the ADAQ framework. For a ROOT-based event
//       class see $ADAQ/classes/root/ADAQRootEvent.hh.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __ADAQEvent_hh__ 
#define __ADAQEvent_hh__ 1

#include <vector>

class ADAQEvent
{
public:
  ADAQEvent() {Initialize();}
  ~ADAQEvent();

  // Method to initialize/reset all member data
  void Initialize(){
    EventID = RunID = 0;
    TotalEDep = 0.;
    PhotonsCreated = PhotonDetected = 0;
    PhotonCreationTime.clear();
    PhotonDetectionTime.clear();
    VertexPos[0] = VertexPos[1] = VertexPos[2] = 0.;
    VertexMomDir[0] = VertexMomDir[1] = VertexMomDir[2] = 0.;
    VertexKE = 0.;
    VertexPCode = 0;
  }

  // The ID of the detector event
  void SetEventID(int EID) {EventID = EID;}
  int GetEventID() {return EventID;}

  // The ID of the simulation run
  void SetRunID(int RID) {RunID = RID;}
  int GetRunID() {return RunID;}

  // The total ionizing energy deposited
  void SetTotalEDep(double TED) {TotalEDep = TED;}
  double GetTotalEDep() {return TotalEDep;}

  // The number of scintillation/Cerenkov photons created
  void SetPhotonsCreated(int PC) {PhotonsCreated = PC;}
  int GetPhotonsCreated() {return PhotonsCreated;}

  // The number of scintillation/Cerenkov photons detected
  void SetPhotonsDetected(int PD) {PhotonsDetected = PD;}
  int GetPhotonsDetected() {return PhotonsDetected;}

  // Control of vector of scintillation/Cerenkov photon creation time
  void AddPhotonCreationTime(double PCT) {PhotonCreationTime.push_back(PCT);}
  void ClearPhotonCreationTime() {PhotonCreationTime.clear();}
  vector<double> GetPhotonCreationTime() {return PhotonCreationTime;}

  // Control of vector of scintillation/Cerenkov photon detection time
  void AddPhotonDetectionTime(double PDT) {PhotonDetectionTime.push_back(PDT);}
  void ClearPhotonDetectionTime() {PhotonDetectionTime.clear();}
  vector<double> GetPhotonDetectionTime() {return PhotonDetectionTime;}

  // The position of the vertex (track ID == 0) particle
  void SetVertexPos(double VX, double VY, double VZ)
  {VertexPos[0] = VX; VertexPos[1] = VY; VertexPos[2] = VZ;}

  // The momentum direction of the vertex particle
  void SetVertexMomDir(double PX, double PY, double PZ)
  {VertexMomDir[0] = PX; VertexMomDir[1] = PY; VertexPosDir[2] = PZ;}

  // The kinetic energy of the vertex particle particle
  void SetVertexKE(double VKE) {VertexKE = VKE;}
  double GetVertexKE() {return VertexKE;}

  // A unique integer particle ID of the vertex particle
  void SetVertexPCode(int VPC) {VertexPCode = VPC;}
  int GetVertexPCode() {return VertexPCode;}
  

private:

  // Event metadata
  int EventID, RunID;

  // Generic detector data
  double TotalEDep;

  // Scintillation/Cerenkov photon data
  int photonsCreated;
  int photonsDetected;
  vector<double> photonCreationTime;
  vector<double_t> photonDetectionTime;

  // Vertex particle (event track ID == 0) data
  double VertexPos[3];
  double VertexMomDir[3];
  double VertexKE;
  int VertexPCode;
};
