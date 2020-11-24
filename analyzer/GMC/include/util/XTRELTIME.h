#ifndef XTRELTIME_H
#define XTRELTIME_H

#include <TFile.h>
#include <TTree.h>
#include <TVector.h>
#include "XTREL.h"

class XTRELTIME : public XTREL{

public:
  XTRELTIME(Float_t rwire = 0.00125, Float_t rtube = 2., Float_t V = 2000., const char *Fname="storext.dat");
  XTRELTIME(const char *XTFname);
  void Init();
  ~XTRELTIME();

  ClassDef(XTRELTIME,4)  //XT relation use
};

#endif // XTRELTIME_H
