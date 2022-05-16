#ifndef MUSPCLadderSD_hh
#define MUSPCLadderSD_hh
//
// Defines a generic MUSPCtracker sensitive detector
//
// Original author G. Tassielli
//

// MUSPC includes
#include <SensitiveDetector.hh>
#include "MUSPCtracker.hh"

// GMCG4 includes
#include "GeomHandle.hh"

namespace muspc {

  class MUSPCLadderSD : public SensitiveDetector {

  public:
    MUSPCLadderSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~MUSPCLadderSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setMUSPCCenterInDetSys(const G4ThreeVector &origin) {
            _muspcDetCenter = origin + _systOrigin;
    }

  protected:

    int _layer;
    int _phiSec;
    int _ladder;
    int _roSubShell;
    unsigned long _roIdFstSide;
    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<MUSPCtracker> muspctracker;
    //MUSPCtracker::GeomType _ittype;

    // MUSPC point of origin in ref. System
    G4ThreeVector _muspcDetCenter;

  };

} // namespace muspc

#endif /* MUSPCLadderSD_hh */
