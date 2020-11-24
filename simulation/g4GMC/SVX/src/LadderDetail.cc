//
// Original author G. Tassielli
//

#include "LadderDetail.hh"

using namespace std;

namespace svx {

  LadderDetail::LadderDetail():
          _width(0.0),
          _scndWidth(0.0),
          _halfLength(0.0),
          _thickness(0.0),
          _nShells(0)
  {
  }

  LadderDetail::LadderDetail( double width, double halfLength, std::vector<double> & thicknesses, std::vector<std::string> & materialNames, double scndWidth
                           ):
                           _width(width),
                           _halfLength(halfLength),
                           _materialNames(materialNames),
                           _shellsThicknesses(thicknesses)
  {
    if (scndWidth>0.0) { _scndWidth=scndWidth; }
    else { _scndWidth=0.000000000000; }
    _thickness=0.0;
    vector<double>::iterator ithick = thicknesses.begin();
    while(ithick!= thicknesses.end()){
            _thickness += *ithick;
            ++ithick;
    }
    ithick = thicknesses.begin();
    double yPos=-0.5*_thickness;
    while(ithick!= thicknesses.end()){
      yPos+=0.5*(*ithick);
      _shellsYPoss.push_back(yPos);
      yPos+=0.5*(*ithick);
      ++ithick;
    }
    _nShells = thicknesses.size();
  }

  LadderDetail::~LadderDetail (){
  }

} // namespace svx
