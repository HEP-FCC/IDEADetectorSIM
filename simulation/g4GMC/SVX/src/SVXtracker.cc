// SVX tracker manager class
//
// Original author G. Tassielli
//

#include "SVXtracker.hh"
#include "Layer.hh"
#include "ROGeometryHandle.hh"

#include "manageException.hh"

using namespace std;

namespace svx {

SVXtracker::SVXtracker(std::string name) : Detector(name) {
        _r0              =0.0;
        _z0              =0.0;
        _rOut            =0.0;
        _extFile         ="";
        _isExternal      =false;
        _nLayers         =0;
        _nFwdLayers      =0;
        _zHalfLength     =0.0;
        _geomType        =00;
//        _sensType        =SVXtracker::Pixel;
}

ROGeometryHandle* SVXtracker::getROGeometryHandle() const {
  return _roChhnd.get();
}


Layer* SVXtracker::getLayer(int n) const /*throw(cet::exception)*/ {
//        if (n>=0 && n< _nSuperLayers){
//                return &(_sprlr[n]);
//        } else {
//          throw cet::exception("GEOM")<< "Super Layer number: "<< n <<" not present";
        if (n<0 || n>= _nLayers){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Super Layer number: "<< n <<" not present";
          e.error();
        }
        return &(_lr[n]);
}

} // namespace svx
