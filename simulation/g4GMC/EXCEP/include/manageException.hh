/*
 * manageException.hh
 *
 *  Created on: Apr 3, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_EXCEP_INCLUDE_MANAGEEXCEPTION_HH_
#define G4GMC_EXCEP_INCLUDE_MANAGEEXCEPTION_HH_

//C++ include files
#include <sstream>
//
////G4 include files
//#include <globals.hh>

namespace exc {

//class exceptionG4;
//
//inline void error(const exceptionG4& er );

class exceptionG4 : public std::ostringstream {
public:
  exceptionG4(const char *excLocaName, const char *errType, int type);
  ~exceptionG4();
  void error();

//  friend void error(const exceptionG4& er ){
//    G4Exception(er._excLocaName.c_str(),er._errType.c_str(),
//        (G4ExceptionSeverity)er._type,
//        er.str().c_str());
//  }

private:
  std::string _excLocaName;
  std::string _errType;

  int _type;
};

//inline void exceptionG4(const char *excLocaName, const char *errType, int type, const char *message) {
//  G4Exception(excLocaName,errType,
//              static_cast<G4ExceptionSeverity>(type),
//              message);
//}

// example for the Mu2e framework style
//inline void thowexcept(const char *excLocaName="", const char *errType, const char *message) const throw(cet::exception) {
//  throw cet::exception(excLocaName)
//    << errType
//    << message <<std::endl;
//}

} //namescpa exc

#endif /* G4GMC_EXCEP_INCLUDE_MANAGEEXCEPTION_HH_ */
