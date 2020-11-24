//
// Original author G. Tassielli
//

#include "CDCHGasLayerSD_Square.hh"
#include "Layer.hh"
#include "Cell.hh"

#include <utilFunctions.hh>
#include "manageException.hh"

#include "G4VProcess.hh"

#include "TMath.h"

#include <string>

using namespace std;

namespace cdch {

  CDCHGasLayerSD_Square::CDCHGasLayerSD_Square(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
      CDCHGasLayerSD( name, hitsCollectionName, config) { }


  CDCHGasLayerSD_Square::~CDCHGasLayerSD_Square() { }

  G4bool CDCHGasLayerSD_Square::ProcessHits(G4Step* aStep, G4TouchableHistory*){

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

          string volName = aStep->GetTrack()->GetVolume()->GetName();
          if ( verboseLevel>=2 )  cout<<"Step vol name "<<aStep->GetTrack()->GetVolume()->GetName()<<endl;

//          _superlayer=atoi(volName.substr(5,2).c_str());
//          _ring=atoi(volName.substr(8,2).c_str());
          _superlayer=atoi(volName.substr(5,3).c_str());
          _ring=atoi(volName.substr(9,2).c_str());
          if ( (cdchtracker->geomType()==50 || cdchtracker->geomType()==60) && _ring>=cdchtracker->maxNLayPerSL() ) {
            _ring%=cdchtracker->maxNLayPerSL();
          }

          try {
            if ( verboseLevel>=2 )  cout<<"SuperLayer "<<_superlayer<<" Ring "<<_ring<<endl;
              cdchtracker->getCellGeometryHandle()->SelectCell(_superlayer,_ring,0);
                  _nwires=cdchtracker->getCellGeometryHandle()->GetLayer()->nCells();
                  _Dphi=CLHEP::twopi/_nwires;
          }catch (std::exception &e) {
                  cerr<<e.what();
                  _nwires=0;
                  _Dphi=0.0;
          }

          int ring = _ring;
          //        if(ring==0) ring++;/*return false;*/
          //G4double edep = aStep->GetTotalEnergyDeposit();

          //cout<<"Step vol name "<<aStep->GetTrack()->GetVolume()->GetName()<<endl;

          // Eventually we will want this but not now.
          //if(edep==0.) return false;


          // Origin of the CDCHtracker.  Need to get this from G4.
          //static G4ThreeVector detectorOrigin( -3904., -7350., 6200.);


          // Position at start of step point, in world system and in
          // a system in which the center of the tracking detector is the origin.
          G4ThreeVector prePosWorld   = aStep->GetPreStepPoint()->GetPosition();
          if ( verboseLevel>=2 )  std::cout<<"G4 hit pos in World"<<prePosWorld[0]<<" "<<prePosWorld[1]<<" "<<prePosWorld[2]<<std::endl;
          G4ThreeVector prePosTracker = prePosWorld - _cdchDetCenter;
          G4ThreeVector postPosTracker = aStep->GetPostStepPoint()->GetPosition() - _cdchDetCenter;

          G4ThreeVector preMomWorld = aStep->GetPreStepPoint()->GetMomentum();

          //GeomHandle<CDCHtracker> cdchtracker;

          G4ThreeVector pos(prePosTracker);

          double phihit=TMath::ATan2(pos[1],pos[0]);
          double hit[4]={pos[0],pos[1],pos[2],aStep->GetPreStepPoint()->GetGlobalTime()};

          bool isDML_Upstrm=false;
          if (cdchtracker->isDumbbell()) {
             if (pos[2]<cdchtracker->zZonesLimits()[0]) {
                     isDML_Upstrm=true;
             }
             else if (pos[2]<=cdchtracker->zZonesLimits()[1]) {
                     return false;
             }
          }

          float xywire[3];
          double phiwire1=0;
          int wire1=-1;
          double cosstereo;
          double invcosstereo;

          try {
                  if ( verboseLevel>=2 )  std::cout<<"S "<<_superlayer<<" R "<<ring<<std::endl;

                  cdchtracker->getCellGeometryHandle()->SelectCell(_superlayer,ring,0);

                  cosstereo=TMath::Cos(cdchtracker->getCellGeometryHandle()->GetWireEpsilon());
                  invcosstereo=1.0/cosstereo;
                  if ( verboseLevel>=2 )  std::cout<<"stereo "<<cdchtracker->getCellGeometryHandle()->GetWireEpsilon()<<" invcosstereo "<<invcosstereo<<std::endl;
                  cdchtracker->getCellGeometryHandle()->WirePosAtZ(pos[2]/**invcosstereo*/,xywire);

                  if ( verboseLevel>=2 )  std::cout<<"0 wire center "<<xywire[0]<<" "<<xywire[1]<<" "<<xywire[2]<<std::endl;
                  if ( verboseLevel>=2 )  std::cout<<"hit pos "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;
                  if ( verboseLevel>=2 )  std::cout<<"hit to wire 0 dist " <<sqrt(sum_of_squares(pos[0]-xywire[0], pos[1]-xywire[1], pos[2]-xywire[2])) <<std::endl;

   //                //---------------- test --------------------
   //                //TRandom *rn = new TRandom();
   //                pos[0]=xywire[0]+gRandom->Uniform(-6,6);
   //                pos[1]=xywire[1]+gRandom->Uniform(-6,6);
   //                hit[0]=pos[0];
   //                hit[1]=pos[1];
   //                prePosTracker[0]=pos[0];
   //                prePosTracker[1]=pos[1];
   //                phihit=TMath::ATan2(pos[1],pos[0]);
   //                //------------------------------------------

                  phiwire1=TMath::ATan2(xywire[1],xywire[0]);

                  if ( verboseLevel>=2 )  std::cout<<"phihit "<<phihit <<" phiwire1 "<<phiwire1 <<" _Dphi "<<_Dphi<<std::endl;

                  phihit-=phiwire1;
                  int wire;
                  wire = TMath::Nint(phihit/_Dphi);
                  //                wire = TMath::Nint(phihit/Dphi);

                  if (wire<0) wire+=_nwires;

                  double tdist, mdist;
                  mdist=1.0e+9;
                  int maxW = _nwires-1;

                  cdchtracker->getCellGeometryHandle()->SelectCell(_superlayer,ring,wire);
                  mdist=cdchtracker->getCellGeometryHandle()->DistFromWireCenter(hit);
                  wire1=wire;
                  --wire1;
                  if (wire1<0) wire1+=_nwires;
                  cdchtracker->getCellGeometryHandle()->SelectCell(_superlayer,ring,wire1);
                  tdist=cdchtracker->getCellGeometryHandle()->DistFromWireCenter(hit);
                  if (mdist>tdist) {
                          mdist=tdist;
                          wire=wire1;
                  }else {
                          ++wire1;
                          ++wire1;
                          if (wire1>maxW) wire1-=_nwires;
                          cdchtracker->getCellGeometryHandle()->SelectCell(_superlayer,ring,wire1);
                          tdist=cdchtracker->getCellGeometryHandle()->DistFromWireCenter(hit);
                          if (mdist>tdist) {
                                  mdist=tdist;
                                  wire=wire1;
                          }
                  }
                  //if (mdist>cdchtracker->getCellGeometryHandle()->GetCellRad()) return false;

                  if ( verboseLevel>=2 )  std::cout<<"Cirumscribed Cell Radius "<<cdchtracker->getCellGeometryHandle()->GetCellRad()<<std::endl;
                  if ( verboseLevel>=2 )  std::cout<<"selected wire "<<ring<<" "<<wire<<std::endl;

                  unsigned long det=0;
                  if (cdchtracker->isDumbbell()) {
                          det = cdchtracker->getCellGeometryHandle()->computeDet(_superlayer,ring,wire,isDML_Upstrm);
                  }
                  else {
                          det = cdchtracker->getCellGeometryHandle()->computeDet(_superlayer,ring,wire);
                  }

   //                StepPointG4* newHit =
   //                                new StepPointG4( aStep->GetTrack()->GetTrackID(),
   //                                                det,
   //                                                edep,
   //                                                prePosTracker,
   //                                                preMomWorld,
   //                                                aStep->GetPreStepPoint()->GetGlobalTime(),
   //                                                aStep->GetPreStepPoint()->GetProperTime(),
   //                                                aStep->GetStepLength()
   //                                );
   //
   //                // The collection takes ownership of the hit.
   //                _collection->insert( newHit );
   //
   //                newHit->Print();
   //                //newHit->Draw();

                 // Add the hit to the framework collection.
                 // The point's coordinates are saved in the cdch coordinate system.
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

                  return true;
          }catch (std::exception &e) {
                  cerr<<e.what();
                  return false;
          }

   }


} //namespace cdch
