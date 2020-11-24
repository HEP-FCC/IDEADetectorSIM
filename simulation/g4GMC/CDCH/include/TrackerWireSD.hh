#ifndef TrackerWireSD_hh
#define TrackerWireSD_hh
//
// Defines a generic Tracker wire sensitive detector
//
// Original author G. Tassielli
//

// CDCH includes
#include "SensitiveDetector.hh"

#include "SimpleConfig.hh"

namespace cdch {

  class TrackerWireSD : public SensitiveDetector {

  public:
    TrackerWireSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~TrackerWireSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setDCHCenterInDetSys(const G4ThreeVector &origin) {
      _cdchDetCenter = origin + _systOrigin;
    }

  protected:

    // CDCH point of origin in ref. System
    G4ThreeVector _cdchDetCenter;

  };

} // namespace cdch

#endif /* TrackerWireSD_hh */
