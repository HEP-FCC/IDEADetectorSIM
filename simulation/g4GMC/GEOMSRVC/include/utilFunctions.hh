#ifndef UTILFUNCTIONS_HH
#define UTILFUNCTIONS_HH

// CLHEP includes
#include "CLHEP/Vector/ThreeVector.h"

template< class T >
inline T
diff_of_squares( T x, T y )
{ return (x+y) * (x-y); }

template< class T >
inline T
sum_of_squares( T x, T y )
{ return (x*x) + (y*y); }

template< class T >
inline T
sum_of_squares( T x, T y, T z )
{ return (x*x) + (y*y) + (z*z); }

class TwoLinePCA{

  //
  // Given two lines in 3D, compute the distance of closest
  // approach between the two lines.  The lines are
  // specified in point-slope form.
  //
  // $Id: TwoLinePCA.hh,v 1.6 2011/05/18 02:27:18 wb Exp $
  // $Author: wb $
  // $Date: 2011/05/18 02:27:18 $
  //
  // Original author Rob Kutschke
  //
  // There is special treatment for lines that are almost parallel.
  // Lines are defined as close to parallel if sin**2 of the angle
  // between the two tracks is less than a given cut, which has a
  // default value specified in the declaration of the constructor.
  //
  // Constructor arguments:
  // p1 - a point on line 1.
  // t1 - a vector in the direction of line 1.
  // p2 - a point on line 2.
  // tc - a vector in the direction of line 2.
  //
  // parallelcut - if the sin**2 of the angle between the lines
  //               is less than this cut, then the lines
  //               are declared parallel and the
  //
  //

public:
  TwoLinePCA( CLHEP::Hep3Vector const& p1,
      CLHEP::Hep3Vector const& t1,
      CLHEP::Hep3Vector const& p2,
      CLHEP::Hep3Vector const& t2,
      double closeToParallelCut = 1.e-8
  );
  ~TwoLinePCA(){}

  // Accessors for the 3d and 2d DCA.
  double dca()   const { return _dca; }
  double dca2d() const { return _dca2d; }

  // Accessors for the endpoints on the line-segment of closest approach.
  CLHEP::Hep3Vector const& point1() const { return _pca1; }
  CLHEP::Hep3Vector const& point2() const { return _pca2; }

  // "Local z" of the pca.  ( Length along the straw from mid point to pca ).
  double s1() const { return _s1; }
  double s2() const { return _s2; }

  // Were the lines treated as parallel?
  bool closeToParallel() const { return _closeToParallel; }

private:

  // A point on each track and a unit vector in the direction of each track.
  CLHEP::Hep3Vector _p1, _t1;
  CLHEP::Hep3Vector _p2, _t2;

  // Distances along each track from the points p1 or p2 to the points of closest approach.
  double _s1, _s2;

  // Points on each line at that terminate the line-segment of closest approach.
  CLHEP::Hep3Vector _pca1, _pca2;

  // Distance of closest approach in 3d and 2d(projection onto xy plane).
  double _dca;
  double _dca2d;

  // Did we trigger the close to parallel cut.
  bool _closeToParallel;

  // The value of that cut.
  double _cut;

};

class LinePointPCA{

public:
  LinePointPCA( CLHEP::Hep3Vector const& p,
                CLHEP::Hep3Vector const& t,
                CLHEP::Hep3Vector const& q
                );

  // Accept compiler generated versions of the
  // d'tor, copy cc'tor and assignment operator.


  // The 3d and 2d distances of closest approach.
  double dca()   const { return _dca; }
  double dca2d() const { return _dca2d; }

  // The point on the line that is closest to _q.
  CLHEP::Hep3Vector const& pca() const { return _pca; }

  // The unit vector in the direction from _pca to _q.
  CLHEP::Hep3Vector const& unit() const { return _unit; }

private:

  // The line, in point slope form.
  CLHEP::Hep3Vector _p, _t;

  // The external point.
  CLHEP::Hep3Vector _q;

  // The point on the line that is closest to _q.
  CLHEP::Hep3Vector _pca;

  // Unit vector from pca to external refernce point.
  CLHEP::Hep3Vector _unit;

  // Distance along the line from p to the pca.
//  double _s;

  // Distance of closest approach in 3d and 2d(projection onto xy plane).
  double _dca;
  double _dca2d;

};


#endif
