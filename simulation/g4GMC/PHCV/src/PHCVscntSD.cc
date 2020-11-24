//
// Original author G. Tassielli
//

// GMCG4 includes
#include "PHCVscntSD.hh"

#include "GeomService.hh"
#include "manageException.hh"

// PHCV includes
#include "SLayer.hh"
//#include "ROGeometryHandle.hh"

#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include <string>

using namespace std;

namespace phcv {

  PHCVscntSD::PHCVscntSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
                  SensitiveDetector(name,hitsCollectionName,config)
                  ,_shell(0)
                  ,_layer(0)
//                  ,_phiSec(0)
//                  ,_ladder(0)
//                  ,_roSubShell(0)
//                  ,_roIdFstSide(0)
//                  ,_roIdSndSide(0)
  {
//          art::ServiceHandle<GeometryService> geom;
//
//          if ( !geom->hasElement<PHCVtracker>() ) {
//                  throw cet::exception("GEOM")
//                  << "Expected I Trackers but found neither.\n";
//          }

    if ( !GeomService::Instance()->hasElement<PHCVtcounter>() ) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<< "Expected PHCV tcounter but found neither.\n";
      e.error();
    }
  }

  PHCVscntSD::~PHCVscntSD(){ }

  G4bool PHCVscntSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ){

          _currentSize += 1;

          if( _sizeLimit>0 && _currentSize>_sizeLimit ) {
                  if( (_currentSize - _sizeLimit)==1 ) {
//                          mf::LogWarning("G4") << "Maximum number of particles reached "
//                                          << SensitiveDetectorName
//                                          << ": "
//                                          << _currentSize << endl;
                          exc::exceptionG4 e("G4","Warning",4);
                          e<< "PHCV: Maximum number of particles reached "
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

          string volName = aStep->GetTrack()->GetVolume()->GetName();
          if ( verboseLevel>=2 )  cout<<"edep "<<edep<<" nidep "<<nidep<<" step "<<step<<endl;
          // I am not sure why we get these cases but we do.  Skip them.
          if ( (edep == 0. /*|| idep == 0.*/)/*&& step == 0.*/ ) {
                  if ( verboseLevel>=2 )  cout<<"Skipped in vol "<<volName<<endl;
                  return false;
          }
          _shell = atoi(volName.substr(7,3).c_str());
          _layer = atoi(volName.substr(11,2).c_str());

//          cout<<"Hit in vol "<<volName<<" sub "<<volName.substr(10,3)<<" Layer "<<_layer<<" subLadder "<<_roSubShell<<endl;
//          cout<<"volCpNum "<<volCpNum<<" Sec "<<_phiSec<<" lad "<<_ladder<<endl;

          // Position at start of step point, in world system and in
          // a system in which the center of the tracking detector is the origin.
          G4ThreeVector prePosWorld = aStep->GetPreStepPoint()->GetPosition();
          if ( verboseLevel>=2 )  std::cout<<"PHCV: G4 hit pos in World "<<prePosWorld[0]<<" "<<prePosWorld[1]<<" "<<prePosWorld[2]<<std::endl;
          G4ThreeVector prePosTracker = prePosWorld - _phcvDetCenter;
          G4ThreeVector postPosTracker = aStep->GetPostStepPoint()->GetPosition() - _phcvDetCenter;

          G4ThreeVector preMomWorld = aStep->GetPreStepPoint()->GetMomentum();
          HepGeom::Point3D<double> prePosGbl(prePosTracker);
//          HepGeom::Point3D<double> prePosLoc;
//          ROGeometryHandle *roghndl = phcvtracker->getROGeometryHandle();
//          roghndl->SelectLadder(_layer,_phiSec,_ladder);
//          roghndl->Global2Local(prePosGbl,prePosLoc);

          try {

        	  unsigned long det=_shell;
            fHitsCollection->insert( new GMCG4TrackerHit(aStep->GetTrack()->GetTrackID(),
                                                det,
                                                /*idep,*/edep,
                                                aStep->GetNonIonizingEnergyDeposit(),
                                                aStep->GetPreStepPoint()->GetGlobalTime(),
                                                aStep->GetPreStepPoint()->GetProperTime(),
                                                prePosTracker,
                                                postPosTracker,
                                                preMomWorld,
                                                step,
                                                aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()
                                                ));
////            ((GMCG4TrackerHit*)fHitsCollection->GetHit(fHitsCollection->entries()-1))->SetChannelNb(channel);
//            fHitsCollection->GetVector()->back()->SetChannelNb(channel);
//
//            if (verboseLevel>=2) {
//              roghndl->SelectRO(roghndl->computeChambID(0,0,0),roghndl->computeChannelID(0,0));
//              int tmpChmbID = fHitsCollection->GetVector()->back()->GetChamberNb();
//              int tmpChId = fHitsCollection->GetVector()->back()->GetChannelNb();
//              roghndl->SelectRO(tmpChmbID,tmpChId);
//              const CLHEP::Hep3Vector ROcent = roghndl->GetROChanCenterGbl();
//              double maxDist = 1.2 * sqrt( pow(roghndl->GetROChanFstSdDim(),2) + pow(roghndl->GetROChanSndSdDim(),2) + pow(0.5*roghndl->GetLadder()->getDetail()->shellThickness(_roSubShell),2) );
//              maxDist = round(maxDist*100.0)/100.0;
//              if (prePosGbl.distance(ROcent)>maxDist) {
//                cout<<"--------- Max Dist exceeded ---------"<<endl;
////                if (isFw) { cout<<"-------- FW ---------"<<endl; }
//                cout<<"ChmbID "<<tmpChmbID<<" ChId "<<tmpChId<<" midPos (loc) "<<roghndl->GetROChanCenterLcl()<<" (gbl) "<<ROcent<<endl;
//                cout<<"HitPos "<<prePosGbl<<endl;
//                cout<<"distance: "<<prePosGbl.distance(ROcent)<<endl;
//                cout<<"dist on plane "<<sqrt(pow(prePosGbl[0]-ROcent[0],2)+pow(prePosGbl[1]-ROcent[1],2))<<endl;
//              }
//            }

            return true;
    }catch (std::exception &e) {
            cerr<<e.what();
            return false;
    }

  }

} //namespace phcv
