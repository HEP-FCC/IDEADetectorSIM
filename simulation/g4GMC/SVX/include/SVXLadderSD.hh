#ifndef SVXLadderSD_hh
#define SVXLadderSD_hh
//
// Defines a generic SVXtracker sensitive detector
//
// Original author G. Tassielli
//

// SVX includes
#include <SensitiveDetector.hh>
#include "SVXtracker.hh"

// GMCG4 includes
#include "GeomHandle.hh"

namespace svx {

  class SVXLadderSD : public SensitiveDetector {

  public:
    SVXLadderSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~SVXLadderSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setSVXCenterInDetSys(const G4ThreeVector &origin) {
            _svxDetCenter = origin + _systOrigin;
    }

  protected:

    int _layer;
    int _phiSec;
    int _ladder;
    int _roSubShell;
    unsigned long _roIdFstSide;
    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<SVXtracker> svxtracker;
    //SVXtracker::GeomType _ittype;

    // SVX point of origin in ref. System
    G4ThreeVector _svxDetCenter;

  };

} // namespace svx

#endif /* SVXLadderSD_hh */
