// CDCH geometry maker
//
// Original author G. Tassielli
//

#include <utilFunctions.hh>
#include "manageException.hh"
#include "SimpleConfig.hh"

#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"

#include "CDCHMaker.hh"
#include "CDCHtracker.hh"
#include "SuperLayer.hh"
#include "Layer.hh"
#include "Cell.hh"
#include "CellId.hh"
#include "CellGeometryHandle.hh"
#include "CellGeometryHandle_ExtGeom.hh"
#include "CellGeometryHandle_v2.hh"
#include "CellGeometryHandle_v3.hh"
#include "CellGeometryHandle_v4.hh"
#include "CellGeometryHandle_v2_DBL.hh"
#include "CellGeometryHandle_v3_DBL.hh"

#include <cmath>
#include <iomanip>
#include <iostream>

#ifndef __CINT__

using CLHEP::Hep3Vector;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

using namespace std;

namespace cdch {

void cellPrinter( const Cell& s){
        cout << s.Id() << endl;
}

//void cellPrinter2( const Cell* s, int& i){
//        cout << s->Id() <<  " | "
//                        << s->hack << " "
//                        << ++i << endl;
//}
//
//void cellHacker( Cell* s, int& i){
//        s->hack = 2;
//}

void layerPrinter( const SuperLayer& l){
        cout << "    Layer: " << l.Id() << endl;
}

// Constructor that gets information from the config file instead of
// from arguments.
CDCHMaker::CDCHMaker( crd::SimpleConfig const& config):
                                                    _center(){

        _isExternal     = false;
        int nWireShells, nWallShells;
        _extFile        = config.getString("cdch.extFile");
        if ( _extFile.size()>1 && ( _extFile.find_last_of(".gdml") || _extFile.find_last_of(".GDML") )!=0 ) _isExternal = true;
        _extWireFile    = config.getString("cdch.extWireFile");
        if ( _isExternal && _extWireFile.size()<1 ) {
//          throw cet::exception("GEOM")<< "Using the external geometry file you have to insert the file name for the Wire Rotation matrix data\n";
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<< "Using the external geometry file you have to insert the file name for the Wire Rotation matrix data\n";
          e.error();
        }
        _r0             = config.getDouble("cdch.r0");
        _z0             = config.getDouble("cdch.z0");
        _halfLength     = config.getDouble("cdch.zHalfLength");
        _rOut           = config.getDouble("cdch.rOut");
        _drop           = config.getDouble("cdch.drop",0.0);
        _alpha          = config.getDouble("cdch.alpha",0.0);
        _isDumbbell     = config.getBool("cdch.isDumbbell",false);
        if (_isDumbbell) config.getVectorDouble("cdch.zZonesLimits", _zZones, 2);

        _fillMaterial   = config.getString("cdch.fillMaterial");

        _geomType       = config.getInt("cdch.geomType");
        _endCapType     = config.getInt("cdch.endCapType");
        _voxFactor      = config.getDouble("cdch.voxelization");
        if (_voxFactor<0) _notExtVoxel=true;
        else _notExtVoxel=false;

        _displayGasLayer= config.getBool("cdch.displayGasLayer",false);
        _displayWires   = config.getBool("cdch.displayWires",false);

        _nSWire         = config.getInt("cdch.nSWire",-1);
        _nSDeltaWire    = config.getInt("cdch.nSDeltaWire",-1);
        _nSuperLayer    = config.getInt("cdch.nSuperLayer",1);
        _nRing          = config.getInt("cdch.nRing",1);
        _nVerticalFWire = config.getInt("cdch.nVerticalFWire",0);
        _cellDimension  = config.getDouble("cdch.cellDimension",0.0);
        _FWireStep      = config.getDouble("cdch.FWireStep",0.0);
        _StoFWireRatio  = config.getInt("cdch.StoFWireRation",1);
        if (_geomType==30 || _geomType==31 || _geomType==41 || _geomType==42) {
                _nSuperLayer    = config.getInt("cdch.nLayer");
                _nRing          = 1;
        }

        _nOuterShell    = config.getInt("cdch.nOuterShell",0);
        _OSnTotSuperLayer=0;
        if (_geomType==60) {
            config.getVectorInt("cdch.OSnSWire", _OSnSWire, _nOuterShell);
            config.getVectorDouble("cdch.OScellDimension", _OScellDimension, _nOuterShell);
            config.getVectorInt("cdch.OSnSDeltaWire", _OSnSDeltaWire, _nOuterShell);
            config.getVectorInt("cdch.OSnSuperLayer", _OSnSuperLayer, _nOuterShell);
            config.getVectorInt("cdch.OSnRing", _OSnRing, _nOuterShell);
            config.getVectorDouble("cdch.OSalpha", _OSalpha, _nOuterShell);
            config.getVectorDouble("cdch.OSservSpace", _OSservSpace, _nOuterShell);
        	for (int iOS=0; iOS<_nOuterShell; ++iOS) {
        		_OSnTotSuperLayer+=_OSnSuperLayer.at(iOS);
        	}
        	_nSuperLayer+=_OSnTotSuperLayer;
        }

        nWireShells     = config.getInt("cdch.nFieldWireShells");
        config.getVectorString("cdch.fieldWireMaterials", _fwMaterialsName, nWireShells);
        config.getVectorDouble("cdch.fieldWireShellsThicknesses", _fwShellsThicknesses, nWireShells);
        _fWireDiameter  = 0.0;
        for (int is=0; is<nWireShells; is++) {
                _fWireDiameter +=_fwShellsThicknesses.at(is);
        }
        _fWireDiameter*=2.0;

        nWireShells     = config.getInt("cdch.nCntFieldWireShells",0);
        _cntfWireDiameter  = 0.0;
        if (nWireShells>0) {
          config.getVectorString("cdch.CntfieldWireMaterials", _cntfwMaterialsName, nWireShells);
          config.getVectorDouble("cdch.CntfieldWireShellsThicknesses", _cntfwShellsThicknesses, nWireShells);
          for (int is=0; is<nWireShells; is++) {
            _cntfWireDiameter +=_cntfwShellsThicknesses.at(is);
          }
          _cntfWireDiameter*=2.0;
        }

        nWireShells     = config.getInt("cdch.nSenseWireShells");
        config.getVectorString("cdch.senseWireMaterials", _swMaterialsName, nWireShells);
        config.getVectorDouble("cdch.senseWireShellsThicknesses", _swShellsThicknesses, nWireShells);
        _sWireDiameter  = 0.0;
        for (int is=0; is<nWireShells; is++) {
                _sWireDiameter +=_swShellsThicknesses.at(is);
        }
        _sWireDiameter*=2.0;

        _nInGuardWires  = config.getInt("cdch.nInGuardWires",-1);
        _inGuardRad     = config.getDouble("cdch.inGuardRad",0.0);
        nWireShells     = config.getInt("cdch.nInGuardWireShells");
        config.getVectorString("cdch.inGuardWireMaterials", _inGwMaterialsName, nWireShells);
        config.getVectorDouble("cdch.inGuardWireShellsThicknesses", _inGwShellsThicknesses, nWireShells);
        _inGWireDiameter= 0.0;
        for (int is=0; is<nWireShells; is++) {
                _inGWireDiameter +=_inGwShellsThicknesses.at(is);
        }
        _inGWireDiameter*=2.0;

        _nOutGuardWires = config.getInt("cdch.nOutGuardWires",-1);
        _outGuardRad    = config.getDouble("cdch.outGuardRad",0.0);
        nWireShells     = config.getInt("cdch.nOutGuardWireShells");
        config.getVectorString("cdch.outGuardWireMaterials", _outGwMaterialsName, nWireShells);
        config.getVectorDouble("cdch.outGuardWireShellsThicknesses", _outGwShellsThicknesses, nWireShells);
        _outGWireDiameter= 0.0;
        for (int is=0; is<nWireShells; is++) {
                _outGWireDiameter +=_outGwShellsThicknesses.at(is);
        }
        _outGWireDiameter*=2.0;


        _walls.insert( pair<Wall::Walltype,Wall*>(Wall::inner,new Wall(Wall::inner)) );
        nWallShells    = config.getInt("cdch.nInnerWallShells");
        std::vector<std::string> *tempWallMaterialsName = new std::vector<std::string>();
        std::vector<double> *tempWallShellsThicknesses = new std::vector<double>();
        config.getVectorString("cdch.innerWallMaterials", *tempWallMaterialsName, nWallShells);
        config.getVectorDouble("cdch.innerWallShellsThicknesses", *tempWallShellsThicknesses, nWallShells);
        multimap<Wall::Walltype,Wall* >::iterator walls_it;
        walls_it=_walls.begin();
        walls_it->second->addMaterials(nWallShells,tempWallMaterialsName,tempWallShellsThicknesses);

        _walls.insert( pair<Wall::Walltype,Wall*>(Wall::outer,new Wall(Wall::outer)) );
        nWallShells    = config.getInt("cdch.nOuterWallShells");
        tempWallMaterialsName = new std::vector<std::string>();
        tempWallShellsThicknesses = new std::vector<double>();
        config.getVectorString("cdch.outerWallMaterials", *tempWallMaterialsName, nWallShells);
        config.getVectorDouble("cdch.outerWallShellsThicknesses", *tempWallShellsThicknesses, nWallShells);
        walls_it++;
        walls_it->second->addMaterials(nWallShells,tempWallMaterialsName,tempWallShellsThicknesses);

        _walls.insert( pair<Wall::Walltype,Wall*>(Wall::endcap,new Wall(Wall::endcap)) );
        nWallShells    = config.getInt("cdch.nEndCapWallShells");
        tempWallMaterialsName = new std::vector<std::string>();
        tempWallShellsThicknesses = new std::vector<double>();
        config.getVectorString("cdch.endcapWallMaterials", *tempWallMaterialsName, nWallShells);
        config.getVectorDouble("cdch.endcapWallShellsThicknesses", *tempWallShellsThicknesses, nWallShells);
        walls_it++;
        walls_it->second->addMaterials(nWallShells,tempWallMaterialsName,tempWallShellsThicknesses);

        _detailedWireSupport = config.getBool("cdch.detailedWireSupport",false);
        _isElectCont = ( config.getDouble("cdch.elctContRmax",0.0)>_rOut )? true : false;
        if (_isElectCont) { _elctContWallThick =  config.getDouble("cdch.elctContWallThick"); }

        // Do the real work.
        Build( );
}



CDCHMaker::~CDCHMaker (){}

void CDCHMaker::Build(){

        _ltt = unique_ptr<CDCHtracker>(new CDCHtracker());
        _ltt->_isExternal = _isExternal;
        _ltt->_geomType=_geomType;

        if (_nRing>_ltt->maxNLayPerSL()) {
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Number of Layer per Superlayer is grater than maximum allowed for geometry type:"<<_geomType<<"\n";
          e.error();
        }

        if (_isExternal) {
//                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Using GDML file option is temporarily disabled\n";
          e.error();


                //                _ltt->_z0         = _z0;
                //                /*
                //        _ltt->_r0         = _r0;
                //        _ltt->_rOut       = _rOut;
                //                 */
                //                _ltt->_extFile    = _extFile;
                //                _ltt->_nSWire     = _nSWire;
                //                _ltt->_nSDeltaWire= _nSDeltaWire;
                //                _ltt->_nSuperLayer= _nSuperLayer;
                //                _ltt->_nRing      = _nRing;
                //                _ltt->_cellhnd.reset(new CellGeometryHandle_ExtGeom(_extWireFile.c_str()));

        } else {

                Wall *tmpInnerWall, *tmpOuterWall, *tmpEndCapWall;

                _ltt->_nSWire          = _nSWire;
                _ltt->_nSDeltaWire     = _nSDeltaWire;
                _ltt->_nSuperLayers    = _nSuperLayer;
                _ltt->_nRing           = _nRing;

                _ltt->_r0              = _r0;
                _ltt->_z0              = _z0;
                _ltt->_rOut            = _rOut;

                _ltt->_zHalfLength     = _halfLength;

                _ltt->_displayGasLayer = _displayGasLayer;
                _ltt->_displayWires    = _displayWires;

                _ltt->_isDumbbell      = _isDumbbell;
                if (_isDumbbell) {
                        _ltt->_zZonesLimits.get()[0] = _zZones[0];
                        _ltt->_zZonesLimits.get()[1] = _zZones[1];
                }
                _ltt->_nOuterShell     = _nOuterShell;

                /*int additionalLayer = 0;
                if (_nInGuardWires!=0) {++additionalLayer;}
                if (_nOutGuardWires!=0) {++additionalLayer;}
                SuperLayer *_sprlr     = new SuperLayer[_nSuperLayer+additionalLayer];*/
                SuperLayer *_sprlr     = new SuperLayer[_nSuperLayer];
                std::cout<<"TOT N Supt "<<_nSuperLayer<<std::endl;
                double epsilonOutGwRing, thetaOutGwRing, ringangleOutGwRing, ringangleOutGwRing_1, halfalphaOutGwRing, zlengthOutGwRing;
                epsilonOutGwRing=thetaOutGwRing=ringangleOutGwRing=ringangleOutGwRing_1=halfalphaOutGwRing=zlengthOutGwRing=0.0;

                tmpInnerWall           = _walls.find(Wall::inner)->second;
                tmpOuterWall           = _walls.find(Wall::outer)->second;
                tmpEndCapWall          = _walls.find(Wall::endcap)->second;

                //------------------------------------------------------------------------------

                double inner_radius             =        _r0                               ;
                //double endcap_inner_radius;
                double outer_radius             =        _rOut                             ;
                double fieldwire_diameter       =        _fWireDiameter                    ;
                //double sensewire_diameter       =      _sWireDiameter                    ;
                double envelop_Inner_thickness  =        tmpInnerWall->getTotalThickness() ;
                double envelop_Outer_thickness  =        tmpOuterWall->getTotalThickness() ;
                double envelop_EndCap_thickness =        tmpEndCapWall->getTotalThickness();
                double extra_EndCap_dist;

                int   num_wire_sense            =        _nSWire                           ;

                int   delta_num_wire_sense      =        _nSDeltaWire                      ;
                int   nsuperlayer               =        _nSuperLayer                      ;
                int   nring                     =        _nRing                            ;
                int   geomType                  =        _geomType                         ;
                double voxelizationFactor       =        _voxFactor                        ;

                double drop                     =        _drop                             ;
                double halfalpha                =        _alpha * 0.5*CLHEP::degree        ;
                double halfLength               =        _halfLength                       ;

                int   EndCap_type               =        _endCapType                       ;

                //-------------------

                double max_EndCap_dim;


                double EndCap_Wall_theta_inner   ;
                double EndCap_Wall_theta_outer   ;


                double FWradii, radius_ring_0, radius_ring, epsilon, radius_ringOut_0, radius_ringOut, epsilonOut, radius_ringIn_0,
                radius_ringIn, epsilonIn, cellBase, inscribedRadius, circumscribedRadius, delta_radius_ring, zlength, phi=0.0, theta_ring=0.0, ringangle=0.0;
                int   sign_epsilon      = -1;
                int   num_wire          =  0;

                double secure           = 1.0e-2;                        //Extra volume layer thickness to avoid wire illegal overlap
                double capGasLayer      = 1.0e-3;                        //Thickness of the closing inner gas layer, its is less enough just to be different from 0
                double extShiftFW       = 1.55e-3;                       //Extra distance between Field wire with opposite sign to avoid overlaps

                double CntFWradii=0.5*_cntfWireDiameter;

                //char wshape[30], gshape[30], wvol[30], gvol[30], shape_name_FD[30], shape_name_SD[30], vol_name_FD[30], vol_name_SD[30];

                boost::shared_ptr<Layer> lay;

                inscribedRadius         = 0.0;


                //endcap_inner_radius     = inner_radius;
                extra_EndCap_dist       = 0.0*CLHEP::mm;
                max_EndCap_dim          = halfLength;

                EndCap_Wall_theta_inner = 0.;
                EndCap_Wall_theta_outer = 0.;

                if(EndCap_type==0) {
                        _ltt->_endcapType     = CDCHtracker::Plane;
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
                        _ltt->_endcapType     = CDCHtracker::Spherical;
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

                Wall *tmpEndCapWall_L = new Wall(*tmpEndCapWall);
                tmpEndCapWall_L->_name               = "EndCapWall_L";
                tmpEndCapWall_L->_pos                = HepGeom::RotateY3D(180.0*CLHEP::degree)*tmpEndCapWall_L->_pos;
                _walls.insert( pair<Wall::Walltype,Wall*>(tmpEndCapWall_L->getType(), tmpEndCapWall_L) );

                _ltt->_max_EndCap_dim = max_EndCap_dim;

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

                _sprlr[0]._id = SuperLayerId(0);

                _sprlr[0].addLayer(new Layer());
                lay = _sprlr[0]._layers.back();

                double fakeLayerInIWthick(-0.0001);
                int nSub = tmpInnerWall->getNShells();
                for (int ishell=0; ishell<nSub; ishell++){
                        std::string iWallShellMat = tmpInnerWall->getMaterialsName()->at(ishell);
                        if ( iWallShellMat.find("Gas")!=std::string::npos ) {
                                fakeLayerInIWthick+=tmpInnerWall->getThicknesses()->at(ishell);
                        }
                }

                lay->_detail.reset( new LayerDetail(inner_radius+envelop_Inner_thickness-fakeLayerInIWthick,radius_ringOut_0,0.0,epsilonOut,zlength,_fillMaterial) );
                //lay->_id._sid=&(_sprlr[0]._id);
                //lay->_id._id=-1;
                lay->_id = LayerId(&_sprlr[0]._id, -1);


                if (geomType==20) {
                        _ltt->_cellType   = CDCHtracker::Hexagonal;
                        if(_isDumbbell) {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v2_DBL(_ltt.get()));
                        } else {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v2(_ltt.get()));
                        }

                        for ( superlayer=0;superlayer<nsuperlayer/*2*/;superlayer++ ) {
                                cout <<"Building super layer: "<<superlayer+1<<endl;

                                _sprlr[superlayer]._id = SuperLayerId(superlayer);

                                num_wire     = num_wire_sense+superlayer*delta_num_wire_sense;
                                phi          = CLHEP::twopi/((double) num_wire);
                                sign_epsilon *=-1;

                                theta_ring   = phi/3.0;

                                if (_notExtVoxel) voxelizationFactor = 5.0/(3.0*((double)num_wire));

                                for( iring=0; iring< nring; iring++ ){

                                        radius_ring      = radius_ring_0+drop;
                                        halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                                        epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                                        radius_ringIn_0  = radius_ringOut_0;
                                        radius_ringIn    = radius_ringOut;
                                        epsilonIn        = epsilonOut;

                                        radius_ringOut_0 = radius_ring_0+FWradii+secure;
                                        radius_ringOut   = radius_ringOut_0+drop;
                                        //epsilonOut       = atan((radius_ringOut+drop)/halfLength*sin(halfalpha));
                                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                                        if ((iring%2)==0){
                                                ringangle = 0.;
                                        }
                                        else{
                                                ringangle = -(1.5*theta_ring);
                                        }

                                        cellBase = 2.*radius_ring_0*sin(theta_ring*0.5);
                                        delta_radius_ring = cellBase * cos(30.*CLHEP::degree);

                                        if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                        else zlength = halfLength;

                                        _sprlr[superlayer].addLayer(new Layer(),true);
                                        lay = _sprlr[superlayer]._layers.back();
                                        //                lay->_detail = new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial);
                                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                        lay->_id = LayerId(&_sprlr[superlayer]._id, iring);
                                        lay->_layerType=Layer::wire;
                                        lay->_voxelizationFactor=voxelizationFactor;

                                        zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                        zlength/=cos(epsilon);

                                        boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                                        CDCHFldWireLocater(fw,lay,2*num_wire,radius_ring_0,theta_ring,ringangle,sign_epsilon*epsilon,halfalpha);

                                        boost::shared_ptr<WireDetail> sw;
                                        boost::shared_ptr<CellDetail> celld;
                                        Wire::Wtype wireType;
                                        int copyNumOffset=0;
                                        if (iring==0) {
                                                sw=fw;
                                                wireType = Wire::field;
                                                copyNumOffset = 2*num_wire;
                                        }
                                        else {
                                                sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                                                wireType = Wire::sense;
                                                celld.reset( new CellDetail(cellBase,inscribedRadius,sw) );

                                        }

                                        CDCHWireLocater(sw,wireType,lay,num_wire,radius_ring_0,phi,ringangle+2.0*theta_ring,sign_epsilon*epsilon,halfalpha,copyNumOffset,&celld);

                                        radius_ring_0    += delta_radius_ring;

                                        radius_ringIn_0  = radius_ringOut_0;
                                        radius_ringIn    = radius_ringOut;
                                        epsilonIn        = epsilonOut;
                                        radius_ringOut_0 = radius_ring_0-FWradii-secure;
                                        radius_ringOut   = radius_ringOut_0+drop;
                                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                                        if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                        else zlength  = halfLength;

                                        _sprlr[superlayer].addLayer(new Layer());
                                        lay = _sprlr[superlayer]._layers.back();
                                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                        lay->_id = LayerId(&_sprlr[superlayer]._id, iring);
                                        lay->_layerType=Layer::gas;

                                        inscribedRadius = delta_radius_ring;

                                }

                        }

                        radius_ring      = radius_ring_0+drop;
                        halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                        epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                        radius_ringIn_0  = radius_ringOut_0;
                        radius_ringIn    = radius_ringOut;
                        epsilonIn        = epsilonOut;

                        radius_ringOut_0 = radius_ring_0+FWradii+secure;
                        radius_ringOut   = radius_ringOut_0+drop;
                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                        ringangle = 0.;
                        if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                        else zlength = halfLength;

                        --superlayer;

                        _sprlr[superlayer].addLayer(new Layer());
                        lay = _sprlr[superlayer]._layers.back();
                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                        lay->_id = LayerId(&_sprlr[superlayer]._id, iring);
                        lay->_layerType=Layer::wire;
                        lay->_voxelizationFactor=voxelizationFactor;

                        zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                        zlength/=cos(epsilon);

                        boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                        CDCHFldWireLocater(fw,lay,2*num_wire,radius_ring_0,theta_ring,ringangle,sign_epsilon*epsilon,halfalpha);

                        CDCHWireLocater(fw,Wire::field,lay,num_wire,radius_ring_0,phi,ringangle+2.0*theta_ring,sign_epsilon*epsilon,halfalpha,2*num_wire);

                }
                else if (geomType==30) {

                        _ltt->_cellType     = CDCHtracker::Square;
                        if (_isDumbbell) {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3_DBL(_ltt.get()));
                        } else {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3(_ltt.get()));
                        }

