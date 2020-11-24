//
// PSHWtrackerBuilder builder class for the PSHW in geant4
//
// Original author G. Tassielli
//

#ifndef PSHWrackerBuilder_hh
#define PSHWrackerBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace svx {
class Ladder;
}

namespace pshw {

  class PSHWBuilder {
  public:
    static VolumeInfo constructTracker( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="PSHWHitsCollection");
    static void constructRadiator( G4LogicalVolume* pshwmother/*, double zOff*/ );
  private:
    static VolumeInfo buildLadder(svx::Ladder &tld);
  };

} //namespace pshw

#endif /* PSHWrackerBuilder_hh */
