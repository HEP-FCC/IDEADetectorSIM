//
// Original author G. Tassielli
//

#ifndef LadderDetail_hh
#define LadderDetail_hh

#include <string>
#include <vector>

#include "manageException.hh"

namespace svx {

class LadderDetail{

//  friend class SVXtrackerBuilder;

public:
  LadderDetail();

  LadderDetail( double width, double halfLength, std::vector<double> & thicknesses, std::vector<std::string> & materialNames, double scndWidth=-1.0 );

  ~LadderDetail ();

  double      width()   const { return _width;}
  double      scndWidth()   const { return _scndWidth;}
  double      halfLength()   const { return _halfLength;}
  double      thickness()   const { return _thickness;}
  int         nShells()   const { return _nShells;}

  std::string const materialName(int idx) const /*throw(cet::exception)*/ {
//          try {
//                  return _materialNames.at(idx);
//          } catch (cet::exception e) {
//              throw cet::exception("GEOM")
//                << "No material defined for the wire \n";
//          }
    try {
      _materialNames.at(idx);
    } catch (std::exception &ex) {
      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
      e<< "No material defined for the wire \n";
      e.error();
    }
    return _materialNames.at(idx); //just to remove warning
  }

  const std::vector<std::string> &materialNames() const { return _materialNames;}

  double /*const*/ shellThickness(int idx) const/* throw(cet::exception)*/ {
//          try {
//              return _shellsThicknesses.at(idx);
//          } catch (cet::exception e) {
//              throw cet::exception("GEOM")
//                << "No shells thicknesses defined for the wire \n";
//          }
    try {
      /*return*/ _shellsThicknesses.at(idx);
    } catch (std::exception &ex) {
      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
      e<< "No shells thicknesses "<< idx<<" found for the ladder \n";
      e.error();
    }
    return _shellsThicknesses.at(idx); //just to remove warning
  }

  const std::vector<double> &shellsThicknesses() const { return _shellsThicknesses;}

  double shellYPos(int idx) const {
    try {
      /*return*/ _shellsYPoss.at(idx);
    } catch (std::exception &ex) {
      exc::exceptionG4 e("GEOM","Fatal Error in Arguments",1);
      e<< "No shells YPosition "<< idx<<" found for the ladder \n";
      e.error();
    }
    return _shellsYPoss.at(idx); //just to remove warning
  }

  const std::vector<double> &shellsYPoss() const { return _shellsYPoss;}

protected:
  void addSellYPos(double yPos) { _shellsYPoss.push_back(yPos); }

private:

  double _width;
  double _scndWidth;
  double _halfLength;
  double _thickness;
  int    _nShells;

  // Order of materials and shells dimensions is:
  std::vector<std::string> _materialNames;
  std::vector<double> _shellsThicknesses;
  std::vector<double> _shellsYPoss;

};

}  //namespace svx

#endif /* LadderDetail_hh */
