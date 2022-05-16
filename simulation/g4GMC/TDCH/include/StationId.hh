//
// $Id: StationId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#ifndef StationId_hh
#define StationId_hh

#include <iostream>

namespace tdch {

  class StationId;
  std::ostream& operator<<(std::ostream& ost,
                                  const StationId& s );

class StationId{

public:

  StationId();

  StationId( int id );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int getStation() const;

  int getUId() const;

  bool operator==(const StationId s) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const StationId& s );

private:

  int _id;

};

} //namespace tdch

#endif /* StationId_hh */
