//
// Original author G. Tassielli
//

#include "LadderId.hh"
#include "LayerId.hh"

namespace svx {

  LadderId::LadderId():
    _lyid(nullptr),
    _ldid(-1)
  {
  }

  LadderId::LadderId( LayerId *lyid,
             long ldid
             ):
    _lyid(lyid),
    _ldid(ldid)
  {
  }

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const LayerId& LadderId::getLayerId() const {
    return *_lyid;
  }

  int LadderId::getLayer() const {
    return _lyid->getLayer();
  }

  long LadderId::getLadder() const{
    return _ldid;
  }

  bool LadderId::operator==(const LadderId& l) const{
    return ( *_lyid == *(l._lyid) && _ldid == l._ldid );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const LadderId& l ){
    ost << "Ladder Id: ("
        << *l._lyid << " "
        << l._ldid
        << " )";
    return ost;
  }

}
