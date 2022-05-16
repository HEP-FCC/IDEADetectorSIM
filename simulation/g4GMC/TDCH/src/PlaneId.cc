//
// Original author G. Tassielli
//

#include "PlaneId.hh"
#include "StationId.hh"

namespace tdch {

  PlaneId::PlaneId():
	_stid(nullptr)
    ,_id(-1)
  {
  }

  PlaneId::PlaneId( StationId *station, int id ):
    _stid(station)
    ,_id(id)
  {
  }

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const StationId& PlaneId::getStationId() const {
    return *_stid;
  }

  int PlaneId::getStation() const{
    return _stid->getStation();
  }

  int PlaneId::getPlane() const{
    return _id;
  }

  int PlaneId::getUId() const {
	  int pUid = _stid->getUId();
	  pUid+=(_id+1)*10000;
	  // w 1000 in pa
	  // pan 10 in pl
	  // pl 100 in St
	  return pUid;
  }

  bool PlaneId::operator==(const PlaneId& pl) const{
    return ( *_stid == *(pl._stid) && _id == pl._id );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const PlaneId& pl ){
    ost << "Plane Id: ("
        << pl.getStationId() << " "
        << pl._id
        << " )";
    return ost;
  }

}
