#ifndef CLUSTERSIZE_H
#define CLUSTERSIZE_H

#include "TObject.h"
#include "TH1F.h"

class ClusterSize : public TObject {
  
 protected:

  ClusterSize();
  virtual ~ClusterSize();


 private:
  static ClusterSize *Instance;

  void   SetGasMixtureParameters();
  
  
 public:

  static     ClusterSize *GetInstance();
  Double_t   GetNrOfPrimary()      const  { return fNrOfPrimary; }
  Double_t   GetIonizationEnergy() const  { return fIonizationEnergy; }
  TH1F*      GetDistribution()     const  { return fClusterDistr; }


  TH1F *fClusterDistr;
  Double_t  fNrOfPrimary;
  Double_t  fIonizationEnergy;

  ClassDef(ClusterSize,1) 
  
};

#endif
