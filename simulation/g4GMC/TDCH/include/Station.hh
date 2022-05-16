// TDCH station description
//
// Original author G. Tassielli
//

#ifndef Station_hh
#define Station_hh

//#include <Plane.hh>
#include <deque>
#include <vector>

#include "manageException.hh"

#include <boost/shared_ptr.hpp>
#include "StationId.hh"
#include "StationInfo.hh"

namespace tdch {

class Plane;
class PlaneId;

class Station{

  friend class Plane;
  friend class TDCHtracker;
  friend class TDCHMaker;

public:

  // A free function, returning void, that takes a const Plane& as an argument.
  typedef void (*StationFunction)( const Station& s);

  Station();

  Station(StationId& id);

  Station(StationId&   id,
                  std::vector< boost::shared_ptr<Plane> > &plane);

  Station( int &id);

  Station( int &id,
                  std::vector< boost::shared_ptr<Plane> > &plane);

  ~Station ();

  const StationId& Id() const { return _id;}

  int nPlanes() const { return _nPlanes; }

  boost::shared_ptr<Plane> getMeasrPlane( int n ) const;

  boost::shared_ptr<Plane> getPlane( int n ) const;
//  {
//    //if (n>=0 && n<_nPlanes) return _planes.at(n);
//    //else { //throw  cet::exception("GEOM")<< "Plane number: "<< n <<" not present in "<<_id;
//    if (n<0 || n>=_nPlanes) {
//      exc::exceptionG4 e("GEOM","Fatal Error",0);
//      e<< "Plane number: "<< n <<" not present in "<<_id;
//      e.error();
//    }
//    return _planes.at(n);
//  }

  boost::shared_ptr<Plane> getPlane( PlaneId& id ) const;
//  {
//    return getPlane(id.getPlane());
//  }

  const std::vector< boost::shared_ptr<Plane> >& getPlanes() const;
//  {
//    return _planes;
//  }

//  Station& operator=(const Station &sl) {
//          if (this!=&sl) {
//                  _id = sl.Id();
//                  _nPlanes = sl.nPlanes();
//                  _planes = sl.
//          }
//  }

protected:

  StationId _id;

  int _nPlanes;

  std::vector< boost::shared_ptr<Plane> > _planes;
  std::vector<int> _msrLayPos;

  void addPlane(Plane *tdchpl, bool isMsrPl=false);
//  {
//          _planes.push_back(boost::shared_ptr<Plane>(tdchpl));
//          _nPlanes++;
//  }

};
}

#endif /* Station_hh */
