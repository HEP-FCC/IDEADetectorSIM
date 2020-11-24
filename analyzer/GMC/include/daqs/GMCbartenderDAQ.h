#ifndef GMCbartenderDAQ_H
#define GMCbartenderDAQ_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCbartenderDAQ                                                            //
//                                                                            //
// bartender DAQ class for GMCAnalyzer.                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "ROMEDAQSystem.h"

class GMCbartenderDAQ : public ROMEDAQSystem
{

private:
   GMCbartenderDAQ(const GMCbartenderDAQ &c); // not implemented
   GMCbartenderDAQ &operator=(const GMCbartenderDAQ &c); // not implemented

public:
   GMCbartenderDAQ();
   virtual ~GMCbartenderDAQ() {}
   Int_t    GetTimeStamp() { return 0; }
   const char* GetName() const { return "bartender"; }

   Bool_t   Init();
   Bool_t   BeginOfRun();
   Bool_t   Event(Long64_t event);
   Long64_t Seek(Long64_t event);
   Bool_t   EndOfRun();
   Bool_t   Terminate();

   // Change 'kFALSE' to 'kTRUE' if trees must be filled in every events
   // When this is 'kFALSE', the program does not fill a tree unless any folders in the tree is modified.
   Bool_t   GetAlwaysFillTrees() const { return kFALSE; }

   ClassDef(GMCbartenderDAQ, 0)
};

#endif   // GMCbartenderDAQ_H
