// DR 4PI calorimeter manager class
//
// Original author G. Tassielli
//

#include "DRFPIcalorimeter.hh"

#include "Tower.hh"
#include "manageException.hh"

using namespace std;

namespace drc {

DRFPIcalorimeter::DRFPIcalorimeter() : Detector("DRFPIcalorimeter") {
	_z0=0.0;
	_extFile         ="";
    _isExternal      =false;
	_nTower=0;
	_zhalfLength=0.0;
    _geomType        =00;

	_innerR=0.0;
	_tower_height=0.0; 
	_NbOfBarrel=0;
	_DltNbB_E=0;
	_NbOfEndcap=0;
	_NbOfZRot=0;
	_PMTT=0.0;

}

//ROGeometryHandle* PHCVtcounter::getROGeometryHandle() const {
//  return _roChhnd.get();
//}
//

Tower* DRFPIcalorimeter::getTower(int n) const /*throw(cet::exception)*/ {
        if (n<0 || n>= _nTower){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Tower number: "<< n <<" not present";
          e.error();
        }
        return &(_tower[n]);
}

//double DRFPIcalorimeter::getShellThick(int n) const /*throw(cet::exception)*/ {
////        if (n>=0 && n< _nSuperLayers){
////                return &(_sprlr[n]);
////        } else {
////          throw cet::exception("GEOM")<< "Super Layer number: "<< n <<" not present";
//        if (n<0 || n>= _nShell){
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<<"PHCV Shell n: "<< n <<" not present";
//          e.error();
//        }
//        return _shellThick[n];
//}
//
//double DRFPIcalorimeter::getShellRadIn(int n) const {
//        if (n<0 || n>= _nShell){
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<<"PHCV radiator n: "<< n <<" not present";
//          e.error();
//        }
//        return _shellRadIn[n];
//}
//
//double DRFPIcalorimeter::getRadThick(int n) const {
//        if (n<0 || n>= _nShell){
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<<"PHCV radiator n: "<< n <<" not present";
//          e.error();
//        }
//        return _radThick[n];
//}
//
//double DRFPIcalorimeter::getScintThick(int n) const {
//        if (n<0 || n>= _nShell){
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<<"PHCV scintillator n: "<< n <<" not present";
//          e.error();
//        }
//        return _scintThick[n];
//}

} // namespace drc