                        delta_radius_ring   = _cellDimension;
                        float fwireDist     = _FWireStep;
                        unsigned int nFwire = 0;
                        int nHorizontalFWire;
                        float iradius, idelta_radius;
                        idelta_radius = delta_radius_ring/((float) (1+_nVerticalFWire));
                        double senseWireRing_radius_0;
                        inscribedRadius     = 0.5*_cellDimension;
                        circumscribedRadius = inscribedRadius*sqrt(2.0);

                        nHorizontalFWire    = _StoFWireRatio-_nVerticalFWire;

                        for ( superlayer=0;superlayer!=nsuperlayer;++superlayer ) {
                                std::cout <<"Building layer: "<<superlayer+1<<std::endl;

                                //_sprlr[superlayer]._id._id = superlayer;
                                _sprlr[superlayer]._id = SuperLayerId(superlayer);

                                senseWireRing_radius_0 = radius_ring_0+inscribedRadius;
                                num_wire               = (int)(CLHEP::twopi*senseWireRing_radius_0/_cellDimension);
                                phi                    = CLHEP::twopi/((float) num_wire);
                                nFwire                 = nHorizontalFWire*num_wire;
                                //if ( (CLHEP::twopi*radius_ring_0/((float)nFwire))<fwireDist ) throw cet::exception("GEOM")<< "Error during field wire positioning: "<< nFwire
                                //                                                        <<" field wires don't fit on a circumference with radius of "<<radius_ring_0<<" using a step of "<<fwireDist<<std::endl;

                                sign_epsilon           *=-1;

                                ringangle              = -0.5*phi;

                                iring                  = 0;
                                radius_ring            = radius_ring_0+drop;
                                halfalpha              = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                                epsilon                = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                                radius_ringIn_0        = radius_ringOut_0;
                                radius_ringIn          = radius_ringOut;
                                epsilonIn              = epsilonOut;

                                radius_ringOut_0       = radius_ring_0+FWradii+secure;
                                radius_ringOut         = radius_ringOut_0+drop;
                                //epsilonOut             = atan((radius_ringOut+drop)/halfLength*sin(halfalpha));
                                epsilonOut             = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                                if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                else zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer());
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::wire;
                                if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                                lay->_voxelizationFactor=voxelizationFactor;

