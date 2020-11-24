#ifndef SensitiveDetector_hh
#define SensitiveDetector_hh
//
// Defines a base sensitive detector
//
// Original author G. F. Tassielli
//

// Mu2e includes
//#include "Mu2eG4/inc/EventNumberList.hh"
//#include "Mu2eG4/inc/SimParticleHelper.hh"
//#include "MCDataProducts/inc/StepPointMCCollection.hh"

// G4 includes
//#include "G4VSensitiveDetector.hh"

// Art includes
//#include "art/Persistency/Provenance/ProductID.h"
//#include "art/Framework/Principal/Event.h"
#include "GMCG4TrackerSD.hh"

class G4Step;
class G4HCofThisEvent;

namespace crd{
  class SimpleConfig;
}

//namespace cdch {

//  class PhysicsProcessInfo;

  class SensitiveDetector : public GMCG4TrackerSD {

  public:

    SensitiveDetector(const G4String &name, const G4String &hitsCollectionName, const crd::SimpleConfig &config);

    virtual void Initialize(G4HCofThisEvent* hce);

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    virtual void EndOfEvent(G4HCofThisEvent* hce);

  protected:

    // Ref. System point of origin
    const G4ThreeVector _systOrigin;

    // Limit maximum size of the steps collection
    int _sizeLimit;
    int _currentSize;

  };

//} // namespace cdch

#endif /* SensitiveDetector_hh */
