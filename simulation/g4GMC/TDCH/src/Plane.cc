// TDCH slayer description
//
// Original author G. Tassielli
//

#include "Plane.hh"

#ifndef __CINT__

using namespace std;

namespace tdch {

  Plane::Plane():
    _id(PlaneId()),
    _detailIndex(0),
    _planeType(undefined),
    _nPanels(0),
    _voxelizationFactor(2),
    _Panels(0x0)
  {
  }

  Plane::Plane(PlaneId& id,
                         boost::shared_ptr<PlaneDetail> &detail,
                         int detailIndex,
                         std::vector<boost::shared_ptr<Panel> > &panels

               ):
    _id(id),
    _detailIndex(detailIndex),
    _planeType(undefined),
    _voxelizationFactor(2),
    _Panels(panels)
  {
          _detail=detail;

          _nPanels=panels.size();
         _Panels=panels;

  }

  Plane::~Plane(){
  }

} // namespace tdch

#endif

