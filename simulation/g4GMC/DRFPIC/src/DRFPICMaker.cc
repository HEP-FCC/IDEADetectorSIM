// DRFPIC geometry maker
//
// Original author G. Tassielli
//

#include "DRFPICMaker.hh"

#include "manageException.hh"
#include "SimpleConfig.hh"
#include "GeomHandle.hh"
//#include "messagefacility/MessageLogger/MessageLogger.h"

#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"
//#include "CLHEP/Hep3Vector/ThreeVector.h"

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include "DRFPIcalorimeter.hh"
#include "Tower.hh"

#include "dimensionB.hh"
#include "dimensionE.hh"

#ifndef __CINT__

using CLHEP::Hep3Vector;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

using namespace std;

namespace drc {

DRFPICMaker::DRFPICMaker( crd::SimpleConfig const& config):
                                                    _center(){

        _isExternal     = false;
        _extFile        = config.getString("drc.extFile");
        if ( _extFile.size()>1 && ( _extFile.find_last_of(".gdml") || _extFile.find_last_of(".GDML") )!=0 ) _isExternal = true;

        _z0             = config.getDouble("drc.z0");
        _geomType       = config.getInt("drc.geomType");

//        _halfLength     = config.getDouble("drc.halfLength");
//
        _innerR         = config.getInt("drc.innerR");
        _NbOfBarrel     = config.getInt("drc.NbOfBarrel");  
        _NbOfEndcap     = config.getInt("drc.NbOfEndcap");  
        _tower_height   = config.getDouble("drc.tower_height"); 
        _NbOfZRot       = config.getInt("drc.NbOfZRot");
		_PMTT           = config.getDouble("drc.PMTT")*mm;

//
//        config.getVectorDouble("drc.shellThick",_shellThick,_nShell);
//
//        _CDCHnSrvShell  = 0;

//        loadTCounter(config);

        cout<<"------------------- DRFPIC -------------------"<<endl;
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

DRFPICMaker::~DRFPICMaker (){}

//void DRFPIMaker::loadTCounter( crd::SimpleConfig const& config ){
//
//    config.getVectorDouble("drc.scintThick",_scintThick,_nShell);
//    _fillScintMaterial = config.getString("drc.scintMat");
//
//}

void DRFPICMaker::Build(){

  _ldrfpic = unique_ptr<DRFPIcalorimeter>(new DRFPIcalorimeter());
  _ldrfpic->_isExternal = _isExternal;
  _ldrfpic->_geomType=_geomType;

  int nTower   = 2*_NbOfBarrel+2*_NbOfEndcap;

  if (nTower<1) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"DRFPIC: Number of Tower is not acceptable\n";
    e.error();
  }

  if (_isExternal) {
    //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"DRFPIC: Using GDML file option is temporarily disabled\n";
    e.error();

  } else {

    _ldrfpic->_nTower = nTower;
//    _ldrfpic->_zhalfLength = _halfLength;
    _ldrfpic->_z0 = _z0;

	_ldrfpic->_innerR =_innerR;
	_ldrfpic->_tower_height = _tower_height; 
	_ldrfpic->_NbOfBarrel = _NbOfBarrel;
	_DltNbB_E = _NbOfBarrel-_NbOfEndcap;
	_ldrfpic->_DltNbB_E = _DltNbB_E;
	_ldrfpic->_NbOfEndcap = _NbOfEndcap;
	_ldrfpic->_NbOfZRot = _NbOfZRot;
	_ldrfpic->_PMTT = _PMTT;
	_ldrfpic->_outerR = ceil( (_innerR + _tower_height)/cos(M_PI/(double)_NbOfZRot) );


    // Master geometry for the CDCHtracker.
//    GeomHandle<drc::DRFPIcalorimeter> drfpicalo;
//    if (_CDCHnSrvShell!=_nTower) {
//      exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//      e<<"DRFPI: nShell differs from n. of CDCH Service Shells \n";
//      e.error();
//    }

    Tower *_tower     = new Tower[nTower];
//    boost::shared_ptr<SLayer> lay;

    double thetaofcenter=0.;
    double fulltheta=0.;
    G4ThreeVector pt[8];
    double thetaE = 0;

//    char name[20];

    //Inizialise for Barrel R
    double *deltatheta_barrel= new double[_NbOfBarrel];
    dimB = new dimensionB();
    dimB->SetInnerR(_innerR);
    dimB->SetTower_height(_tower_height);
    dimB->SetNumZRot(_NbOfZRot);
    dimB->SetPMTT(_PMTT);
          
    // barrel R
    G4cout << "Barrel R..." << G4endl;
    dimB->Rbool(1);
    for(int i=0;i<_NbOfBarrel;i++){    //i<NbOfBarrel		
		deltatheta_barrel[i] = M_PI/4/(_NbOfBarrel);	
    	thetaofcenter=fulltheta+deltatheta_barrel[i]/2.;
    	dimB->SetDeltaTheta(deltatheta_barrel[i]);
    	dimB->SetThetaOfCenter(thetaofcenter);
    	dimB->CalBasic();
    	dimB->Getpt(pt);
    	CLHEP::Hep3Vector c = dimB->GetOrigin(0);

		_tower[i]._towerType=Tower::barrel;
 		_tower[i]._towerSide=Tower::right;
		_tower[i]._deltatheta=deltatheta_barrel[i];
		_tower[i]._thetaofcenter=thetaofcenter;
		for (int j=0; j<8; j++) _tower[i]._pt[j]=pt[j];
    	_tower[i]._rotM.rotateX(-thetaofcenter);  	
    	_tower[i]._center.set(c.getY(),-c.getZ(),c.getX()-(_innerR+0.5*_tower_height));
		
    	_tower[i]._v[0] = dimB->GetV1();
    	_tower[i]._v[1] = dimB->GetV2();
    	_tower[i]._v[2] = dimB->GetV3();
    	_tower[i]._v[3] = dimB->GetV4();
    	_tower[i]._innerR_new = dimB->GetInnerR_new();

    	fulltheta = fulltheta+deltatheta_barrel[i];	
    }
    _ldrfpic->_zInHalfLength = floor(_innerR*tan(fulltheta));
    _ldrfpic->_zhalfLength = _ldrfpic->_zInHalfLength +_tower_height+1.0; //1.0mm to avoid overlap due to approximation (floor)

    // barrel L
    G4cout << "Barrel L..." << G4endl;
    dimB->Rbool(0);
    thetaofcenter=0;
    fulltheta=0;

    int k=_NbOfBarrel;
    for(int i=0;i<_NbOfBarrel;i++){  //i<NbOfBarrel
    	thetaofcenter=fulltheta+deltatheta_barrel[i]/2.;
    	dimB->SetDeltaTheta(deltatheta_barrel[i]);
    	dimB->SetThetaOfCenter(thetaofcenter);
    	dimB->CalBasic();
    	dimB->Getpt(pt);
    	CLHEP::Hep3Vector c = dimB->GetOrigin(0);

 		_tower[k]._towerType=Tower::barrel;
 		_tower[k]._towerSide=Tower::left;
		_tower[k]._deltatheta=deltatheta_barrel[i];
		_tower[k]._thetaofcenter=thetaofcenter;
		for (int j=0; j<8; j++) _tower[k]._pt[j]=pt[j];
    	_tower[k]._rotM.rotateX(thetaofcenter);  	
     	_tower[k]._center.set(c.getY(),-c.getZ(),c.getX()-(_innerR+0.5*_tower_height));

    	_tower[k]._v[0] = dimB->GetV1();
    	_tower[k]._v[1] = dimB->GetV2();
    	_tower[k]._v[2] = dimB->GetV3();
    	_tower[k]._v[3] = dimB->GetV4();
    	_tower[k]._innerR_new = dimB->GetInnerR_new();
   	
    	fulltheta = fulltheta+deltatheta_barrel[i];
		++k;
    }
    	
    	
    // endcap R
    G4cout << "Endcap R..." << G4endl;
    double *deltatheta_endcap= new double[_NbOfEndcap];
    dimE = new dimensionE();
    dimE->SetInnerR(_innerR);
    dimE->SetTower_height(_tower_height);
    dimE->SetNumZRot(_NbOfZRot);
    dimE->SetPMTT(_PMTT);

    dimE->Rbool(1);
    thetaofcenter=0;
    double thetaofcenter2=0.;
    //for(int i=0;i<_NbOfEndcap+_DltNbB_E;i++) thetaE += deltatheta_endcap[i];
    //fulltheta = thetaE;
	thetaE = fulltheta; //nel codice org. fulltheta = thetaE; ma thetaE è uguale a fulltheta dopo il ciclo sul barrel
        
    k=2*_NbOfBarrel;
    for(int i=0;i<_NbOfEndcap;i++){//NbofEndcap-1
        deltatheta_endcap[i] = M_PI/4/(_NbOfEndcap+_DltNbB_E);
    	thetaofcenter=fulltheta-deltatheta_endcap[i]/2.;
        thetaofcenter2=thetaofcenter-deltatheta_endcap[i]/2.-deltatheta_endcap[i+1]/2.;        
        dimE->SetDeltaTheta(deltatheta_endcap[i]);
    	dimE->SetThetaOfCenter(thetaofcenter);
        dimE->SetDeltaTheta2(deltatheta_endcap[i+1]);
    	dimE->SetThetaOfCenter2(thetaofcenter2);
    	dimE->CalBasic();
    	dimE->Getpt(pt);
    	
    	CLHEP::Hep3Vector c = dimE->GetOrigin(0);

 		_tower[k]._towerType=Tower::endcap;
 		_tower[k]._towerSide=Tower::right;
		_tower[k]._deltatheta=deltatheta_endcap[i];
		_tower[k]._thetaofcenter=thetaofcenter;
		for (int j=0; j<8; j++) _tower[k]._pt[j]=pt[j];
    	_tower[k]._rotM.rotateX(thetaofcenter);  	
     	_tower[k]._center.set(-c.getY(),c.getZ(),c.getX()-(_innerR+0.5*_tower_height));

    	_tower[k]._v[0] = dimE->GetV1();
    	_tower[k]._v[1] = dimE->GetV2();
    	_tower[k]._v[2] = dimE->GetV3();
    	_tower[k]._v[3] = dimE->GetV4();
    	_tower[k]._innerR_new = dimE->GetInnerR_new();

        fulltheta = fulltheta-deltatheta_endcap[i];
		++k;
	}
    _ldrfpic->_endc_holeAng=fulltheta;
    	
    // endcap L
    G4cout << "Endcap L..." << G4endl;
    dimE->Rbool(0);
    thetaofcenter=0;
    thetaofcenter2=0;
    fulltheta = thetaE;
    
	k=2*_NbOfBarrel+_NbOfEndcap;
    for(int i=0;i<_NbOfEndcap;i++){
    	thetaofcenter=fulltheta-deltatheta_endcap[i]/2.;
        thetaofcenter2=thetaofcenter-deltatheta_endcap[i]/2.-deltatheta_endcap[i+1]/2.;
        dimE->SetDeltaTheta(deltatheta_endcap[i]);
    	dimE->SetThetaOfCenter(thetaofcenter);
        dimE->SetDeltaTheta2(deltatheta_endcap[i+1]);
    	dimE->SetThetaOfCenter2(thetaofcenter2);
    	dimE->CalBasic();
    	dimE->Getpt(pt);
    	
    	CLHEP::Hep3Vector c = dimE->GetOrigin(0);

 		_tower[k]._towerType=Tower::endcap;
 		_tower[k]._towerSide=Tower::left;
		_tower[k]._deltatheta=deltatheta_endcap[i];
		_tower[k]._thetaofcenter=thetaofcenter;
		for (int j=0; j<8; j++) _tower[k]._pt[j]=pt[j];
    	_tower[k]._rotM.rotateX(thetaofcenter);  	
     	_tower[k]._center.set(c.getY(),-c.getZ(),c.getX()-(_innerR+0.5*_tower_height));

    	_tower[k]._v[0] = dimE->GetV1();
    	_tower[k]._v[1] = dimE->GetV2();
    	_tower[k]._v[2] = dimE->GetV3();
    	_tower[k]._v[3] = dimE->GetV4();
    	_tower[k]._innerR_new = dimE->GetInnerR_new();

        fulltheta = fulltheta-deltatheta_endcap[i];
		++k;
    }
    
    _ldrfpic->_tower.reset(_tower);
  }
}

} // namespace drc

#endif
