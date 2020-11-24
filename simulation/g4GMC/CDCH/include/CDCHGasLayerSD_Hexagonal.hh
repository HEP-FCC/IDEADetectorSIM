#ifndef CDCHGasLayerSD_Hexagonal_hh
#define CDCHGasLayerSD_Hexagonal_hh
//
// sensitive detector of the CDCHtracker in the case of hexagonal cells
//
// Original author G. Tassielli
//

// CDCH includes
#include "CDCHGasLayerSD.hh"

namespace cdch {

  class CDCHGasLayerSD_Hexagonal : public CDCHGasLayerSD {

  public:
    CDCHGasLayerSD_Hexagonal(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    ~CDCHGasLayerSD_Hexagonal();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  };

} // namespace cdch

#endif /* CDCHGasLayerSD_Hexagonal_hh */
