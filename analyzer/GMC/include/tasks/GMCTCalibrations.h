#ifndef GMCTCalibrations_H
#define GMCTCalibrations_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTCalibrations                                                           //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "generated/GMCTCalibrations_Base.h"

class GMCTCalibrations : public GMCTCalibrations_Base
{

private:
   GMCTCalibrations(const GMCTCalibrations &c); // not implemented
   GMCTCalibrations &operator=(const GMCTCalibrations &c); // not implemented

public:
   GMCTCalibrations(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTCalibrations_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTCalibrations() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();


   ClassDef(GMCTCalibrations,0)
};

#endif   // GMCTCalibrations_H
