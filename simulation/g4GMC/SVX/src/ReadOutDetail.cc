// SVX cells description
//
// Original author G. Tassielli
//

#include "ReadOutDetail.hh"

using namespace std;

namespace svx {

  ReadOutDetail::ReadOutDetail():
        _firstSideDim(0.0),
        _firstSideInsul(0.0),
        _secondSideDim(0.0),
        _secondSideInsul(0.0),
//        _thickness(0.0),
        _angle(0.0),
        _nShells(0),
        _shellIds()
  {}

  ReadOutDetail::ReadOutDetail( double firstSideDim, double firstSideInsul, double secondSideDim, double secondSideInsul, double angle, std::vector<int> & shellIds
                           ):
                           _firstSideDim(firstSideDim),
                           _firstSideInsul(firstSideInsul),
                           _secondSideDim(secondSideDim),
                           _secondSideInsul(secondSideInsul),
                           _angle(angle),
                           _shellIds(shellIds)
  {
//    vector<double>::iterator ithick = thicknesses.begin();
//    while(ithick!= thicknesses.end()){
//            _thickness += *ithick;
//            ++ithick;
//    }
//    _nShells = thicknesses.size();
    _nShells = _shellIds.size();
  }

  ReadOutDetail::~ReadOutDetail (){
  }

} // namespace svx
