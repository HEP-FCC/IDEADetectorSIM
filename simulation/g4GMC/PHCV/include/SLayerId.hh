//
// Original author G. Tassielli
//

#ifndef SLayerId_hh
#define SLayerId_hh

//#include "SuperLayerId.hh"
#include <iostream>

namespace phcv {

class ShellId;
class SLayerId;

std::ostream& operator<<(std::ostream& ost,
                                const SLayerId& l );

class SLayerId{

        friend class ShellId;

public:

  SLayerId();

  SLayerId( ShellId *sid, int id );

  // Use compiler-generated copy c'tor, copy assignment, and d'tor

  const ShellId& getShellId() const;

  int getShell() const;

  int getSLayer() const;

  bool operator==(const SLayerId& l) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const SLayerId& l );

private:

  ShellId *_sid;
  int _id;

};

}
#endif /* SLayerId_hh */
