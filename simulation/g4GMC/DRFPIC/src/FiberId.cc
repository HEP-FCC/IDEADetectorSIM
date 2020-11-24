//
//
// Original author G. Tassielli
//

#include "FiberId.hh"

#include "TowerId.hh"

namespace drc {

  FiberId::FiberId():
    _lid(nullptr),
    _n(-1){
  }

  FiberId::FiberId( TowerId *layer,
           int n
           ):
    _lid(layer),
    _n(n){
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const TowerId& FiberId::getTowerId() const {
    return *_lid;
  }

  int FiberId::getTower() const{
    return _lid->getTower();
  }

  int FiberId::getFiber() const{
    return _n;
  }

  int FiberId::getUId() const{
	int fUid = (_lid->getTower()+1)*10000;//Fix when use real fibers
//	fUid*=10;
	fUid+=_n;
    return fUid;
  }

  bool FiberId::operator==(const FiberId w) const{
    return ( *_lid == *(w._lid) && _n == w._n );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const FiberId& w ){
    ost << "FbrWr Id: ("
        << w.getTowerId() << " "
        << w._n
        << " )";
    return ost;
  }

}
