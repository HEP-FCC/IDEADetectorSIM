//
// Original author G. Tassielli
//

#ifndef FiberDetail_hh
#define FiberDetail_hh

#include <vector>
#include <string>

// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include "manageException.hh"

namespace drc {

class FiberDetail{

public:
  FiberDetail():
    _materialNames(),
    _shellsThicknesses(),
    _radius(-1.),
    _halfLength(-1.)
  {}

  FiberDetail( std::vector<double> & thicknesses, std::vector<std::string> & materialNames,
               double halfLength
               );

  ~FiberDetail ();

  std::string const materialName(int idx) const /*throw(cet::exception)*/ {
//          try {
//                  return _materialNames.at(idx);
//          } catch (cet::exception e) {
//              throw cet::exception("GEOM")
//                << "No material defined for the wire \n";
//          }
    try {
      _materialNames.at(idx);
    } catch (std::exception &ex) {
      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
      e<< "No material defined for the fiber \n";
      e.error();
    }
    return _materialNames.at(idx); //just to remove warning
  }

  const std::vector<std::string> &materialNames() const { return _materialNames;}

  double /*const*/ shellThickness(int idx) const/* throw(cet::exception)*/ {
//          try {
//              return _shellsThicknesses.at(idx);
//          } catch (cet::exception e) {
//              throw cet::exception("GEOM")
//                << "No shells thicknesses defined for the wire \n";
//          }
    try {
      /*return*/ _shellsThicknesses.at(idx);
    } catch (std::exception &ex) {
      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
      e<< "No shells thicknesses defined for the fiber \n";
      e.error();
    }
    return _shellsThicknesses.at(idx); //just to remove warning
  }

  const std::vector<double> &shellsThicknesses() const { return _shellsThicknesses;}

  double      outerRadius()   const { return _radius;}
  double      length()        const { return _halfLength*2.0; }
  double      halfLength()    const { return _halfLength; }

private:

  // Order of materials and shells dimensions is:
  std::vector<std::string> _materialNames;
  std::vector<double> _shellsThicknesses;

  double _radius;
  double _halfLength;

};

}  //namespace drc

#endif /* FiberDetail_hh */
