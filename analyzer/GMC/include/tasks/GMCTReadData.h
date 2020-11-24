#ifndef GMCTReadData_H
#define GMCTReadData_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTReadData                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "util/Geometry.h"

#include "generated/GMCTReadData_Base.h"

class GMCTReadData : public GMCTReadData_Base
{


 protected:
  Geometry *fGeom;

private:
   GMCTReadData(const GMCTReadData &c); // not implemented
   GMCTReadData &operator=(const GMCTReadData &c); // not implemented

public:
   GMCTReadData(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTReadData_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTReadData() {}

protected:

   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();

   ClassDef(GMCTReadData,0)
};

#endif   // GMCTReadData_H
