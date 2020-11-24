//
// Original author G. Tassielli
//

#ifndef LayerDetail_hh
#define LayerDetail_hh

#include <string>

namespace svx {

class LayerDetail{

public:
  LayerDetail();

  LayerDetail( double inRad, double outRad, double halfLength, double zpos=0.0 );

  ~LayerDetail ();

  double      InnerRadius()   const { return _InnerRadius;}
  double      OuterRadius()   const { return _OuterRadius;}
  double      halfLength()    const { return _halfLength;}
  double      zPosition()     const { return _zpos;}

private:

  double _InnerRadius;
  double _OuterRadius;
  double _halfLength;
  double _zpos;

};

}  //namespace svx

#endif /* LayerDetail_hh */
