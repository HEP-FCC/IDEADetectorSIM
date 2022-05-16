// TDCH geometry maker
//
// Original author G. Tassielli
//

#include <utilFunctions.hh>
#include "manageException.hh"
#include "SimpleConfig.hh"
//#include "GeomHandle.hh"
//#include "messagefacility/MessageLogger/MessageLogger.h"

#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"

#include "TDCHMaker.hh"
#include "TDCHtracker.hh"
#include "Station.hh"
#include "Plane.hh"
#include "Panel.hh"
#include "Cell.hh"
#include "CellId.hh"
#include "CellGeometryHandle.hh"

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>

#ifndef __CINT__

using CLHEP::Hep3Vector;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

using namespace std;

namespace tdch {

void cellPrinter( const Cell& s){
        cout << s.Id() << endl;
}

void stationPrinter( const Station& st){
        cout << "    Station: " << st.Id() << endl;
}

void planePrinter( const Plane& pl){
        cout << "    Plane: " << pl.Id() << endl;
}

void panelPrinter( const Panel& pn){
        cout << "    Panel: " << pn.Id() << endl;
}

TDCHMaker::TDCHMaker( crd::SimpleConfig const& config):
                                                    		_center(){

	_isExternal     = false;
	int nWireShells, nFoilShells, nWallShells;
	_extFile        = config.getString("tdch.extFile");
	if ( _extFile.size()>1 && ( _extFile.find_last_of(".gdml") || _extFile.find_last_of(".GDML") )!=0 ) _isExternal = true;
	_extWireFile    = config.getString("tdch.extWireFile");
	if ( _isExternal && _extWireFile.size()<1 ) {
		//          throw cet::exception("GEOM")<< "Using the external geometry file you have to insert the file name for the Wire Rotation matrix data\n";
		exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
		e<< "Using the external geometry file you have to insert the file name for the Wire Rotation matrix data\n";
		e.error();
	}
	_r0             = config.getDouble("tdch.r0");
	_z0             = config.getDouble("tdch.z0");
	_halfLength     = config.getDouble("tdch.zHalfLength");
	_rOut           = config.getDouble("tdch.rOut");

	_fillMaterial   = config.getString("tdch.fillMaterial");

	_geomType       = config.getInt("tdch.geomType");
	_endCapType     = config.getInt("tdch.endCapType");
	//    _voxFactor      = config.getDouble("tdch.voxelization");
	//    if (_voxFactor<0) _notExtVoxel=true;
	//    else _notExtVoxel=false;

	_displayGasLayer= config.getBool("tdch.displayGasLayer",false);
	_displayWires   = config.getBool("tdch.displayWires",false);

	_nSWire         = config.getInt("tdch.nSWire",-1);
//	_nSDeltaWire    = config.getInt("tdch.nSDeltaWire",-1);
	_nStations      = config.getInt("tdch.nStation",1);
	_nMeasPlane     = config.getInt("tdch.MeasPlane",1);
	_nSrvPlane      = config.getInt("tdch.SrvPlane",0);
	_nPlane         = _nMeasPlane + _nSrvPlane;
	_nLayer         = config.getInt("tdch.nLayer");
	_nVerticalFWire = config.getInt("tdch.nVerticalFWire",0);
	_cellDimension  = config.getDouble("tdch.cellDimension",0.0);
//	_StoFWireRatio  = config.getInt("tdch.StoFWireRation",1);
	if (_geomType==20) {
		nWireShells     = config.getInt("tdch.nFieldWireShells",0);
		_fWireDiameter  = 0.0;
		if (nWireShells) {
			config.getVectorString("tdch.fieldWireMaterials", _fwMaterialsName, nWireShells);
			config.getVectorDouble("tdch.fieldWireShellsThicknesses", _fwShellsThicknesses, nWireShells);
			for (int is=0; is<nWireShells; is++) {
				_fWireDiameter +=_fwShellsThicknesses.at(is);
			}
			_fWireDiameter*=2.0;
		}
	} else if (_geomType==30 || _geomType==40) {
		nFoilShells     = config.getInt("tdch.nFieldFoilShells",0);
		_fFoilThickness  = 0.0;
		if (nFoilShells) {
			config.getVectorString("tdch.fieldFoilMaterials", _fFoilMaterialsName, nFoilShells);
			config.getVectorDouble("tdch.fieldFoilShellsThicknesses", _fflShellsThicknesses, nFoilShells);
			for (int is=0; is<nFoilShells; is++) {
				_fFoilThickness +=_fflShellsThicknesses.at(is);
			}
		}
	}

	if (_geomType==20 || _geomType==30) {
		nWireShells     = config.getInt("tdch.nCntFieldWireShells",0);
		_cntfWireDiameter  = 0.0;
		if (nWireShells) {
			config.getVectorString("tdch.CntfieldWireMaterials", _cntfwMaterialsName, nWireShells);
			config.getVectorDouble("tdch.CntfieldWireShellsThicknesses", _cntfwShellsThicknesses, nWireShells);
			for (int is=0; is<nWireShells; is++) {
				_cntfWireDiameter +=_cntfwShellsThicknesses.at(is);
			}
			_cntfWireDiameter*=2.0;
		}
	}

	nWireShells     = config.getInt("tdch.nSenseWireShells");
	config.getVectorString("tdch.senseWireMaterials", _swMaterialsName, nWireShells);
	config.getVectorDouble("tdch.senseWireShellsThicknesses", _swShellsThicknesses, nWireShells);
	_sWireDiameter  = 0.0;
	for (int is=0; is<nWireShells; is++) {
		_sWireDiameter +=_swShellsThicknesses.at(is);
	}
	_sWireDiameter*=2.0;

	//    _nInGuardWires  = config.getInt("tdch.nInGuardWires",-1);
	//    _inGuardRad     = config.getDouble("tdch.inGuardRad",0.0);
	//    nWireShells     = config.getInt("tdch.nInGuardWireShells");
	//    config.getVectorString("tdch.inGuardWireMaterials", _inGwMaterialsName, nWireShells);
	//    config.getVectorDouble("tdch.inGuardWireShellsThicknesses", _inGwShellsThicknesses, nWireShells);
	//    _inGWireDiameter= 0.0;
	//    for (int is=0; is<nWireShells; is++) {
	//            _inGWireDiameter +=_inGwShellsThicknesses.at(is);
	//    }
	//    _inGWireDiameter*=2.0;
	//
	//    _nOutGuardWires = config.getInt("tdch.nOutGuardWires",-1);
	//    _outGuardRad    = config.getDouble("tdch.outGuardRad",0.0);
	//    nWireShells     = config.getInt("tdch.nOutGuardWireShells");
	//    config.getVectorString("tdch.outGuardWireMaterials", _outGwMaterialsName, nWireShells);
	//    config.getVectorDouble("tdch.outGuardWireShellsThicknesses", _outGwShellsThicknesses, nWireShells);
	//    _outGWireDiameter= 0.0;
	//    for (int is=0; is<nWireShells; is++) {
	//            _outGWireDiameter +=_outGwShellsThicknesses.at(is);
	//    }
	//    _outGWireDiameter*=2.0;


	_hasWalls = config.getBool("tdch.hasWalls",false);
	if (_hasWalls) {
		_walls.insert( pair<cdch::Wall::Walltype,cdch::Wall*>(cdch::Wall::inner,new cdch::Wall(cdch::Wall::inner)) );
		nWallShells    = config.getInt("tdch.nInnerWallShells");
		std::vector<std::string> *tempWallMaterialsName = new std::vector<std::string>();
		std::vector<double> *tempWallShellsThicknesses = new std::vector<double>();
		config.getVectorString("tdch.innerWallMaterials", *tempWallMaterialsName, nWallShells);
		config.getVectorDouble("tdch.innerWallShellsThicknesses", *tempWallShellsThicknesses, nWallShells);
		multimap<cdch::Wall::Walltype,cdch::Wall* >::iterator walls_it;
		walls_it=_walls.begin();
		walls_it->second->addMaterials(nWallShells,tempWallMaterialsName,tempWallShellsThicknesses);

		_walls.insert( pair<cdch::Wall::Walltype,cdch::Wall*>(cdch::Wall::outer,new cdch::Wall(cdch::Wall::outer)) );
		nWallShells    = config.getInt("tdch.nOuterWallShells");
		tempWallMaterialsName = new std::vector<std::string>();
		tempWallShellsThicknesses = new std::vector<double>();
		config.getVectorString("tdch.outerWallMaterials", *tempWallMaterialsName, nWallShells);
		config.getVectorDouble("tdch.outerWallShellsThicknesses", *tempWallShellsThicknesses, nWallShells);
		walls_it++;
		walls_it->second->addMaterials(nWallShells,tempWallMaterialsName,tempWallShellsThicknesses);

		_walls.insert( pair<cdch::Wall::Walltype,cdch::Wall*>(cdch::Wall::endcap,new cdch::Wall(cdch::Wall::endcap)) );
		nWallShells    = config.getInt("tdch.nEndCapWallShells");
		tempWallMaterialsName = new std::vector<std::string>();
		tempWallShellsThicknesses = new std::vector<double>();
		config.getVectorString("tdch.endcapWallMaterials", *tempWallMaterialsName, nWallShells);
		config.getVectorDouble("tdch.endcapWallShellsThicknesses", *tempWallShellsThicknesses, nWallShells);
		walls_it++;
		walls_it->second->addMaterials(nWallShells,tempWallMaterialsName,tempWallShellsThicknesses);
	}

	_detailedWireSupport = config.getBool("tdch.detailedWireSupport",false);
//    _isElectCont = ( config.getDouble("tdch.elctContRmax",0.0)>_rOut )? true : false;
//    if (_isElectCont) { _elctContWallThick =  config.getDouble("tdch.elctContWallThick"); }

	// Do the real work.
	Build( );
}

TDCHMaker::~TDCHMaker (){}

//void TDCHMaker::loadTCounter( crd::SimpleConfig const& config ){
//
//    config.getVectorDouble("tdch.scintThick",_scintThick,_nShell);
//    _fillScintMaterial = config.getString("tdch.scintMat");
//
//}

void TDCHMaker::Build(){

  _ltdch = unique_ptr<TDCHtracker>(new TDCHtracker());
  _ltdch->_isExternal = _isExternal;
  _ltdch->_geomType=_geomType;

  if (_nStations<1) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"TDCH: Number of Station is not acceptable\n";
    e.error();
  }
  if (_nPlane>_ltdch->maxNPlnPerSttn()) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"Number of Plane per Station is grater than maximum allowed for geometry type:"<<_geomType<<"\n";
    e.error();
  }

  if (_isExternal) {
    //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"TDCH: Using GDML file option is temporarily disabled\n";
    e.error();

  } else {

      cdch::Wall *tmpInnerWall, *tmpOuterWall, *tmpEndCapWall;

      _ltdch->_nSWire          = _nSWire;
//      _ltdch->_nSDeltaWire     = _nSDeltaWire;
      _ltdch->_nLayer          = _nLayer;
      _ltdch->_nStations       = _nStations;
      _ltdch->_nPlane          = _nPlane;
      _ltdch->_nMeasPlane      = _nMeasPlane;
      _ltdch->_nSrvPlane       = _nSrvPlane;
      _ltdch->_nPanel          = _nPanel;

      _ltdch->_r0              = _r0;
      _ltdch->_z0              = _z0;
      _ltdch->_rOut            = _rOut;

      _ltdch->_zHalfLength     = _halfLength;

      _ltdch->_displayGasLayer = _displayGasLayer;
      _ltdch->_displayWires    = _displayWires;

      _ltdch->_hasWalls        = _hasWalls;

      Station *_sttn     = new Station[_nStations];
      std::cout<<"TOT N Stations "<<_nStations<<std::endl;
//      double epsilonOutGwRing, thetaOutGwRing, ringangleOutGwRing, ringangleOutGwRing_1, halfalphaOutGwRing, zlengthOutGwRing;
//      epsilonOutGwRing=thetaOutGwRing=ringangleOutGwRing=ringangleOutGwRing_1=halfalphaOutGwRing=zlengthOutGwRing=0.0;
/*

      if (_hasWalls) {

    	  tmpInnerWall           = _walls.find(cdch::Wall::inner)->second;
    	  tmpOuterWall           = _walls.find(cdch::Wall::outer)->second;
    	  tmpEndCapWall          = _walls.find(cdch::Wall::endcap)->second;

    	  //------------------------------------------------------------------------------

    	  double inner_radius             =        _r0                               ;
    	  //double endcap_inner_radius;
    	  double outer_radius             =        _rOut                             ;
    	  double envelop_Inner_thickness  =        tmpInnerWall->getTotalThickness() ;
    	  double envelop_Outer_thickness  =        tmpOuterWall->getTotalThickness() ;
    	  double envelop_EndCap_thickness =        tmpEndCapWall->getTotalThickness();
    	  double extra_EndCap_dist;

    	  double halfLength               =        _halfLength                       ;

    	  int    EndCap_type              =        _endCapType                       ;

    	  //-------------------

    	  double max_EndCap_dim;

    	  double EndCap_Wall_theta_inner   ;
    	  double EndCap_Wall_theta_outer   ;

//    	  boost::shared_ptr<Layer> lay;

    	  inscribedRadius         = 0.0;


    	  //endcap_inner_radius     = inner_radius;
    	  extra_EndCap_dist       = 0.0*CLHEP::mm;
    	  max_EndCap_dim          = halfLength;

    	  EndCap_Wall_theta_inner = 0.;
    	  EndCap_Wall_theta_outer = 0.;

    	  if(EndCap_type==0) {
    		  _ltdch->_endcapType     = CDCHtracker::Plane;
    		  halfLength = halfLength-envelop_EndCap_thickness;

    		  tmpEndCapWall->_pRmin = inner_radius;
    		  tmpEndCapWall->_pRmax = outer_radius;
    		  tmpEndCapWall->_pSPhi = 0.0;
    		  tmpEndCapWall->_pDPhi = 360.0*CLHEP::degree;
    		  tmpEndCapWall->_pDz   = envelop_EndCap_thickness*0.5;
    		  tmpEndCapWall->_name  = "EndCapWall_R";
    		  tmpEndCapWall->_pos   = HepGeom::Translate3D(0.0,0.0,halfLength+tmpEndCapWall->_pDz);

    	  }
    	  else if(EndCap_type==1){
    		  _ltdch->_endcapType     = CDCHtracker::Spherical;
    		  max_EndCap_dim = sqrt(sum_of_squares(halfLength, outer_radius));
    		  EndCap_Wall_theta_inner = asin(inner_radius/(max_EndCap_dim-envelop_EndCap_thickness)) * CLHEP::radian;
    		  EndCap_Wall_theta_outer = acos(halfLength/max_EndCap_dim) * CLHEP::radian;
    		  halfLength-=envelop_EndCap_thickness*halfLength/max_EndCap_dim;  // is equivalent (max_EndCap_dim-envelop_EndCap_thickness)*halfLength/max_EndCap_dim;
    		  extra_EndCap_dist=sqrt(diff_of_squares(max_EndCap_dim-envelop_EndCap_thickness, inner_radius)) - halfLength;

    		  tmpEndCapWall->_pRmin   = max_EndCap_dim-envelop_EndCap_thickness;
    		  tmpEndCapWall->_pRmax   = max_EndCap_dim;
    		  tmpEndCapWall->_pSPhi   = 0.0;
    		  tmpEndCapWall->_pDPhi   = 360.0*CLHEP::degree;
    		  tmpEndCapWall->_pDz     = envelop_EndCap_thickness*0.5;
    		  tmpEndCapWall->_name    = "EndCapWall_R";
    		  tmpEndCapWall->_pSTheta = EndCap_Wall_theta_inner;
    		  tmpEndCapWall->_pDTheta = EndCap_Wall_theta_outer-EndCap_Wall_theta_inner;
    	  }

    	  cdch::Wall *tmpEndCapWall_L = new cdch::Wall(*tmpEndCapWall);
    	  tmpEndCapWall_L->_name               = "EndCapWall_L";
    	  tmpEndCapWall_L->_pos                = HepGeom::RotateY3D(180.0*CLHEP::degree)*tmpEndCapWall_L->_pos;
    	  _walls.insert( pair<cdch::Wall::Walltype,cdch::Wall*>(tmpEndCapWall_L->getType(), tmpEndCapWall_L) );

    	  _ltdch->_max_EndCap_dim = max_EndCap_dim;

    	  tmpInnerWall->_pRmin  = inner_radius;
    	  tmpInnerWall->_pRmax  = inner_radius+envelop_Inner_thickness;
    	  tmpInnerWall->_pSPhi  = 0.0;
    	  tmpInnerWall->_pDPhi  = 360.0*CLHEP::degree;
    	  tmpInnerWall->_pDz    = halfLength + extra_EndCap_dist;
    	  tmpInnerWall->_name   = "InnerWall";

    	  tmpOuterWall->_pRmin  = outer_radius - envelop_Outer_thickness;
    	  tmpOuterWall->_pRmax  = outer_radius;
    	  tmpOuterWall->_pSPhi  = 0.0;
    	  tmpOuterWall->_pDPhi  = 360.0*CLHEP::degree;
    	  tmpOuterWall->_pDz    = halfLength;
    	  tmpOuterWall->_name   = "OuterWall";

    	  FWradii           = 0.5*fieldwire_diameter;
    	  radius_ring_0     = inner_radius + envelop_Inner_thickness + FWradii + secure + capGasLayer;
    	  delta_radius_ring = 0.0;
    	  zlength           = halfLength;

    	  radius_ringOut_0  = radius_ring_0-FWradii-secure;  // is the radius In, there is Out just for a computation optimization
    	  radius_ringOut    = radius_ringOut_0+drop;
    	  //epsilonOut        = atan((radius_ringOut+drop)/halfLength*sin(halfalpha));
    	  epsilonOut        = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0)) / halfLength) * CLHEP::radian;

    	  int superlayer=0, iring=0;

    	  if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );

    	  _sttn[0]._id = StationId(0);

    	  _sttn[0].addLayer(new Layer());
    	  lay = _sttn[0]._layers.back();

    	  double fakeLayerInIWthick(-0.0001);
    	  int nSub = tmpInnerWall->getNShells();
    	  for (int ishell=0; ishell<nSub; ishell++){
    		  std::string iWallShellMat = tmpInnerWall->getMaterialsName()->at(ishell);
    		  if ( iWallShellMat.find("Gas")!=std::string::npos ) {
    			  fakeLayerInIWthick+=tmpInnerWall->getThicknesses()->at(ishell);
    		  }
    	  }

//    	  lay->_detail.reset( new LayerDetail(inner_radius+envelop_Inner_thickness-fakeLayerInIWthick,radius_ringOut_0,0.0,epsilonOut,zlength,_fillMaterial) );
//    	  //lay->_id._sid=&(_sttn[0]._id);
//    	  //lay->_id._id=-1;
//    	  lay->_id = LayerId(&_sttn[0]._id, -1);

      }


	  double fieldwire_diameter       =        _fWireDiameter                    ;
	  //double sensewire_diameter       =      _sWireDiameter                    ;

	  int   num_wire_sense            =        _nSWire                           ;

	  int   delta_num_wire_sense      =        _nSDeltaWire                      ;
	  int   nstation                  =        _nStation                         ;
	  int   nlayer                    =        _nLayer                           ;
	  int   geomType                  =        _geomType                         ;

	  double FWradii, radius_ring_0, radius_ring, epsilon, radius_ringOut_0, radius_ringOut, epsilonOut, radius_ringIn_0,
	  radius_ringIn, epsilonIn, cellBase, inscribedRadius, circumscribedRadius, delta_radius_ring, zlength, phi=0.0, theta_ring=0.0, ringangle=0.0;
	  int   sign_epsilon      = -1;
	  int   num_wire          =  0;

	  double secure           = 1.0e-2;                        //Extra volume layer thickness to avoid wire illegal overlap
	  double capGasLayer      = 1.0e-3;                        //Thickness of the closing inner gas layer, its is less enough just to be different from 0
	  double extShiftFW       = 1.55e-3;                       //Extra distance between Field wire with opposite sign to avoid overlaps

	  double CntFWradii=0.5*_cntfWireDiameter;
*/

	  //char wshape[30], gshape[30], wvol[30], gvol[30], shape_name_FD[30], shape_name_SD[30], vol_name_FD[30], vol_name_SD[30];



    // Master geometry for the CDCHtracker.
//    GeomHandle<tdch::TDCHtracker> tdch;
//    for (int iCDCHsl=0; iCDCHsl<tdch->nSuperLayers(); ++iCDCHsl) {
//    	cdch::SuperLayer* cdchSl = tdch->getSuperLayer(iCDCHsl);
//    	for (int iCDCHly=0; iCDCHly<cdchSl->nLayers(); ++iCDCHly) {
//    		boost::shared_ptr<cdch::Layer> cdchLy = cdchSl->getLayer(iCDCHly);
//    		if (cdchLy->getLayerType() == cdch::Layer::serv ) {
//    			++_CDCHnSrvShell;
//    			boost::shared_ptr<cdch::LayerDetail> cdchLyD = cdchLy->getDetail();
//    			_CDCHSrvLyInRad.push_back(cdchLyD->centerInnerRadiusRing());
//    			_CDCHSrvLyOutRad.push_back(cdchLyD->centerOuterRadiusRing());
//    			_CDCHSrvLyInAngl.push_back(cdchLyD->stereoAngleInnerRing());
//    			_CDCHSrvLyOutAngl.push_back(cdchLyD->stereoAngleOuterRing());
//    			_CDCHSrvLyHalfLng.push_back(cdchLyD->halfLength());
//    			_CDCHSrvLySlid.push_back(cdchLy->Id().getSuperLayer());
//    		}
//    	}
//    }

//    if (_CDCHnSrvShell!=_nStation) {
//      exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//      e<<"TDCH: nShell differs from n. of CDCH Service Shells \n";
//      e.error();
//    }

    Station *_station     = new Station[_nStations];
//    boost::shared_ptr<SLayer> lay;
    for (int iShl=0; iShl<_nStations; ++iShl) {
//
//    	if (_shellThick[iShl]>(_CDCHSrvLyOutRad[iShl]-_CDCHSrvLyInRad[iShl])) {
//    		exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//    		e<<"TDCH: "<<iShl<<" Shell thickness is larger than the available space in the corresponding CDCH Service Shell \n";
//    		e<<"Shell thick: "<<_shellThick[iShl]<<" Serv Thick: "<<(_CDCHSrvLyOutRad[iShl]-_CDCHSrvLyInRad[iShl])<<"\n";
//    		e.error();
//    	}
//    	_ltdch->_shellThick.push_back(_shellThick[iShl]);
//    	_ltdch->_scintThick.push_back(_scintThick[iShl]);
//    	_ltdch->_radThick.push_back(_radThick[iShl]);
//    	_ltdch->_shellRadIn.push_back(_CDCHSrvLyInRad[iShl]);
//    	double radIn = _CDCHSrvLyOutRad[iShl]-_scintThick[iShl]-_radThick[iShl];
//
//    	_shell[iShl]._id = ShellId(iShl,_CDCHSrvLySlid[iShl]);
//
//    	_shell[iShl].addSLayer(new SLayer());
//        lay = _shell[iShl]._layers.back();
//        lay->_detail.reset( new StationDetail(radIn,radIn+_radThick[iShl],_CDCHSrvLyInAngl[iShl],_CDCHSrvLyInAngl[iShl],_CDCHSrvLyHalfLng[iShl],_fillRadMaterial) );
//        lay->_id = StationId(&_shell[iShl]._id, 0);
//        lay->_layerType=SLayer::radiat;
//
//        _shell[iShl].addSLayer(new SLayer());
//        lay = _shell[iShl]._layers.back();
//        lay->_detail.reset( new StationDetail(radIn+_radThick[iShl],_CDCHSrvLyOutRad[iShl],_CDCHSrvLyOutAngl[iShl],_CDCHSrvLyOutAngl[iShl],_CDCHSrvLyHalfLng[iShl],_fillScintMaterial) );
//        lay->_id = StationId(&_shell[iShl]._id, 1);
//        lay->_layerType=SLayer::scint;
////        if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
////        lay->_voxelizationFactor=voxelizationFactor;
//
////FIX for future! do something like this to insert Fiber and Wires
////		boost::shared_ptr<WireDetail> sw;
////		boost::shared_ptr<CellDetail> celld;
////		sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
////		celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );
////
////		epsilon          = atan(senseWireRing_radius_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
////
////		CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);
//

    }

    _ltdch->_station.reset(_station);
    //    _ltdch->_roChhnd.reset( new ROGeometryHandle(_ltdch.get()) );

  }
}

} // namespace tdch

#endif
