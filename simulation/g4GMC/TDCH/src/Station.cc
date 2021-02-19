// CDCH superlayer description
//
// Original author G. Tassielli
//

#include "Station.hh"
#include "Plane.hh"

#ifndef __CINT__

using namespace std;

using CLHEP::Hep3Vector;

namespace tdch {

Station::Station():
                    _id(StationId()),
                    _nPlanes(0),
                    _planes(*(new std::vector< boost::shared_ptr<Plane> >(0)))
                    {
                    }

Station::Station(StationId& id):
                        _id(id),
                        _nPlanes(0),
                        _planes(*(new std::vector< boost::shared_ptr<Plane> >(0)))
                        {
                        }

Station::Station(StationId& id, std::vector< boost::shared_ptr<Plane> > &planes):
                        _id(id),
                        _planes(planes)
                        {
        _nPlanes = planes.size();
                        }

Station::Station( int& id):
                        _id(StationId(id)),
                        _nPlanes(0),
                        _planes(*(new std::vector< boost::shared_ptr<Plane> >(0)))
                        {
                        }

Station::Station( int& id, std::vector< boost::shared_ptr<Plane> > &planes):
                        _id(StationId(id)),
                        _planes(planes)
                        {
        _nPlanes = planes.size();
                        }

Station::~Station(){
        //         for ( std::vector<Plane*>::iterator j=_planes.begin(); j != _planes.end(); j++){
        //                 delete (*j);
        //         }
}

boost::shared_ptr<Plane> Station::getMeasrPlane( int n ) const {
	  if (n<0 || n>=_nPlanes) {
	    exc::exceptionG4 e("GEOM","Fatal Error",0);
	    e<< "Plane number: "<< n <<" not present in "<<_id;
	    e.error();
	  }
	  return _planes.at(_msrLayPos.at(n));
}

boost::shared_ptr<Plane> Station::getPlane( int n ) const {
  //if (n>=0 && n<_nPlanes) return _planes.at(n);
  //else { //throw  cet::exception("GEOM")<< "Plane number: "<< n <<" not present in "<<_id;
  if (n<0 || n>=_nPlanes) {
    exc::exceptionG4 e("GEOM","Fatal Error",0);
    e<< "Plane number: "<< n <<" not present in "<<_id;
    e.error();
  }
  return _planes.at(n);
}

boost::shared_ptr<Plane> Station::getPlane( PlaneId& id ) const {
  return getPlane(id.getPlane());
}

const std::vector< boost::shared_ptr<Plane> >& Station::getPlanes() const {
  return _planes;
}

void Station::addPlane(Plane *tdchpl, bool isMsrPl){
        _planes.push_back(boost::shared_ptr<Plane>(tdchpl));
        if (isMsrPl) {_msrLayPos.push_back(_nPlanes);}
        _nPlanes++;
}

} // namespace tdch

#endif

