#ifndef Cell_hh
#define Cell_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "CellDetail.hh"
#include "CellId.hh"
#include "Wire.hh"
#include "WireId.hh"

//#include "TrackerGeom/inc/Straw.hh"

#include "CLHEP/Vector/ThreeVector.h"

namespace cdch {

class Cell /*: public Straw*/ {

  friend class CellGeometryHandle;
  friend class CellGeometryHandle_v2;
  friend class CellGeometryHandle_v3;
  friend class CellGeometryHandle_v4;
  friend class CellGeometryHandle_v2_DBL;
  friend class CellGeometryHandle_v3_DBL;
  friend class CDCHtracker;
  friend class CDCHMaker;
  friend class SuperLayer;
  friend class Layer;

public:

  // A free function, returning void, that takes a const Cell& as an argument.
  typedef void (*CellFunction)( const Cell& s);

  Cell();

  // Constructor using sense wire info.
  Cell( CellId id,
         boost::shared_ptr<CellDetail> detail,
         boost::shared_ptr<Wire> senseWire
         );

//  // Constructor using sense wire info.
//  Cell( CellId id,
//         CellIndex index,
//         boost::shared_ptr<CellDetail> detail,
//         int detailIndex,
//         boost::shared_ptr<Wire> senseWire
//         );

  ~Cell ();

  CellId Id() const { return _id;}
//  CellIndex Index() const { return _index;}

  boost::shared_ptr<CellDetail> getDetail() const { return _detail;}

  boost::shared_ptr<Wire> getWire() const { return  _senseWire; }

  const CLHEP::Hep3Vector& getMidPoint()  const;

  const CLHEP::Hep3Vector& getDirection() const;

  double getHalfLength() const { return _senseWire.get()->getDetail()->halfLength();}

  double getRadius() const { return getDetail()->CirumscribedRadius();}
  double getThickness() const { return 0.;}

  size_t nFWires() const { return _fieldWires.size(); }
  boost::shared_ptr<Wire> getFWire(int iFw) const;
//  int hack;

protected:

  // Identifier
  CellId _id;

  // Detailed description of a cell.
  boost::shared_ptr<CellDetail> _detail;
  int _detailIndex;

  boost::shared_ptr<Wire> _senseWire;
  std::vector<boost::shared_ptr<Wire> > _fieldWires;

  CLHEP::Hep3Vector _tmpMidPoint;
  CLHEP::Hep3Vector _tmpDirection;

};

}  //namespace cdch

#endif /* Cell_hh */