                                zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilon);

                                boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                                theta_ring = CLHEP::twopi/((float) nFwire);

                                CDCHWireLocater(fw,Wire::field,lay,nFwire,radius_ring_0,theta_ring,ringangle,sign_epsilon*epsilon,halfalpha);

                                iradius          = radius_ring_0;

                                radius_ring_0    += delta_radius_ring;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;
                                radius_ringOut_0 = radius_ring_0-FWradii-secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                                if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                else zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer(),true);
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::gas;
                                if (_notExtVoxel) voxelizationFactor = 5.0/((float)((1+_nVerticalFWire)*num_wire));
                                lay->_voxelizationFactor=voxelizationFactor;

                                boost::shared_ptr<WireDetail> sw;
                                boost::shared_ptr<CellDetail> celld;
                                sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                                celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );
                                CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0,sign_epsilon*epsilon,halfalpha,0,&celld);

                                boost::shared_ptr<WireDetail> fw1( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                                for( iring=0; iring< _nVerticalFWire ; iring++ ){

                                        iradius+=idelta_radius;
                                        CDCHWireLocater(fw1,Wire::field,lay,num_wire,iradius,phi,ringangle,sign_epsilon*epsilon,halfalpha,iring*num_wire);

                                }

                        }

                        radius_ring      = radius_ring_0+drop;
                        halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                        epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                        radius_ringIn_0  = radius_ringOut_0;
                        radius_ringIn    = radius_ringOut;
                        epsilonIn        = epsilonOut;

                        radius_ringOut_0 = radius_ring_0+FWradii+secure;
                        radius_ringOut   = radius_ringOut_0+drop;
                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                        if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                        else zlength = halfLength;

                        --superlayer;
                        _sprlr[superlayer].addLayer(new Layer());
                        lay = _sprlr[superlayer]._layers.back();
                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                        lay->_id = LayerId(&_sprlr[superlayer]._id, 1);
                        lay->_layerType=Layer::wire;
                        if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                        lay->_voxelizationFactor=voxelizationFactor;

                        zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                        zlength/=cos(epsilon);

                        boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                        nFwire = (unsigned int) (CLHEP::twopi*radius_ring_0/fwireDist);
                        theta_ring = CLHEP::twopi/((float) nFwire);

                        CDCHWireLocater(fw,Wire::field,lay,nFwire,radius_ring_0,theta_ring,ringangle,sign_epsilon*epsilon,halfalpha);
                        iring=0;

                }
                else if (geomType==31) {

                        _ltt->_cellType      = CDCHtracker::Square;
                        if (_isDumbbell) {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3_DBL(_ltt.get()));
                        } else {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3(_ltt.get()));
                        }

                        delta_radius_ring    = _cellDimension;
                        float fwireDist      = _FWireStep;
                        unsigned int nFwire  = 0;
                        unsigned int nFwire1 = 0;
                        int nHorizontalFWire;
                        double cellStaggering;
                        //bool isCellStaggered;
                        double theta_ring1;
                        float iradius, idelta_radius;
                        idelta_radius = delta_radius_ring/((float) (1+_nVerticalFWire));
                        double senseWireRing_radius_0;
                        inscribedRadius      = 0.5*_cellDimension;
                        circumscribedRadius  = inscribedRadius*sqrt(2.0);

                        radius_ring_0+=FWradii;
                        nHorizontalFWire     = _StoFWireRatio-_nVerticalFWire;
                        //isCellStaggered     = ((nHorizontalFWire/2)%2==0) ? true : false;

                        for ( superlayer=0;superlayer<nsuperlayer;superlayer++ ) {
                                std::cout <<"Building layer: "<<superlayer+1<<std::endl;

                                _sprlr[superlayer]._id = SuperLayerId(superlayer);

                                //num_wire               = num_wire_sense+superlayer*delta_num_wire_sense;
                                senseWireRing_radius_0 = radius_ring_0+inscribedRadius;
                                num_wire               = (int)(CLHEP::twopi*senseWireRing_radius_0/_cellDimension);
                                phi                    = CLHEP::twopi/((float) num_wire);
                                nFwire                 = nHorizontalFWire*num_wire;
                                //if ( (CLHEP::twopi*radius_ring_0/((float)nFwire))<fwireDist ) throw cet::exception("GEOM")<< "Error during field wire positioning: "<< nFwire
                                //                                                        <<" field wires don't fit on a circumference with radius of "<<radius_ring_0<<" using a step of "<<fwireDist<<std::endl;

                                sign_epsilon     *=-1;

                                ringangle        = -0.5*phi;

                                iring            = 0;
                                radius_ring      = radius_ring_0+drop;
                                halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                                epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;

                                radius_ringOut_0 = radius_ring_0+_fWireDiameter+secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                //epsilonOut       = atan((radius_ringOut+drop)/halfLength*sin(halfalpha));
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                                if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                else zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer());
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::wire;
                                if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                                lay->_voxelizationFactor=voxelizationFactor;

                                zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilon);

                                boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                                theta_ring = CLHEP::twopi/((float) nFwire);
                                if (/*isCellStaggered &&*/ (superlayer%2==1)) cellStaggering=theta_ring;
                                else cellStaggering=0.0;

                                nFwire1=nFwire;
                                nFwire1/=2;
                                theta_ring1=2.0*theta_ring;
                                CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle,sign_epsilon*epsilon,halfalpha);
                                CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii,theta_ring1,ringangle+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha,nFwire1);

                                iradius          = radius_ring_0;

                                radius_ring_0    += delta_radius_ring;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;
                                radius_ringOut_0 = radius_ring_0-_fWireDiameter-secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                                if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                else zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer(),true);
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::gas;
                                if (_notExtVoxel) voxelizationFactor = 5.0/((float)((1+_nVerticalFWire)*num_wire));
                                lay->_voxelizationFactor=voxelizationFactor;

                                boost::shared_ptr<WireDetail> sw;
                                boost::shared_ptr<CellDetail> celld;
                                sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                                celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );
                                CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);

                                boost::shared_ptr<WireDetail> fw1( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                                for( iring=0; iring< _nVerticalFWire ; iring++ ){

                                        iradius+=idelta_radius;
                                        CDCHWireLocater(fw1,Wire::field,lay,num_wire,iradius,phi,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha,iring*num_wire);

                                }

                        }

                        radius_ring_0    +=_fWireDiameter;
                        radius_ring      = radius_ring_0+drop;
                        halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                        epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                        radius_ringIn_0  = radius_ringOut_0;
                        radius_ringIn    = radius_ringOut;
                        epsilonIn        = epsilonOut;

                        radius_ringOut_0 = radius_ring_0+_fWireDiameter+secure;
                        radius_ringOut   = radius_ringOut_0+drop;
                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                        if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                        else zlength = halfLength;

                        --superlayer;
                        _sprlr[superlayer].addLayer(new Layer());
                        lay = _sprlr[superlayer]._layers.back();
                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                        lay->_id = LayerId(&_sprlr[superlayer]._id, 1);
                        lay->_layerType=Layer::wire;
                        if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                        lay->_voxelizationFactor=voxelizationFactor;

                        zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                        zlength/=cos(epsilon);

                        boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                        nFwire = (unsigned int) (CLHEP::twopi*radius_ring_0/fwireDist);
                        theta_ring = CLHEP::twopi/((float) nFwire);

                        nFwire1=nFwire;
                        nFwire1/=2;
                        theta_ring1=2.0*theta_ring;
                        CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle,sign_epsilon*epsilon,halfalpha);
                        CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii,theta_ring1,ringangle+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha,nFwire1);
                        iring=0;

                }
                else if (geomType==41) {

                        _ltt->_cellType     = CDCHtracker::Square;
                        if(_isDumbbell) {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3_DBL(_ltt.get()));
                        } else {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3(_ltt.get()));
                        }

                        num_wire            = _nSWire;
                        delta_radius_ring   = _cellDimension;
                        //float fwireDist     = _FWireStep;
                        unsigned int nFwire, nFwire1;
                        int nHorizontalFWire;
                        double cellStaggering;
                        //bool isCellStaggered;
                        double theta_ring1;
                        float iradius, idelta_radius=0.0;
                        double senseWireRing_radius_0;

                        double scaleFactor = (1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);

                        radius_ring_0+=FWradii;
                        nHorizontalFWire    = _StoFWireRatio-_nVerticalFWire;
                        //isCellStaggered     = ((nHorizontalFWire/2)%2==0) ? true : false;

                        phi         = CLHEP::twopi/((float) num_wire);
                        nFwire      = nHorizontalFWire*num_wire;
                        theta_ring  = CLHEP::twopi/((float) nFwire);
                        nFwire1     = nFwire;
                        nFwire1     /= 2;
                        theta_ring1 = 2.0*theta_ring;

                        for ( superlayer=0;superlayer<nsuperlayer;superlayer++ ) {
                                std::cout <<"Building layer: "<<superlayer+1<<std::endl;

                                _sprlr[superlayer]._id = SuperLayerId(superlayer);

                                //num_wire               = num_wire_sense+superlayer*delta_num_wire_sense;
                                inscribedRadius        = 0.5*delta_radius_ring; //delta_radius_ring is equal to the cell height of each layer
                                circumscribedRadius    = inscribedRadius*sqrt(2.0);
                                senseWireRing_radius_0 = radius_ring_0+inscribedRadius;
                                //if ( (CLHEP::twopi*radius_ring_0/((float)nFwire))<fwireDist ) throw cet::exception("GEOM")<< "Error during field wire positioning: "<< nFwire
                                //                                                        <<" field wires don't fit on a circumference with radius of "<<radius_ring_0<<" using a step of "<<fwireDist<<std::endl;

                                sign_epsilon     *=-1;

                                ringangle        = -0.5*phi;

                                iring            = 0;
                                radius_ring      = radius_ring_0+drop;
                                halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                                epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;

                                radius_ringOut_0 = radius_ring_0+_fWireDiameter+secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                //epsilonOut       = atan((radius_ringOut+drop)/halfLength*sin(halfalpha));
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                                //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                //else zlength = halfLength;
                                zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer());
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::wire;
                                if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                                lay->_voxelizationFactor=voxelizationFactor;

                                zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilon);

                                boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                                if (/*isCellStaggered &&*/ (superlayer%2==1)) cellStaggering=theta_ring;
                                else cellStaggering=0.0;

                                CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle,sign_epsilon*epsilon,halfalpha);
                                CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii,theta_ring1,ringangle+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha,nFwire1);

                                iradius          = radius_ring_0;

                                radius_ring_0    += delta_radius_ring;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;
                                radius_ringOut_0 = radius_ring_0-_fWireDiameter-secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                                //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                //else zlength = halfLength;
                                zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer(),true);
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::gas;
                                if (_notExtVoxel) voxelizationFactor = 5.0/((float)((1+_nVerticalFWire)*num_wire));
                                lay->_voxelizationFactor=voxelizationFactor;
				
                                zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilon);

                                boost::shared_ptr<WireDetail> sw;
                                boost::shared_ptr<CellDetail> celld;
                                sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                                celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );
                                CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);

                                boost::shared_ptr<WireDetail> fw1( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                                idelta_radius = delta_radius_ring/((float) (1+_nVerticalFWire));
                                for( iring=0; iring< _nVerticalFWire ; iring++ ){

                                        iradius+=idelta_radius;
                                        CDCHWireLocater(fw1,Wire::field,lay,num_wire,iradius,phi,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha,iring*num_wire);

                                }

                                delta_radius_ring *= scaleFactor;

                        }

                        radius_ring_0    +=_fWireDiameter;
                        radius_ring      = radius_ring_0+drop;
                        halfalpha        = acos(1.-(drop/radius_ring)) * CLHEP::radian;
                        epsilon          = atan(radius_ring/halfLength*sin(halfalpha)) * CLHEP::radian;

                        radius_ringIn_0  = radius_ringOut_0;
                        radius_ringIn    = radius_ringOut;
                        epsilonIn        = epsilonOut;

                        radius_ringOut_0 = radius_ring_0+_fWireDiameter+secure;
                        radius_ringOut   = radius_ringOut_0+drop;
                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                        //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                        //else zlength = halfLength;
                        zlength = halfLength;

                        --superlayer;
                        _sprlr[superlayer].addLayer(new Layer());
                        lay = _sprlr[superlayer]._layers.back();
                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                        lay->_id = LayerId(&_sprlr[superlayer]._id, 1);
                        lay->_layerType=Layer::wire;
                        if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                        lay->_voxelizationFactor=voxelizationFactor;

                        zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                        zlength/=cos(epsilon);

                        boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                        //nFwire = (unsigned int) (CLHEP::twopi*radius_ring_0/fwireDist);
                        //theta_ring = CLHEP::twopi/((float) nFwire);

                        //nFwire1=nFwire;
                        //nFwire1/=2;
                        //theta_ring1=2.0*theta_ring;
                        CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle,sign_epsilon*epsilon,halfalpha);
                        CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii,theta_ring1,ringangle+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha,nFwire1);
                        iring=0;

                }
                else if (geomType==42) {

                        _ltt->_cellType     = CDCHtracker::Square;
                        if(_isDumbbell) {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3_DBL(_ltt.get()));
                        } else {
                                _ltt->_cellhnd.reset(new CellGeometryHandle_v3(_ltt.get()));
                        }

                        num_wire             = _nSWire;
                        radius_ring_0       += FWradii;
                        //_cellDimension     = radius_ring_0*CLHEP::twopi/((float) num_wire);
                        delta_radius_ring    = _cellDimension;
                        //float fwireDist    = _FWireStep;
                        unsigned int nFwire, nFwire1;
                        int nHorizontalFWire;
                        double cellStaggering=0.0;
                        //bool isCellStaggered;
                        double theta_ring1;
                        float iradius, idelta_radius=0.0;
                        double senseWireRing_radius_0;

                        double scaleFactor   = (1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                        double dropFactor    = (1.0/cos(halfalpha)-1.0);
                        double epsilonFactor = sin(halfalpha)/halfLength;

                        lay = _sprlr[0]._layers.back();
                        lay->_detail->setStereoAngleOuterRing(atan(lay->_detail->centerOuterRadiusRing()*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian);

                        nHorizontalFWire    = _StoFWireRatio-_nVerticalFWire;
                        //isCellStaggered     = ((nHorizontalFWire/2)%2==0) ? true : false;

                        phi         = CLHEP::twopi/((float) num_wire);
                        nFwire      = nHorizontalFWire*num_wire;
                        theta_ring  = CLHEP::twopi/((float) nFwire);
                        nFwire1     = nFwire;
                        nFwire1    /= 2;
                        theta_ring1 = 2.0*theta_ring;

                        drop              = radius_ringOut_0*dropFactor;
                        radius_ringOut    = radius_ringOut_0+drop;
                        epsilonOut        = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0)) / halfLength) * CLHEP::radian;

                        for ( superlayer=0;superlayer<nsuperlayer;superlayer++ ) {
                                std::cout <<"Building layer: "<<superlayer+1<<std::endl;

                                _sprlr[superlayer]._id = SuperLayerId(superlayer);

                                //num_wire               = num_wire_sense+superlayer*delta_num_wire_sense;
                                inscribedRadius        = 0.5*delta_radius_ring; //delta_radius_ring is equal to the cell height of each layer
                                circumscribedRadius    = inscribedRadius*sqrt(2.0);
                                senseWireRing_radius_0 = radius_ring_0+inscribedRadius;
                                //if ( (CLHEP::twopi*radius_ring_0/((float)nFwire))<fwireDist ) throw cet::exception("GEOM")<< "Error during field wire positioning: "<< nFwire
                                //                                                        <<" field wires don't fit on a circumference with radius of "<<radius_ring_0<<" using a step of "<<fwireDist<<std::endl;

                                sign_epsilon    *=-1;

                                ringangle        = -0.5*phi;

                                iring            = 0;
                                drop             = radius_ring_0*dropFactor;
                                radius_ring      = radius_ring_0+drop;
                                epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;

                                radius_ringOut_0 = radius_ring_0+_fWireDiameter+secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                //epsilonOut       = atan((radius_ringOut+drop)*epsilonFactor);
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                                //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                //else zlength = halfLength;
                                zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer());
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::wire;
                                if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                                lay->_voxelizationFactor=voxelizationFactor;

                                zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilon);

                                boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                                if (/*isCellStaggered &&*/ (superlayer%2==1)) cellStaggering=theta_ring;
                                else cellStaggering=0.0;

                                CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle+cellStaggering-theta_ring,-1.0*sign_epsilon*epsilon,halfalpha);
                                CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii+extShiftFW,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha,nFwire1);

                                iradius          = radius_ring_0;

                                radius_ring_0    += delta_radius_ring;
                                drop             = radius_ring_0*dropFactor;

                                radius_ringIn_0  = radius_ringOut_0;
                                radius_ringIn    = radius_ringOut;
                                epsilonIn        = epsilonOut;
                                radius_ringOut_0 = radius_ring_0-_fWireDiameter-secure;
                                radius_ringOut   = radius_ringOut_0+drop;
                                epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                                //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                                //else zlength = halfLength;
                                zlength = halfLength;

                                _sprlr[superlayer].addLayer(new Layer(),true);
                                lay = _sprlr[superlayer]._layers.back();
                                lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[superlayer]._id, 0);
                                lay->_layerType=Layer::gas;
                                if (_notExtVoxel) voxelizationFactor = 5.0/((float)((1+_nVerticalFWire)*num_wire));
                                lay->_voxelizationFactor=voxelizationFactor;

                                zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilon);

                                boost::shared_ptr<WireDetail> sw;
                                boost::shared_ptr<CellDetail> celld;
                                sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                                celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );

                                epsilon          = atan(senseWireRing_radius_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;

                                CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);
                                std::cout<<"i-slayer "<<superlayer<<" sense wire rad at z=0 "<<senseWireRing_radius_0<<" drop "<<drop<<" cell dim "<<delta_radius_ring<<" epsilon "<<sign_epsilon*epsilon<<std::endl;

                                boost::shared_ptr<WireDetail> fw1( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                                idelta_radius = delta_radius_ring/((float) (1+_nVerticalFWire));
                                for( iring=0; iring< _nVerticalFWire ; iring++ ){

                                        iradius+=idelta_radius;
                                        epsilon          = atan(iradius*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                        CDCHWireLocater(fw1,Wire::field,lay,num_wire,iradius,phi,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha,iring*num_wire);

                                }

                                delta_radius_ring *= scaleFactor;


                        }

                        radius_ring_0   += _fWireDiameter;
                        drop             = radius_ring_0*dropFactor;
                        radius_ring      = radius_ring_0+drop;
                        epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                        radius_ringIn_0  = radius_ringOut_0;
                        radius_ringIn    = radius_ringOut;
                        epsilonIn        = epsilonOut;

                        radius_ringOut_0 = radius_ring_0+_fWireDiameter+secure;
                        radius_ringOut   = radius_ringOut_0+drop;
                        epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                        //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                        //else zlength = halfLength;
                        zlength = halfLength;

                        --superlayer;
                        _sprlr[superlayer].addLayer(new Layer());
                        lay = _sprlr[superlayer]._layers.back();
                        lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                        lay->_id = LayerId(&_sprlr[superlayer]._id, 1);
                        lay->_layerType=Layer::wire;
                        if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                        lay->_voxelizationFactor=voxelizationFactor;

                        zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                        zlength/=cos(epsilon);

                        boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                        //nFwire = (unsigned int) (CLHEP::twopi*radius_ring_0/fwireDist);
                        //theta_ring = CLHEP::twopi/((float) nFwire);

                        //nFwire1=nFwire;
                        //nFwire1/=2;
                        //theta_ring1=2.0*theta_ring;
                        CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);
                        CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii+extShiftFW,theta_ring1,ringangle+cellStaggering+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha,nFwire1);
                        iring=0;

                        //int guarWireLayer = superlayer;
                        if (_nInGuardWires!=0) {
                                //++guarWireLayer;
                                if (_nInGuardWires<0) { _nInGuardWires = nFwire; }
                                /*_sprlr[guarWireLayer]._id = SuperLayerId(-1);
                                _sprlr[guarWireLayer].addLayer(new Layer());
                                lay = _sprlr[guarWireLayer]._layers.back();
                                double radiusInGwRing_In_0  = _inGuardRad - _inGWireDiameter;
                                double radiusInGwRing_Out_0 = _inGuardRad + _inGWireDiameter;
                                double epsilonInGwRing_In  = atan(radiusInGwRing_In_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                double epsilonInGwRing_Out = atan(radiusInGwRing_Out_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                lay->_detail.reset( new LayerDetail(radiusInGwRing_In_0,radiusInGwRing_Out_0,epsilonInGwRing_In,epsilonInGwRing_Out,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[guarWireLayer]._id, 0);
                                lay->_layerType=Layer::wire;*/
                                lay = _sprlr[0].getLayer(0);
                                if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                                lay->_voxelizationFactor=voxelizationFactor;

                                double GWradii = 0.5*_inGWireDiameter;
                                double epsilonInGwRing  = atan(_inGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                zlength = halfLength;
                                zlength-=sin(epsilonInGwRing)*GWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilonInGwRing);

                                boost::shared_ptr<WireDetail> inGw( new WireDetail(_inGwShellsThicknesses,_inGwMaterialsName,zlength) );
                                CDCHWireLocater(inGw,Wire::field,lay,_nInGuardWires/2,_inGuardRad-GWradii,theta_ring1,ringangle,epsilonInGwRing,halfalpha);
                                CDCHWireLocater(inGw,Wire::field,lay,_nInGuardWires/2,_inGuardRad+GWradii+extShiftFW,theta_ring1,ringangle+theta_ring,-1.0*epsilonInGwRing,halfalpha,_nInGuardWires/2);
                        }
                        /*if (_nOutGuardWires!=0) {
                                ++guarWireLayer;
                                if (_nOutGuardWires<0) { _nOutGuardWires = nFwire; }
                                _sprlr[guarWireLayer]._id = SuperLayerId(superlayer+1);
                                _sprlr[guarWireLayer].addLayer(new Layer());
                                lay = _sprlr[guarWireLayer]._layers.back();
                                double radiusOutGwRing_In_0  = _outGuardRad - _outGWireDiameter;
                                double radiusOutGwRing_Out_0 = _outGuardRad + _outGWireDiameter;
                                double epsilonOutGwRing_In  = atan(radiusOutGwRing_In_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                double epsilonOutGwRing_Out = atan(radiusOutGwRing_Out_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                lay->_detail.reset( new LayerDetail(radiusOutGwRing_In_0,radiusOutGwRing_Out_0,epsilonOutGwRing_In,epsilonOutGwRing_Out,zlength,_fillMaterial) );
                                lay->_id = LayerId(&_sprlr[guarWireLayer]._id, 0);
                                lay->_layerType=Layer::wire;
                                if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                                lay->_voxelizationFactor=voxelizationFactor;

                                double GWradii = 0.5*_outGWireDiameter;
                                double epsilonOutGwRing  = atan(_outGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                zlength-=sin(epsilonOutGwRing)*GWradii;//protect from to extrud of mother volume
                                zlength/=cos(epsilonOutGwRing);

                                boost::shared_ptr<WireDetail> outGw( new WireDetail(_outGwShellsThicknesses,_outGwMaterialsName,zlength) );
                                CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad-GWradii,theta_ring1,ringangle,epsilonOutGwRing,halfalpha);
                                CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad+GWradii,theta_ring1,ringangle+theta_ring,-1.0*epsilonOutGwRing,halfalpha,_nOutGuardWires/2);
                        }*/
                        if (_nOutGuardWires!=0) {
                                if (_nOutGuardWires<0) { _nOutGuardWires = nFwire; }
                                epsilonOutGwRing     = atan(_outGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                                zlengthOutGwRing     = halfLength;
                                thetaOutGwRing       = theta_ring1;
                                ringangleOutGwRing   = ringangle;
                                ringangleOutGwRing_1 = ringangle+theta_ring;
                                halfalphaOutGwRing   = halfalpha;
                        }

                }
                else if (geomType==50) {

                  _ltt->_cellType     = CDCHtracker::Square;
//                  if(_isDumbbell) {
//                    _ltt->_cellhnd.reset(new CellGeometryHandle_v3_DBL(_ltt.get()));
//                  } else {
                    _ltt->_cellhnd.reset(new CellGeometryHandle_v4(_ltt.get()));
//                  }

//                  num_wire            = _nSWire;
//                  radius_ring_0+=FWradii;
                  //_cellDimension      = radius_ring_0*CLHEP::twopi/((float) num_wire);
                  delta_radius_ring   = _cellDimension;
                  //float fwireDist     = _FWireStep;
                  unsigned int nFwire, nFwire1;
                  int nHorizontalFWire;
                  double cellStaggering=0.0;
                  //bool isCellStaggered;
                  double theta_ring1;
                  float iradius, idelta_radius=0.0;
                  double senseWireRing_radius_0;

                  double scaleFactor   = 0.0;//(1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                  double dropFactor    = 0.0;//(1.0/cos(halfalpha)-1.0);
                  double epsilonFactor = 0.0;//sin(halfalpha)/halfLength;

                  extShiftFW=/*0.5**/secure;

//                  lay = _sprlr[0]._layers.back();
//                  lay->_detail->setCenterOuterRadiusRing(lay->_detail->centerOuterRadiusRing()-secure);
//                  lay->_detail->setStereoAngleOuterRing(atan(lay->_detail->centerOuterRadiusRing()*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian);

                  nHorizontalFWire    = _StoFWireRatio-_nVerticalFWire;
                  //isCellStaggered     = ((nHorizontalFWire/2)%2==0) ? true : false;

//                  phi         = CLHEP::twopi/((float) num_wire);
//                  nFwire      = nHorizontalFWire*num_wire;
//                  theta_ring  = CLHEP::twopi/((float) nFwire);
//                  nFwire1     = nFwire;
//                  nFwire1    /= 2;
//                  theta_ring1 = 2.0*theta_ring;
//
////                  drop              = radius_ringOut_0*dropFactor;
////                  radius_ringOut    = radius_ringOut_0+drop;
////                  epsilonOut        = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0)) / halfLength) * CLHEP::radian;
//
////..............................
//
//                  ringangle        = -0.5*phi;
//
//                  drop             = radius_ring_0*dropFactor;
//                  radius_ring      = radius_ring_0+drop;
//                  epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;
//
//                  radius_ringIn_0 = radius_ring_0-FWradii-secure;
//                  radius_ringIn   = radius_ringIn_0+drop;
//                  radius_ringOut_0 = radius_ring_0+FWradii+secure;
//                  radius_ringOut   = radius_ringOut_0+drop;
//                  epsilonIn       = atan(sqrt(diff_of_squares(radius_ringIn, radius_ringIn_0))/halfLength) * CLHEP::radian;
//                  epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
//
//                  zlength = halfLength;
//
//                  _sprlr[0].addLayer(new Layer());
//                  lay = _sprlr[0]._layers.back();
//                  lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
//                  lay->_id = LayerId(&_sprlr[0]._id, -1);
//                  lay->_layerType=Layer::wire;
//                  if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
//                  lay->_voxelizationFactor=voxelizationFactor;
//
//                  zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
//                  zlength/=cos(epsilon);
//
//                  boost::shared_ptr<WireDetail> fwDw0( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
//
//                  cellStaggering=0.0;
//
//                  CDCHWireLocater(fwDw0,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle+cellStaggering-theta_ring,sign_epsilon*epsilon,halfalpha);
////..............................

                  epsilon=0.0; //just to remove compilation warning
                  theta_ring=theta_ring1=0.0; //just to remove compilation warning
                  nFwire=nFwire1=0; //just to remove compilation warning

                  for ( superlayer=0;superlayer<nsuperlayer;superlayer++ ) {
                    std::cout <<"Building super layer: "<<superlayer+1<<std::endl;

                    _sprlr[superlayer]._id = SuperLayerId(superlayer);

                    num_wire    = _nSWire+superlayer*_nSDeltaWire;
//                    scaleFactor = (1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                    phi         = CLHEP::twopi/((float) num_wire);
                    nFwire      = nHorizontalFWire*num_wire;
                    theta_ring  = CLHEP::twopi/((float) nFwire);
                    nFwire1     = nFwire;
                    nFwire1    /= 2;
                    theta_ring1 = 2.0*theta_ring;

                    scaleFactor   = (1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                    dropFactor    = (1.0/cos(halfalpha)-1.0);
                    epsilonFactor = sin(halfalpha)/halfLength;

                    ringangle        = -0.5*phi;

                    if (superlayer==0) {
                      lay = _sprlr[0]._layers.back();
                      lay->_detail->setCenterOuterRadiusRing(lay->_detail->centerOuterRadiusRing()-secure);
                      lay->_detail->setStereoAngleOuterRing(atan(lay->_detail->centerOuterRadiusRing()*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian);

                      drop             = radius_ring_0*dropFactor;
                      radius_ring      = radius_ring_0+drop;
                      epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                      radius_ringIn_0  = radius_ring_0-FWradii-2.0*secure;
                      radius_ringIn    = radius_ringIn_0+drop;
                      radius_ringOut_0 = radius_ring_0+FWradii;
                      radius_ringOut   = radius_ringOut_0+drop;
                      epsilonIn        = atan(sqrt(diff_of_squares(radius_ringIn, radius_ringIn_0))/halfLength) * CLHEP::radian;
                      epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                      //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                      //else zlength = halfLength;
                      zlength = halfLength;

                      _sprlr[0].addLayer(new Layer());
                      lay = _sprlr[0]._layers.back();
                      lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                      lay->_id = LayerId(&_sprlr[0]._id, _ltt->maxNLayPerSL()*3);
                      lay->_layerType=Layer::wire;
                      if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                      lay->_voxelizationFactor=voxelizationFactor;

                      zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                      zlength/=cos(epsilon);

                      boost::shared_ptr<WireDetail> fwDw0( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                      cellStaggering=0.0;

                      CDCHWireLocater(fwDw0,Wire::field,lay,nFwire1,radius_ring_0-FWradii-extShiftFW,theta_ring1,ringangle+cellStaggering-theta_ring,sign_epsilon*epsilon,halfalpha);

                      radius_ring_0+=FWradii; // just to fix the Sw radius with the oder type.

                    } else {
                      delta_radius_ring = CLHEP::twopi*radius_ringOut_0/(num_wire-CLHEP::pi);
                    }

                    for( iring=0; iring< nring; iring++ ){
                      std::cout <<"Building layer: "<<iring+1<<std::endl;

                      //                    _sprlr[superlayer]._id = SuperLayerId(superlayer);

                      inscribedRadius        = 0.5*delta_radius_ring; //delta_radius_ring is equal to the cell height of each layer
                      circumscribedRadius    = inscribedRadius*sqrt(2.0);
                      senseWireRing_radius_0 = radius_ring_0+inscribedRadius;
                      //if ( (CLHEP::twopi*radius_ring_0/((float)nFwire))<fwireDist ) throw cet::exception("GEOM")<< "Error during field wire positioning: "<< nFwire
                      //                                                        <<" field wires don't fit on a circumference with radius of "<<radius_ring_0<<" using a step of "<<fwireDist<<std::endl;

                      sign_epsilon    *=-1;

                      //                    ringangle        = -0.5*phi;

                      //                    iring            = 0;
                      //                    drop             = radius_ring_0*dropFactor;
                      //                    radius_ring      = radius_ring_0+drop;
                      //                    epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                      radius_ringIn_0  = radius_ringOut_0;
                      radius_ringIn    = radius_ringOut;
                      epsilonIn        = epsilonOut;

                      radius_ringOut_0 = radius_ringIn_0+_fWireDiameter+2.0*secure;
                      radius_ringOut   = radius_ringOut_0+drop;
                      //epsilonOut       = atan((radius_ringOut+drop)*epsilonFactor);
                      epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                      //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                      //else zlength = halfLength;
                      zlength = halfLength;

                      _sprlr[superlayer].addLayer(new Layer());
                      lay = _sprlr[superlayer]._layers.back();
                      lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                      lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*1+iring);
                      lay->_layerType=Layer::wire;
                      if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                      lay->_voxelizationFactor=voxelizationFactor;

                      zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                      zlength/=cos(epsilon);

                      boost::shared_ptr<WireDetail> fwDw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                      if (/*isCellStaggered &&*/ (iring%2==1)) cellStaggering=theta_ring;
                      else cellStaggering=0.0;

                      CDCHWireLocater(fwDw,Wire::field,lay,nFwire1,radius_ringIn_0+FWradii+extShiftFW,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);

                      iradius          = radius_ring_0;

                      radius_ring_0    += delta_radius_ring;
                      drop             = radius_ring_0*dropFactor;

                      radius_ringIn_0  = radius_ringOut_0;
                      radius_ringIn    = radius_ringOut;
                      epsilonIn        = epsilonOut;
                      radius_ringOut_0 = radius_ring_0-_fWireDiameter-2.0*secure;
                      radius_ringOut   = radius_ringOut_0+drop;
                      epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                      //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                      //else zlength = halfLength;
                      zlength = halfLength;

                      _sprlr[superlayer].addLayer(new Layer(),true);
                      lay = _sprlr[superlayer]._layers.back();
                      lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                      lay->_id = LayerId(&_sprlr[superlayer]._id, iring);
                      lay->_layerType=Layer::gas;
                      if (_notExtVoxel) voxelizationFactor = 5.0/((float)((1+_nVerticalFWire)*num_wire));
                      lay->_voxelizationFactor=voxelizationFactor;

                      zlength-=sin(epsilon)*CntFWradii;//protect from to extrud of mother volume
                      zlength/=cos(epsilon);

                      boost::shared_ptr<WireDetail> sw;
                      boost::shared_ptr<CellDetail> celld;
                      sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                      celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );

                      epsilon          = atan(senseWireRing_radius_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;

                      CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);
                      std::cout<<"i-slayer "<<superlayer<<" sense wire rad at z=0 "<<senseWireRing_radius_0<<" drop "<<drop<<" cell dim "<<delta_radius_ring<<" epsilon "<<sign_epsilon*epsilon<<std::endl;

                      boost::shared_ptr<WireDetail> fwCn( new WireDetail(_cntfwShellsThicknesses,_cntfwMaterialsName,zlength) );
                      idelta_radius = delta_radius_ring/((float) (1+_nVerticalFWire));
                      for( int iVrtFw=0; iVrtFw< _nVerticalFWire ; iVrtFw++ ){

                        iradius+=idelta_radius;
                        epsilon          = atan(iradius*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                        CDCHWireLocater(fwCn,Wire::field,lay,num_wire,iradius,phi,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha,iVrtFw*num_wire);

                      }

                      //............

                      radius_ringIn_0  = radius_ringOut_0;
                      radius_ringIn    = radius_ringOut;
                      epsilonIn        = epsilonOut;

                      radius_ringOut_0 = radius_ringIn_0+_fWireDiameter+2.0*secure;
                      radius_ringOut   = radius_ringOut_0+drop;
                      //epsilonOut       = atan((radius_ringOut+drop)*epsilonFactor);
                      epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                      //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                      //else zlength = halfLength;
                      zlength = halfLength;

                      _sprlr[superlayer].addLayer(new Layer());
                      lay = _sprlr[superlayer]._layers.back();
                      lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                      lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*2+iring);
                      lay->_layerType=Layer::wire;
                      if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                      lay->_voxelizationFactor=voxelizationFactor;

                      zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                      zlength/=cos(epsilon);

                      boost::shared_ptr<WireDetail> fwUp( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                      CDCHWireLocater(fwUp,Wire::field,lay,nFwire1,radius_ring_0-FWradii-extShiftFW,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);

                      delta_radius_ring *= scaleFactor;

                    }

                  }

//                  radius_ring_0   += _fWireDiameter;
//                  drop             = radius_ring_0*dropFactor;
//                  radius_ring      = radius_ring_0+drop;
//                  epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                  radius_ringIn_0  = radius_ringOut_0;
                  radius_ringIn    = radius_ringOut;
                  epsilonIn        = epsilonOut;

                  radius_ringOut_0 = radius_ring_0+_fWireDiameter+2.0*secure;
                  radius_ringOut   = radius_ringOut_0+drop;
                  epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                  //if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                  //else zlength = halfLength;
                  zlength = halfLength;

                  --superlayer;
                  _sprlr[superlayer].addLayer(new Layer());
                  lay = _sprlr[superlayer]._layers.back();
                  lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                  lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*3+(nring-1));
                  lay->_layerType=Layer::wire;
                  if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                  lay->_voxelizationFactor=voxelizationFactor;

                  zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                  zlength/=cos(epsilon);

                  boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                  //nFwire = (unsigned int) (CLHEP::twopi*radius_ring_0/fwireDist);
                  //theta_ring = CLHEP::twopi/((float) nFwire);

                  //nFwire1=nFwire;
                  //nFwire1/=2;
                  //theta_ring1=2.0*theta_ring;
//                  CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);
                  CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii+extShiftFW,theta_ring1,ringangle+cellStaggering+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha);
                  iring=0;

                  //int guarWireLayer = superlayer;
                  if (_nInGuardWires!=0) {
                    //++guarWireLayer;
                    if (_nInGuardWires<0) { _nInGuardWires = nFwire; }
                    /*_sprlr[guarWireLayer]._id = SuperLayerId(-1);
                          _sprlr[guarWireLayer].addLayer(new Layer());
                          lay = _sprlr[guarWireLayer]._layers.back();
                          double radiusInGwRing_In_0  = _inGuardRad - _inGWireDiameter;
                          double radiusInGwRing_Out_0 = _inGuardRad + _inGWireDiameter;
                          double epsilonInGwRing_In  = atan(radiusInGwRing_In_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                          double epsilonInGwRing_Out = atan(radiusInGwRing_Out_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                          lay->_detail.reset( new LayerDetail(radiusInGwRing_In_0,radiusInGwRing_Out_0,epsilonInGwRing_In,epsilonInGwRing_Out,zlength,_fillMaterial) );
                          lay->_id = LayerId(&_sprlr[guarWireLayer]._id, 0);
                          lay->_layerType=Layer::wire;*/
                    lay = _sprlr[0].getLayer(0);
                    if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                    lay->_voxelizationFactor=voxelizationFactor;

                    double GWradii = 0.5*_inGWireDiameter;
                    double epsilonInGwRing  = atan(_inGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                    zlength = halfLength;
                    zlength-=sin(epsilonInGwRing)*GWradii;//protect from to extrud of mother volume
                    zlength/=cos(epsilonInGwRing);

                    boost::shared_ptr<WireDetail> inGw( new WireDetail(_inGwShellsThicknesses,_inGwMaterialsName,zlength) );
                    CDCHWireLocater(inGw,Wire::field,lay,_nInGuardWires/2,_inGuardRad-GWradii,theta_ring1,ringangle,epsilonInGwRing,halfalpha);
                    CDCHWireLocater(inGw,Wire::field,lay,_nInGuardWires/2,_inGuardRad+GWradii+extShiftFW,theta_ring1,ringangle+theta_ring,-1.0*epsilonInGwRing,halfalpha,_nInGuardWires/2);
                  }
                  /*if (_nOutGuardWires!=0) {
                          ++guarWireLayer;
                          if (_nOutGuardWires<0) { _nOutGuardWires = nFwire; }
                          _sprlr[guarWireLayer]._id = SuperLayerId(superlayer+1);
                          _sprlr[guarWireLayer].addLayer(new Layer());
                          lay = _sprlr[guarWireLayer]._layers.back();
                          double radiusOutGwRing_In_0  = _outGuardRad - _outGWireDiameter;
                          double radiusOutGwRing_Out_0 = _outGuardRad + _outGWireDiameter;
                          double epsilonOutGwRing_In  = atan(radiusOutGwRing_In_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                          double epsilonOutGwRing_Out = atan(radiusOutGwRing_Out_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                          lay->_detail.reset( new LayerDetail(radiusOutGwRing_In_0,radiusOutGwRing_Out_0,epsilonOutGwRing_In,epsilonOutGwRing_Out,zlength,_fillMaterial) );
                          lay->_id = LayerId(&_sprlr[guarWireLayer]._id, 0);
                          lay->_layerType=Layer::wire;
                          if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                          lay->_voxelizationFactor=voxelizationFactor;

                          double GWradii = 0.5*_outGWireDiameter;
                          double epsilonOutGwRing  = atan(_outGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                          zlength-=sin(epsilonOutGwRing)*GWradii;//protect from to extrud of mother volume
                          zlength/=cos(epsilonOutGwRing);

                          boost::shared_ptr<WireDetail> outGw( new WireDetail(_outGwShellsThicknesses,_outGwMaterialsName,zlength) );
                          CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad-GWradii,theta_ring1,ringangle,epsilonOutGwRing,halfalpha);
                          CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad+GWradii,theta_ring1,ringangle+theta_ring,-1.0*epsilonOutGwRing,halfalpha,_nOutGuardWires/2);
                  }*/
                  if (_nOutGuardWires!=0) {
                    if (_nOutGuardWires<0) { _nOutGuardWires = nFwire; }
                    epsilonOutGwRing     = atan(_outGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                    zlengthOutGwRing     = halfLength;
                    thetaOutGwRing       = theta_ring1;
                    ringangleOutGwRing   = ringangle;
                    ringangleOutGwRing_1 = ringangle+theta_ring;
                    halfalphaOutGwRing   = halfalpha;
                  }

                }
                else if (geomType==60) {

                	_ltt->_cellType     = CDCHtracker::Square;
                	//                  if(_isDumbbell) {
                	//                    _ltt->_cellhnd.reset(new CellGeometryHandle_v3_DBL(_ltt.get()));
                	//                  } else {
                	_ltt->_cellhnd.reset(new CellGeometryHandle_v4(_ltt.get()));
                	//                  }

                	//                  num_wire            = _nSWire;
                	//                  radius_ring_0+=FWradii;
                	//_cellDimension      = radius_ring_0*CLHEP::twopi/((float) num_wire);
                	delta_radius_ring   = _cellDimension;
                	//float fwireDist     = _FWireStep;
                	unsigned int nFwire, nFwire1;
                	int nHorizontalFWire;
                	double cellStaggering=0.0;
                	//bool isCellStaggered;
                	double theta_ring1;
                	float iradius, idelta_radius=0.0;
                	double senseWireRing_radius_0;

                	double scaleFactor   = 0.0;//(1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                	double dropFactor    = 0.0;//(1.0/cos(halfalpha)-1.0);
                	double epsilonFactor = 0.0;//sin(halfalpha)/halfLength;

                	extShiftFW=/*0.5**/secure;

                	//                  lay = _sprlr[0]._layers.back();
                	//                  lay->_detail->setCenterOuterRadiusRing(lay->_detail->centerOuterRadiusRing()-secure);
                	//                  lay->_detail->setStereoAngleOuterRing(atan(lay->_detail->centerOuterRadiusRing()*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian);

                	nHorizontalFWire    = _StoFWireRatio-_nVerticalFWire;
                	//isCellStaggered     = ((nHorizontalFWire/2)%2==0) ? true : false;

                	epsilon=0.0; //just to remove compilation warning
                	theta_ring=theta_ring1=0.0; //just to remove compilation warning
                	nFwire=nFwire1=0; //just to remove compilation warning

                	//                    int INnTotSuperLayer = nsuperlayer-_OSnTotSuperLayer;
                	int lnSuperlayer=0;
                	int nSWire=0;
                	int nSDeltaWire;
                	superlayer=-1;
                	for (int iShell=-1; iShell<_nOuterShell; ++iShell){
                		if (iShell==-1) {
                			lnSuperlayer=nsuperlayer-_OSnTotSuperLayer;
                			nSWire=_nSWire;
                			nSDeltaWire=_nSDeltaWire;
                		}
                		else {
                			lnSuperlayer=_OSnSuperLayer[iShell];
                			nSWire=_OSnSWire[iShell];
                			nSDeltaWire=_OSnSDeltaWire[iShell];
                			radius_ring_0=_OScellDimension[iShell]*_OSnSWire[iShell]/CLHEP::twopi-_OScellDimension[iShell]*0.5;
                			halfalpha=_OSalpha[iShell] * 0.5*CLHEP::degree;
                			nring=_OSnRing[iShell];
                		}
                		for ( int lSuperlayer=0; lSuperlayer<lnSuperlayer;lSuperlayer++ ) {
                			++superlayer;
                			std::cout <<"Building super layer: "<<superlayer+1<<std::endl;

                			_sprlr[superlayer]._id = SuperLayerId(superlayer);

                			num_wire    = nSWire+lSuperlayer*nSDeltaWire;
                			//                    scaleFactor = (1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                			phi         = CLHEP::twopi/((float) num_wire);
                			nFwire      = nHorizontalFWire*num_wire;
                			theta_ring  = CLHEP::twopi/((float) nFwire);
                			nFwire1     = nFwire;
                			nFwire1    /= 2;
                			theta_ring1 = 2.0*theta_ring;

                			scaleFactor   = (1.0+CLHEP::pi/num_wire)/(1.0-CLHEP::pi/num_wire);
                			dropFactor    = (1.0/cos(halfalpha)-1.0);
                			epsilonFactor = sin(halfalpha)/halfLength;

                			ringangle        = -0.5*phi;
                			if (lSuperlayer==0) {
                				if (iShell==-1) {
                					lay = _sprlr[superlayer/*0*/]._layers.back();
                					lay->_detail->setCenterOuterRadiusRing(lay->_detail->centerOuterRadiusRing()-secure);
                					lay->_detail->setStereoAngleOuterRing(atan(lay->_detail->centerOuterRadiusRing()*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian);
                				}

                				drop             = radius_ring_0*dropFactor;
                				radius_ring      = radius_ring_0+drop;
                				epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                				radius_ringIn_0  = radius_ring_0-FWradii-2.0*secure;
                				radius_ringIn    = radius_ringIn_0+drop;
                				radius_ringOut_0 = radius_ring_0+FWradii;
                				radius_ringOut   = radius_ringOut_0+drop;
                				epsilonIn        = atan(sqrt(diff_of_squares(radius_ringIn, radius_ringIn_0))/halfLength) * CLHEP::radian;
                				epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                				//if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                				//else zlength = halfLength;
                				zlength = halfLength;

                				_sprlr[superlayer/*0*/].addLayer(new Layer());
                				lay = _sprlr[superlayer/*0*/]._layers.back();
                				lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                				lay->_id = LayerId(&_sprlr[superlayer/*0*/]._id, _ltt->maxNLayPerSL()*3);
                				lay->_layerType=Layer::wire;
                				if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                				lay->_voxelizationFactor=voxelizationFactor;

                				zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                				zlength/=cos(epsilon);

                				boost::shared_ptr<WireDetail> fwDw0( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                				cellStaggering=0.0;

                				CDCHWireLocater(fwDw0,Wire::field,lay,nFwire1,radius_ring_0-FWradii-extShiftFW,theta_ring1,ringangle+cellStaggering-theta_ring,sign_epsilon*epsilon,halfalpha);

                				if (iShell!=-1) {
                    				_sprlr[superlayer].addLayer(new Layer());
                    				lay = _sprlr[superlayer]._layers.back();
                    				lay->_detail.reset( new LayerDetail(radius_ringIn_0-_OSservSpace[iShell],radius_ringIn_0,epsilonIn,epsilonIn,zlength,_fillMaterial) );
                    				lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*4);
                    				lay->_layerType=Layer::serv;
                				}
                				radius_ring_0+=FWradii; // just to fix the Sw radius with the oder type.

                			} else {
                				delta_radius_ring = CLHEP::twopi*radius_ringOut_0/(num_wire-CLHEP::pi);
                			}

                			for( iring=0; iring< nring; iring++ ){
                				std::cout <<"Building layer: "<<iring+1<<std::endl;

                				//                    _sprlr[superlayer]._id = SuperLayerId(superlayer);

                				inscribedRadius        = 0.5*delta_radius_ring; //delta_radius_ring is equal to the cell height of each layer
                				circumscribedRadius    = inscribedRadius*sqrt(2.0);
                				senseWireRing_radius_0 = radius_ring_0+inscribedRadius;
                				//if ( (CLHEP::twopi*radius_ring_0/((float)nFwire))<fwireDist ) throw cet::exception("GEOM")<< "Error during field wire positioning: "<< nFwire
                				//                                                        <<" field wires don't fit on a circumference with radius of "<<radius_ring_0<<" using a step of "<<fwireDist<<std::endl;

                				sign_epsilon    *=-1;

                				//                    ringangle        = -0.5*phi;

                				//                    iring            = 0;
                				//                    drop             = radius_ring_0*dropFactor;
                				//                    radius_ring      = radius_ring_0+drop;
                				//                    epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                				radius_ringIn_0  = radius_ringOut_0;
                				radius_ringIn    = radius_ringOut;
                				epsilonIn        = epsilonOut;

                				radius_ringOut_0 = radius_ringIn_0+_fWireDiameter+2.0*secure;
                				radius_ringOut   = radius_ringOut_0+drop;
                				//epsilonOut       = atan((radius_ringOut+drop)*epsilonFactor);
                				epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                				//if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                				//else zlength = halfLength;
                				zlength = halfLength;

                				_sprlr[superlayer].addLayer(new Layer());
                				lay = _sprlr[superlayer]._layers.back();
                				lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                				lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*1+iring);
                				lay->_layerType=Layer::wire;
                				if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                				lay->_voxelizationFactor=voxelizationFactor;

                				zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                				zlength/=cos(epsilon);

                				boost::shared_ptr<WireDetail> fwDw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );

                				if (/*isCellStaggered &&*/ (iring%2==1)) cellStaggering=theta_ring;
                				else cellStaggering=0.0;

                				CDCHWireLocater(fwDw,Wire::field,lay,nFwire1,radius_ringIn_0+FWradii+extShiftFW,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);

                				iradius          = radius_ring_0;

                				radius_ring_0    += delta_radius_ring;
                				drop             = radius_ring_0*dropFactor;

                				radius_ringIn_0  = radius_ringOut_0;
                				radius_ringIn    = radius_ringOut;
                				epsilonIn        = epsilonOut;
                				radius_ringOut_0 = radius_ring_0-_fWireDiameter-2.0*secure;
                				radius_ringOut   = radius_ringOut_0+drop;
                				epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;
                				//if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                				//else zlength = halfLength;
                				zlength = halfLength;

                				_sprlr[superlayer].addLayer(new Layer(),true);
                				lay = _sprlr[superlayer]._layers.back();
                				lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                				lay->_id = LayerId(&_sprlr[superlayer]._id, iring);
                				lay->_layerType=Layer::gas;
                				if (_notExtVoxel) voxelizationFactor = 5.0/((float)((1+_nVerticalFWire)*num_wire));
                				lay->_voxelizationFactor=voxelizationFactor;

                				zlength-=sin(epsilon)*CntFWradii;//protect from to extrud of mother volume
                				zlength/=cos(epsilon);

                				boost::shared_ptr<WireDetail> sw;
                				boost::shared_ptr<CellDetail> celld;
                				sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
                				celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );

                				epsilon          = atan(senseWireRing_radius_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;

                				CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);
                				std::cout<<"i-slayer "<<superlayer<<" sense wire rad at z=0 "<<senseWireRing_radius_0<<" drop "<<drop<<" cell dim "<<delta_radius_ring<<" epsilon "<<sign_epsilon*epsilon<<std::endl;

                				boost::shared_ptr<WireDetail> fwCn( new WireDetail(_cntfwShellsThicknesses,_cntfwMaterialsName,zlength) );
                				idelta_radius = delta_radius_ring/((float) (1+_nVerticalFWire));
                				for( int iVrtFw=0; iVrtFw< _nVerticalFWire ; iVrtFw++ ){

                					iradius+=idelta_radius;
                					epsilon          = atan(iradius*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                					CDCHWireLocater(fwCn,Wire::field,lay,num_wire,iradius,phi,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha,iVrtFw*num_wire);

                				}

                				//............

                				radius_ringIn_0  = radius_ringOut_0;
                				radius_ringIn    = radius_ringOut;
                				epsilonIn        = epsilonOut;

                				radius_ringOut_0 = radius_ringIn_0+_fWireDiameter+2.0*secure;
                				radius_ringOut   = radius_ringOut_0+drop;
                				//epsilonOut       = atan((radius_ringOut+drop)*epsilonFactor);
                				epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                				//if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                				//else zlength = halfLength;
                				zlength = halfLength;

                				_sprlr[superlayer].addLayer(new Layer());
                				lay = _sprlr[superlayer]._layers.back();
                				lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                				lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*2+iring);
                				lay->_layerType=Layer::wire;
                				if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                				lay->_voxelizationFactor=voxelizationFactor;

                				zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                				zlength/=cos(epsilon);

                				boost::shared_ptr<WireDetail> fwUp( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                				CDCHWireLocater(fwUp,Wire::field,lay,nFwire1,radius_ring_0-FWradii-extShiftFW,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);

                				delta_radius_ring *= scaleFactor;

                			}

                		}

                		//                  radius_ring_0   += _fWireDiameter;
                		//                  drop             = radius_ring_0*dropFactor;
                		//                  radius_ring      = radius_ring_0+drop;
                		//                  epsilon          = atan(radius_ring*epsilonFactor) * CLHEP::radian;

                		radius_ringIn_0  = radius_ringOut_0;
                		radius_ringIn    = radius_ringOut;
                		epsilonIn        = epsilonOut;

                		radius_ringOut_0 = radius_ring_0+_fWireDiameter+2.0*secure;
                		radius_ringOut   = radius_ringOut_0+drop;
                		epsilonOut       = atan(sqrt(diff_of_squares(radius_ringOut, radius_ringOut_0))/halfLength) * CLHEP::radian;

                		//if (EndCap_type==1) zlength = sqrt( diff_of_squares(max_EndCap_dim, radius_ringOut) );
                		//else zlength = halfLength;
                		zlength = halfLength;

                		//                    --superlayer;
                		_sprlr[superlayer].addLayer(new Layer());
                		lay = _sprlr[superlayer]._layers.back();
                		lay->_detail.reset( new LayerDetail(radius_ringIn_0,radius_ringOut_0,epsilonIn,epsilonOut,zlength,_fillMaterial) );
                		lay->_id = LayerId(&_sprlr[superlayer]._id, _ltt->maxNLayPerSL()*3+(nring-1));
                		lay->_layerType=Layer::wire;
                		if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
                		lay->_voxelizationFactor=voxelizationFactor;

                		zlength-=sin(epsilon)*FWradii;//protect from to extrud of mother volume
                		zlength/=cos(epsilon);

                		boost::shared_ptr<WireDetail> fw( new WireDetail(_fwShellsThicknesses,_fwMaterialsName,zlength) );
                		//nFwire = (unsigned int) (CLHEP::twopi*radius_ring_0/fwireDist);
                		//theta_ring = CLHEP::twopi/((float) nFwire);

                		//nFwire1=nFwire;
                		//nFwire1/=2;
                		//theta_ring1=2.0*theta_ring;
                		//                  CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0-FWradii,theta_ring1,ringangle+cellStaggering,sign_epsilon*epsilon,halfalpha);
                		CDCHWireLocater(fw,Wire::field,lay,nFwire1,radius_ring_0+FWradii+extShiftFW,theta_ring1,ringangle+cellStaggering+theta_ring,-1.0*sign_epsilon*epsilon,halfalpha);
                		iring=0;

                	}//close loop on shells

                	//int guarWireLayer = superlayer;
                	if (_nInGuardWires!=0) {
                		//++guarWireLayer;
                		if (_nInGuardWires<0) { _nInGuardWires = nFwire; }
                		/*_sprlr[guarWireLayer]._id = SuperLayerId(-1);
                            _sprlr[guarWireLayer].addLayer(new Layer());
                            lay = _sprlr[guarWireLayer]._layers.back();
                            double radiusInGwRing_In_0  = _inGuardRad - _inGWireDiameter;
                            double radiusInGwRing_Out_0 = _inGuardRad + _inGWireDiameter;
                            double epsilonInGwRing_In  = atan(radiusInGwRing_In_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                            double epsilonInGwRing_Out = atan(radiusInGwRing_Out_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                            lay->_detail.reset( new LayerDetail(radiusInGwRing_In_0,radiusInGwRing_Out_0,epsilonInGwRing_In,epsilonInGwRing_Out,zlength,_fillMaterial) );
                            lay->_id = LayerId(&_sprlr[guarWireLayer]._id, 0);
                            lay->_layerType=Layer::wire;*/
                		lay = _sprlr[0].getLayer(0);
                		if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                		lay->_voxelizationFactor=voxelizationFactor;

                		double GWradii = 0.5*_inGWireDiameter;
                		double epsilonInGwRing  = atan(_inGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                		zlength = halfLength;
                		zlength-=sin(epsilonInGwRing)*GWradii;//protect from to extrud of mother volume
                		zlength/=cos(epsilonInGwRing);

                		boost::shared_ptr<WireDetail> inGw( new WireDetail(_inGwShellsThicknesses,_inGwMaterialsName,zlength) );
                		CDCHWireLocater(inGw,Wire::field,lay,_nInGuardWires/2,_inGuardRad-GWradii,theta_ring1,ringangle,epsilonInGwRing,halfalpha);
                		CDCHWireLocater(inGw,Wire::field,lay,_nInGuardWires/2,_inGuardRad+GWradii+extShiftFW,theta_ring1,ringangle+theta_ring,-1.0*epsilonInGwRing,halfalpha,_nInGuardWires/2);
                	}
                	/*if (_nOutGuardWires!=0) {
                            ++guarWireLayer;
                            if (_nOutGuardWires<0) { _nOutGuardWires = nFwire; }
                            _sprlr[guarWireLayer]._id = SuperLayerId(superlayer+1);
                            _sprlr[guarWireLayer].addLayer(new Layer());
                            lay = _sprlr[guarWireLayer]._layers.back();
                            double radiusOutGwRing_In_0  = _outGuardRad - _outGWireDiameter;
                            double radiusOutGwRing_Out_0 = _outGuardRad + _outGWireDiameter;
                            double epsilonOutGwRing_In  = atan(radiusOutGwRing_In_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                            double epsilonOutGwRing_Out = atan(radiusOutGwRing_Out_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                            lay->_detail.reset( new LayerDetail(radiusOutGwRing_In_0,radiusOutGwRing_Out_0,epsilonOutGwRing_In,epsilonOutGwRing_Out,zlength,_fillMaterial) );
                            lay->_id = LayerId(&_sprlr[guarWireLayer]._id, 0);
                            lay->_layerType=Layer::wire;
                            if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire);
                            lay->_voxelizationFactor=voxelizationFactor;

                            double GWradii = 0.5*_outGWireDiameter;
                            double epsilonOutGwRing  = atan(_outGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                            zlength-=sin(epsilonOutGwRing)*GWradii;//protect from to extrud of mother volume
                            zlength/=cos(epsilonOutGwRing);

                            boost::shared_ptr<WireDetail> outGw( new WireDetail(_outGwShellsThicknesses,_outGwMaterialsName,zlength) );
                            CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad-GWradii,theta_ring1,ringangle,epsilonOutGwRing,halfalpha);
                            CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad+GWradii,theta_ring1,ringangle+theta_ring,-1.0*epsilonOutGwRing,halfalpha,_nOutGuardWires/2);
                    }*/
                	if (_nOutGuardWires!=0) {
                		if (_nOutGuardWires<0) { _nOutGuardWires = nFwire; }
                		epsilonOutGwRing     = atan(_outGuardRad*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
                		zlengthOutGwRing     = halfLength;
                		thetaOutGwRing       = theta_ring1;
                		ringangleOutGwRing   = ringangle;
                		ringangleOutGwRing_1 = ringangle+theta_ring;
                		halfalphaOutGwRing   = halfalpha;
                	}

                }

                radius_ringIn_0  = radius_ringOut_0;
                radius_ringIn    = radius_ringOut;
                epsilonIn        = epsilonOut;

                _sprlr[superlayer].addLayer(new Layer());
                lay = _sprlr[superlayer]._layers.back();
                lay->_detail.reset( new LayerDetail(radius_ringIn_0,outer_radius-envelop_Outer_thickness-0.0001,epsilonOut,0.0,halfLength,_fillMaterial) );
                lay->_id = LayerId(&_sprlr[superlayer]._id, ++iring);
                if (_nOutGuardWires!=0) {
                        if (_notExtVoxel) voxelizationFactor = 1.0/((float)_nOutGuardWires);
                        lay->_voxelizationFactor=voxelizationFactor;
                        double GWradii = 0.5*_outGWireDiameter;
                        zlengthOutGwRing -= sin(epsilonOutGwRing)*GWradii;//protect from to extrud of mother volume
                        zlengthOutGwRing /= cos(epsilonOutGwRing);

                        boost::shared_ptr<WireDetail> outGw( new WireDetail(_outGwShellsThicknesses,_outGwMaterialsName,zlengthOutGwRing) );
                        CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad-GWradii,thetaOutGwRing,ringangleOutGwRing,epsilonOutGwRing,halfalphaOutGwRing);
                        CDCHWireLocater(outGw,Wire::field,lay,_nOutGuardWires/2,_outGuardRad+GWradii+extShiftFW,thetaOutGwRing,ringangleOutGwRing_1,-1.0*epsilonOutGwRing,halfalphaOutGwRing,_nOutGuardWires/2);
                }

                _ltt->_sprlr.reset(_sprlr);

                double constrainR = outer_radius;
                cout<<"rIn "<<radius_ringIn<<" drop "<<drop<<" OR "<<outer_radius;
                if (_detailedWireSupport && _isElectCont ) {
                        cout<<" Othk "<<_elctContWallThick<<endl;
                        constrainR -= _elctContWallThick;
                } else {
                        cout<<" Othk "<<envelop_Outer_thickness<<endl;
                        constrainR -= envelop_Outer_thickness;
                }
                if ( (radius_ringIn_0+drop) > constrainR ) {
//                        throw cet::exception("GEOM") <<"The CDCHtracker gas layer doesn't fit inside the CDCHtracker outer wall\n";
                  exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
                  e<<"The CDCHtracker gas layer doesn't fit inside the CDCHtracker outer wall\n";
                  e.error();
                }

                multimap<Wall::Walltype,Wall* >::iterator walls_it;
                for ( walls_it=_walls.begin() ; walls_it != _walls.end(); walls_it++ ) {
                        _ltt->addWall(walls_it->second);
                }

                AssignFieldWireToCell();
        }

}

