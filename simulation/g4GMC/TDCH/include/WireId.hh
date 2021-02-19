//
// Original author G. Tassielli
//

#ifndef WireId_TDCH_hh
#define WireId_TDCH_hh

//#include <PanelId.hh>
#include <iostream>

namespace tdch {

class PanelId;
class WireId;

std::ostream& operator<<(std::ostream& ost,
                                const WireId& w );

class WireId{

        friend class PanelId;

public:

  WireId();

  WireId( PanelId *panel, int n );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  const PanelId& getPanelId() const;

  int getPanel() const;

  int getWire() const;

  int getUId() const;

  bool operator==(const WireId w) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const WireId& w );

private:

  PanelId *_pid;
  int _n;

};

}
#endif /* WireId_TDCH_hh */
