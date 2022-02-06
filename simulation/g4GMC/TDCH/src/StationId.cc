//
// $Id: StationId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#include "StationId.hh"

namespace tdch {

  StationId::StationId():
    _id(-1){
  }

  StationId::StationId( int id ):
    _id(id)
  {
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int StationId::getStation() const{
    return _id;
  }

  int StationId::getUId() const {
	  int sUid = (_id+1)*1000000;
	  // w 1000 in pa
	  // pan 10 in pl
	  // pl 100 in St
	  return sUid;
  }

  bool StationId::operator==(const StationId s) const{
    return ( _id == s._id );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const StationId& s ){
    ost << "Station Id: "<<s._id << " )";
    return ost;
  }

} //namespace tdch
