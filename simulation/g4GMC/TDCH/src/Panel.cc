// DR CAlorimeter fiber description
//
// Original author G. Tassielli
//

#include "Panel.hh"

#include "CLHEP/Geometry/Vector3D.h"

#ifndef __CINT__

namespace tdch {

Panel::Panel():
  _id(PanelId()),
  _pnType(undefined),
  _c(CLHEP::Hep3Vector(0.,0.,0.)),
  _w(CLHEP::Hep3Vector(0.,0.,1.)),
  _pos(0x0),
  _invpos(HepGeom::Transform3D())
  ,_detailIndex(0)
//  _epsilon(0.0),
//  _alpha(0.0)
  ,_nCells(0)
  ,_nFiledWires(0)
  ,_cells(0x0)
  ,_fieldWires(0x0)
{
}


Panel::Panel( PanelId id,
                boost::shared_ptr<PanelDetail> detail,
                HepGeom::Transform3D *pos,
//                double epsilon,
//                double alpha,
		         int detailIndex,
		         std::vector<boost::shared_ptr<Cell> > &cells,
		         std::vector<boost::shared_ptr<Wire> > &fieldWires
            ):
  _id(id),
  _detailIndex(detailIndex),
  _pnType(undefined),
  _pos(pos)//,
//  _epsilon(epsilon),
//  _alpha(alpha)
  ,_cells(cells)
  ,_fieldWires(fieldWires)
{
  _detail=detail;
  _c.set(_pos->dx(), _pos->dy(), _pos->dz());
  _invpos=_pos->inverse();
  _w.set(_pos->xz(),_pos->yz(),_pos->zz());

  _nCells=cells.size();
  _nFiledWires=fieldWires.size();

  _cells=cells;
  _fieldWires=fieldWires;

}

Panel::~Panel (){
//        try {
//                delete *_detail; *_detail=nullptr;
//                delete _pos;
//        } catch (cet::exception e) {
//            throw cet::exception("GEOM")
//                << "Error during deleting fiber data \n";
//        }

  delete _pos;
}

} // namespace tdch

#endif
