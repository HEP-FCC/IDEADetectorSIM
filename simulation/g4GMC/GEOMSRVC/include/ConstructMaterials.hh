/*
 * GMCG4ConstructMaterials.hh
 *
 *  Created on: Mar 29, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_GEOMSRVC_INCLUDE_CONSTRUCTMATERIALS_HH_
#define G4GMC_GEOMSRVC_INCLUDE_CONSTRUCTMATERIALS_HH_

// G4 includes
#include <G4String.hh>

//c++ includes
#include <vector>

// CLHEP includes
#include "CLHEP/Units/PhysicalConstants.h"

class G4Element;

namespace crd {
  class SimpleConfig;
}

namespace gmc {

// Return type of the isNeeded() method.
class CheckedG4String{
public:
  CheckedG4String ():
    doit(false),
    name(){
  }
  CheckedG4String ( bool b, G4String const& n):
    doit(b),
    name(n){
  }
  bool doit;
  G4String name;
};

class ConstructMaterials {

public:

  ConstructMaterials();
  ~ConstructMaterials(){}

  void construct();

private:
  void contructGeneralMaterial();
  void contructCDCHMaterial();

  // Wrapper around FindOrBuildElement.
  G4Element* getElementOrThrow( G4String const& name);

  // Do we need to build this material?
  CheckedG4String isNeeded(  std::vector<std::string> const& V, std::string const& name);

  // Check that a material name is not already in use.
  void uniqueMaterialOrThrow( G4String const& name);

  // Check to see if a string appears within a vector of strings.
  bool isRequested( std::vector<std::string> const& V, std::string const& name);

  const crd::SimpleConfig *cRd;

  void doGasHe(std::string matName, double pressure=1.0*CLHEP::atmosphere, double temperature=293.15*CLHEP::kelvin);
  void doGasHeIsobMix(double frcHe, std::string matName, double pressure=1.0*CLHEP::atmosphere, double temperature=293.15*CLHEP::kelvin);
  void doGasHeCF4Mix(double frcHe, std::string matName, double pressure=1.0*CLHEP::atmosphere, double temperature=293.15*CLHEP::kelvin);

};

} // end namespace gmc

#endif /* G4GMC_GEOMSRVC_INCLUDE_CONSTRUCTMATERIALS_HH_ */
