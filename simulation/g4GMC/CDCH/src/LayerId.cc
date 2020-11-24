//
// Original author G. Tassielli
//

#include "LayerId.hh"
#include "SuperLayerId.hh"

namespace cdch {

  LayerId::LayerId():
    _sid(),
    _id(-1)
  {
  }

  LayerId::LayerId( SuperLayerId *sid,
             int id
             ):
    _sid(sid),
    _id(id)
  {
  }

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const SuperLayerId& LayerId::getSuperLayerId() const {
    return *_sid;
  }

  int LayerId::getSuperLayer() const {
    return _sid->getSuperLayer();
  }

  int LayerId::getLayer() const{
    return _id;
  }

  bool LayerId::operator==(const LayerId& l) const{
    return ( *_sid == *(l._sid) && _id == l._id );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const LayerId& l ){
    ost << "Layer Id: ("
        << l.getSuperLayerId() << " "
        << l._id
        << " )";
    return ost;
  }

}
