#ifndef DRFPICscntSD_hh
#define DRFPICscntSD_hh
//
// Defines a generic DRFPIcalorimeter sensitive detector
//
// Original author G. Tassielli
//

// DRFPI includes
#include <SensitiveDetector.hh>
#include "DRFPIcalorimeter.hh"
#include "GeomHandle.hh"

namespace drc {

  class DRFPICscntSD : public SensitiveDetector {

  public:
    DRFPICscntSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~DRFPICscntSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setDRFPICenterInDetSys(const G4ThreeVector &origin) {
            _drfpicDetCenter = origin + _systOrigin;
    }

  protected:

    int _tower;
//    int _phiSec;
//    int _ladder;
//    int _roSubShell;
//    unsigned long _roIdFstSide;
//    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<DRFPIcalorimeter> drfpic;

    // DRFPI point of origin in ref. System
    G4ThreeVector _drfpicDetCenter;

  };

} // namespace drc

#endif /* DRFPICscntSD_hh */
