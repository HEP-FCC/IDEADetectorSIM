#ifndef GMCTReadMCData_H
#define GMCTReadMCData_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTReadMCData                                                             //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "generated/GMCTReadMCData_Base.h"
#include "generated/GMCDCHHit.h"

class Geometry;

class GMCTReadMCData : public GMCTReadMCData_Base
{

private:
   GMCTReadMCData(const GMCTReadMCData &c); // not implemented
   GMCTReadMCData &operator=(const GMCTReadMCData &c); // not implemented

public:
   GMCTReadMCData(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTReadMCData_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTReadMCData() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();
   void LoadEvent(Int_t nev);  
   void DefineHitsInDCH();
   void DefineHitsInMPX();
   void DefineClustersInDCH(GMCDCHHit *ah, TVector3 pos, TVector3 trkDir);
   Bool_t DCACalculation(Int_t nCh, TVector3 pos, TVector3 trkDir);   

   TTree* fDataTree;
   TClonesArray* fBrDataTrk;
   TClonesArray* fBrDataDch;
   TClonesArray* fBrDataMpx;   
   TClonesArray* fDCHHitsMap;   
   Geometry*     fGeometry;

   Double_t      fDCA;      //distance of clesest approach
   Double_t      fDCA2d;    //distance of clesest approach in the transverse pl.
   TVector3      fCApoint;

   ClassDef(GMCTReadMCData,0)
};

#endif   // GMCTReadMCData_H
