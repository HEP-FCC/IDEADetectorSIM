//
// Original author G. Tassielli
//

#ifndef CellId_hh
#define CellId_hh

//#include "WireId.hh"
#include "LayerId.hh"
#include <iostream>

namespace cdch {

//class LayerId;
class WireId;
//class CellId;

class CellId{

  friend class LayerId;

public:

  CellId();

  CellId( WireId &swid);

//  CellId( LayerId *layer, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const LayerId& getLayerId() const;

  int getWireN() const;

  int getLayer() const;

  int getCell() const;

  bool operator==(const CellId c) const;

//  friend std::ostream& operator<<(std::ostream& ost,
//                                  const CellId& c );

private:

  WireId *_swid;

};

//std::ostream& operator<<(std::ostream& ost,
//                                const CellId& c );
inline std::ostream& operator<<(std::ostream& ost,
                                const CellId& c ){
  ost << "Cell Id: ("
      << c.getLayerId() << " "
      << c.getWireN()
      << " )";
  return ost;
}

}
#endif /* CellId_hh */
