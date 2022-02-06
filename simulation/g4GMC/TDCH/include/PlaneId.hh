//
// Original author G. Tassielli
//

#ifndef PlaneId_hh
#define PlaneId_hh

#include <iostream>

namespace tdch {

class StationId;
class PlaneId;

std::ostream& operator<<(std::ostream& ost,
                                const PlaneId& pl );

class PlaneId{

public:

  PlaneId();

  PlaneId( StationId *station, int id );

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const StationId& getStationId() const;

  int getStation() const;

  int getPlane() const;

  int getUId() const;

  bool operator==(const PlaneId& pl) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const PlaneId& pl );

private:

  StationId *_stid;
  int _id;

};

}
#endif /* PlaneId_hh */
