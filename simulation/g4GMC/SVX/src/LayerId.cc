//
// $Id: LayerId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#include "LayerId.hh"

namespace svx {

  LayerId::LayerId():
    _id(-1){
  }

  LayerId::LayerId( int id ):
    _id(id)
  {
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int LayerId::getLayer() const{
    return _id;
  }

  bool LayerId::operator==(const LayerId s) const{
    return ( _id == s._id );
  }

//  std::ostream& operator<<(std::ostream& ost,
//                                  const LayerId& s ){
//    ost << "Layer Id: "<<s._id << " ";
//    return ost;
//  }

} //namespace svx
