//
// Original author G. Tassielli
//

#include "LayerDetail.hh"

using namespace std;

namespace svx {

  LayerDetail::LayerDetail():
          _InnerRadius(0.0),
          _OuterRadius(0.0),
          _halfLength(0.0),
          _zpos(0.0)
  {}

  LayerDetail::LayerDetail( double inRad, double outRad, double halfLength, double zpos ):
              _InnerRadius(inRad),
              _OuterRadius(outRad),
              _halfLength(halfLength),
              _zpos(zpos)
  {}

  LayerDetail::~LayerDetail (){
  }

} // namespace svx
