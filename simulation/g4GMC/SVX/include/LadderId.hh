//
// Original author G. Tassielli
//

#ifndef LadderId_hh
#define LadderId_hh

//#include <LayerId.hh>
#include <iostream>

namespace svx {

class LayerId;
class LadderId;

std::ostream& operator<<(std::ostream& ost,
                                const LadderId& l );

class LadderId{

        friend class LayerId;

public:

  LadderId();

  LadderId( LayerId *lyid, long ldid );

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const LayerId& getLayerId() const;

  int getLayer() const;

  long getLadder() const;

  bool operator==(const LadderId& l) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const LadderId& l );

private:

  LayerId *_lyid;
  long _ldid;

};

}
#endif /* LadderId_hh */
