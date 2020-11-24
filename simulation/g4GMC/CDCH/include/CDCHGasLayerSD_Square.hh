#ifndef CDCHGasLayerSD_Square_hh
#define CDCHGasLayerSD_Square_hh
//
// sensitive detector of the CDCHtracker in the case of square cells
//
// Original author G. Tassielli
//

// CDCH includes
#include "CDCHGasLayerSD.hh"

namespace cdch {

  class CDCHGasLayerSD_Square : public CDCHGasLayerSD {

  public:
    CDCHGasLayerSD_Square(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    ~CDCHGasLayerSD_Square();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  };

} // namespace cdch

#endif /* CDCHGasLayerSD_Square_hh */
