// PSHW geometry maker
//
// Original author G. Tassielli
//

#ifndef PSHWMaker_hh
#define PSHWMaker_hh

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

namespace pshw {

class PSHWtracker;
class PSHWRadiator;
//class Wall;

class PSHWMaker{

public:

  PSHWMaker( crd::SimpleConfig const& config );
  ~PSHWMaker ();

  // This is the accessor that will remain.
  std::unique_ptr<PSHWtracker> getPSHWtrackerPtr() { return std::move(_lpst); }
  std::unique_ptr<PSHWRadiator> getPSHWradiatorPtr() { return std::move(_lpsrad); }

private:

  void loadBarrelTracker( crd::SimpleConfig const& config );
  void loadForwardTracker( crd::SimpleConfig const& config );
  void Build();
  void loadBarrelRadiator( crd::SimpleConfig const& config );
  void loadForwardRadiator( crd::SimpleConfig const& config );
  void BuildRadiator();

  // Name of external gdml geometry file description.
  std::string _extFile;
  bool _isExternal;

  // Basic geometry element parameter
  //Barrel Region ones
  int _nLayers;                 //Number of Layers
  double _r0;                   //Nominal Inner radius of the tracker
  double _halfLength;           //Nominal Half-Length of the tracker in the barrel region
  double _rOut;                 //Nominal Outer radius of the tracker

  //Forward Region ones
  int _nFwdLayers;              //Number of Layers in The Forward region
  double _r0_fwd;               //Nominal Inner radius of the tracker
  double _halfLength_fwd;       //Nominal Half-Length of the tracker in the barrel region
  double _rOut_fwd;             //Nominal Outer radius of the tracker
  double _zPos_fwd;             //position of the Forward mother volume

  int _geomType;                //Verison number (see PSHWtracker for definifiton) (from it depends the Cell Geometry type: 2 hexagonal, 3 square)

  double _z0;                   //Shift along z of the center of the tracker

  //Detailed parameters

  std::vector<double> _LayersInnerRad;
  std::vector<int>    _nPhiSectors;
  std::vector<int>    _nLaddersPerSector;
  std::vector<double> _LaddersHalfLength;
  std::vector<double> _LaddersWidth;
  std::vector<double> _LaddersThickness;
  std::vector<int>    _ROTypes;
  std::vector<double>    _ROfirstSideDim;
  std::vector<double>    _ROfirstSideInsul;
  std::vector<double>    _ROSecondSideDim;
  std::vector<double>    _ROSecondSideInsul;
  std::vector<double>    _ROangle;
  std::vector<unsigned int> _nROsFstSdPerLadder;
  std::vector<unsigned int> _nROsSndSdPerLadder;
  std::vector<unsigned long> _nROsPerLadder;

  std::vector<double> _phiAngles;

  std::vector<int>    _LaddersNmShells;
  std::vector< std::vector<std::string> > _LaddersShellsMaterial;
  std::vector< std::vector<double> > _LaddersShellsThick;
  std::vector<int>    _ReadoutsNmShells;
  std::vector< std::vector<int> > _ReadOutsShellIds;

  //Detailed parameters for the Forward layers

  std::vector<double> _LayersInnerRad_fwd;
  std::vector<double> _LayersZPos_fwd;
  std::vector<double> _LayersHeight_fwd;
  std::vector<int>    _nPhiSectors_fwd;
  std::vector<int>    _nLaddersPerSector_fwd;
//  std::vector<double> _LayersHeight_fwd;
  std::vector<double> _LaddersThickness_fwd;
  std::vector<int>    _ROTypes_fwd;
  std::vector<double>    _ROfirstSideDim_fwd;
  std::vector<double>    _ROfirstSideInsul_fwd;
  std::vector<double>    _ROSecondSideDim_fwd;
  std::vector<double>    _ROSecondSideInsul_fwd;
  std::vector<double>    _ROangle_fwd;

  std::vector<double> _phiAngles_fwd;

  std::vector< std::vector<double> > _LaddersHeight_fwd;
  std::vector< std::vector<double> > _LaddersShrtSdDim_fwd;
  std::vector< std::vector<double> > _LaddersLngSdDim_fwd;

  std::vector<int>    _LaddersNmShells_fwd;
  std::vector< std::vector<std::string> > _LaddersShellsMaterial_fwd;
  std::vector< std::vector<double> > _LaddersShellsThick_fwd;
  std::vector<int>    _ReadoutsNmShells_fwd;
  std::vector< std::vector<int> > _ReadOutsShellIds_fwd;

  std::vector< std::vector<unsigned int> > _nROsFstSdShrtPerLadder_fwd;
  std::vector< std::vector<unsigned int> > _nROsFstSdLngPerLadder_fwd;
  std::vector< std::vector<unsigned int> > _nROsSndSdPerLadder_fwd;
  std::vector< std::vector<unsigned long> > _nROsPerLadder_fwd;
  std::vector< std::vector<int> > _LaddersNRwDltRO_fwd;
  std::vector< std::vector<int> > _LaddersNRwAddDltRO_fwd;

  //Absorber material description
  //Barrel region
  int _nRadiatLayers;
  std::vector<double> _RadiatInRasius;
  std::vector<double> _RadiatHalfLengths;
  std::vector<int>    _RadiatNmShells;
  std::vector< std::vector<std::string> > _RadiatShellsMaterial;
  std::vector< std::vector<double> > _RadiatShellsThick;
  std::vector< double > _RadiatersThickness;

  //Forward region
  int _nRadiatLayers_fwd;
  std::vector<double> _RadiatInRasius_fwd;
  std::vector<double> _RadiatOutRasius_fwd;
  std::vector<double> _RadiatZpos_fwd;
  std::vector<int>    _RadiatNmShells_fwd;
  std::vector< std::vector<std::string> > _RadiatShellsMaterial_fwd;
  std::vector< std::vector<double> > _RadiatShellsThick_fwd;
  std::vector< double > _RadiatersThickness_fwd;

  // Center of the tracker.
  CLHEP::Hep3Vector _center;

  std::unique_ptr<PSHWtracker> _lpst;
  std::unique_ptr<PSHWRadiator> _lpsrad;

};

}  //namespace pshw

#endif /* PSHWMaker_hh */
