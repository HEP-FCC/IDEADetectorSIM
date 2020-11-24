#include "util/CreateClusters.h"
#include "util/ClusterSize.h"
#include "TRandom.h"
#include "TMath.h"

#include <iostream>
#include <vector>

using namespace std;

ClassImp(CreateClusters)

//______________________________________________________________
CreateClusters::CreateClusters() {

  fNClusters = 0;

  fClusterDistr = ClusterSize::GetInstance();

}

//_______________________________________________________________
CreateClusters::CreateClusters(Double_t len) {

  fClusterDistr = ClusterSize::GetInstance();

  Double_t meanOfcls = len*0.1*fClusterDistr->GetNrOfPrimary();

  printf(" ***** Number of Primary/cm = %3.1f \n",fClusterDistr->GetNrOfPrimary());
  
  fLengthTrk = len;
  fNClusters = 99;
  //  cout<<"pippo pappo media della poissoniana "<<meanOfcls<<"  "<<len<<endl;
  /*  while (fNClusters > 30 ) */fNClusters = gRandom->Poisson(meanOfcls);

  fClusterCharge = std::vector<Int_t> (fNClusters,0);

  int val;
  for (int j=0;j<fNClusters;j++) {
    Bool_t found = false;
    while(!found) {
      val = TMath::Nint(((TH1F*)fClusterDistr->GetDistribution())->GetRandom());
      if (val < 30) found = true;
    }
    fClusterCharge.at(j) = val;
  }
 
  GenerateClusters();

  //gaussian smearing on drift distance: sigma= 0.130mm
  GenerateSmearing();
}

//_______________________________________________________________
CreateClusters::CreateClusters(Double_t len, Double_t enedep) {

  fClusterDistr = ClusterSize::GetInstance();

  fLengthTrk = len;

  printf(" ***** Energy loss / Ionization Energy %5.2f eV / %5.2f eV \n",1.e06*enedep, fClusterDistr->GetIonizationEnergy());

  Double_t totEnergy = 1.e06*enedep;
  Int_t val;

  int icluster = 0;
  while (totEnergy >= fClusterDistr->GetIonizationEnergy()) {
    val = round(((TH1F*)fClusterDistr->GetDistribution())->GetRandom());

    fClusterCharge.insert(fClusterCharge.begin()+icluster,val);

    icluster++;

    totEnergy -= val*fClusterDistr->GetIonizationEnergy();

   }

  fNClusters = icluster;   

  GenerateClusters();

  //gaussian smearing on drift distance: sigma= 0.130mm
  GenerateSmearing();
}
//_______________________________________________________________
CreateClusters::~CreateClusters() {

}

//_______________________________________________________________
void CreateClusters::GenerateClusters() {

  Double_t  val;

  printf(" ----> Number of clusters generated = %d \n",fNClusters);

  //generate the cluster position along the particle track
  Bool_t check;
  Int_t nExtr = 0;
  
  for (int j=0;j<fNClusters;j++) {
    
    check = false;
    while (!check) {
      val = gRandom->Uniform(-1., fLengthTrk+1.);
      if (val >= 0. && val <= fLengthTrk) check = true;
    }
    
    
    for (int iex=0;iex<nExtr;iex++) {
      if (val < fClusterPos.at(iex)) {
	fClusterPos.insert(fClusterPos.begin()+iex,val);
	check = false;
	break;
      }
    }
    
    if (check) fClusterPos.push_back(val);
    nExtr++;
  }
  
}

//_______________________________________________________________
void CreateClusters::GenerateSmearing() {

  //gaussian smearing on drift distance: sigma= 0.130mm
  for (int i=0;i<fNClusters;i++) {
  
    fClusterSmearing.insert(fClusterSmearing.begin()+i,gRandom->Gaus(0.,0.130));

  }

}

//_______________________________________________________________
void CreateClusters::SetNClusters(Int_t npoints) {

  fNClusters = npoints;

}

