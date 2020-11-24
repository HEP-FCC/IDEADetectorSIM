#ifndef GMCTPhotonConvStudy_H
#define GMCTPhotonConvStudy_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTPhotonConvStudy                                                        //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "generated/GMCTPhotonConvStudy_Base.h"

#include <util/GeometrySrvHndl.h>

#include "TVector3.h"

#include <vector>
#include <map>
#include <utility>

struct pIDs {
	int TrkID;
	int nTrk;
};

struct phProp {
	pIDs ids;
	TVector3 mom;
	std::vector<TVector3> intVert;
	std::vector<pIDs> secPart;
};

struct partData {
	pIDs ids;
	double eStart;
	double eLossInRad;
	double eLossInScint;
	double timeScIn;
	TVector3 momStart;
	TVector3 vertScIn;
	TVector3 vertScOut;
	TVector3 momInSc;
	bool fitted;
	bool extrap;
	double mcMom0;
	TVector3 recMom0;
	TVector3 recMom;
	std::vector<int> dchHits;
	std::vector<int> dchHitsAllSL;
};

struct pairProp {
	int phId;
	TVector3 vert;
	partData elData;
	partData psData;
	int shellId;
};

struct vertChek {
	int nPart;
	TVector3 momIn[2];
	TVector3 momIn0[2];
	TVector3 xyzIn[2];
	TVector3 xyzIn0[2];
};

class GMCTPhotonConvStudy : public GMCTPhotonConvStudy_Base
{

private:
   GMCTPhotonConvStudy(const GMCTPhotonConvStudy &c); // not implemented
   GMCTPhotonConvStudy &operator=(const GMCTPhotonConvStudy &c); // not implemented

public:
   GMCTPhotonConvStudy(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTPhotonConvStudy_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTPhotonConvStudy() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void LoadEvent(/*Int_t nev*/);
   void EndOfRun();
   void Terminate();

   void InitMat();
   void FindShellRads();
   void FindPhotonInter();
   void FindPairLoss();
   void FindPairDCHHits();
   void FindFittableTracks();
   bool FitTrack(int iTrk);
   void FitPairs();
   void doPlot();

   GeometrySrvHndl   *fGeometry;
   TClonesArray   *fBrDataTrk;
   TClonesArray* fBrDataPHCV;
   TClonesArray* fBrDataPHCVRd;
   TClonesArray   *fBrHitsDC;

   double _zLength;
   int _nShells;
   std::vector<double> _sRadIn;
   std::vector<double> _sRadOut;
   std::vector<double> _sThick;
   std::vector<double> _sTheta;
   std::vector<int> _OSnSuperLayer;

   std::vector<phProp> _phPrps;
   std::vector<pairProp> _pairs;
   std::vector<vertChek> _pairsInfo;

   std::vector<std::pair<int,int> > _trkPairMap;  //iPair, part Type (0:elec, 1:posi)

   // Parameters
   double _minimumEnergy;  // minimum energy deposition of G4 step
   int _fDebug;
   bool _doFit;
   bool _fitOnlyFst;
   bool _doExtrapol;

   ClassDef(GMCTPhotonConvStudy,0)
};

#endif   // GMCTPhotonConvStudy_H
