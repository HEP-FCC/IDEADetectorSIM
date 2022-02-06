// TDCH tracker manager class
//
// Original author G. Tassielli
//

#include "TDCHtracker.hh"
#include "Station.hh"
//#include "Wall.hh"

#include "manageException.hh"

using namespace std;

namespace tdch {

TDCHtracker::TDCHtracker() : Detector("TDCHtracker") {
        _r0              =0.0;
        _z0              =0.0;
        _nSWire          =0;
//        _nSDeltaWire     =0;
        _nLayer          =0;
        _nStations       =0;
        _nPlane          =0;
        _nMeasPlane      =0;
        _nSrvPlane       =0;
        _nPanel          =0;
        _nWalls          =0;
        _rOut            =0.0;
        _extFile         ="";
        _isExternal      =false;
        _zHalfLength     =0.0;
        _max_EndCap_dim  =0.0;
        _geomType        =30;
        _cellType        =TDCHtracker::Square;
        _endcapType      =TDCHtracker::Plane;
        _hasWalls        =false;
        _displayGasLayer =false;
        _displayWires    =false;
        _lastSeenWall    =-1;
}

Station* TDCHtracker::getStation(int n) const /*throw(cet::exception)*/ {
//        if (n>=0 && n< _nStations){
//                return &(_sprlr[n]);
//        } else {
//          throw cet::exception("GEOM")<< "Super Layer number: "<< n <<" not present";
        if (n<0 || n>= _nStations){
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"Station number: "<< n <<" not present";
          e.error();
        }
        return &(_station[n]);
}

//        const boost::shared_ptr<Wall> TDCHtracker::getWall(int n) throw(cet::exception) {
//                if (n>=0 && n< _nWalls){
//                        //_walls_it --;//+= (n-_lastSeenWall);
//                        advance (_walls_it,n-_lastSeenWall);
//                        _lastSeenWall = n;
//                        return _walls_it->second;
//                }
//                else throw cet::exception("GEOM")<< "Wall number: "<< n <<" not present";
//        }

void TDCHtracker::addWall(cdch::Wall *wall){
        if (_nWalls==0) _walls.reset(new std::multimap<cdch::Wall::Walltype,boost::shared_ptr<cdch::Wall> >() );
        _walls->insert(std::pair<cdch::Wall::Walltype,boost::shared_ptr<cdch::Wall> >(wall->getType(),boost::shared_ptr<cdch::Wall>(wall)) );
        _nWalls++;
        if (_nWalls==1){
                _walls_it=_walls->begin();
                _lastSeenWall=0;
        }
}

} // namespace tdch
