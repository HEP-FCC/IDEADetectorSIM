#ifndef XTREL_H
#define XTREL_H

#include <TFile.h>
#include <TTree.h>
#include <TVector.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include "XTDataVlGas.h"

class XTREL : public TObject{

public:
  //  XTREL(); //default constructor (init an analytic function by C.Chiri)

  XTREL(Float_t rwire = 0.00125, Float_t rtube = 2., Float_t V = 2000., const char *Fname="storext.dat");
  XTREL(const char *XTFname);
  virtual void Init(){}
  void Init(Int_t nbin, Float_t *tr, Float_t *tt, Float_t *tdl=0x0, Float_t *tdt=0x0, Float_t *tsdl=0x0, Float_t *tsdt=0x0);
  void Init(Int_t nbin, Double_t *tr, Double_t *tt, Double_t *tdl=0x0, Double_t *tdt=0x0, Double_t *tsdl=0x0, Double_t *tsdt=0x0);
//   void Close();
  ~XTREL();
  virtual void WriteXT();
  virtual void OpenXT();
  virtual Float_t* xtadjust(Float_t time, Int_t version=0);
  Float_t* timeToSpace(Float_t time, Int_t version=0);
  virtual Float_t* spaceToTime(Float_t dist, Int_t version=0);
  Int_t GetNpoints()      {return xtnbin;}
  Float_t *GetDistData()  {return Xxtx;}
  Float_t *GetTimeData()  {return Xxttime;}
  Float_t *GetDiffLData() {return XDiffL;}
  TF1 *GetFitFunc() {return xtFunc;}
  TGraphErrors *GetXTGraph() {return xtGraph;}
  
  Float_t GetInnerRtube() {return InnerRtube;}
  Float_t GetRwire() {return Rwire;}
  Float_t GetVolts() {return Volts;}
  
  //  Double_t DirectXT(Double_t dist);

protected:
  const char *filename;
  TString xtfilename;
  Float_t pos, minpos;
  Float_t drifttime, mindrifttime, firsttime;
  Int_t xtnbin;
  Int_t curentr;
  Float_t InnerRtube, Rwire, Volts;


  Float_t time, mintime;
  Float_t driftdist, mindriftdist, firstdist;
  Int_t curentrSpc;
  Float_t maxDrfTime;


  TFile *trxtf;
  TTree *trdata, *trxt;
  TVectorF *radius, *integrTime, *integrDiffL, *integrDiffT, *coeffDiffL, *coeffDiffT;
  Float_t *Xxtx, *Xxttime, *XDiffL, *XDiffT, *XcoeffDiffL, *XcoeffDiffT;
  Float_t Emax, Emin, Econst;
  Float_t *E, *Vdrift, *DiffL, *DiffT;
  XTDataVlGas *xtData;
  Float_t *x;
  
  Int_t relType;
  
  TGraphErrors *xtGraph;
  Float_t *xtGraph_X;
  Float_t *xtGraph_ErrX;
  Float_t *xtGraph_Y;
  Float_t *xtGraph_ErrY;
  TF1 *xtFunc;
  TF1 *dXtFunc;

//   TFile *xtf;
//   std::vector <Float_t> xtx, xttime;
//   XTDataVl *xtData;


  ClassDef(XTREL,1)  //XT relation use
};

#endif // XTREL_H
