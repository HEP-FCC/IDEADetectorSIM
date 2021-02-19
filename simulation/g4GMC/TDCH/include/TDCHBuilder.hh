//
// TDCHBuilder builder class for the TDCH in geant4
//
// Original author G. Tassielli
//

#ifndef TDCHBuilder_hh
#define TDCHBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace tdch {

  class TDCHBuilder {
  public:
    static VolumeInfo construct( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="TDCHHitsCollection");
//    static void constructRadiator( G4LogicalVolume* tdchmother/*, double zOff*/ );
  };

} //namespace tdch

#endif /* TDCHBuilder_hh */
