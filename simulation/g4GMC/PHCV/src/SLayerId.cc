//
// Original author G. Tassielli
//

#include "SLayerId.hh"
#include "ShellId.hh"

namespace phcv {

  SLayerId::SLayerId():
    _sid(),
    _id(-1)
  {
  }

  SLayerId::SLayerId( ShellId *sid,
             int id
             ):
    _sid(sid),
    _id(id)
  {
  }

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const ShellId& SLayerId::getShellId() const {
    return *_sid;
  }

  int SLayerId::getShell() const {
    return _sid->getShell();
  }

  int SLayerId::getSLayer() const{
    return _id;
  }

  bool SLayerId::operator==(const SLayerId& l) const{
    return ( *_sid == *(l._sid) && _id == l._id );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const SLayerId& l ){
    ost << "Layer Id: ("
        << l.getShellId() << " "
        << l._id
        << " )";
    return ost;
  }

}
