// SVXtracker cells description
//
// Original author G. Tassielli
//

#ifndef ReadOutDetail_hh
#define ReadOutDetail_hh

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

//// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include "manageException.hh"

namespace svx {

class ReadOutDetail{

public:
  ReadOutDetail();

  ReadOutDetail( double firstSideDim, double firstSideInsul, double secondSideDim, double secondSideInsul, double angle, std::vector<int> & shellIds );

  ~ReadOutDetail ();

  double      firstSideDim()   const { return _firstSideDim;}
  double      firstSideInsul()   const { return _firstSideInsul;}
  double      secondSideDim()   const { return _secondSideDim;}
  double      secondSideInsul()   const { return _secondSideInsul;}
//  double      thickness()   const { return _thickness;}
  double      angle()   const { return _angle;}
  int         nShells()   const { return _nShells;}

//  std::string const materialName(int idx) const /*throw(cet::exception)*/ {
////          try {
////                  return _materialNames.at(idx);
////          } catch (cet::exception e) {
////              throw cet::exception("GEOM")
////                << "No material defined for the wire \n";
////          }
//    try {
//      _materialNames.at(idx);
//    } catch (std::exception &ex) {
//      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
//      e<< "No material defined for the wire \n";
//      e.error();
//    }
//    return _materialNames.at(idx); //just to remove warning
//  }
//
//  const std::vector<std::string> &materialNames() const { return _materialNames;}

  int /*const*/ shellId(int idx) const/* throw(cet::exception)*/ {
//          try {
//              return _shellsThicknesses.at(idx);
//          } catch (cet::exception e) {
//              throw cet::exception("GEOM")
//                << "No shells thicknesses defined for the wire \n";
//          }
    try {
      /*return*/ _shellIds.at(idx);
    } catch (std::exception &ex) {
      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
      e<< "No shells id defined for the ReadOut \n";
      e.error();
    }
    return _shellIds.at(idx); //just to remove warning
  }

  const std::vector<int> &shellIds() const { return _shellIds;}

private:

  double _firstSideDim;
  double _firstSideInsul;
  double _secondSideDim;
  double _secondSideInsul;
//  double _thickness;
  double _angle;
  int    _nShells;

  // Order of shells :
  std::vector<int> _shellIds;

};

}  //namespace svx

#endif /* ReadOutDetail_hh */
