#ifndef PSHWLadderSD_hh
#define PSHWLadderSD_hh
//
// Defines a generic PSHWtracker sensitive detector
//
// Original author G. Tassielli
//

// PSHW includes
#include <SensitiveDetector.hh>
#include "PSHWtracker.hh"

// GMCG4 includes
#include "GeomHandle.hh"

namespace pshw {

  class PSHWLadderSD : public SensitiveDetector {

  public:
    PSHWLadderSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~PSHWLadderSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setPSHWCenterInDetSys(const G4ThreeVector &origin) {
            _pshwDetCenter = origin + _systOrigin;
    }

  protected:

    int _layer;
    int _phiSec;
    int _ladder;
    int _roSubShell;
    unsigned long _roIdFstSide;
    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<PSHWtracker> pshwtracker;
    //PSHWtracker::GeomType _ittype;

    // PSHW point of origin in ref. System
    G4ThreeVector _pshwDetCenter;

  };

} // namespace pshw

#endif /* PSHWLadderSD_hh */
