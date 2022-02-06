//
//
// Original author G. Tassielli
//

#include "PanelId.hh"

#include "PlaneId.hh"

namespace tdch {

  PanelId::PanelId():
    _plid(nullptr),
    _n(-1){
  }

  PanelId::PanelId( PlaneId *plane,
           int n
           ):
    _plid(plane),
    _n(n){
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const PlaneId& PanelId::getPlaneId() const {
    return *_plid;
  }

  int PanelId::getPlane() const{
    return _plid->getPlane();
  }

  int PanelId::getPanel() const{
    return _n;
  }

  int PanelId::getUId() const{
	int pUid = _plid->getUId();
	pUid+=(_n+1)*1000;
	  // w 1000 in pa
	  // pan 10 in pl
	  // pl 100 in St
    return pUid;
  }

  bool PanelId::operator==(const PanelId pn) const{
    return ( *_plid == *(pn._plid) && _n == pn._n );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const PanelId& pn ){
    ost << "Panel Id: ("
        << pn.getPlaneId() << " "
        << pn._n
        << " )";
    return ost;
  }

}
