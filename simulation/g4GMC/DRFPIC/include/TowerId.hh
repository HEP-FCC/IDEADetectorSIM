//
// Original author G. Tassielli
//

#ifndef TowerId_hh
#define TowerId_hh

#include <iostream>

namespace drc {

class TowerId;

std::ostream& operator<<(std::ostream& ost,
                                const TowerId& l );

class TowerId{

public:

  TowerId();

  TowerId( int id );

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  int getTower() const;

  bool operator==(const TowerId& l) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const TowerId& l );

private:

  int _id;

};

}
#endif /* TowerId_hh */
