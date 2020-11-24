//
// Original author G. Tassielli
//

#include "SLayerDetail.hh"

using namespace std;

namespace phcv {

  SLayerDetail::SLayerDetail():
          _center_radius_ringIn(0.0),
          _center_radius_ringOut(0.0),
          _epsilonIn(0.0),
          _epsilonOut(0.0),
          _halfLength(0.0),
          _materialNames("")
  {
  }

  SLayerDetail::SLayerDetail(  double center_radius_ringIn, double center_radius_ringOut, double epsilonIn,
                     double epsilonOut, double halfLength, std::string materialNames
                     ):
     _center_radius_ringIn(center_radius_ringIn),
     _center_radius_ringOut(center_radius_ringOut),
    _epsilonIn(epsilonIn),
    _epsilonOut(epsilonOut),
    _halfLength(halfLength),
    _materialNames(materialNames)
  {
  }

  SLayerDetail::~SLayerDetail (){
  }

} // namespace phcv
