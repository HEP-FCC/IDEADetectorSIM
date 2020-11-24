//
// Original author G. Tassielli
//

#ifndef WireId_hh
#define WireId_hh

//#include <LayerId.hh>
#include <iostream>

namespace cdch {

class LayerId;
class WireId;

std::ostream& operator<<(std::ostream& ost,
                                const WireId& w );

class WireId{

        friend class LayerId;

public:

  WireId();

  WireId( LayerId *layer, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const LayerId& getLayerId() const;

  int getLayer() const;

  int getWire() const;

  int getUId() const;

  bool operator==(const WireId w) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const WireId& w );

private:

  LayerId *_lid;
  int _n;

};

}
#endif /* WireId_hh */
