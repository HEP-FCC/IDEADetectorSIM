// PHCV geometry maker
//
// Original author G. Tassielli
//

//#include "ROGeometryHandle.hh"
#include "manageException.hh"
#include "SimpleConfig.hh"
#include "GeomHandle.hh"
//#include "messagefacility/MessageLogger/MessageLogger.h"

#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include "PHCVMaker.hh"
#include "Shell.hh"
#include "SLayer.hh"
#include "PHCVtcounter.hh"

#include "CDCHtracker.hh"
#include "SuperLayer.hh"
//#include "Layer.hh"

#ifndef __CINT__

using CLHEP::Hep3Vector;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

using namespace std;

namespace phcv {

PHCVMaker::PHCVMaker( crd::SimpleConfig const& config):
                                                    _center(){

        _isExternal     = false;
        _extFile        = config.getString("phcv.extFile");
        if ( _extFile.size()>1 && ( _extFile.find_last_of(".gdml") || _extFile.find_last_of(".GDML") )!=0 ) _isExternal = true;

        _z0             = config.getDouble("phcv.z0");
        _geomType       = config.getInt("phcv.geomType");

        _halfLength     = config.getDouble("phcv.halfLength");

        _nShell         = config.getInt("phcv.nShell");

        config.getVectorDouble("phcv.shellThick",_shellThick,_nShell);

        _CDCHnSrvShell  = 0;

        loadTCounter(config);

        loadRadiator(config);

        cout<<"------------------- PHCV -------------------"<<endl;
//        cout<<"r0 "<<_r0<<" rOut "<<_rOut<<" leng "<<2.0*_halfLength<<endl;
//        cout<<"nLayers "<<_nLayers<<endl;
//        unsigned long totNumRO = 0;
//        for (int il=0; il<_nLayers; ++il) {
//          cout<<"Ly: "<<il<<endl;
//          cout<<"InR "<<_LayersInnerRad[il]<<" Thick "<<_LaddersThickness[il]<<endl;
//          cout<<"nRoX "<<_nROsFstSdPerLadder[il]<<" nRoY "<<_nROsSndSdPerLadder[il]<<" nRo "<<_nROsPerLadder[il]<<endl;
//          totNumRO +=  _nROsPerLadder[il]*_nLaddersPerSector[il]*_nPhiSectors[il];
//        }
//        cout<<"nFwdLayers "<<_nFwdLayers<<endl;
//        for (int il=0; il<_nFwdLayers; ++il) {
//          cout<<"Ly: "<<_nLayers+il<<endl;
//          cout<<"InR "<<_LayersInnerRad_fwd[il]<<" Thick "<<_LaddersThickness_fwd[il]<<endl;
//          for( int ilad=0; ilad<_nLaddersPerSector_fwd[il]; ++ilad ){
//            cout<<"Lad: "<<ilad<<endl;
//            cout<<"nRoX "<<_nROsFstSdShrtPerLadder_fwd[il][ilad]<<" nRoX2 "<<_nROsFstSdLngPerLadder_fwd[il][ilad]<<" nRoY "<<_nROsSndSdPerLadder_fwd[il][ilad]<<" nRo "<<_nROsPerLadder_fwd[il][ilad]<<endl;
//            totNumRO +=  _nROsPerLadder_fwd[il][ilad]*_nPhiSectors_fwd[il];
//          }
//        }

//        cout<<"Tot Numb of Channels "<<totNumRO<<endl;
//        cout<<"nRadiator Layers "<<_nRadiatLayers<<endl;

//        _r0-=1*CLHEP::mm;
//        _rOut+=1*CLHEP::mm;
//        _halfLength+=1*CLHEP::mm;


        // Do the real work.
        Build( );
}

PHCVMaker::~PHCVMaker (){}

void PHCVMaker::loadTCounter( crd::SimpleConfig const& config ){

    config.getVectorDouble("phcv.scintThick",_scintThick,_nShell);
    _fillScintMaterial = config.getString("phcv.scintMat");

}

void PHCVMaker::loadRadiator( crd::SimpleConfig const& config ){

    config.getVectorDouble("phcv.radThick",_radThick,_nShell);
    _fillRadMaterial = config.getString("phcv.radiatorMat");

}

void PHCVMaker::Build(){

  _lpst = unique_ptr<PHCVtcounter>(new PHCVtcounter());
  _lpst->_isExternal = _isExternal;
  _lpst->_geomType=_geomType;

  if (_nShell<1) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"PHCV: Number of Shell is not acceptable\n";
    e.error();
  }

