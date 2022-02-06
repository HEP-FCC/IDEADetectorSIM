//
// Original author G. Tassielli
//

#include "CellId.hh"
#include "PanelId.hh"
#include "WireId.hh"

namespace tdch {

  CellId::CellId():
    _swid(nullptr)
  {
  }

  CellId::CellId( WireId &swid):
    _swid(&swid)
  {
  }

//  CellId::CellId( PanelId *layer,
//          int n
//        ):
//    _swid(new WireId(layer,n))
//  {
//  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const PanelId& CellId::getPanelId() const {
    return _swid->getPanelId();
  }

  int CellId::getPanel() const{
    return _swid->getPanel();
  }

  int CellId::getCell() const{
    return _swid->getWire();
  }

  bool CellId::operator==(const CellId c) const{
    return ( (*_swid) == (*(c._swid)) );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const CellId& c ){
    ost << "Cell Id: ("
        << c.getPanelId() << " "
        << c._swid->getWire()
        << " )";
    return ost;
  }

}
