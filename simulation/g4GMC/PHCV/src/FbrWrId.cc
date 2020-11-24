//
//
// Original author G. Tassielli
//

#include "FbrWrId.hh"

#include "SLayerId.hh"

namespace phcv {

  FbrWrId::FbrWrId():
    _lid(nullptr),
    _n(-1){
  }

  FbrWrId::FbrWrId( SLayerId *layer,
           int n
           ):
    _lid(layer),
    _n(n){
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const SLayerId& FbrWrId::getSLayerId() const {
    return *_lid;
  }

  int FbrWrId::getSLayer() const{
    return _lid->getSLayer();
  }

  int FbrWrId::getFbrWr() const{
    return _n;
  }

  int FbrWrId::getUId() const{
	int wUid = ((_lid->getShell()+1)*100000)+((_lid->getSLayer()+1)*1000);//Fix when use real fibers and wires
	wUid*=10;
	wUid+=_n;
    return wUid;
  }

  bool FbrWrId::operator==(const FbrWrId w) const{
    return ( *_lid == *(w._lid) && _n == w._n );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const FbrWrId& w ){
    ost << "FbrWr Id: ("
        << w.getSLayerId() << " "
        << w._n
        << " )";
    return ost;
  }

}
