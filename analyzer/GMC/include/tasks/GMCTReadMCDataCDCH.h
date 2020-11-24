#ifndef GMCTReadMCDataCDCH_H
#define GMCTReadMCDataCDCH_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTReadMCData                                                             //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <util/GeometrySrvHndl.h>
//#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "generated/GMCTReadMCDataCDCH_Base.h"
#include "generated/GMCDCHHit.h"
#include <vector>
#include <map>

class GMCDCGeantStep;

class GMCTReadMCDataCDCH : public GMCTReadMCDataCDCH_Base
{

private:
   GMCTReadMCDataCDCH(const GMCTReadMCDataCDCH &c); // not implemented
   GMCTReadMCDataCDCH &operator=(const GMCTReadMCDataCDCH &c); // not implemented

public:
   GMCTReadMCDataCDCH(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTReadMCDataCDCH_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTReadMCDataCDCH() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();
   void LoadEvent(Int_t nev);  
   void DefineHitsInDCH();
   void DefineHitsInDCH2();
//   void DefineHitsInMPX();
   void DefineClustersInDCH(GMCDCHHit *ah, TVector3 &pos, TVector3 &trkDir);
   Bool_t DCACalculation(Int_t nCh, TVector3 &pos, TVector3 &trkDir);

//   TTree* fDataTree;
   TClonesArray* fBrDataTrk;
   TClonesArray* fBrDataDch;
//   TClonesArray* fBrDataMpx;
//   TClonesArray* fDCHHitsMap;
   GeometrySrvHndl* fGeometry;

   Double_t      fDCA;      //distance of clesest approach
   Double_t      fDCA2d;    //distance of clesest approach in the transverse pl.
   TVector3      fCApoint;

   typedef std::map<int,std::vector<GMCDCGeantStep *> > DriftCellHitMap;

   void fillHitMap ( DriftCellHitMap& hitmap );

   // Parameters
   double _timetodist;     // const to convert delata t in delat z along the wire in mm/ns
   double _distSigma;      // sigma of dealta z in mm
   double _minimumEnergy;  // minimum energy deposition of G4 step
   double _minimumLength;  // is G4Step is shorter than this, consider it a point
   double _driftVelocity;
   double _driftSigma;
   double _minimumTimeGap;
   int    _nzHittingZone;
   bool   _zHittingZonePresent;
   std::vector<double> _zZoneLimits;
   std::vector<bool>   _zZoneActive;
   std::vector<double> _zZoneMinLiveTime;
   bool   _useDeltaT;
   int    _mapCellDTtype;

   ClassDef(GMCTReadMCDataCDCH,0)
};

#endif   // GMCTReadMCDataCDCH_H
