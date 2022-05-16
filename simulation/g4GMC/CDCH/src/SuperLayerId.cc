//
// $Id: SuperLayerId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#include "SuperLayerId.hh"

namespace cdch {

  SuperLayerId::SuperLayerId():
    _id(-1){
  }

  SuperLayerId::SuperLayerId( int id ):
    _id(id)
  {
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int SuperLayerId::getSuperLayer() const{
    return _id;
  }

  bool SuperLayerId::operator==(const SuperLayerId s) const{
    return ( _id == s._id );
  }

//  std::ostream& operator<<(std::ostream& ost,
//                                  const SuperLayerId& s ){
//    ost << "SuperLayer Id: "<<s.getSuperLayer() << " )";
//    return ost;
//  }

} //namespace cdch
