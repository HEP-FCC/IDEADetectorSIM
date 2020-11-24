//
// Original author G. Tassielli
//

#include "TowerDetail.hh"

using namespace std;

namespace drc {

  TowerDetail::TowerDetail():
          _center_radius_ringIn(0.0),
          _center_radius_ringOut(0.0),
//          _epsilonIn(0.0),
//          _epsilonOut(0.0),
          _halfLength(0.0),
          _materialNames("")
  {
  }

  TowerDetail::TowerDetail(  double center_radius_ringIn, double center_radius_ringOut, /*double epsilonIn,
                     double epsilonOut,*/ double halfLength, std::string materialNames
                     ):
     _center_radius_ringIn(center_radius_ringIn),
     _center_radius_ringOut(center_radius_ringOut),
//    _epsilonIn(epsilonIn),
//    _epsilonOut(epsilonOut),
    _halfLength(halfLength),
    _materialNames(materialNames)
  {
  }

  TowerDetail::~TowerDetail (){
  }

} // namespace drc
