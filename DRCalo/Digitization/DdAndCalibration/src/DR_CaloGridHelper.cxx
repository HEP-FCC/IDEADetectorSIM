#include "DR_CaloGridHelper.h"

#include <TMath.h>
#include <TVector3.h>
#include <TCanvas.h>

#include <iostream>

DR_CaloGridHelper::DR_CaloGridHelper(double distPar):
  m_rm(distPar), m_caloEffDist(2500.),m_delta(0), m_spacing(1000000),m_maxBinPhi(0)
{
}

void DR_CaloGridHelper::CreateGrid()
{
  m_delta = TMath::Sqrt(TMath::Pi()) * m_rm/m_caloEffDist; // This follows from requiring the area of a circle of radius m_rm being equal to the element of area m_caloEffDist^2 * d cos theta * dphi and setting both d cos theta and dphi equal to m_delta
  m_maxBinPhi = static_cast<DR_GridID>(2*TMath::Pi()/m_delta);
  std::cout << m_delta << std::endl;
}

DR_GridID DR_CaloGridHelper::GetID(float theta, float phi) // Theta and Phi both in rads 
{
  // Make sure that theta and phi are defined between 0 and pi and 0-2pi
  while (phi > 2*TMath::Pi()){
    phi = phi - 2*TMath::Pi();
  }
  while (phi < 0) {
    phi = phi + 2*TMath::Pi();
  }
  
  if (theta < 0) {
    std::cerr << "DR_CaloGridHelper::GetID: theta should be positively defined and instead it is " << theta << std::endl;
    std::cerr << "Things are going to go horribly wrong" << std::endl;
    return 0;
  }
  if (m_delta == 0){
    std::cerr << "DR_CaloGridHelper::GetID : m_delta has an illegal value " << m_delta << ". Perhaps you forgot to call CreateGrid?" << std::endl;
    return 0;
  }
  
  DR_GridID   int_phi = static_cast<DR_GridID>(phi/m_delta);  
  DR_GridID  int_costheta = static_cast<DR_GridID>((TMath::Cos(theta)+1.)/m_delta);

  return  int_phi + int_costheta*m_spacing;
}

double DR_CaloGridHelper::GetCosTheta(DR_GridID id)
{
  return static_cast<double>((id/m_spacing)*m_delta) -1;
}

double DR_CaloGridHelper::GetTheta(DR_GridID id)
{
  return TMath::ACos(GetCosTheta(id));
}

double DR_CaloGridHelper::GetPhi(DR_GridID id)
{
  double retval = (id%m_spacing)*m_delta;
  if (retval > 2*TMath::Pi() || retval < 0){
    std::cout << "WARNING: anomalous value of " << retval << " for phi, which should be between 0 and 2Pi" << std::endl;
  }
  if (retval > TMath::Pi()) retval = retval - 2*TMath::Pi();
  return retval;
}

double DR_CaloGridHelper::GetEnergy(DR_GridID id)
{

  if (m_caloGrid.find(id) == m_caloGrid.end()) return 0;
  double retval;
  retval = 0;  
  for (edm4hep::ConstCalorimeterHit * hit : m_caloGrid[id]){
    retval += hit->getEnergy();
  }
  return retval;
}

double DR_CaloGridHelper::GetEnergy(float theta, float phi)
{
  return GetEnergy(GetID(theta,phi));
}

std::vector<DR_GridID> DR_CaloGridHelper::ListOfAdjacentCellID(DR_GridID l_ID)
{
  std::vector<DR_GridID> retval;
  retval.clear();
  // For phi need to pay attention at the cut
  DR_GridID cosThetaBin,phiBin;
  cosThetaBin = l_ID/m_spacing;
  phiBin = l_ID%m_spacing;
  DR_GridID nextPhi, previousPhi;
  nextPhi = 0;
  previousPhi = phiBin - 1;
  
  if (phiBin >= m_maxBinPhi) {
    nextPhi = 0; // Crossed max bin in phi, restart counting from 0
    if (phiBin > m_maxBinPhi) std::cout << "WTF????????????" << std::endl;
  } else {
    nextPhi = phiBin + 1;
  }
  if (phiBin == 0) previousPhi = m_maxBinPhi;
  else previousPhi = phiBin - 1;
  
 
  retval.push_back((cosThetaBin * m_spacing) + nextPhi);
  retval.push_back((cosThetaBin * m_spacing) + previousPhi);
  retval.push_back(((cosThetaBin+1) * m_spacing) + phiBin);
  retval.push_back(((cosThetaBin+1) * m_spacing) + nextPhi);
  retval.push_back(((cosThetaBin+1) * m_spacing) + previousPhi);
  retval.push_back(((cosThetaBin-1) * m_spacing) + phiBin);
  retval.push_back(((cosThetaBin-1) * m_spacing) + nextPhi);
  retval.push_back(((cosThetaBin-1) * m_spacing) + previousPhi);

  return retval;
}

