//
// Original author G. Tassielli
//

#ifndef FbrWrId_hh
#define FbrWrId_hh

//#include <SLayerId.hh>
#include <iostream>

namespace phcv {

class SLayerId;
class FbrWrId;

std::ostream& operator<<(std::ostream& ost,
                                const FbrWrId& w );

class FbrWrId{

        friend class SLayerId;

public:

  FbrWrId();

  FbrWrId( SLayerId *layer, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const SLayerId& getSLayerId() const;

  int getSLayer() const;

  int getFbrWr() const;

  int getUId() const;

  bool operator==(const FbrWrId w) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const FbrWrId& w );

private:

  SLayerId *_lid;
  int _n;

};

}
#endif /* FbrWrId_hh */
