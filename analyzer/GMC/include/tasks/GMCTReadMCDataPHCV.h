#ifndef GMCTReadMCDataPHCV_H
#define GMCTReadMCDataPHCV_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTReadMCDataPHCV                                                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "generated/GMCTReadMCDataPHCV_Base.h"

class GMCTReadMCDataPHCV : public GMCTReadMCDataPHCV_Base
{

private:
   GMCTReadMCDataPHCV(const GMCTReadMCDataPHCV &c); // not implemented
   GMCTReadMCDataPHCV &operator=(const GMCTReadMCDataPHCV &c); // not implemented

public:
   GMCTReadMCDataPHCV(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTReadMCDataPHCV_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTReadMCDataPHCV() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();


   ClassDef(GMCTReadMCDataPHCV,0)
};

#endif   // GMCTReadMCDataPHCV_H
