//
// Original author G. Tassielli
//

#ifndef PlaneDetail_hh
#define PlaneDetail_hh

#include <string>

namespace tdch {

class PlaneDetail{

public:
  PlaneDetail();

  PlaneDetail( double radiusIn, double radiusOut,
     double halfLength, std::string materialNames
  );

  ~PlaneDetail ();

  double      innerRadius()    const { return _radiusIn;}
  double      outerRadius()    const { return _radiusOut;}
//  double      stereoAngleInnerRing()     const { return _epsilonIn;}
//  double      stereoAngleOuterRing()     const { return _epsilonOut;}
  double      halfLength()               const { return _halfLength; }
  std::string materialName()             const { return _materialNames; }

  void        setInnerRadius(double radiusIn)    { _radiusIn  = radiusIn;  }
  void        setOuterRadius(double radiusOut)   { _radiusOut = radiusOut; }
//  void        setStereoAngleInnerRing(double epsilonIn)                { _epsilonIn             = epsilonIn;             }
//  void        setStereoAngleOuterRing(double epsilonOut)               { _epsilonOut            = epsilonOut;            }
  void        setHalfLength(double halfLength)                         { _halfLength            = halfLength;            }
  void        setMaterialName(std::string materialNames)               { _materialNames         = materialNames;         }

private:

  double _radiusIn;   //Inner radius
  double _radiusOut;  //Outer radius
//  double _epsilonIn;              //Inner surface stereo angle
//  double _epsilonOut;             //Outer surface stereo angle
  double _halfLength;             //Half z length
  std::string _materialNames;     //G4 material name

};

}  //namespace tdch

#endif /* PlaneDetail_hh */
