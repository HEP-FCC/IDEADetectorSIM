// PHCV slayer description
//
// Original author G. Tassielli
//

#include "SLayer.hh"

#ifndef __CINT__

using namespace std;

namespace phcv {

  SLayer::SLayer():
    _id(SLayerId()),
    _detailIndex(0),
    _layerType(undefined),
    _nFbrWr(0),
    _voxelizationFactor(2),
    _FieberWires(0x0)
  {
  }

  SLayer::SLayer(SLayerId& id,
                         boost::shared_ptr<SLayerDetail> &detail,
                         int detailIndex,
                         std::vector<boost::shared_ptr<FbrWr> > &fieberWires

               ):
    _id(id),
    _detailIndex(detailIndex),
    _layerType(undefined),
    _voxelizationFactor(2),
    _FieberWires(fieberWires)
  {
          _detail=detail;

          _nFbrWr=fieberWires.size();
         _FieberWires=fieberWires;

  }

  SLayer::~SLayer(){
  }

} // namespace phcv

#endif

