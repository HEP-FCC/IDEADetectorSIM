// DR CAlorimeter fiber description
//
// Original author G. Tassielli
//

#include "Fiber.hh"

#include "CLHEP/Geometry/Vector3D.h"

#ifndef __CINT__

namespace drc {

Fiber::Fiber():
  _id(FiberId()),
  _fbrType(undefined),
  _c(CLHEP::Hep3Vector(0.,0.,0.)),
  _w(CLHEP::Hep3Vector(0.,0.,1.)),
  _pos(0x0),
  _invpos(HepGeom::Transform3D())//,
//  _epsilon(0.0),
//  _alpha(0.0)
{
}


Fiber::Fiber( FiberId id,
                boost::shared_ptr<FiberDetail> detail,
                HepGeom::Transform3D *pos,
//                double epsilon,
//                double alpha,
                Fbrtype wireType
            ):
  _id(id),
  _fbrType(wireType),
  _pos(pos)//,
//  _epsilon(epsilon),
//  _alpha(alpha)
{
  _detail=detail;
  _c.set(_pos->dx(), _pos->dy(), _pos->dz());
  _invpos=_pos->inverse();
  _w.set(_pos->xz(),_pos->yz(),_pos->zz());

}

Fiber::~Fiber (){
//        try {
//                delete *_detail; *_detail=nullptr;
//                delete _pos;
//        } catch (cet::exception e) {
//            throw cet::exception("GEOM")
//                << "Error during deleting fiber data \n";
//        }

  delete _pos;
}

} // namespace drc

#endif
