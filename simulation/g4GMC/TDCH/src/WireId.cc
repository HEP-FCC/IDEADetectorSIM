//
//
// Original author G. Tassielli
//

#include  "WireId.hh"
#include <PanelId.hh>

namespace tdch {

  WireId::WireId():
    _pid(nullptr),
    _n(-1){
  }

  WireId::WireId( PanelId *panel,
           int n
           ):
    _pid(panel),
    _n(n){
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const PanelId& WireId::getPanelId() const {
    return *_pid;
  }

  int WireId::getPanel() const{
    return _pid->getPanel();
  }

  int WireId::getWire() const{
    return _n;
  }

  int WireId::getUId() const{
	int wUid = _pid->getUId();
	wUid+=_n;
	// w 1000 in pa
	// pan 10 in pl
	// pl 100 in St
    return wUid;
  }

  bool WireId::operator==(const WireId w) const{
    return ( *_pid == *(w._pid) && _n == w._n );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const WireId& w ){
    ost << "Wire Id: ("
        << w.getPanelId() << " "
        << w._n
        << " )";
    return ost;
  }

}
