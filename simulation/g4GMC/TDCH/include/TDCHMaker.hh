// PHCV geometry maker
//
// Original author G. Tassielli
//

#ifndef TDCHMaker_hh
#define TDCHMaker_hh

#include <map>
#include <memory>
#include <string>
#include <vector>

//#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include "CLHEP/Vector/ThreeVector.h"

#include "StationInfo.hh"
#include "Wall.hh"
#include "Wire.hh"

namespace crd {
class SimpleConfig;
}

namespace tdch {

class TDCHtracker;
class Plane;

class WireDetail;
class CellDetail;

class TDCHMaker{

public:

  TDCHMaker( crd::SimpleConfig const& config );
  ~TDCHMaker ();

  // This is the accessor that will remain.
  std::unique_ptr<TDCHtracker> getTDCHtrackerPtr() { return std::move(_ltdch); }

private:

  void Build();

  // Name of external gdml geometry file description.
  std::string _extFile;
  std::string _extWireFile;
  bool _isExternal;

  // Basic geometry element parameter
  int _nSWire;                  //Number of sense wire of a panel layer
  int _nShift;                  //n. of cell quarter shift between the cell layers
  int _nLayer;                  //Number of cell layers  per Panel
  int _nPanel;                  //Number of Panel per Plane
  int _nMeasPlane;              //Number of (active) Planes per Station
  double  _planeRot;            //rotation angle between consecutive planes in a Station
  int _nSrvPlane;               //Number of (inactive/for service) Planes per Station
  int _nPlane;                  //Number of Planes per Station
  int _nStations;               //Number of Station
  double  _stationRot;          //rotation angle between consecutive Stations

  double _cellDimension;        //Cell dimension in the case of Square cells
  int _cellType;                //0: open cell, 1: field made with planes, 2: straw

  int _nVerticalFWire;          //if cellType 0 or 1: Number of additional Vertical Field Wire in the case of Square cells
  int _nFWirePerCell;           //if cellType 0: Number of Field Wire per cells

  double _r0;                   //Nominal Inner radius of the tracker
  double _halfLength;           //Nominal Half-Length of the tracker in the barrel region
  double _rOut;                 //Nominal Outer radius of the tracker
  bool _hasWalls;

  int _geomType;                //Verison number (see TDCHtracker for definifiton) (from it depends the Cell Geometry type: 2 square, 3 cylindrical)
  int _endCapType;              //EndCap shape type: 0 plane, 1 spherical
//  double _voxFactor;            //voxelization optimization factor
//  bool _notExtVoxel;
  bool _displayGasLayer;        //Allow to display the gas inside the chamber
  bool _displayWires;           //Allow to display every wires inside gas inside the chamber.

  double _z0;                   //Shift along z of the center of the tracker

  bool _detailedWireSupport;
//  bool _isElectCont;
//  double _elctContWallThick;

  // Number of planes and of cells per layer in station.
  std::vector<StationInfo> _stationInfo;

  //Active gas material
  std::string _fillMaterial;

  //Wire dimensions and materials composition
  double _fWireDiameter;
  double _cntfWireDiameter;
  double _fFoilThickness;
  double _cntfFoilThickness;
  double _sWireDiameter;
  // Names of the materials of the wires.
  std::vector<std::string> _fwMaterialsName;
  std::vector<std::string> _cntfwMaterialsName;
  std::vector<std::string> _fFoilMaterialsName;
  std::vector<std::string> _cntfFoilMaterialsName;
  std::vector<std::string> _swMaterialsName;

  std::vector<double> _fwShellsThicknesses;
  std::vector<double> _cntfwShellsThicknesses;
  std::vector<double> _fflShellsThicknesses;
  std::vector<double> _cntfflShellsThicknesses;
  std::vector<double> _swShellsThicknesses;

//  //Guard Wire dimensions and materials composition
//  int _nInGuardWires;
//  int _nOutGuardWires;
//  double _inGuardRad;
//  double _outGuardRad;
//
//  double _inGWireDiameter;
//  double _outGWireDiameter;
//  // Names of the materials of the wires.
//  std::vector<std::string> _inGwMaterialsName;
//  std::vector<std::string> _outGwMaterialsName;
//
//  std::vector<double> _inGwShellsThicknesses;
//  std::vector<double> _outGwShellsThicknesses;

  //Walls descriptions
  std::multimap<cdch::Wall::Walltype,cdch::Wall* > _walls;

  // Center of the tracker.
  CLHEP::Hep3Vector _center;

  std::unique_ptr<TDCHtracker> _ltdch;

};

}  //namespace tdch

#endif /* TDCHMaker_hh */
