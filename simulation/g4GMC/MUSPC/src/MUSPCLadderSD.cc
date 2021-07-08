//
// Original author G. Tassielli
//

// GMCG4 includes
#include "GeomService.hh"
#include "manageException.hh"

// MUSPC includes
#include "MUSPCLadderSD.hh"
#include "Layer.hh"
#include "ROGeometryHandle.hh"

#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include <string>

using namespace std;
using namespace svx;

namespace muspc {

  MUSPCLadderSD::MUSPCLadderSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
                  SensitiveDetector(name,hitsCollectionName,config)
                  ,_layer(0)
                  ,_phiSec(0)
                  ,_ladder(0)
                  ,_roSubShell(0)
                  ,_roIdFstSide(0)
                  ,_roIdSndSide(0)
  {
//          art::ServiceHandle<GeometryService> geom;
//
//          if ( !geom->hasElement<MUSPCtracker>() ) {
//                  throw cet::exception("GEOM")
//                  << "Expected I Trackers but found neither.\n";
//          }

    if ( !GeomService::Instance()->hasElement<MUSPCtracker>() ) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<< "Expected MUSPC Trackers but found neither.\n";
      e.error();
    }
  }

  MUSPCLadderSD::~MUSPCLadderSD(){ }

  G4bool MUSPCLadderSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ){

          _currentSize += 1;

          if( _sizeLimit>0 && _currentSize>_sizeLimit ) {
                  if( (_currentSize - _sizeLimit)==1 ) {
//                          mf::LogWarning("G4") << "Maximum number of particles reached "
//                                          << SensitiveDetectorName
//                                          << ": "
//                                          << _currentSize << endl;
                          exc::exceptionG4 e("G4","Warning",4);
                          e<< "MUSPC: Maximum number of particles reached "
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
          _layer = atoi(volName.substr(10,3).c_str());

//          string volName = aStep->GetTrack()->GetVolume()->GetName();
          int volCpNum = 0;
          _roSubShell = 0;
          Layer* tmpLayer = muspctracker->getLayer(_layer);
          if (tmpLayer->getLadder(0)->getDetail()->nShells()>1) {
//            volCpNum = aTouch->GetCopyNumber(1);
            G4TouchableHandle touch1 = aStep->GetPreStepPoint()->GetTouchableHandle();
            volCpNum = touch1->GetCopyNumber(1);
            _roSubShell = atoi(volName.substr(24,1).c_str());
          } else {
            volCpNum = aStep->GetTrack()->GetVolume()->GetCopyNo();
          }
          if ( verboseLevel>=2 )  cout<<"Step vol name "<<volName<<" Copy num "<<volCpNum<<endl;

          int nLadPerSec = tmpLayer->nLaddersPerSector();
          bool isFw = (tmpLayer->getLayerZone()==Layer::forward);
          if (isFw) {
            _phiSec = volCpNum;
            _ladder = atoi(volName.substr(15,5).c_str())%100;
          } else {
            _phiSec = volCpNum/nLadPerSec;
            _ladder = volCpNum%nLadPerSec;
          }
          if ( verboseLevel>=2 && isFw ) { cout<<"-------- FW ---------"<<endl; }

//          cout<<"Hit in vol "<<volName<<" sub "<<volName.substr(10,3)<<" Layer "<<_layer<<" subLadder "<<_roSubShell<<endl;
//          cout<<"volCpNum "<<volCpNum<<" Sec "<<_phiSec<<" lad "<<_ladder<<endl;


          // Position at start of step point, in world system and in
          // a system in which the center of the tracking detector is the origin.
          G4ThreeVector prePosWorld = aStep->GetPreStepPoint()->GetPosition();
          if ( verboseLevel>=2 )  std::cout<<"MUSPC: G4 hit pos in World "<<prePosWorld[0]<<" "<<prePosWorld[1]<<" "<<prePosWorld[2]<<std::endl;
          G4ThreeVector prePosTracker = prePosWorld - _muspcDetCenter;
          G4ThreeVector postPosTracker = aStep->GetPostStepPoint()->GetPosition() - _muspcDetCenter;

          G4ThreeVector preMomWorld = aStep->GetPreStepPoint()->GetMomentum();
          HepGeom::Point3D<double> prePosGbl(prePosTracker);
          HepGeom::Point3D<double> prePosLoc;
          ROGeometryHandle *roghndl = muspctracker->getROGeometryHandle();
          roghndl->SelectLadder(_layer,_phiSec,_ladder);
          roghndl->Global2Local(prePosGbl,prePosLoc);

          if (verboseLevel>=2) {
            cout<<"Selected Ladder: "<<roghndl->GetLadder()->Id()<<"Sub shell "<<_roSubShell<<endl;
//            //cout<<*roghndl->GetLadder()<<endl;
            cout<<"Ladder Mid Point "<<roghndl->GetLadder()->getMidPoint()<<endl;
            cout<<"Hit Global "<<prePosGbl<<" Local "<<prePosLoc<<endl;
//
//            cout<<"Ladder width: "<<roghndl->GetLadder()->getDetail()->width()<<" width2: "<<roghndl->GetLadder()->getDetail()->scndWidth();
//            cout<<" halfLength "<<roghndl->GetLadder()->getDetail()->halfLength()<<endl;
          }
          if (isFw) {
            prePosLoc[0] -= (roghndl->GetLadder()->getDetail()->width()*0.5-roghndl->GetROChannel()->getFstSideInsul());
            prePosLoc[2] += (roghndl->GetLadder()->getDetail()->halfLength()-roghndl->GetROChannel()->getSndSideInsul());
            prePosLoc[0] *= -1.0;
//            prePosLoc[2] *= -1.0;
          } else {

            prePosLoc[0] -= (roghndl->GetLadder()->getDetail()->width()*0.5-roghndl->GetROChannel()->getFstSideInsul());
            prePosLoc[2] -= (roghndl->GetLadder()->getDetail()->halfLength()-roghndl->GetROChannel()->getSndSideInsul());
            prePosLoc[0] *= -1.0;
            prePosLoc[2] *= -1.0;
          }

//          cout<<" referred to top right corner "<<prePosLoc<<endl;

          roghndl->SelectRO(_layer,_phiSec,_ladder,0,0,_roSubShell);
          _roIdSndSide = prePosLoc[2]/(roghndl->GetROChanSndSdDim()+roghndl->GetROChannel()->getSndSideInsul());
          if (isFw) {
//            cout<<"prePosLoc "<<prePosLoc[0]<<endl;
            int extraChanPerRw = _roIdSndSide*roghndl->GetLadder()->nSndSdDeltaRO();
            if (roghndl->GetLadder()->nSndSdAddDeltaRO()!=0) {
              extraChanPerRw += _roIdSndSide/roghndl->GetLadder()->nSndSdAddDeltaRO();
            }
            if (verboseLevel>=2) {
              cout<<"extraChanPerRw "<<extraChanPerRw<<endl;
              cout<<"nSndSdDeltaRO "<<roghndl->GetLadder()->nSndSdDeltaRO()<<" nSndSdAddDeltaRO "<<roghndl->GetLadder()->nSndSdAddDeltaRO()<<endl;
            }
            prePosLoc[0] += (extraChanPerRw * (roghndl->GetROChanFstSdDim()+roghndl->GetROChannel()->getFstSideInsul()) *0.5);
          }
          _roIdFstSide = prePosLoc[0]/(roghndl->GetROChanFstSdDim()+roghndl->GetROChannel()->getFstSideInsul());
          if (verboseLevel>=2) {
            cout<<"prePosLoc "<<prePosLoc[0]<<" ChanFstSdDim "<<(roghndl->GetROChanFstSdDim()+roghndl->GetROChannel()->getFstSideInsul())<<endl;
            cout<<"RO "<<_roIdFstSide<<","<<_roIdSndSide<<endl;
          }
          if ( prePosLoc[0]<0.0 ) {
                  if ( verboseLevel>=2 )  cout<<"Skipped, hit in the dead zone of the wedge "<<volName<<endl;
                  return false;
          }
          roghndl->SelectRO(_layer,_phiSec,_ladder,_roIdFstSide,_roIdSndSide,_roSubShell);
          if (verboseLevel>=2) {
            cout<<"RO "<<_roIdFstSide<<","<<_roIdSndSide<<" in ladder pos, Gbl "<<roghndl->GetROChanCenterGbl()<<" lcl "<<roghndl->GetROChanCenterLcl()<<endl;
          }

          int det = roghndl->computeChambID(_layer,_phiSec,_ladder);
          int channel = roghndl->computeChannelID(_roIdFstSide,_roIdSndSide,_roSubShell);
//          cout<<"Channel ID "<<channel<<endl;

          try {

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
//            ((GMCG4TrackerHit*)fHitsCollection->GetHit(fHitsCollection->entries()-1))->SetChannelNb(channel);
            fHitsCollection->GetVector()->back()->SetChannelNb(channel);

            if (verboseLevel>=2) {
              roghndl->SelectRO(roghndl->computeChambID(0,0,0),roghndl->computeChannelID(0,0));
              int tmpChmbID = fHitsCollection->GetVector()->back()->GetChamberNb();
              int tmpChId = fHitsCollection->GetVector()->back()->GetChannelNb();
              roghndl->SelectRO(tmpChmbID,tmpChId);
              const CLHEP::Hep3Vector ROcent = roghndl->GetROChanCenterGbl();
              double maxDist = 1.2 * sqrt( pow(roghndl->GetROChanFstSdDim(),2) + pow(roghndl->GetROChanSndSdDim(),2) + pow(0.5*roghndl->GetLadder()->getDetail()->shellThickness(_roSubShell),2) );
              maxDist = round(maxDist*100.0)/100.0;
              if (prePosGbl.distance(ROcent)>maxDist) {
                cout<<"--------- Max Dist exceeded ---------"<<endl;
//                if (isFw) { cout<<"-------- FW ---------"<<endl; }
                cout<<"ChmbID "<<tmpChmbID<<" ChId "<<tmpChId<<" midPos (loc) "<<roghndl->GetROChanCenterLcl()<<" (gbl) "<<ROcent<<endl;
                cout<<"HitPos "<<prePosGbl<<endl;
                cout<<"distance: "<<prePosGbl.distance(ROcent)<<endl;
                cout<<"dist on plane "<<sqrt(pow(prePosGbl[0]-ROcent[0],2)+pow(prePosGbl[1]-ROcent[1],2))<<endl;
              }
            }

            return true;
    }catch (std::exception &e) {
            cerr<<e.what();
            return false;
    }

  }

} //namespace muspc
