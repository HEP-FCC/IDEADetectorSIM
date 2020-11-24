//
//
//
//

// GMCG4 includes
#include "manageException.hh"

// CDCH includes
#include "TrackerWireSD.hh"

// G4 includes
#include "G4ThreeVector.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4ios.hh"

using namespace std;

namespace cdch {

//  G4ThreeVector TrackerWireSD::_cdchDetCenter;

  TrackerWireSD::TrackerWireSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
                  SensitiveDetector(name, hitsCollectionName, config) { }


  TrackerWireSD::~TrackerWireSD(){ }

  G4bool TrackerWireSD::ProcessHits(G4Step* aStep, G4TouchableHistory*){

          _currentSize += 1;

          if( _sizeLimit>0 && _currentSize>_sizeLimit ) {
                  if( (_currentSize - _sizeLimit)==1 ) {
//                          mf::LogWarning("G4") << "Maximum number of particles reached "
//                                          << SensitiveDetectorName
//                                          << ": "
//                                          << _currentSize << endl;
                    exc::exceptionG4 e("G4","Warning",4);
                    e<< "Maximum number of particles reached "
                        << SensitiveDetectorName
                        << ": "
                        << _currentSize << endl;
                    e.error();
                  }
                  return false;
          }

          G4double edep  = aStep->GetTotalEnergyDeposit();
          G4double nidep = aStep->GetNonIonizingEnergyDeposit();
          G4double step  = aStep->GetStepLength();
          //G4double idep  = edep-nidep;

          if ( verboseLevel>=2 )  cout<<"edep "<<edep<<" nidep "<<nidep<<" step "<<step<<endl;
          // I am not sure why we get these cases but we do.  Skip them.
          if ( (edep == 0. /*|| idep == 0.*/)/*&& step == 0.*/ ) {
                  if ( verboseLevel>=2 )  cout<<"Skipped"<<endl;
                  return false;
          }

          G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
          if ( preStepPoint->GetPhysicalVolume()->GetName().find("vol")!=string::npos ) { return false; }

          G4int motherCopyNo = preStepPoint->GetTouchableHandle()->GetReplicaNumber(1);

          if ( verboseLevel>=2 )  {
        	  cout<<"Step vol name "<<aStep->GetTrack()->GetVolume()->GetName()<<endl;
        	  cout<<"Step vol copyNo "<<" mother copyNo "<<motherCopyNo<<endl;
          }

          // Add the hit to the framework collection.
          // The point's coordinates are saved in the mu2e coordinate system.
          fHitsCollection->insert( new GMCG4TrackerHit(aStep->GetTrack()->GetTrackID(),
                                  motherCopyNo,
                                  aStep->GetTotalEnergyDeposit(),
                                  aStep->GetNonIonizingEnergyDeposit(),
                                  aStep->GetPreStepPoint()->GetGlobalTime(),
                                  aStep->GetPreStepPoint()->GetProperTime(),
                                  aStep->GetPreStepPoint()->GetPosition() - _cdchDetCenter,
                                  aStep->GetPostStepPoint()->GetPosition() - _cdchDetCenter,
                                  aStep->GetPreStepPoint()->GetMomentum(),
                                  aStep->GetStepLength(),
                                  aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()
                                  ));

          return true;
   }

} //namespace mu2e
