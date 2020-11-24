#ifndef GMCTTrackFit_H
#define GMCTTrackFit_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTTrackFit                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TBranch.h"
#include "TMinuit.h"

#include "util/Geometry.h"
#include "generated/GMCTTrackFit_Base.h"

class GMCRecoTracks;

class GMCTTrackFit : public GMCTTrackFit_Base
{

private:
   GMCTTrackFit(const GMCTTrackFit &c); // not implemented
   GMCTTrackFit &operator=(const GMCTTrackFit &c); // not implemented

public:
   GMCTTrackFit(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTTrackFit_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTTrackFit() {}

protected:

   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();
   void LoadEvent(Int_t ev);
   void StoreReconstructedTrack();
   Bool_t PixelReconstruction();
   Bool_t ChamberReconstruction();
   Bool_t Fit(GMCRecoTracks *aTrack);

  TList *pixel_0,*pixel_1;
   TTree          *fDataTree;
//   TClonesArray   *fBrHitsDC;
   TClonesArray   *fBrHitsPX;
   Geometry       *fGeometry;
   TMinuit        *fMinuit;


   Double_t       fTheta;
   Double_t       fPhi;
   Double_t       fVertX;
   Double_t       fVertY;
//   Double_t       fChiSquare;
//   Double_t       fStatus;  

   ClassDef(GMCTTrackFit,0)
};

#endif   // GMCTTrackFit_H
