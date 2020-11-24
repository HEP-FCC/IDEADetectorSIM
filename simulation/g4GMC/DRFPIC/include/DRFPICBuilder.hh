//
// DRFPICBuilder builder class for the DRFPI in geant4
//
// Original author G. Tassielli
//

#ifndef DRFPICBuilder_hh
#define DRFPICBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"
#include "Tower.hh"

// G4 includes
#include "G4LogicalVolume.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"


namespace drc {

  class DRFPICBuilder {
  public:
    static VolumeInfo construct( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="DRFPICHitsCollection");
//    static void constructRadiator( G4LogicalVolume* drcmother/*, double zOff*/ );

  private:

    static void constructMaterial();

    static void fiberBR(G4int ,Tower *, G4Trap*, G4LogicalVolume *);
    static void fiberBL(G4int ,Tower *, G4Trap*, G4LogicalVolume *);
    static void fiberER(G4int ,Tower *, G4Trap*, G4LogicalVolume *);
    static void fiberEL(G4int ,Tower *, G4Trap*, G4LogicalVolume *);

  };

} //namespace drc

#endif /* DRFPICBuilder_hh */
