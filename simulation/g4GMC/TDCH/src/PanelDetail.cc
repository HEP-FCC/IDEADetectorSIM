//
// Original author G. Tassielli
//

#include "PanelDetail.hh"

using namespace std;

namespace tdch {

  PanelDetail::PanelDetail( std::vector<double> & thicknesses,
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

  PanelDetail::~PanelDetail (){
  }

} // namespace tdch
