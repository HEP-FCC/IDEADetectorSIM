//
// $Id: ShellId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#ifndef ShellId_hh
#define ShellId_hh

#include <iostream>

namespace phcv {

  class ShellId;
  std::ostream& operator<<(std::ostream& ost,
                                  const ShellId& s );

class ShellId{

public:

  ShellId();

  ShellId( int id, int refId=-1 );

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int getShell() const;

  int getRefId() const;

  bool operator==(const ShellId s) const;

  friend std::ostream& operator<<(std::ostream& ost,
                                  const ShellId& s );

private:

  int _id;
  int _refId; //ID of CDCH service superlayer

};

} //namespace phcv

#endif /* ShellId_hh */
