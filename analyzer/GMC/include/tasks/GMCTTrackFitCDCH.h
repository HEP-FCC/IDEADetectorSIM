#ifndef GMCTTrackFitCDCH_H
#define GMCTTrackFitCDCH_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTTrackFitCDCH                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <util/GeometrySrvHndl.h>
#include "generated/GMCTTrackFitCDCH_Base.h"
#include "TVector3.h"

class GMCRecoTracks;

class GMCTTrackFitCDCH : public GMCTTrackFitCDCH_Base
{

private:
   GMCTTrackFitCDCH(const GMCTTrackFitCDCH &c); // not implemented
   GMCTTrackFitCDCH &operator=(const GMCTTrackFitCDCH &c); // not implemented

public:
   GMCTTrackFitCDCH(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTTrackFitCDCH_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTTrackFitCDCH() {}

protected:

   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();
   void StoreReconstructedTrack();
   Bool_t Fit(GMCRecoTracks *aTrack);
   Bool_t Fit2(GMCRecoTracks *aTrack);
   void MCFinder();

  GeometrySrvHndl   *fGeometry;
   TClonesArray   *fBrHitsDC;
   TClonesArray   *fBrHitsSVX;
   TClonesArray   *fBrHitsPSHW;
   TClonesArray   *fBrDataTrk;

   Double_t       fTheta;
   Double_t       fPhi;
   TVector3       fVert;
//   Double_t       fChiSquare;
//   Double_t       fStatus;  

   ClassDef(GMCTTrackFitCDCH,0)
};

#endif   // GMCTTrackFitCDCH_H
