// Author: Chiri Claudio

#ifndef GMCTGMCMain_H
#define GMCTGMCMain_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTGMCMain                                                                //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "generated/GMCTGMCMain_Base.h"

class GMCTGMCMain : public GMCTGMCMain_Base
{

private:
   GMCTGMCMain(const GMCTGMCMain &c); // not implemented
   GMCTGMCMain &operator=(const GMCTGMCMain &c); // not implemented

public:
   GMCTGMCMain(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTGMCMain_Base(name,title,level,taskSuffix,histoFolder) {}
   virtual ~GMCTGMCMain() {}

protected:
   // Event Methods
   void Init();
   void BeginOfRun();
   void Event();
   void EndOfRun();
   void Terminate();


   ClassDef(GMCTGMCMain,0) // Main task. This task does common initialization for all the tasks.
};

#endif   // GMCTGMCMain_H
