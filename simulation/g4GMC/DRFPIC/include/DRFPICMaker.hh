// PHCV geometry maker
//
// Original author G. Tassielli
//

#ifndef DRFPIMaker_hh
#define DRFPIMaker_hh

#include <map>
#include <memory>
#include <string>
#include <vector>

//#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include "G4ThreeVector.hh"
#include "CLHEP/Vector/ThreeVector.h"
#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4FieldManager.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4GenericTrap.hh"
#include "dimensionB.hh"
#include "dimensionE.hh"

#include "globals.hh"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

//using CLHEP::Hep3Vector;

namespace crd {
class SimpleConfig;
}

namespace drc {

class DRFPIcalorimeter;

class DRFPICMaker{

public:

  DRFPICMaker( crd::SimpleConfig const& config );
  ~DRFPICMaker ();

  // This is the accessor that will remain.
  std::unique_ptr<DRFPIcalorimeter> getDRFPIcalorimeterPtr() { return std::move(_ldrfpic); }

private:

//  void loadTCounter( crd::SimpleConfig const& config );
  void Build();

  // Name of external gdml geometry file description.
  std::string _extFile;
  bool _isExternal;

//  // Basic geometry element parameter
  int _nTower;
//  double _halfLength;
//  std::vector<double> _shellThick;
//  std::vector<double> _radThick;
//  std::vector<double> _scintThick;
//  std::string _fillScintMaterial;
//  std::string _fillRadMaterial;

  int _geomType;                //Verison number (see PHCVtcounter for definifiton) (from it depends the Cell Geometry type: 2 hexagonal, 3 square)

  double _z0;                   //Shift along z of the center of the tcounter

  //Detailed parameters
  //.....
//add 
  dimensionB* dimB;
  dimensionE* dimE;

  double _innerR;
  double _tower_height; 
  int _NbOfBarrel;
  int _DltNbB_E;
  int _NbOfEndcap;
  int _NbOfZRot;
  double _PMTT;
    
  // Center of the tcounter.
  CLHEP::Hep3Vector _center;

  std::unique_ptr<DRFPIcalorimeter> _ldrfpic;


//  int _CDCHnSrvShell;
//  std::vector<double> _CDCHSrvLyInRad;
//  std::vector<double> _CDCHSrvLyOutRad;
//  std::vector<double> _CDCHSrvLyInAngl;
//  std::vector<double> _CDCHSrvLyOutAngl;
//  std::vector<double> _CDCHSrvLyHalfLng;
//  std::vector<int> _CDCHSrvLySlid;

};

}  //namespace drc

#endif /* DRFPIMaker_hh */
