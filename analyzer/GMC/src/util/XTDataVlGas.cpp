#include "XTDataVlGas.h"

ClassImp(XTDataVlGas)

XTDataVlGas::XTDataVlGas(){
  Npoints=0;
  Rwire = 0.;
  InnerRtube = 0.;
  Volts = 0.;
  Time = 0x0;
  Dist = 0x0;
  DiffL = 0x0;
  DiffT = 0x0;
  CoeffDiffL = 0x0;
  CoeffDiffT = 0x0;
}

XTDataVlGas::~XTDataVlGas(){
  Npoints=0;
  Rwire = 0.;
  InnerRtube = 0.;
  Volts = 0.;
  if (Time) delete [] Time;
  if (Dist) delete [] Dist;
  if (DiffL) delete [] DiffL;
  if (DiffT) delete [] DiffT;
  if (CoeffDiffL) delete [] CoeffDiffL;
  if (CoeffDiffT) delete [] CoeffDiffT;
}
