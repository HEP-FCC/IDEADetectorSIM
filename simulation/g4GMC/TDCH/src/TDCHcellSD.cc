//
// Original author G. Tassielli
//

// GMCG4 includes
#include "GeomService.hh"
#include "manageException.hh"

// TDCH includes
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include <string>

#include "TDCHcellSD.hh"

#include "../include/Station.hh"

using namespace std;

namespace tdch {

  TDCHcellSD::TDCHcellSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
                  SensitiveDetector(name,hitsCollectionName,config)
                  ,_station(0)
//                  ,_phiSec(0)
//                  ,_ladder(0)
//                  ,_roSubShell(0)
//                  ,_roIdFstSide(0)
//                  ,_roIdSndSide(0)
  {

    if ( !GeomService::Instance()->hasElement<TDCHtracker>() ) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<< "Expected TDCH tracker but found neither.\n";
      e.error();
    }
  }

  TDCHcellSD::~TDCHcellSD(){ }

  G4bool TDCHcellSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ){

          _currentSize += 1;

          if( _sizeLimit>0 && _currentSize>_sizeLimit ) {
                  if( (_currentSize - _sizeLimit)==1 ) {
//                          mf::LogWarning("G4") << "Maximum number of particles reached "
//                                          << SensitiveDetectorName
//                                          << ": "
//                                          << _currentSize << endl;
                          exc::exceptionG4 e("G4","Warning",4);
                          e<< "TDCH: Maximum number of particles reached "
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
//          _shell = atoi(volName.substr(7,3).c_str());
          _station = atoi(volName.substr(11,2).c_str());

//          cout<<"Hit in vol "<<volName<<" sub "<<volName.substr(10,3)<<" Layer "<<_tower<<" subLadder "<<_roSubShell<<endl;
//          cout<<"volCpNum "<<volCpNum<<" Sec "<<_phiSec<<" lad "<<_ladder<<endl;

          // Position at start of step point, in world system and in
          // a system in which the center of the tracking detector is the origin.
          G4ThreeVector prePosWorld = aStep->GetPreStepPoint()->GetPosition();
          if ( verboseLevel>=2 )  std::cout<<"TDCH: G4 hit pos in World "<<prePosWorld[0]<<" "<<prePosWorld[1]<<" "<<prePosWorld[2]<<std::endl;
          G4ThreeVector prePosTracker = prePosWorld - _tdchDetCenter;
          G4ThreeVector postPosTracker = aStep->GetPostStepPoint()->GetPosition() - _tdchDetCenter;

          G4ThreeVector preMomWorld = aStep->GetPreStepPoint()->GetMomentum();
          HepGeom::Point3D<double> prePosGbl(prePosTracker);
//          HepGeom::Point3D<double> prePosLoc;
//          ROGeometryHandle *roghndl = phcvtracker->getROGeometryHandle();
//          roghndl->SelectLadder(_tower,_phiSec,_ladder);
//          roghndl->Global2Local(prePosGbl,prePosLoc);

          try {

        	  unsigned long det=_station;
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

} //namespace tdch
