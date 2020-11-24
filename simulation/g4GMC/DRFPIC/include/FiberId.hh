//
// Original author G. Tassielli
//

#ifndef FiberId_hh
#define FiberId_hh

#include <iostream>

namespace drc {

class TowerId;
class FiberId;

std::ostream& operator<<(std::ostream& ost,
                                const FiberId& w );

class FiberId{

        friend class TowerId;

public:

  FiberId();

  FiberId( TowerId *layer, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const TowerId& getTowerId() const;

  int getTower() const;

  int getFiber() const;

  int getUId() const;

  bool operator==(const FiberId w) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const FiberId& w );

private:

  TowerId *_lid;
  int _n;

};

}
#endif /* FiberId_hh */
