// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include "manageException.hh"

// CDCH includes
#include "Cell.hh"

#ifndef __CINT__

using CLHEP::Hep3Vector;

namespace cdch {

Cell::Cell():
  _id(CellId()),
  _detailIndex(0)
{
}

Cell::Cell( CellId id,
            boost::shared_ptr<CellDetail> detail,
            boost::shared_ptr<Wire> senseWire
            ):
  _id(id),
  _detail(detail),
  _senseWire(senseWire)
{
        _tmpMidPoint = _senseWire.get()->getMidPoint();
        _tmpDirection = _senseWire.get()->getDirection();
}

Cell::~Cell (){
//         try {
//                  delete _detail;
//                  delete _senseWire;
//         } catch (cet::exception e) {
//             throw cet::exception("GEOM")
//                  << "Error during deleting cell data \n";
//         }
}

boost::shared_ptr<Wire> Cell::getFWire(int iFw) const {
//        if (iFw>=0 && iFw<(int)_fieldWires.size()) {
//                return  _fieldWires.at(iFw);
//        }
//        else {
//          throw cet::exception("GEOM")<< "Field wire number: "<< iFw <<" not present in Cell"<<_id;
        if (iFw<0 || iFw>=(int)_fieldWires.size()) {
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<< "Field wire number: "<< iFw <<" not present in Cell"<<_id;
          e.error();
        }
        return  _fieldWires.at(iFw);
}

const CLHEP::Hep3Vector& Cell::getMidPoint()  const {
        //if (_tmpMidPoint.mag()==0.0) { _tmpMidPoint = _senseWire.get()->getMidPoint(); }
        return _tmpMidPoint;
}

const CLHEP::Hep3Vector& Cell::getDirection() const {
        //if (_tmpDirection.mag()==0.0) { _tmpDirection = _senseWire.get()->getDirection(); }
        return _tmpDirection;
}

} // namespace cdch

#endif
