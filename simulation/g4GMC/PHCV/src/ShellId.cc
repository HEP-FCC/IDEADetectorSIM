//
// $Id: SuperLayerId.hh,v 1.6 2012/09/25 10:08:30 tassiell Exp $
// $Author: tassiell $
// $Date: 2012/09/25 10:08:30 $
//
// Original author G. Tassielli
//

#include "ShellId.hh"

namespace phcv {

  ShellId::ShellId():
    _id(-1),
	_refId(-1){
  }

  ShellId::ShellId( int id, int refId ):
    _id(id),
	_refId(refId)
  {
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  int ShellId::getShell() const{
    return _id;
  }

  int ShellId::getRefId() const{
    return _refId;
  }

  bool ShellId::operator==(const ShellId s) const{
    return ( _id == s._id );
  }

  std::ostream& operator<<(std::ostream& ost,
                                  const ShellId& s ){
    ost << "Shell Id: "<<s._id << " )";
    return ost;
  }

} //namespace phcv
