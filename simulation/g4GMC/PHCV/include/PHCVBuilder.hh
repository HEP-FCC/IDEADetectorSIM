//
// PHCVBuilder builder class for the PHCV in geant4
//
// Original author G. Tassielli
//

#ifndef PHCVBuilder_hh
#define PHCVBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace phcv {

  class PHCVBuilder {
  public:
    static VolumeInfo construct( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="PHCVHitsCollection");
//    static void constructRadiator( G4LogicalVolume* phcvmother/*, double zOff*/ );
  };

} //namespace phcv

#endif /* PHCVBuilder_hh */
