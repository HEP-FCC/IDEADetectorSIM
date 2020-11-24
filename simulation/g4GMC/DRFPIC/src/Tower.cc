// DRFPI slayer description
//
// Original author G. Tassielli
//

#include "Tower.hh"

#ifndef __CINT__

using namespace std;

namespace drc {

  Tower::Tower():
    _id(TowerId()),
    _detailIndex(0),
    _towerType(undefined),
    _towerSide(undef),
//    _nFibers(0),
    _voxelizationFactor(2),
//    _Fiebers(0x0),
	_deltatheta(0.0),
	_thetaofcenter(0.0)
  {
  }

  Tower::Tower(TowerId& id
                         ,boost::shared_ptr<TowerDetail> &detail
                         ,int detailIndex
//                         ,std::vector<boost::shared_ptr<Fiber> > &fiebers

               ):
    _id(id)
    ,_detailIndex(detailIndex)
    ,_towerType(undefined)
    ,_voxelizationFactor(2)
//    ,_Fiebers(fiebers)
  {
          _detail=detail;

//          _nFibers=fiebers.size();
//         _Fiebers=fiebers;

  }

  Tower::~Tower(){
  }

} // namespace drc

#endif