void CDCHMaker::CDCHFldWireLocater ( boost::shared_ptr<WireDetail> &wdetail, boost::shared_ptr<Layer> &lay, int NofWire,
                double PosRadius, double Theta, double ThetaOffset, double Stereo, double halfAlpha )
{
        HepGeom::Transform3D Transform  ( HepGeom::Translate3D(PosRadius, 0., 0.) * HepGeom::RotateX3D(Stereo) );
        for (int copyNo=0; copyNo<NofWire; copyNo++){
                HepGeom::RotateZ3D iRot (ThetaOffset + Theta*((copyNo/2)+copyNo) );

                lay->addFieldWire( new Wire( WireId(&(lay->_id),copyNo), wdetail, new HepGeom::Transform3D(iRot * Transform), Stereo, 2.0*halfAlpha, Wire::field ) );
        }
}

void CDCHMaker::CDCHWireLocater ( boost::shared_ptr<WireDetail> &wdetail, Wire::Wtype wireType, boost::shared_ptr<Layer> &lay/*Layer *lay*/, int NofWire,
                double PosRadius, double Theta, double ThetaOffset, double Stereo, double halfAlpha, int copyNunOffset, boost::shared_ptr<CellDetail> *celldetail )
{
        HepGeom::Transform3D Transform  ( HepGeom::Translate3D(PosRadius, 0., 0.) * HepGeom::RotateX3D(Stereo) );
        if ( wireType == Wire::sense ){
                for (int copyNo=0; copyNo<NofWire; copyNo++){
                        HepGeom::RotateZ3D iRot ( ThetaOffset + Theta*copyNo );
                        WireId twid (&(lay->_id),copyNunOffset+copyNo);
                        lay->addCell(
                                        new Cell(
                                                        CellId(twid), *celldetail,
                                                        boost::shared_ptr<Wire> (
                                                                        new Wire( twid, wdetail, new HepGeom::Transform3D(iRot * Transform), Stereo, 2.0*halfAlpha, wireType )
                                                        )
                                        )
                        );
                }
        } else {
                for (int copyNo=0; copyNo<NofWire; copyNo++){
                        HepGeom::RotateZ3D iRot ( ThetaOffset + Theta*copyNo );
                        lay->addFieldWire( new Wire( WireId(&(lay->_id),copyNunOffset+copyNo), wdetail, new HepGeom::Transform3D(iRot * Transform), Stereo, 2.0*halfAlpha, wireType ) );
                }
        }
}

