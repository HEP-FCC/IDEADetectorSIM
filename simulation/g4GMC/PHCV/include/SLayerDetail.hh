//
// Original author G. Tassielli
//

#ifndef SLayerDetail_hh
#define SLayerDetail_hh

#include <string>

namespace phcv {

class SLayerDetail{

public:
  SLayerDetail();

  SLayerDetail( double center_radius_ringIn, double center_radius_ringOut, double epsilonIn,
     double epsilonOut, double halfLength, std::string materialNames
  );

  ~SLayerDetail ();

  double      centerInnerRadiusRing()    const { return _center_radius_ringIn;}
  double      centerOuterRadiusRing()    const { return _center_radius_ringOut;}
  double      stereoAngleInnerRing()     const { return _epsilonIn;}
  double      stereoAngleOuterRing()     const { return _epsilonOut;}
  double      halfLength()               const { return _halfLength; }
  std::string materialName()             const { return _materialNames; }

  void        setCenterInnerRadiusRing(double center_radius_ringIn)    { _center_radius_ringIn  = center_radius_ringIn;  }
  void        setCenterOuterRadiusRing(double center_radius_ringOut)   { _center_radius_ringOut = center_radius_ringOut; }
  void        setStereoAngleInnerRing(double epsilonIn)                { _epsilonIn             = epsilonIn;             }
  void        setStereoAngleOuterRing(double epsilonOut)               { _epsilonOut            = epsilonOut;            }
  void        setHalfLength(double halfLength)                         { _halfLength            = halfLength;            }
  void        setMaterialName(std::string materialNames)               { _materialNames         = materialNames;         }

private:

  double _center_radius_ringIn;   //Inner surface radius
  double _center_radius_ringOut;  //Outer surface radius
  double _epsilonIn;              //Inner surface stereo angle
  double _epsilonOut;             //Outer surface stereo angle
  double _halfLength;             //Half z length
  std::string _materialNames;     //G4 material name

};

}  //namespace phcv

#endif /* SLayerDetail_hh */