/*void DR_CaloGridHelper::Add(float theta, float phi, float energy)
{
  static DR_GridID id;
  id = GetID(theta,phi);
  if (m_caloGrid.find(id) == m_caloGrid.end()) {
    CreateEntry(id);
  }
  m_caloGrid[id] = energy + GetEnergy(id); // This now assumes that the individual elements of the grid should be massless. I believe this is the right thing to do (for example, it wouldn't make sense to sum the 4-vectors of the individual fiber, if the fibers arise from a single particle). In any case, the effect should be very small for small size of the grid elements
  }*/

void DR_CaloGridHelper::Add(edm4hep::ConstCalorimeterHit * caloHit)
{
  static TVector3 l_hit;
  l_hit.SetXYZ(
	       caloHit->getPosition().x,
	       caloHit->getPosition().y,
	       caloHit->getPosition().z
	       );
  
  static DR_GridID id;
  id = GetID(l_hit.Theta(),l_hit.Phi());
  if (m_caloGrid.find(id) == m_caloGrid.end()) {
    CreateEntry(id);
  }
  m_caloGrid[id].push_back(caloHit);
}


void DR_CaloGridHelper::CreateEntry(DR_GridID gridID)
{
  m_caloGrid[gridID] = DR_CaloHitVec();

  /*m_caloGrid[gridID] = edm4hep::ClusterCollection();
  auto l_cluster = m_caloGrid[gridID].create();

  static TVector3 l_vec;
    // Set the position of the cluster half an m_delta away (in phi and costheta) from the bin edge 
  l_vec.SetMagThetaPhi(
		       m_caloEffDist,
		       (GetTheta(gridID) + GetTheta(gridID + m_spacing))/2.,
		       (GetPhi(gridID) + GetPhi(gridID + 1))/2.
		       );
  
 l_cluster.setEnergy(0);
  std::cout << "About to set the cluster position to " << l_vec.X() << '\t' << l_vec.Y() << '\t' << l_vec.Z() << std::endl;
  std::cout << "Corresponding to theta " << l_vec.Theta() << " phi " << l_vec.Phi() << std::endl;
  l_cluster.setPosition( {float(l_vec.X()),float(l_vec.Y()),float(l_vec.Z()) } );
  std::cout << "pippo "<< l_cluster.getPosition().x << '\t' << l_cluster.getPosition().y << '\t' << l_cluster.getPosition().z << std::endl; */
  
}

void DR_CaloGridHelper::Print()
{
  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << "DR_CaloGridHelper current status:\n\n\n" << std::endl;
  for (auto itr = m_caloGrid.begin(); itr != m_caloGrid.end(); ++itr){
    std::cout << "ID " << itr->first << " Theta = " << GetTheta(itr->first) << " Phi = " << GetPhi(itr->first) << " E = " << GetEnergy(itr->first) << std::endl;
  }
  std::cout << "--------------------------------------------------------" << std::endl;
}

void DR_CaloGridHelper::Reset()
{
  m_caloGrid.clear();
}

void DR_CaloGridHelper::EventDisplay(TString filename, float minCosTheta, float maxCosTheta, float minPhi, float maxPhi)
{

  int NbinsX = static_cast<int>((maxCosTheta - minCosTheta)/m_delta);
  int NbinsY = static_cast<int>((maxPhi - minPhi)/m_delta);

  TH2F h("h","",NbinsX,minCosTheta,maxCosTheta,NbinsY,minPhi,maxPhi);

  for (auto itr = m_caloGrid.begin(); itr != m_caloGrid.end(); ++itr){
    h.Fill(GetCosTheta(itr->first),GetPhi(itr->first),GetEnergy(itr->first));
  }

  TCanvas c;
  
  h.Draw("COLZ");

  c.Print(filename);
}
  
