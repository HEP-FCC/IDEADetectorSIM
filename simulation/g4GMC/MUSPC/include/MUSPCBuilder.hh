//
// MUSPCBuilder builder class for the MUSPC in geant4
//
// Original author G. Tassielli
//

#ifndef MUSPCBuilder_hh
#define MUSPCBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace svx {
class Ladder;
}

namespace muspc {

  class MUSPCBuilder {
  public:
    static VolumeInfo constructTracker( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="MUSPCHitsCollection");
    static void constructRadiator( G4LogicalVolume* muspcmother/*, double zOff*/ );
  private:
    static VolumeInfo buildLadder(svx::Ladder &tld);
  };

} //namespace muspc

#endif /* MUSPCBuilder_hh */
