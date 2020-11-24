#ifndef XTDATAVLGAS_H
#define XTDATAVLGAS_H

#include <TObject.h>
#include <Riostream.h>

class XTDataVlGas : public TObject {
  public:
    XTDataVlGas();
    ~XTDataVlGas();

  public:
    void SetNpoints    (Int_t lNpoints)       { Npoints = lNpoints;}
    void SetRwire      (Float_t lRwire)       { Rwire = lRwire;}
    void SetInnerRtube (Float_t lInnerRtube)  { InnerRtube = lInnerRtube;}
    void SetVolts      (Float_t lVolts)       { Volts = lVolts;}
    void SetTime       (Float_t *lTime)       { Time = lTime;}
    void SetDist       (Float_t *lDist)       { Dist = lDist;}
    void SetDiffL      (Float_t *lDiffL)      { DiffL = lDiffL;}
    void SetDiffT      (Float_t *lDiffT)      { DiffT = lDiffT;}
    void SetCoeffDiffL (Float_t *lCoeffDiffL) { CoeffDiffL = lCoeffDiffL;}
    void SetCoeffDiffT (Float_t *lCoeffDiffT) { CoeffDiffT = lCoeffDiffT;}

    Int_t    GetNpoints()    { return Npoints;}
    Float_t  GetRwire()      { return Rwire;}
    Float_t  GetInnerRtube() { return InnerRtube;}
    Float_t  GetVolts()      { return Volts;}
    Float_t* GetTime()       { return Time;}
    Float_t* GetDist()       { return Dist;}
    Float_t* GetDiffL()	     { return DiffL;}
    Float_t* GetDiffT()	     { return DiffT;}
    Float_t* GetCoeffDiffL() { return CoeffDiffL;}
    Float_t* GetCoeffDiffT() { return CoeffDiffT;}

  private:
    Int_t   Npoints;     //Npoints
    Float_t Rwire;       //Rwire
    Float_t InnerRtube;  //InnerRtube
    Float_t Volts;       //Volts
    Float_t *Time;       //[Npoints]  Time
    Float_t *Dist;       //[Npoints]  Dist
    Float_t *DiffL;      //[Npoints]  DiffL
    Float_t *DiffT;      //[Npoints]  DiffT
    Float_t *CoeffDiffL; //[Npoints]  CoeffDiffL
    Float_t *CoeffDiffT; //[Npoints]  CoeffDiffT

  ClassDef(XTDataVlGas,1)
  
};

#endif // XTDATAVLGAS_H
