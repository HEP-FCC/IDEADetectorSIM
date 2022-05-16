//
// Original author G. Tassielli
//

#include "WireDetail.hh"

using namespace std;

namespace tdch {

  WireDetail::WireDetail( std::vector<double> & thicknesses,
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

  WireDetail::~WireDetail (){
  }

} // namespace tdch
