// TDCH cells description
//
// Original author G. Tassielli
//

#include "CellDetail.hh"

using namespace std;

namespace tdch {

  CellDetail::CellDetail( double circumscribedRadius, double inscribedCircleRadius, boost::shared_ptr<WireDetail> senseWire
                           ):
                           _circumscribedRadius(circumscribedRadius),
                           _inscribedCircleRadius(inscribedCircleRadius),
                           _senseWire(senseWire)
  {
  }

  CellDetail::~CellDetail (){
//                try {
//                        delete *_senseWire; *_senseWire=nullptr;
//                } catch (cet::exception e) {
//                    throw cet::exception("GEOM")
//                        << "Error during deleting cell wire detail data \n";
//                }

  }

} // namespace tdch
