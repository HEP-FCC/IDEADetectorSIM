// TDCHtracker cells description
//
// Original author G. Tassielli
//

#ifndef CellDetail_TDCH_hh
#define CellDetail_TDCH_hh

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

//// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include "manageException.hh"

// TDCH includes
#include "WireDetail.hh"

namespace tdch {

class CellDetail{

public:
  CellDetail():
          _circumscribedRadius(0.0),
          _inscribedCircleRadius(0.0)
  {}

  CellDetail( double circumscribedRadius, double inscribedCircleRadius, boost::shared_ptr<WireDetail> senseWire );

  ~CellDetail ();

  double      CirumscribedRadius()   const { return _circumscribedRadius;}
  double      InscribedCircleRadius()   const { return _inscribedCircleRadius;}
  double      wireRadius()/* const throw(cet::exception)*/ {
    //          try {
    //                  return _senseWire.get()->outerRadius();
    //          } catch (cet::exception e) {
    //              throw cet::exception("GEOM")
    //                << "No sense wire defined for the Cell \n";
    //          return 0.0;
    //          }
    if (!_senseWire) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<<"No sense wire defined for the Cell \n";
      e.error();
      return 0.0;
    } else {
      return _senseWire->outerRadius();
    }
  }

  double      halfLength()/* const throw(cet::exception)*/ {
//          try {
//                  return _senseWire->halfLength();
//          } catch (cet::exception e) {
//              throw cet::exception("GEOM")
//                << "No sense wire defined for the Cell \n";
//          return 0.0;
//          }
    if (!_senseWire) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<<"No sense wire defined for the Cell \n";
      e.error();
      return 0.0;
    } else {
      return _senseWire->halfLength();
    }
  }

private:

  double _circumscribedRadius;
  double _inscribedCircleRadius;
  boost::shared_ptr<WireDetail> _senseWire;

};

}  //namespace tdch

#endif /* CellDetail_TDCH_hh */
