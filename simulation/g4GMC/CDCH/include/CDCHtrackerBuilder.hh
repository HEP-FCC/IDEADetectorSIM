//
// CDCHtrackerBuilder builder class for the CDCH in geant4
//
// Original author G. Tassielli
//

#ifndef CDCHrackerBuilder_hh
#define CDCHrackerBuilder_hh

#include "CDCHtracker.hh"
#include "VolumeInfo.hh"
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace cdch {

  class CDCHtrackerBuilder {
  public:
    static VolumeInfo constructTracker( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="CDCHHitsCollection");
  private:
    static VolumeInfo buildWire(float radius, float length, char *shapeName, char *volName, const std::vector<std::string> &materialName, const std::vector<double> &thicknesses, bool activeWireSD=false, bool isSense=false);
    static VolumeInfo buildWall(Wall *wall, CDCHtracker::EnCapType endcapType);
    static double constructSpiderWeb(G4LogicalVolume* localMother, crd::SimpleConfig const& config);
    static void   constructWireAnchoring(G4LogicalVolume* localMother, crd::SimpleConfig const& config, double spdWebBaseExcess=0.0);
    static void   constructSignalCables(G4LogicalVolume* localMother, crd::SimpleConfig const& config);
    static void   constructHvCables(G4LogicalVolume* localMother, crd::SimpleConfig const& config);

  };

} //namespace cdch

#endif /* CDCHrackerBuilder_hh */
