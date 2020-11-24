#ifndef CDCHGasLayerSD_hh
#define CDCHGasLayerSD_hh
//
// Defines a generic CDCHtracker sensitive detector
//
// Original author G. Tassielli
//

// CDCH includes
#include "SensitiveDetector.hh"
#include "CDCHtracker.hh"

// GMCG4 includes
#include "GeomHandle.hh"

namespace cdch {

  class CDCHGasLayerSD : public SensitiveDetector {

  public:
    CDCHGasLayerSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~CDCHGasLayerSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) { return false; }

    void setDCHCenterInDetSys(const G4ThreeVector &origin) {
            _cdchDetCenter = origin + _systOrigin;
    }

  protected:

    int _superlayer;
    int _ring;
    int _nwires;
    double _Dphi;

    GeomHandle<CDCHtracker> cdchtracker;
    //CDCHtracker::GeomType _ittype;

    // CDCH point of origin in ref. System
    G4ThreeVector _cdchDetCenter;

  };

} // namespace cdch

#endif /* CDCHGasLayerSD_hh */
