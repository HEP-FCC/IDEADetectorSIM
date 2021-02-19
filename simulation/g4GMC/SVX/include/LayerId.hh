//
// $Id: LayerId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#ifndef SVX_LayerId_hh
#define SVX_LayerId_hh

#include <iostream>

namespace svx {

  // class LayerId;
  // std::ostream& operator<<(std::ostream& ost,
  //                                  const LayerId& s );

class LayerId{

public:

  LayerId();

  LayerId( int id );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int getLayer() const;

  bool operator==(const LayerId s) const;

  // friend std::ostream& operator<<(std::ostream& ost,
  //                                  const LayerId& s );

private:

  int _id;

};

  inline std::ostream& operator<<(std::ostream& ost,
				  const LayerId& s ){
    ost << "Layer Id: "<<s.getLayer() << " ";
    return ost;
  }

} //namespace svx

#endif /* SVX_LayerId_hh */
