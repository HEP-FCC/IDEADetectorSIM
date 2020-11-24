// PHCV geometry maker
//
// Original author G. Tassielli
//

#ifndef PHCVMaker_hh
#define PHCVMaker_hh

#include <map>
#include <memory>
#include <string>
#include <vector>

//#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include "CLHEP/Vector/ThreeVector.h"


namespace crd {
class SimpleConfig;
}

namespace phcv {

class PHCVtcounter;
//class PHCVRadiator;

class PHCVMaker{

public:

  PHCVMaker( crd::SimpleConfig const& config );
  ~PHCVMaker ();

  // This is the accessor that will remain.
  std::unique_ptr<PHCVtcounter> getPHCVtcounterPtr() { return std::move(_lpst); }
//  std::unique_ptr<PHCVRadiator> getPHCVradiatorPtr() { return std::move(_lpsrad); }

private:

  void loadTCounter( crd::SimpleConfig const& config );
  void loadRadiator( crd::SimpleConfig const& config );
  void Build();

  // Name of external gdml geometry file description.
  std::string _extFile;
  bool _isExternal;

  // Basic geometry element parameter
  int _nShell;
  double _halfLength;
  std::vector<double> _shellThick;
  std::vector<double> _radThick;
  std::vector<double> _scintThick;
  std::string _fillScintMaterial;
  std::string _fillRadMaterial;

  int _geomType;                //Verison number (see PHCVtcounter for definifiton) (from it depends the Cell Geometry type: 2 hexagonal, 3 square)

  double _z0;                   //Shift along z of the center of the tcounter

  //Detailed parameters
  //.....

  // Center of the tcounter.
  CLHEP::Hep3Vector _center;

  std::unique_ptr<PHCVtcounter> _lpst;
//  std::unique_ptr<PHCVRadiator> _lpsrad;


  int _CDCHnSrvShell;
  std::vector<double> _CDCHSrvLyInRad;
  std::vector<double> _CDCHSrvLyOutRad;
  std::vector<double> _CDCHSrvLyInAngl;
  std::vector<double> _CDCHSrvLyOutAngl;
  std::vector<double> _CDCHSrvLyHalfLng;
  std::vector<int> _CDCHSrvLySlid;

};

}  //namespace phcv

#endif /* PHCVMaker_hh */
