#ifndef CREATECLUSTERS_H
#define CREATECLUSTERS_H

#include "TObject.h" 
#include "TH1F.h"
#include "util/ClusterSize.h"

#include <vector>


class CreateClusters : public TObject {

 protected:

  Int_t fNClusters;
  Double_t fLengthTrk;
  std::vector<Double_t >  fClusterPos;
  std::vector<Int_t >  fClusterCharge;  //multiple of elementary charge
  std::vector<Double_t >  fClusterSmearing;

  ClusterSize *fClusterDistr;

 public:

  CreateClusters();
  CreateClusters(Double_t length);
  CreateClusters(Double_t len, Double_t enedep);
  virtual ~CreateClusters();

  void       GenerateClusters();
  void       GenerateSmearing();
  void       SetNClusters(Int_t npoints);

  Int_t      GetNrClusters()                  const  { return fNClusters; }
  Double_t   GetClusterPositionAt(Int_t idx)  const  { return fClusterPos.at(idx); }
  Int_t      GetClusterChargeAt(Int_t idx)    const  { return fClusterCharge.at(idx); }
  Double_t   GetClusterSmearingAt(Int_t idx)  const  { return fClusterSmearing.at(idx); }
  

  std::vector<Double_t > &GetClusterPosition()  { return fClusterPos; }
  std::vector<Int_t >    &GetClusterCharge()    { return fClusterCharge; }
  std::vector<Double_t > &GetClusterSmearing()  { return fClusterSmearing; }
  

  
  ClassDef(CreateClusters,1) //Waveform class
  
};

#endif 
