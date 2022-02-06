//
// Original author G. Tassielli
//

#ifndef CellId_TDCH_hh
#define CellId_TDCH_hh

//#include "WireId.hh"
#include <iostream>

namespace tdch {

class PanelId;
class WireId;
class CellId;

std::ostream& operator<<(std::ostream& ost,
                                const CellId& c );

class CellId{

  friend class PanelId;

public:

  CellId();

  CellId( WireId &swid);

//  CellId( PanelId *layer, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const PanelId& getPanelId() const;

  int getPanel() const;

  int getCell() const;

  bool operator==(const CellId c) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const CellId& c );

private:

  WireId *_swid;

};

}
#endif /* CellId_TDCH_hh */
