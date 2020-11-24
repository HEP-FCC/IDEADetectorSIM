/*
 * manageException.cc
 *
 *  Created on: Apr 3, 2017
 *      Author: tassiell
 */

#include "manageException.hh"

//G4 include files
#include <globals.hh>

namespace exc {

exceptionG4::exceptionG4(const char *excLocaName, const char *errType, int type) :
        _excLocaName(excLocaName),
        _errType(errType),
        _type(type) {}

exceptionG4::~exceptionG4() {}

void exceptionG4::error() {
  G4Exception(_excLocaName.c_str(),_errType.c_str(),
      static_cast<G4ExceptionSeverity>(_type),
      this->str().c_str());
}

} //namescpa exc
