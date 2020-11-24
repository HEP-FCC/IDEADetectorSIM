#ifndef GMCTMCGenerator_H
#define GMCTMCGenerator_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTMCGenerator                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "generated/GMCTMCGenerator_Base.h"

#include "TVector3.h"

class GMCTMCGenerator : public GMCTMCGenerator_Base
{

private:
   GMCTMCGenerator(const GMCTMCGenerator &c); // not implemented
   GMCTMCGenerator &operator=(const GMCTMCGenerator &c); // not implemented

public:
   GMCTMCGenerator(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTMCGenerator_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTMCGenerator() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();

   Int_t Intersection(TVector3 c,TVector3 *p1, TVector3 *p2);
   void SetClustersGlobal(TVector3 c,TVector3 p1,TVector3 p2, Int_t tube);
   void GenerateHits();

   ClassDef(GMCTMCGenerator,0)
};

#endif   // GMCTMCGenerator_H
