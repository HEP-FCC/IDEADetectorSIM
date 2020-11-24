#ifndef PHCVradSD_hh
#define PHCVradSD_hh
//
// Defines a generic PHCVtracker sensitive detector
//
// Original author G. Tassielli
//

// PHCV includes
#include <SensitiveDetector.hh>
#include "GeomHandle.hh"
#include "PHCVtcounter.hh"

namespace phcv {

  class PHCVradSD : public SensitiveDetector {

  public:
    PHCVradSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~PHCVradSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setPHCVCenterInDetSys(const G4ThreeVector &origin) {
            _phcvDetCenter = origin + _systOrigin;
    }

  protected:

    int _shell;
    int _layer;
//    int _phiSec;
//    int _ladder;
//    int _roSubShell;
//    unsigned long _roIdFstSide;
//    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<PHCVtcounter> phcvtc;

    // PHCV point of origin in ref. System
    G4ThreeVector _phcvDetCenter;

  };

} // namespace phcv

#endif /* PHCVradSD_hh */
