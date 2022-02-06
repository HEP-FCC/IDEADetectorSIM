//
// Original author G. Tassielli
//

#ifndef PanelId_hh
#define PanelId_hh

#include <iostream>

namespace tdch {

class PlaneId;
class PanelId;

std::ostream& operator<<(std::ostream& ost,
                                const PanelId& pn );

class PanelId{

        friend class PlaneId;

public:

  PanelId();

  PanelId( PlaneId *plane, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const PlaneId& getPlaneId() const;

  int getPlane() const;

  int getPanel() const;

  int getUId() const;

  bool operator==(const PanelId w) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const PanelId& pn );

private:

  PlaneId *_plid;
  int _n;

};

}
#endif /* PanelId_hh */
