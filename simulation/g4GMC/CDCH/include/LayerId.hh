//
// Original author G. Tassielli
//

#ifndef LayerId_hh
#define LayerId_hh

#include "SuperLayerId.hh"
#include <iostream>

namespace cdch {

//class SuperLayerId;
//class LayerId;

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

//  friend std::ostream& operator<<(std::ostream& ost,
//                                  const LayerId& l );

//  std::ostream& operator<<(std::ostream& ost) const;

private:

  SuperLayerId *_sid;
  int _id;

};

//std::ostream& operator<<(std::ostream& ost,
//                                const LayerId& l );
inline std::ostream& operator<<(std::ostream& ost,
                                const LayerId& l ){
  ost << "Layer Id: ("
      << l.getSuperLayerId() << " "
      << l.getLayer()
      << " )";
  return ost;
}

}
#endif /* LayerId_hh */
