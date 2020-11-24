//
// Original author G. Tassielli
//

#include "TowerId.hh"

namespace drc {

  TowerId::TowerId():
    _id(-1)
  {
  }

  TowerId::TowerId( int id ):
    _id(id)
  {
  }

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  int TowerId::getTower() const{
    return _id;
  }

  bool TowerId::operator==(const TowerId& l) const{
    return ( _id == l._id );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const TowerId& l ){
    ost << "Tower Id: ("
        << l._id
        << " )";
    return ost;
  }

}
