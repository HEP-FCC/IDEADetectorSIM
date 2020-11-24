#ifndef GMCTReadMCDataSVX_H
#define GMCTReadMCDataSVX_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTReadMCDataSVX                                                          //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <util/GeometrySrvHndl.h>

#include "generated/GMCTReadMCDataSVX_Base.h"

#include "TClonesArray.h"
#include <map>

class GMCSVXGeantStep;

class GMCTReadMCDataSVX : public GMCTReadMCDataSVX_Base
{

private:
   GMCTReadMCDataSVX(const GMCTReadMCDataSVX &c); // not implemented
   GMCTReadMCDataSVX &operator=(const GMCTReadMCDataSVX &c); // not implemented

public:
   GMCTReadMCDataSVX(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTReadMCDataSVX_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTReadMCDataSVX() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();
   void LoadEvent(Int_t nev);
   void DefineHitsInSVX();

   TClonesArray* fBrDataTrk;
   TClonesArray* fBrDataSvx;
///   TClonesArray* fSVXHitsMap;
   GeometrySrvHndl* fGeometry;

   typedef std::map<unsigned long,std::vector<GMCSVXGeantStep *> > SvxChHitMap;

   void fillHitMap ( SvxChHitMap& hitmap );

   double _minimumEnergy;  // minimum energy deposition of G4 step

   ClassDef(GMCTReadMCDataSVX,0)
};

#endif   // GMCTReadMCDataSVX_H
