//
// Original author G. Tassielli
//

#ifndef LayerId_hh
#define LayerId_hh

//#include "SuperLayerId.hh"
#include <iostream>

namespace cdch {

class SuperLayerId;
class LayerId;

std::ostream& operator<<(std::ostream& ost,
                                const LayerId& l );

class LayerId{

        friend class SuperLayerId;

public:

  LayerId();

  LayerId( SuperLayerId *sid, int id );

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const SuperLayerId& getSuperLayerId() const;

  int getSuperLayer() const;

  int getLayer() const;

  bool operator==(const LayerId& l) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const LayerId& l );

private:

  SuperLayerId *_sid;
  int _id;

};

}
#endif /* LayerId_hh */
