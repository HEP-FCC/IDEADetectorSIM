
#include "util/ClusterSize.h"
#include "generated/GMCAnalyzer.h"
#include "generated/GMCGlobalSteering.h"

#include "TH1F.h"
#include <iostream>

ClassImp(ClusterSize)

const Double_t fIonEnergy_HE = 41.;   //eV
const Double_t fIonEnergy_Isob = 23.;   //eV
const Double_t fNrPrimAtCm_HE = 4.2;   //primary/cm
const Double_t fNrPrimAtCm_Isob = 84;   //primary/cm


ClusterSize* ClusterSize::Instance = 0;

//___________________________________________________________________________
ClusterSize::ClusterSize() {

  //cluster size from blum

   fClusterDistr = new TH1F("fClusterDistr","ClusterDistrution Hist",100,0.5,100.5);
 
   fClusterDistr->SetBinContent(1,76.600);
   fClusterDistr->SetBinContent(2,12.500);
   fClusterDistr->SetBinContent(3,4.600);
   fClusterDistr->SetBinContent(4,2.000);
   fClusterDistr->SetBinContent(5,1.200);
   fClusterDistr->SetBinContent(6,0.750);
   fClusterDistr->SetBinContent(7,0.500);
   fClusterDistr->SetBinContent(8,0.3360);
   fClusterDistr->SetBinContent(9,0.250);
   fClusterDistr->SetBinContent(10,0.190);
   fClusterDistr->SetBinContent(11,0.140);
   fClusterDistr->SetBinContent(12,0.100);
   fClusterDistr->SetBinContent(13,0.080);
   fClusterDistr->SetBinContent(14,0.060);
   fClusterDistr->SetBinContent(15,0.048);
   fClusterDistr->SetBinContent(16,0.043);
   fClusterDistr->SetBinContent(17,0.038);
   fClusterDistr->SetBinContent(18,0.034);
   fClusterDistr->SetBinContent(19,0.030);

   for (int i=20;i<101;i++) fClusterDistr->SetBinContent(i,10.9/(i*i));

   fClusterDistr->Scale(100./fClusterDistr->Integral());

   SetGasMixtureParameters();
}

//___________________________________________________________________________
ClusterSize::~ClusterSize() {

  delete fClusterDistr;
}

//___________________________________________________________________________
ClusterSize *ClusterSize::GetInstance() {

  if (Instance) return Instance; 

  Instance = new ClusterSize();
  return Instance;

}

//___________________________________________________________________________
void ClusterSize::SetGasMixtureParameters() {

  TString mixture = gAnalyzer->GetGSP()->GetGasMixture();

  TString helium = mixture(0,2);
  Double_t fHe = 0.01*helium.Atof();
  Double_t fIsob = 1. - fHe;

  fNrOfPrimary = fNrPrimAtCm_HE*fHe + fNrPrimAtCm_Isob*fIsob;
  fIonizationEnergy = fIonEnergy_HE*fHe + fIonEnergy_Isob*fIsob;
  
}

