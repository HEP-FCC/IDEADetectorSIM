// CDCH tracker manager class
//
// Original author G. Tassielli
//

#include "CDCHtracker.hh"
#include "SuperLayer.hh"
//#include "Wall.hh"

#include "manageException.hh"

using namespace std;

namespace cdch {

CDCHtracker::CDCHtracker() : Detector("CDCHtracker") {
        _r0              =0.0;
        _z0              =0.0;
        _nSWire          =0;
        _nSDeltaWire     =0;
        _nRing           =0;
        _isDumbbell      =false;
        _nOuterShell     =0;
        _rOut            =0.0;
        _extFile         ="";
        _isExternal      =false;
        _nSuperLayers    =0;
        _nWalls          =0;
        _zHalfLength     =0.0;
        _max_EndCap_dim  =0.0;
        _geomType        =30;
        _cellType        =CDCHtracker::Hexagonal;
        _endcapType      =CDCHtracker::Plane;
        _displayGasLayer =false;
        _displayWires    =false;
        _zZonesLimits.reset(new double[2]);
        _zZonesLimits.get()[0] =0.0;
        _zZonesLimits.get()[1] =0.0;
        _lastSeenWall    =-1;
}

SuperLayer* CDCHtracker::getSuperLayer(int n) const /*throw(cet::exception)*/ {
//        if (n>=0 && n< _nSuperLayers){
//                return &(_sprlr[n]);
//        } else {
//          throw cet::exception("GEOM")<< "Super Layer number: "<< n <<" not present";
        if (n<0 || n>= _nSuperLayers){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Super Layer number: "<< n <<" not present";
          e.error();
        }
        return &(_sprlr[n]);
}

//        const boost::shared_ptr<Wall> CDCHtracker::getWall(int n) throw(cet::exception) {
//                if (n>=0 && n< _nWalls){
//                        //_walls_it --;//+= (n-_lastSeenWall);
//                        advance (_walls_it,n-_lastSeenWall);
//                        _lastSeenWall = n;
//                        return _walls_it->second;
//                }
//                else throw cet::exception("GEOM")<< "Wall number: "<< n <<" not present";
//        }

void CDCHtracker::addWall(Wall *wall){
        if (_nWalls==0) _walls.reset(new std::multimap<Wall::Walltype,boost::shared_ptr<Wall> >() );
        _walls->insert(std::pair<Wall::Walltype,boost::shared_ptr<Wall> >(wall->getType(),boost::shared_ptr<Wall>(wall)) );
        _nWalls++;
        if (_nWalls==1){
                _walls_it=_walls->begin();
                _lastSeenWall=0;
        }
}

} // namespace cdch
