////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCbartenderDAQ                                                            //
//                                                                            //
// bartender DAQ class for GMCAnalyzer.                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "daqs/GMCbartenderDAQ.h"
#include "generated/GMCAnalyzer.h"
#include "generated/GMCConfig.h"

ClassImp(GMCbartenderDAQ)
//______________________________________________________________________________
GMCbartenderDAQ::GMCbartenderDAQ()
{
}

//______________________________________________________________________________
Bool_t GMCbartenderDAQ::Init()
{
   return true;
}

//______________________________________________________________________________
Bool_t GMCbartenderDAQ::BeginOfRun()
{
   return true;
}

//______________________________________________________________________________
Bool_t GMCbartenderDAQ::Event(Long64_t event)
{
   return true;
}

//______________________________________________________________________________
Long64_t GMCbartenderDAQ::Seek(Long64_t event)
{
   return -1;
}

//______________________________________________________________________________
Bool_t GMCbartenderDAQ::EndOfRun()
{
   return true;
}

//______________________________________________________________________________
Bool_t GMCbartenderDAQ::Terminate()
{
   return true;
}

