//
// Original author G. Tassielli
//

// GMCG4 includes
#include "GeomService.hh"
//#include "manageException.hh"

// CDCH includes
#include "CDCHGasLayerSD.hh"

#include "G4ThreeVector.hh"

//using namespace std;

namespace cdch {

//  G4ThreeVector CDCHGasLayerSD::_cdchDetCenter;

  CDCHGasLayerSD::CDCHGasLayerSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
                  SensitiveDetector(name,hitsCollectionName,config),
                  _superlayer(0),
                  _ring(0),
                  _nwires(0),
                  _Dphi(0)
  {
//          art::ServiceHandle<GeometryService> geom;
//
//          if ( !geom->hasElement<CDCHtracker>() ) {
//                  throw cet::exception("GEOM")
//                  << "Expected I Trackers but found neither.\n";
//          }

    if ( !GeomService::Instance()->hasElement<CDCHtracker>() ) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<< "Expected CDCH Trackers but found neither.\n";
      e.error();
    }
  }


  CDCHGasLayerSD::~CDCHGasLayerSD(){ }

} //namespace cdch
