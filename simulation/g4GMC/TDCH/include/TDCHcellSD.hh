#ifndef TDCHcellSD_hh
#define TDCHcellSD_hh
//
// Defines a generic TDCHtracker sensitive detector
//
// Original author G. Tassielli
//

// TDCH includes
#include <SensitiveDetector.hh>
#include "GeomHandle.hh"
#include "TDCHtracker.hh"

namespace tdch {

  class TDCHcellSD : public SensitiveDetector {

  public:
    TDCHcellSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~TDCHcellSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setTDCHCenterInDetSys(const G4ThreeVector &origin) {
            _tdchDetCenter = origin + _systOrigin;
    }

  protected:

    int _station;
//    int _phiSec;
//    int _ladder;
//    int _roSubShell;
//    unsigned long _roIdFstSide;
//    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<TDCHtracker> tdch;

    // TDCH point of origin in ref. System
    G4ThreeVector _tdchDetCenter;

  };

} // namespace tdch

#endif /* TDCHcellSD_hh */
