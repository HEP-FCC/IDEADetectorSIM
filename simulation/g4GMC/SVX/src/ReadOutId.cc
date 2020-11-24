//
// Original author G. Tassielli
//

#include "ReadOutId.hh"
#include <LadderId.hh>

namespace svx {

  ReadOutId::ReadOutId():
    _ldid(nullptr),
    _sid(0)
  {
  }

//  SensorId( SensorId &sid):
//    _sid(sid._sid)
//  {
//  }

  ReadOutId::ReadOutId( LadderId *ladder,
          unsigned long n
        ):
     _ldid(ladder),
    _sid(n)
  {
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const LadderId& ReadOutId::getLadderId() const {
    return *_ldid;
  }

  int ReadOutId::getLadder() const{
    return _ldid->getLadder();
  }

  unsigned long ReadOutId::getSensor() const{
    return _sid;
  }

  bool ReadOutId::operator==(const ReadOutId &c) const{
    return ( *_ldid==*(c._ldid) && _sid == c._sid );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const ReadOutId& c ){
    ost << "Sensor Id: ("
        << *c._ldid << " "
        << c._sid
        << " )";
    return ost;
  }

}
