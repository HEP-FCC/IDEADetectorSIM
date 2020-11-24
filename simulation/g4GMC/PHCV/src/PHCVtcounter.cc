// PHCV tcounter manager class
//
// Original author G. Tassielli
//

#include "PHCVtcounter.hh"
#include "Shell.hh"

#include "manageException.hh"

using namespace std;

namespace phcv {

PHCVtcounter::PHCVtcounter() : Detector("PHCVtcounter") {
	_z0=0.0;
	_extFile         ="";
    _isExternal      =false;
	_nShell=0;
	_zhalfLength=0.0;
    _geomType        =00;
}

//ROGeometryHandle* PHCVtcounter::getROGeometryHandle() const {
//  return _roChhnd.get();
//}
//

Shell* PHCVtcounter::getShell(int n) const /*throw(cet::exception)*/ {
        if (n<0 || n>= _nShell){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Super Shell number: "<< n <<" not present";
          e.error();
        }
        return &(_shell[n]);
}

double PHCVtcounter::getShellThick(int n) const /*throw(cet::exception)*/ {
//        if (n>=0 && n< _nSuperLayers){
//                return &(_sprlr[n]);
//        } else {
//          throw cet::exception("GEOM")<< "Super Layer number: "<< n <<" not present";
        if (n<0 || n>= _nShell){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"PHCV Shell n: "<< n <<" not present";
          e.error();
        }
        return _shellThick[n];
}

double PHCVtcounter::getShellRadIn(int n) const {
        if (n<0 || n>= _nShell){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"PHCV radiator n: "<< n <<" not present";
          e.error();
        }
        return _shellRadIn[n];
}

double PHCVtcounter::getRadThick(int n) const {
        if (n<0 || n>= _nShell){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"PHCV radiator n: "<< n <<" not present";
          e.error();
        }
        return _radThick[n];
}

double PHCVtcounter::getScintThick(int n) const {
        if (n<0 || n>= _nShell){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"PHCV scintillator n: "<< n <<" not present";
          e.error();
        }
        return _scintThick[n];
}

} // namespace phcv
