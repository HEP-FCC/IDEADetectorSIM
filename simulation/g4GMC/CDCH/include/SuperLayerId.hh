//
// $Id: SuperLayerId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#ifndef SuperLayerId_hh
#define SuperLayerId_hh

#include <iostream>

namespace cdch {

  class SuperLayerId;
  std::ostream& operator<<(std::ostream& ost,
                                  const SuperLayerId& s );

class SuperLayerId{

public:

  SuperLayerId();

  SuperLayerId( int id );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int getSuperLayer() const;

  bool operator==(const SuperLayerId s) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const SuperLayerId& s );

private:

  int _id;

};

} //namespace cdch

#endif /* SuperLayerId_hh */
