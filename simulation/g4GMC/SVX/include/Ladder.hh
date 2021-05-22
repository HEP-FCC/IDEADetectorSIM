// SVX layer description
//
// Original author G. Tassielli
//

#ifndef Ladder_hh
#define Ladder_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <LadderDetail.hh>
#include <LadderId.hh>
#include <ReadOut.hh>

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "manageException.hh"

// SVX includes

namespace pshw {
class PSHWMaker;
}

namespace svx {

class Ladder{

  friend class Layer;
  friend class SVXtracker;
  friend class SVXMaker;
//  friend class SVXtrackerBuilder;

  friend class pshw::PSHWMaker;

public:

  enum Ltype {pixel=1, strip};
  enum Lgtype {plane=0, spherical};

  // A free function, returning void, that takes a const ReadOut& as an argument.
  typedef void (*ReadOutFunction)( const Ladder& s);

  Ladder();

  // Constructor using read out/ladder info.
  Ladder( LadderId& id,
         boost::shared_ptr<LadderDetail> &detail,
//         int detailIndex,
//         CLHEP::Hep3Vector &center,
//         CLHEP::Hep3Vector &direction,
         HepGeom::Transform3D *pos,
//         std::vector<boost::shared_ptr<ReadOut> > &ros,
         boost::shared_ptr<ReadOut> &ros,
         Ltype ladderType=pixel,
         Lgtype ladderGType=plane
         );

  ~Ladder ();

  const LadderId& Id() const { return _id;}

  boost::shared_ptr<LadderDetail> getDetail() const { return _detail;}

  /*const*/ Lgtype getLadderGeomType() const { return _ladderGeomType; }
  /*const*/ Ltype getLadderType() const { return _ladderType; }

  const HepGeom::Transform3D& get3DTransfrom() const {return *_pos;}

  const HepGeom::Transform3D& get3DInvTransfrom() const {return _invpos;}

  const CLHEP::Hep3Vector& getMidPoint() const {return _MidPoint;}

  const CLHEP::Hep3Vector& getFstSdDirection() const { return _FstSdDirection; }
  const CLHEP::Hep3Vector& getSndSdDirection() const { return _SndSdDirection; }

  const CLHEP::Hep3Vector& getROChLoclMidPoint(unsigned long id) const;

  int nReadOuts() const { return _nReadOuts; }
  int nReadOutsFstSd() const { return _nReadOutsFstSd; }
  int nReadOutsSndSd() const { return _nReadOutsSndSd; }
  int nReadOutsFstSd2() const { return _nReadOutsFstSd2; }
  int nSndSdDeltaRO() const { return _nSndSdDltRO; }
  int nSndSdAddDeltaRO() const { return _nSndSdAddDltRO; }
  bool isWedgeType() const { return (_detail->scndWidth()>1e-12); }

//  double voxelizationFactor() const { return _voxelizationFactor; }

  boost::shared_ptr<ReadOut> getReadOut() const /*throw(cet::exception)*/ {
    return _ros;
  }

//  boost::shared_ptr<ReadOut> getReadOut( unsigned long n ) const /*throw(cet::exception)*/ {
//        //if (n>=0 && n<_nReadOuts) return _cells.at(n);
//        //else {//throw cet::exception("GEOM")<< "ReadOut number: "<< n <<" not present in "<<_id;
//        if (/*n<0 ||*/ n>=_nReadOuts) {
//          exc::exceptionG4 e("GEOM","Fatal Error",0 );
//          e<< "ReadOut number: "<< n <<" not present in "<<_id;
//          e.error();
//        }
//        return _ros.at(n);
//  }
//
//  boost::shared_ptr<ReadOut> getReadOut( const ReadOutId& id ) const {
//    return getReadOut(id.getSensor());
//  }

protected:

  // Identifier
  LadderId _id;

  // Detailed description of a ladder.
  boost::shared_ptr<LadderDetail> _detail;
//  int _detailIndex;

  Ltype _ladderType;
  Lgtype _ladderGeomType;

  CLHEP::Hep3Vector _MidPoint;
  CLHEP::Hep3Vector _FstSdDirection;
  CLHEP::Hep3Vector _SndSdDirection;

  const HepGeom::Transform3D *_pos;
  HepGeom::Transform3D _invpos;

  unsigned long _nReadOuts;
  unsigned long _nReadOutsFstSd;
  unsigned long _nReadOutsSndSd;
  unsigned long _nReadOutsFstSd2;
  int _nSndSdDltRO; // n. of Second Side rows increment to have one more ReadOut on the First Side
  int _nSndSdAddDltRO; // n. of Second Side rows increment to have an additional one more ReadOut on the First Side (correction at second order)

//  double _voxelizationFactor;

  // Pointers to the ReadOut in this ladder.
  boost::shared_ptr<ReadOut> _ros;
//  std::vector<boost::shared_ptr<ReadOut> > _ros;
//
//  void addReadOut(ReadOut *ro){
//          _ros.push_back(boost::shared_ptr<ReadOut>(ro));
//          _nReadOuts++;
//  }

  void setPosMatrix( HepGeom::Transform3D &lPos );

};

inline std::ostream& operator<<(std::ostream& ost, const Ladder& ld ){
        ost <<ld.Id()<<" type "<<ld.getLadderType()<<" width "<< ld.getDetail()->width() << " length "<< 2.0*ld.getDetail()->halfLength() <<std::endl;
        ost<<ld.get3DTransfrom().xx()<<" "<<ld.get3DTransfrom().xy()<<" "<<ld.get3DTransfrom().xz()<<" "<<ld.get3DTransfrom().dx()<<std::endl;
        ost<<ld.get3DTransfrom().yx()<<" "<<ld.get3DTransfrom().yy()<<" "<<ld.get3DTransfrom().yz()<<" "<<ld.get3DTransfrom().dy()<<std::endl;
        ost<<ld.get3DTransfrom().zx()<<" "<<ld.get3DTransfrom().zy()<<" "<<ld.get3DTransfrom().zz()<<" "<<ld.get3DTransfrom().dz()<<std::endl;
        ost<<std::endl;
        ost<<"MidPoint "<< ld.getMidPoint();
//        ost<<" Direction "<< ld.getDirection()<<" "<<ld.get3DTransfrom().getRotation()<<" phi "<<ld.get3DTransfrom().getRotation().phi()<<" theta "<<ld.get3DTransfrom().getRotation().theta()<<std::endl;
        return ost;
}

}  //namespace svx

#endif /* Ladder_hh */
