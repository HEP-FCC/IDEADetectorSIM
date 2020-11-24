//
// Original author G. Tassielli
//

#include "FiberDetail.hh"

using namespace std;

namespace drc {

  FiberDetail::FiberDetail( std::vector<double> & thicknesses,
                   std::vector<std::string> & materialNames,
               double halfLength
                           ):
    _materialNames(materialNames),
    _shellsThicknesses(thicknesses),
    _halfLength(halfLength)
  {
          _radius=0.;
          vector<double>::iterator ithick = thicknesses.begin();
          while(ithick!= thicknesses.end()){
                  _radius += *ithick;
                  ++ithick;
          }
  }

  FiberDetail::~FiberDetail (){
  }

} // namespace drc
