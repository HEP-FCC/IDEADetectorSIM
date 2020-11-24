//
// SVXtrackerBuilder builder class for the SVX in geant4
//
// Original author G. Tassielli
//

#ifndef SVXrackerBuilder_hh
#define SVXrackerBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace svx {

class Ladder;

  class SVXtrackerBuilder {
  public:
    static VolumeInfo constructTracker( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="SVXHitsCollection");
  private:
    static VolumeInfo buildLadder(Ladder &tld);
  };

} //namespace svx

#endif /* SVXrackerBuilder_hh */
