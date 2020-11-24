//
// Defines sensitive detector for a typicaly numbered volume using mu2e reference frame
//
// $Id: SensitiveDetector.cc,v 1.6 2013/09/06 16:14:00 gandr Exp $
// $Author: gandr $
// $Date: 2013/09/06 16:14:00 $
//
// Original author KLG
//

#include <SensitiveDetector.hh>
#include <SensitiveDetectorName.hh>
#include <cstdio>

// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
//#include "cetlib/exception.h"

//  includes
#include "manageException.hh"
//#include "Mu2eG4/inc/Mu2eG4UserHelpers.hh"
//#include "Mu2eG4/inc/PhysicsProcessInfo.hh"
//#include "Mu2eG4/inc/SimParticleHelper.hh"
//#include "ConfigTools/inc/SimpleConfig.hh"
//#include "GeometryService/inc/GeomHandle.hh"
//#include "GeometryService/inc/WorldG4.hh"

#include "SimpleConfig.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "RootIO.hh"

using namespace std;

//namespace cdch {

  SensitiveDetector::SensitiveDetector(const G4String &name, const G4String &hitsCollectionName, const crd::SimpleConfig &config):
    GMCG4TrackerSD(name,hitsCollectionName),
    _systOrigin(config.getHep3Vector("g4.detSysCenter",CLHEP::Hep3Vector())),
    _sizeLimit(config.getInt("g4.stepsSizeLimit",0)),
    _currentSize(0)
  {
  }

  void SensitiveDetector::Initialize(G4HCofThisEvent* HCE){

    _currentSize=0;
    GMCG4TrackerSD::Initialize(HCE);

  }


  G4bool SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*){

    _currentSize += 1;

    if ( _sizeLimit>0 && _currentSize>_sizeLimit ) {
      if( (_currentSize - _sizeLimit)==1 ) {
//        mf::LogWarning("G4") << "Maximum number of particles reached in "
//                             << SensitiveDetectorName
//                             << ": "
//                             << _currentSize << endl;
        exc::exceptionG4 e("G4","Warning",4);
        e<< "Maximum number of particles reached "
            << SensitiveDetectorName
            << ": "
            << _currentSize << endl;
        e.error();
      }
      return false;
    }

    if ( verboseLevel>=2 )  {
            G4cout<<"edep "<<aStep->GetTotalEnergyDeposit()
                  <<" nidep "<<aStep->GetNonIonizingEnergyDeposit()
                  <<" step "<<aStep->GetStepLength()<<G4endl;
            G4cout<<"Step vol name "<<aStep->GetTrack()->GetVolume()->GetName()<<G4endl;
    }


    // Add the hit to the framework collection.
    // The point's coordinates are saved in the ref. system coordinate system.
    fHitsCollection->insert( new GMCG4TrackerHit(aStep->GetTrack()->GetTrackID(),
                            aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(),
                            aStep->GetTotalEnergyDeposit(),
                            aStep->GetNonIonizingEnergyDeposit(),
                            aStep->GetPreStepPoint()->GetGlobalTime(),
                            aStep->GetPreStepPoint()->GetProperTime(),
                            aStep->GetPreStepPoint()->GetPosition() - _systOrigin,
                            aStep->GetPostStepPoint()->GetPosition() - _systOrigin,
                            aStep->GetPreStepPoint()->GetMomentum(),
                            aStep->GetStepLength(),
                            aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()
                            ));

    return true;

  }


  void SensitiveDetector::EndOfEvent(G4HCofThisEvent*){

    if( _sizeLimit>0 && _currentSize>=_sizeLimit ) {
//      mf::LogWarning("G4") << "Total of " << _currentSize << " "
//                           << SensitiveDetectorName
//                           << " hits were generated in the event."
//                           << endl
//                           << "Only " << _sizeLimit << " are saved in output collection."
//                           << endl;
      G4cout << "Total of " << _currentSize << " "
           << SensitiveDetectorName
           << " hits were generated in the event."
           << G4endl
           << "Only " << _sizeLimit << " are saved in output collection."
           << G4endl;

    }

    if (verboseLevel>0) {
      G4int NbHits = fHitsCollection->GetSize();
      G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
             << " hits in " << SensitiveDetectorName << ": " << G4endl;
      //for (G4int i=0;i<NbHits;i++) (*fHitsCollection)[i]->Print();
      fHitsCollection->PrintAllHits();
    }

    // storing the hits in ROOT file
    G4int NbHits = fHitsCollection->entries();
    std::vector<GMCG4TrackerHit*> hitsVector;

    for (G4int i=0;i<NbHits;i++)
      hitsVector.push_back((*fHitsCollection)[i]);

//    std::string hitColName="Hits";
//    hitColName+=SensitiveDetectorName;
//    RootIO::GetInstance()->PutMCStepHit(hitColName.c_str(),hitsVector);
    RootIO::GetInstance()->PutMCStepHit(SensitiveDetectorName.c_str(),hitsVector);


  }

//} //namespace cdch
