// MUSPC tracker manager class
//
// Original author G. Tassielli
//

#include "MUSPCtracker.hh"
#include "Layer.hh"
#include "ROGeometryHandle.hh"

#include "manageException.hh"

using namespace std;

namespace muspc {

MUSPCtracker::MUSPCtracker() : svx::SVXtracker("MUSPCtracker") {
//        _r0              =0.0;
//        _z0              =0.0;
//        _rOut            =0.0;
//        _extFile         ="";
//        _isExternal      =false;
//        _nLayers         =0;
//        _zHalfLength     =0.0;
//        _geomType        =00;
////        _sensType        =MUSPCtracker::Pixel;
}

//svx::ROGeometryHandle* MUSPCtracker::getROGeometryHandle() const {
//  return _roChhnd.get();
//}
//
//
//svx::Layer* MUSPCtracker::getLayer(int n) const /*throw(cet::exception)*/ {
////        if (n>=0 && n< _nSuperLayers){
////                return &(_sprlr[n]);
////        } else {
////          throw cet::exception("GEOM")<< "Super Layer number: "<< n <<" not present";
//        if (n<0 || n>= _nLayers){
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<<"Super Layer number: "<< n <<" not present";
//          e.error();
//        }
//        return &(_lr[n]);
//}

} // namespace muspc