  if (_isExternal) {
    //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"PHCV: Using GDML file option is temporarily disabled\n";
    e.error();

  } else {

    _lpst->_nShell = _nShell;
    _lpst->_zhalfLength = _halfLength;
    _lpst->_z0 = _z0;

    // Master geometry for the CDCHtracker.
    GeomHandle<cdch::CDCHtracker> cdchtracker;
    for (int iCDCHsl=0; iCDCHsl<cdchtracker->nSuperLayers(); ++iCDCHsl) {
    	cdch::SuperLayer* cdchSl = cdchtracker->getSuperLayer(iCDCHsl);
    	for (int iCDCHly=0; iCDCHly<cdchSl->nLayers(); ++iCDCHly) {
    		boost::shared_ptr<cdch::Layer> cdchLy = cdchSl->getLayer(iCDCHly);
    		if (cdchLy->getLayerType() == cdch::Layer::serv ) {
    			++_CDCHnSrvShell;
    			boost::shared_ptr<cdch::LayerDetail> cdchLyD = cdchLy->getDetail();
    			_CDCHSrvLyInRad.push_back(cdchLyD->centerInnerRadiusRing());
    			_CDCHSrvLyOutRad.push_back(cdchLyD->centerOuterRadiusRing());
    			_CDCHSrvLyInAngl.push_back(cdchLyD->stereoAngleInnerRing());
    			_CDCHSrvLyOutAngl.push_back(cdchLyD->stereoAngleOuterRing());
    			_CDCHSrvLyHalfLng.push_back(cdchLyD->halfLength());
    			_CDCHSrvLySlid.push_back(cdchLy->Id().getSuperLayer());
    		}
    	}
    }

    if (_CDCHnSrvShell!=_nShell) {
      exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
      e<<"PHCV: nShell differs from n. of CDCH Service Shells \n";
      e.error();
    }

    Shell *_shell     = new Shell[_nShell];
    boost::shared_ptr<SLayer> lay;
    for (int iShl=0; iShl<_nShell; ++iShl) {

    	if (_shellThick[iShl]>(_CDCHSrvLyOutRad[iShl]-_CDCHSrvLyInRad[iShl])) {
    		exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    		e<<"PHCV: "<<iShl<<" Shell thickness is larger than the available space in the corresponding CDCH Service Shell \n";
    		e<<"Shell thick: "<<_shellThick[iShl]<<" Serv Thick: "<<(_CDCHSrvLyOutRad[iShl]-_CDCHSrvLyInRad[iShl])<<"\n";
    		e.error();
    	}
    	_lpst->_shellThick.push_back(_shellThick[iShl]);
    	_lpst->_scintThick.push_back(_scintThick[iShl]);
    	_lpst->_radThick.push_back(_radThick[iShl]);
    	_lpst->_shellRadIn.push_back(_CDCHSrvLyInRad[iShl]);
    	double radIn = _CDCHSrvLyOutRad[iShl]-_scintThick[iShl]-_radThick[iShl];

    	_shell[iShl]._id = ShellId(iShl,_CDCHSrvLySlid[iShl]);

    	_shell[iShl].addSLayer(new SLayer());
        lay = _shell[iShl]._layers.back();
        lay->_detail.reset( new SLayerDetail(radIn,radIn+_radThick[iShl],_CDCHSrvLyInAngl[iShl],_CDCHSrvLyInAngl[iShl],_CDCHSrvLyHalfLng[iShl],_fillRadMaterial) );
        lay->_id = SLayerId(&_shell[iShl]._id, 0);
        lay->_layerType=SLayer::radiat;

        _shell[iShl].addSLayer(new SLayer());
        lay = _shell[iShl]._layers.back();
        lay->_detail.reset( new SLayerDetail(radIn+_radThick[iShl],_CDCHSrvLyOutRad[iShl],_CDCHSrvLyOutAngl[iShl],_CDCHSrvLyOutAngl[iShl],_CDCHSrvLyHalfLng[iShl],_fillScintMaterial) );
        lay->_id = SLayerId(&_shell[iShl]._id, 1);
        lay->_layerType=SLayer::scint;
//        if (_notExtVoxel) voxelizationFactor = 1.0/((float)nFwire1);
//        lay->_voxelizationFactor=voxelizationFactor;

//FIX for future! do something like this to insert Fiber and Wires
//		boost::shared_ptr<WireDetail> sw;
//		boost::shared_ptr<CellDetail> celld;
//		sw.reset( new WireDetail(_swShellsThicknesses,_swMaterialsName,zlength) );
//		celld.reset( new CellDetail(circumscribedRadius,inscribedRadius,sw) );
//
//		epsilon          = atan(senseWireRing_radius_0*(1.0+dropFactor)*epsilonFactor) * CLHEP::radian;
//
//		CDCHWireLocater(sw,Wire::sense,lay,num_wire,senseWireRing_radius_0,phi,0.0+cellStaggering,sign_epsilon*epsilon,halfalpha,0,&celld);


    }

    _lpst->_shell.reset(_shell);
    //    _lpst->_roChhnd.reset( new ROGeometryHandle(_lpst.get()) );

  }
}

} // namespace phcv

#endif
