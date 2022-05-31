#ifndef DR_GEOMETRYHELPER_H
#define DR_GEOMETRYHELPER_H

#include <TVector3.h>
#include <TString.h>

#include <vector>

#include "DRCalo_FiberType.h"

class DR_GeometryHelper
{
 public: 
  DR_GeometryHelper();
  ~DR_GeometryHelper(){};
  TVector3 GetTowerVec(unsigned int index, char side);
  double GetThetaSepDeg() {return m_deltaTheta;}
  double GetThetaSepRad() {return TMath::DegToRad()*m_deltaTheta;}
  double GetPhiSepDeg() {return m_phiUnit;}
  double GetPhiSepRad() {return TMath::DegToRad()*m_phiUnit;}
  unsigned int GetNbOfBarrelTowers() {return m_NbOfBarrel;}
  unsigned int GetNbOfEndCapTowers() {return m_NbOfEndCap;}
  unsigned int GetNbOfTowers() {return m_totTower;}
  unsigned int GetNbOfPhiSlices() {return m_NZrot;}
  std::vector<double> calibcher(std::vector<double> vectorcher);
  std::vector<double> calibscin(std::vector<double> vectorscin);
  float GetCalibrationConstant(TVector3& direction, DRCalo_FiberType fiber);
  void PrintGeometry();
  void PrintCalibration();
 private:
  unsigned int m_NbOfBarrel;
  unsigned int m_NbOfEndCap;
  unsigned int m_NZrot;
  unsigned int m_totTower;
  double m_deltaTheta;
  double m_phiUnit;
  std::vector<float> m_scint_constCalib;
  std::vector<float> m_cher_constCalib;
};
  
#endif // #ifndef DR_GEOMETRYHELPER_H