void CDCHMaker::AssignFieldWireToCell() {
        for (int iSl=0; iSl<_ltt->nSuperLayers(); ++iSl){
                SuperLayer* sl = _ltt->getSuperLayer(iSl);
                for (int iLy=0; iLy<sl->nLayers(); ++iLy){
                        boost::shared_ptr<Layer> ly = sl->getLayer(iLy);
                        if (ly->nCells()>0){
                                for (int iCel=0; iCel<ly->nCells(); ++iCel) {
                                        boost::shared_ptr<Cell> cel = ly->getCell(iCel);
                                        double cellMidRad = cel->getMidPoint().rho();
                                        double maxWiresDist = cel->getRadius()+0.5;
                                        double maxWiresDist2 = maxWiresDist * maxWiresDist;
                                        double cellEpsilob = cel->getWire()->getEpsilon();
                                        //cout<<"Cell "<<cel->Id()<<" rho "<<cellMidRad<<" rmaxWiresDist "<<maxWiresDist<<" eps "<<cellEpsilob<<endl;
                                        for (int iLyFw=0; iLyFw<ly->nFieldWires(); ++iLyFw) {
                                                boost::shared_ptr<Wire> ifw = ly->getFWire(iLyFw);
                                                if ( cel->getMidPoint().diff2(ifw->getMidPoint())<maxWiresDist2 ) {
                                                        cel->_fieldWires.push_back(ifw);
                                                        //cout<<"Select c cell "<<cel->getMidPoint()<<" c fw "<<ifw->getMidPoint()<<" fw eps "<<ifw->getEpsilon()<<endl;
                                                }
                                        }
                                        for (int iLy1=0; iLy1<sl->nLayers(); ++iLy1){
                                                boost::shared_ptr<Layer> ly1 = sl->getLayer(iLy1);
                                                double cellLayRadDist = (iLy1>iLy?ly1->getDetail()->centerInnerRadiusRing():ly1->getDetail()->centerOuterRadiusRing()) - cellMidRad;
                                                if ( iLy!=iLy1 && ly1->nFieldWires()>0 && fabs(cellLayRadDist) < maxWiresDist && cellEpsilob*ly1->getFWire(0)->getEpsilon()>0.0){
                                                        for (int iLyFw=0; iLyFw<ly1->nFieldWires(); ++iLyFw) {
                                                                boost::shared_ptr<Wire> ifw = ly1->getFWire(iLyFw);
                                                                if ( cel->getMidPoint().diff2(ifw->getMidPoint())<maxWiresDist2 ) {
                                                                         cel->_fieldWires.push_back(ifw);
                                                                         //cout<<"Select c cell "<<cel->getMidPoint()<<" c fw "<<ifw->getMidPoint()<<" fw eps "<<ifw->getEpsilon()<<endl;
                                                                 }
                                                         }
                                                }
                                        }
                                        int iSl1=iSl+1;
                                        if (iSl1<_ltt->nSuperLayers()) {
                                                SuperLayer* sl1 = _ltt->getSuperLayer(iSl1);
                                                for (int iLy1=0; iLy1<sl1->nLayers(); ++iLy1){
                                                        boost::shared_ptr<Layer> ly1 = sl1->getLayer(iLy1);
                                                        double cellLayRadDist = ly1->getDetail()->centerInnerRadiusRing() - cellMidRad;
                                                        if ( (cellLayRadDist>0 && cellLayRadDist<maxWiresDist) &&
                                                             ly1->nFieldWires()>0 ){
                                                                for (int iLyFw=0; iLyFw<ly1->nFieldWires(); ++iLyFw) {
                                                                         boost::shared_ptr<Wire> ifw = ly1->getFWire(iLyFw);
                                                                         if ( (cellEpsilob*ifw->getEpsilon())>0.0 && cel->getMidPoint().diff2(ifw->getMidPoint())<maxWiresDist2 ) {
                                                                                 cel->_fieldWires.push_back(ifw);
                                                                                 //cout<<"Select c cell "<<cel->getMidPoint()<<" c fw "<<ifw->getMidPoint()<<" fw eps "<<ifw->getEpsilon()<<endl;
                                                                         }
                                                                 }
                                                        }
                                                }
                                        }
                                        iSl1=iSl-1;
                                        if (iSl1>=0 && iSl1<_ltt->nSuperLayers()) {
                                                SuperLayer* sl1 = _ltt->getSuperLayer(iSl1);
                                                for (int iLy1=0; iLy1<sl1->nLayers(); ++iLy1){
                                                        boost::shared_ptr<Layer> ly1 = sl1->getLayer(iLy1);
                                                        double cellLayRadDist = ly1->getDetail()->centerOuterRadiusRing() - cellMidRad;
                                                        if ( (cellLayRadDist>-maxWiresDist && cellLayRadDist<0) &&
                                                             ly1->nFieldWires()>0 ){
                                                                for (int iLyFw=0; iLyFw<ly1->nFieldWires(); ++iLyFw) {
                                                                         boost::shared_ptr<Wire> ifw = ly1->getFWire(iLyFw);
                                                                         if ( (cellEpsilob*ifw->getEpsilon())>0.0 && cel->getMidPoint().diff2(ifw->getMidPoint())<maxWiresDist2 ) {
                                                                                  cel->_fieldWires.push_back(ifw);
                                                                                  //cout<<"Select c cell "<<cel->getMidPoint()<<" c fw "<<ifw->getMidPoint()<<" fw eps "<<ifw->getEpsilon()<<endl;
                                                                         }
                                                                 }
                                                        }
                                                }
                                        }
                                        //cout<<"cell "<<cel->Id()<<" nField wires "<<cel->nFWires()<<endl;
                                }
                        }
                }
        }
        _ltt->getSuperLayersArray();
}

} // namespace cdch

#endif
