#ifndef ReadOut_hh
#define ReadOut_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <ReadOutDetail.hh>

#include <ReadOutId.hh>
#include "CLHEP/Vector/ThreeVector.h"

namespace pshw {
class PSHWMaker;
}
namespace muspc {
class MUSPCMaker;
}

namespace svx {

class ReadOut {

//  friend class ROGeometryHandle;
//  friend class SVXtracker;
  friend class SVXMaker;
  friend class Ladder;

  friend class pshw::PSHWMaker;
  friend class muspc::MUSPCMaker;

public:

  // A free function, returning void, that takes a const ReadOut& as an argument.
  typedef void (*ReadOutFunction)( const ReadOut& s);

  ReadOut();

  // Constructor using sense wire info.
//  ReadOut( ReadOutId id,
//         boost::shared_ptr<ReadOutDetail> detail
//         );

  ~ReadOut ();

//  ReadOutId Id() const { return _id;}
//  ReadOutIndex Index() const { return _index;}

  boost::shared_ptr<ReadOutDetail> getDetail() const { return _detail;}

  bool selectChannel(unsigned long id);

  const CLHEP::Hep3Vector& getMidPoint() const;
//  const CLHEP::Hep3Vector& getDirection() const;

  double getPitch() const { return _detail->firstSideDim();}
  double getFstSideInsul()   const { return _detail->firstSideInsul();}
  double getLength() const { return _detail->secondSideDim();}
  double getSndSideInsul()   const { return _detail->secondSideInsul();}
//  double getThickness() const { return _detail->thickness();}
  double getAngle() const { return _detail->angle();}
  int getSelSubShell() const { return _selSubShell; }
  unsigned long getSelChFstSd() const { return _selChFstSd; }
  unsigned long getSelChSndSd() const { return _selChSndSd; }

protected:

  // Identifier
//  ReadOutId _id;

  // Detailed description of a cell.
  boost::shared_ptr<ReadOutDetail> _detail;
//  int _detailIndex;

  unsigned long _selId; //must be less than 1e8
  int _selSubShell;
  unsigned long _selChFstSd;
  unsigned long _selChSndSd;
  CLHEP::Hep3Vector _tmpMidPoint;
//  CLHEP::Hep3Vector _tmpDirection;

};

}  //namespace svx

#endif /* ReadOut_hh */
