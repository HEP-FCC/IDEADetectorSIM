//
// Original author G. Tassielli
//

#include "PlaneDetail.hh"

using namespace std;

namespace tdch {

  PlaneDetail::PlaneDetail():
          _radiusIn(0.0),
          _radiusOut(0.0),
//          _epsilonIn(0.0),
//          _epsilonOut(0.0),
          _halfLength(0.0),
          _materialNames("")
  {
  }

  PlaneDetail::PlaneDetail(  double radiusIn, double radiusOut, /*double epsilonIn,
                     double epsilonOut,*/ double halfLength, std::string materialNames
                     ):
     _radiusIn(radiusIn),
     _radiusOut(radiusOut),
//    _epsilonIn(epsilonIn),
//    _epsilonOut(epsilonOut),
    _halfLength(halfLength),
    _materialNames(materialNames)
  {
  }

  PlaneDetail::~PlaneDetail (){
  }

} // namespace tdch
