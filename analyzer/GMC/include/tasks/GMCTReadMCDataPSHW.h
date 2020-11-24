#ifndef GMCTReadMCDataPSHW_H
#define GMCTReadMCDataPSHW_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTReadMCDataPSHW                                                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <util/GeometrySrvHndl.h>


#include "generated/GMCTReadMCDataPSHW_Base.h"

#include "TClonesArray.h"
#include <map>

class GMCPSHWGeantStep;

class GMCTReadMCDataPSHW : public GMCTReadMCDataPSHW_Base
{

private:
   GMCTReadMCDataPSHW(const GMCTReadMCDataPSHW &c); // not implemented
   GMCTReadMCDataPSHW &operator=(const GMCTReadMCDataPSHW &c); // not implemented

public:
   GMCTReadMCDataPSHW(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTReadMCDataPSHW_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTReadMCDataPSHW() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();
   void LoadEvent(Int_t nev);
   void DefineHitsInPSHW();

   TClonesArray* fBrDataTrk;
   TClonesArray* fBrDataPshw;
   GeometrySrvHndl* fGeometry;

   typedef std::map<unsigned long,std::vector<GMCPSHWGeantStep *> > PshwChHitMap;

   void fillHitMap ( PshwChHitMap& hitmap );

   double _minimumEnergy;  // minimum energy deposition of G4 step

   ClassDef(GMCTReadMCDataPSHW,0)
};

#endif   // GMCTReadMCDataPSHW_H
