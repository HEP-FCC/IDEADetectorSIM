// CDCH wire description
//
// Original author G. Tassielli
//

#ifndef CDCHWiredata_hh
#define CDCHWiredata_hh

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  DCH wireposition class                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TObjArray.h"

class CDCHWiredata {

  public:

    CDCHWiredata();

    virtual ~CDCHWiredata();

    TObjArray *PosMatrix; //PosMatrix
    Int_t     NcelLayer;  //NcelLayer
    Float_t   *epsilon;   //[NcelLayer]  epsilon
    Float_t   *alfa;      //[NcelLayer]  alfa
    Float_t   *radius_z0; //[NcelLayer]  layer radius at z=0

};

#endif /* CDCHWiredata_hh */
//
//#ifndef ILCDCHWIREDATA_HH
//#define ILCDCHWIREDATA_HH
//
/////////////////////////////////////////////////////////////////////////////////
////                                                                           //
////  DCH wireposition class                                                   //
////                                                                           //
/////////////////////////////////////////////////////////////////////////////////
//
//class IlcDCHwiredata : public CDCHWiredata {
//
//  public:
//
//    IlcDCHwiredata():CDCHWiredata(){}
//
//    virtual ~IlcDCHwiredata(){}
//
//};

//#endif
