// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include "manageException.hh"

// SVX includes
#include "ReadOut.hh"

#ifndef __CINT__

using CLHEP::Hep3Vector;

namespace svx {


ReadOut::ReadOut () : _selId(1e10), _selSubShell(0), _selChFstSd(0), _selChSndSd(0){
}

//ReadOut::ReadOut():
//  _id(ReadOutId()),
//  _detailIndex(0)
//{
//}
//
//ReadOut::ReadOut( ReadOutId id,
//            boost::shared_ptr<ReadOutDetail> detail
//            ):
//  _id(id),
//  _detail(detail)
//{
////        _tmpMidPoint = _senseWire.get()->getMidPoint();
////        _tmpDirection = _senseWire.get()->getDirection();
//}

ReadOut::~ReadOut (){
}

const CLHEP::Hep3Vector& ReadOut::getMidPoint() const {
        //if (_tmpMidPoint.mag()==0.0) { _tmpMidPoint = _senseWire.get()->getMidPoint(); }
        return _tmpMidPoint;
}

//const CLHEP::Hep3Vector& ReadOut::getDirection() const {
//        //if (_tmpDirection.mag()==0.0) { _tmpDirection = _senseWire.get()->getDirection(); }
//        return _tmpDirection;
//}

bool ReadOut::selectChannel(unsigned long id) {
//  std::cout<<"id "<<id<<std::endl;
  if (id>=1e+10) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"Requested id exceed the maximum number of Readout Channels per Ladder allowed: 1e8 and 8 id for ladder sub shells \n";
    e.error();
  }
  bool changeCh=false;
  if (_selId==1e10 || _selId!=id) {
    _selId = id;
    _selSubShell = _selId/1e+8 -1;
    _selChFstSd = (_selId%100000000)/10000-1;
    _selChSndSd = _selId%10000;
    changeCh=true;
  }
  return changeCh;
}

} // namespace svx

#endif
