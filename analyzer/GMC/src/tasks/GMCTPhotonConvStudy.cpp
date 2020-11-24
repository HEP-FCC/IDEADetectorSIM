////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTPhotonConvStudy                                                        //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//     GeantTrack                                                             //
//     PHCVGeantStep                                                          //
//     PHCVRadGeantStep                                                       //
//     DCHHit                                                                 //
//     RecoTracks                                                             //
//                                                                            //
// This task contains the following histgrams :                               //
//    HResidualUnbiased                                                       //
//    HResidualBiased                                                         //
//    HMomentum                                                               //
//    HMomRes_El                                                              //
//    HMomRes_Ps                                                              //
//    HConvHitPerShell                                                        //
//    HGammaPerShell                                                          //
//    HThetaGamma                                                             //
//    HConvProbPerShell                                                       //
//    HConvEnergy                                                             //
//    HPairEnergy                                                             //
//    HPairEnergyRad                                                          //
//    HPairEnergySct                                                          //
//    HPairEnergyRadDiff                                                      //
//    HPairEnergySctDiff                                                      //
//    HPairThetaRad                                                           //
//    HThetaRad_El                                                            //
//    HThetaRad_Ps                                                            //
//    HPairPhiRad                                                             //
//    HPhiRad_El                                                              //
//    HPhiRad_Ps                                                              //
//    HPairThetaSct                                                           //
//    HPairPhiSct                                                             //
//    HEnergyOut_El                                                           //
//    HElossInRadPerShell_El                                                  //
//    HElossInSctPerShell_El                                                  //
//    HEnergyOut_Ps                                                           //
//    HElossInRadPerShell_Ps                                                  //
//    HElossInSctPerShell_Ps                                                  //
//    HElHits                                                                 //
//    HElHitsDiff                                                             //
//    HPsHits                                                                 //
//    HPsHitsDiff                                                             //
//    HPairEnergyRec                                                          //
//    HPairEnergyRecDiff                                                      //
//    HRecPairEnergy                                                          //
//    HRecPairEnergyDiff                                                      //
//    HRecPairEnergyS                                                         //
//    HRecPairEnergySDiff                                                     //
//    HRecPair2EnergyDiff                                                     //
//    HRecPair2EnergySDiff                                                    //
//    HRecPair2EnergyDiff_1                                                   //
//    HRecPair2EnergySDiff_1                                                  //
//    HRecPairEnergyDebg                                                      //
//    HPairEnergyHit0Debg                                                     //
//    HPairEnergyScntDebg                                                     //
//    HPairVertXYResHt0ScDebg                                                 //
//    HPairVertZResHt0ScDebg                                                  //
//    HPairMomReasHt0ScDebg                                                   //
//                                                                            //
// The histograms/graph are created and saved automaticaly by the task.       //
//                                                                            //
// The following method can be used to get a handle of the histogram/graph :  //
//                                                                            //
// Get<Histogram/Graph Name>()                                                //
//                                                                            //
// For histogram/graph arrays use :                                           //
//                                                                            //
// Get<Histogram/Graph Name>At(Int_t index)                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/GMCTPhotonConvStudyGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/GMCAnalyzer.h"
#include "generated/GMCGlobalSteering.h"
#include "tasks/GMCTPhotonConvStudy.h"
#include "ROMEiostream.h"

#include "generated/GMCGeantTrack.h"
#include "generated/GMCPHCVGeantStep.h"
#include "generated/GMCPHCVRadGeantStep.h"
#include "generated/GMCDCHHit.h"
#include "generated/GMCRecoTracks.h"

#include "GeomService.hh"
#include "CellGeometryHandle.hh"

#include "PHCVtcounter.hh"

#include "TParticlePDG.h"
#include "TDatabasePDG.h"

#include "ConstField.h"
#include "FieldManager.h"
#include <TGeoMaterialInterface.h>
#include <MaterialEffects.h>
#include "AbsTrackRep.h"
#include "RKTrackRep.h"
#include "Track.h"
#include "RectangularFinitePlane.h"
#include <Tools.h>
#include "KalmanFitter.h"
#include "KalmanFitterRefTrack.h"
#include "DAF.h"
#include "KalmanFitterInfo.h"
#include <KalmanFittedStateOnPlane.h>
#include "Exception.h"
#include "WireMeasurement.h"
#include "WirePointMeasurement.h"
#include "SpacepointMeasurement.h"
#include "PlanarMeasurement.h"

#include <TGeoGlobalMagField.h>
#include <TGeoManager.h>
#include <TROOT.h>
#include <TRandom.h>
#include <memory>

using namespace genfit;

// uncomment if you want to include headers of all folders
//#include "GMCAllFolders.h"

extern std::ostream& operator<<( std::ostream& ostr, const TVector3& f );//{return ostr<<f.X()<<" "<<f.Y()<<" "<<f.Z();}

const double elMass2 = TMath::Power(TDatabasePDG::Instance()->GetParticle(11)->Mass()*1000.0,2); //in MeV2

ClassImp(GMCTPhotonConvStudy)

//______________________________________________________________________________
void GMCTPhotonConvStudy::Init()
{
	  fGeometry = GeometrySrvHndl::Instance();
	  if(!fGeometry->GetCellHandle())
	    fGeometry->makeDetectors();

	  FindShellRads();
	  fGeometry->GetGeomService()->getConfig().getVectorInt("cdch.OSnSuperLayer", _OSnSuperLayer, _nShells);

	  _minimumEnergy = GetSP()->GetHitEnergyCut();
	  _fDebug=GetSP()->GetDebugLevel();

	  _doFit=GetSP()->GetFitPairs();
	  _fitOnlyFst=GetSP()->GetFitOnlyFst();
	  _doExtrapol=GetSP()->GetExtrapolToDVrt();
	  if (_doFit) { InitMat(); }

}

//______________________________________________________________________________
void GMCTPhotonConvStudy::BeginOfRun()
{
}

//______________________________________________________________________________
void GMCTPhotonConvStudy::Event()
{
	  if (gAnalyzer->GetCurrentEventNumber()%100==0 ) { printf("\n ***** PhotonConvStudy Load event %lld ************************ \n",gAnalyzer->GetCurrentEventNumber()); }
//	  LoadEvent(gAnalyzer->GetCurrentEventNumber());
	  gAnalyzer->GetCurrentEventNumber();
	  LoadEvent();

	  FindPhotonInter();
	  FindPairLoss();
	  if (fBrHitsDC) {
		  FindPairDCHHits();
		  if (_doFit) {
			  FitPairs();
		  }
	  }
	  doPlot();

}

//______________________________________________________________________________
void GMCTPhotonConvStudy::LoadEvent(/*Int_t nev*/) {

  fBrDataTrk=gAnalyzer->GetGeantTracks();
  fBrDataPHCV=gAnalyzer->GetPHCVGeantSteps();
  fBrDataPHCVRd=gAnalyzer->GetPHCVRadGeantSteps();
  fBrHitsDC=gAnalyzer->GetDCHHits();
}

//______________________________________________________________________________
void GMCTPhotonConvStudy::EndOfRun()
{
	GetHConvProbPerShell()->Divide(GetHGammaPerShell());
}

//______________________________________________________________________________
void GMCTPhotonConvStudy::Terminate()
{
}

void GMCTPhotonConvStudy::InitMat() {
	TGeoManager::Import(gAnalyzer->GetGSP()->GetGeomGDMLfile());
	FieldManager::getInstance()->init(new ConstField(0,0,gAnalyzer->GetGSP()->GetBz()*10.)); //kGs
	MaterialEffects* mateff=MaterialEffects::getInstance();
	//mateff->setEnergyLossBetheBloch(true);
	//mateff->setNoiseBetheBloch(true);
	//mateff->setNoiseCoulomb(true);

	//disable brems effect, as it is not correct for positron in genfit
	mateff->setEnergyLossBrems(false);
	mateff->setNoiseBrems(false);

	MaterialEffects::getInstance()->init(new TGeoMaterialInterface());
}

void GMCTPhotonConvStudy::FindShellRads() {
	const phcv::PHCVtcounter*	phcvtc = fGeometry->GetPHCVtcounter();
	_zLength = phcvtc->zHalfLength();
	_nShells = phcvtc->nShell();
	for (int iSh=0; iSh<phcvtc->nShell(); ++iSh) {
		_sRadIn.push_back(TMath::Floor(phcvtc->getShellRadIn(iSh))-20.0);
		_sRadOut.push_back(_sRadIn.back()+100.0);
		_sThick.push_back(phcvtc->getShellThick(iSh));
		_sTheta.push_back(TMath::ATan(_sRadOut.back()/_zLength));
		std::cout<<"PHCV Shell: "<<iSh<<" in Rad "<<_sRadIn.back()<<" out Rad "<<_sRadOut.back()<<" thick "<<_sThick.back()<<" Theta acc "<<_sTheta.back()<<std::endl;
	}

}

void GMCTPhotonConvStudy::FindPhotonInter() {

	_phPrps.clear();
	_pairs.clear();

	Int_t nmctrack = fBrDataTrk->GetEntriesFast();
	for(int itk=0;itk<nmctrack;++itk){
		GMCGeantTrack* aMCTrack = ((GMCGeantTrack*)fBrDataTrk->At(itk));
		if(aMCTrack&&aMCTrack->GetfParentID()==0&&aMCTrack->GetfPDGCode()==22) {
			_phPrps.push_back(phProp());
			_phPrps.back().ids.TrkID = aMCTrack->GetfTrackID();
			_phPrps.back().ids.nTrk = itk;
			_phPrps.back().mom = *aMCTrack->GetfMomentum();
		}
	}
	for (int iph=0; iph<_phPrps.size(); ++iph) {
		for(int itk=0;itk<nmctrack;++itk){
			GMCGeantTrack* aMCTrack = ((GMCGeantTrack*)fBrDataTrk->At(itk));
			if(aMCTrack&&aMCTrack->GetfParentID()==_phPrps[iph].ids.TrkID) {
				TVector3 *phDVrt = aMCTrack->GetfPosStart();
				if (TMath::Abs(phDVrt->z())<_zLength && phDVrt->Perp()<_sRadOut.back()) {
					_phPrps[iph].intVert.push_back(*phDVrt);
					_phPrps[iph].secPart.push_back(pIDs());
					_phPrps[iph].secPart.back().TrkID = aMCTrack->GetfTrackID();
					_phPrps[iph].secPart.back().nTrk = itk;
				}
			}
		}

		std::vector<pIDs> &dPart = _phPrps[iph].secPart;
		for (int iDgt=0; iDgt<dPart.size(); ++iDgt) {
			GMCGeantTrack* aMCTrack = ((GMCGeantTrack*)fBrDataTrk->At(dPart[iDgt].nTrk));
			if (aMCTrack->GetfPDGCode()==11) {
				for (int jDgt=0; jDgt<dPart.size(); ++jDgt) {
					if (jDgt!=iDgt) {
						GMCGeantTrack* aMCTrackPs = ((GMCGeantTrack*)fBrDataTrk->At(dPart[jDgt].nTrk));
//						if (aMCTrackPs->GetfPDGCode()==-11 && (_phPrps[iph].intVert[iDgt]-_phPrps[iph].intVert[jDgt]).Mag()<1e-3 ) {
						if (aMCTrackPs->GetfPDGCode()==-11 && (_phPrps[iph].intVert[iDgt]==_phPrps[iph].intVert[jDgt]) ) {
							_pairs.push_back(pairProp());
							pairProp &pair = _pairs.back();
							pair.phId=iph;
							pair.elData.ids.TrkID=dPart[iDgt].TrkID;
							pair.elData.ids.nTrk=dPart[iDgt].nTrk;
							pair.psData.ids.TrkID=dPart[jDgt].TrkID;
							pair.psData.ids.nTrk=dPart[jDgt].nTrk;
							pair.vert = _phPrps[iph].intVert[iDgt];
							pair.elData.eStart = TMath::Sqrt(aMCTrack->GetfMomentum()->Mag2()+elMass2);
							pair.elData.momStart = *aMCTrack->GetfMomentum();
							pair.psData.eStart = TMath::Sqrt(aMCTrackPs->GetfMomentum()->Mag2()+elMass2);
							pair.psData.momStart = *aMCTrackPs->GetfMomentum();
							pair.elData.fitted=false;
							pair.elData.extrap=false;
							pair.psData.fitted=false;
							pair.psData.extrap=false;
						}
					}
				}
			}
		}
	}

	if (_fDebug>0) {
		std::cout<<"Event: "<<gAnalyzer->GetCurrentEventNumber()<<" n pairs found "<<_pairs.size()<<std::endl;
		for (int iPr=0; iPr<_pairs.size(); ++iPr) {
			std::cout<<"Pair tot E "<<_pairs[iPr].elData.eStart+_pairs[iPr].psData.eStart<<" vertex "<<_pairs[iPr].vert.X()<<" "<<_pairs[iPr].vert.Y()<<" "<<_pairs[iPr].vert.Z()<<std::endl;
		}
	}

}

void GMCTPhotonConvStudy::FindPairLoss() {

//	std::cout<<"Event: "<<gAnalyzer->GetCurrentEventNumber()<<" n pairs found "<<_pairs.size()<<std::endl;
	for (int iPr=0; iPr<_pairs.size(); ++iPr) {
		pairProp &pair = _pairs[iPr];
		pair.shellId=-1;
		pair.elData.eLossInRad=0;
		pair.elData.eLossInScint=0;
		pair.elData.timeScIn=-1.0;
		pair.psData.eLossInRad=0;
		pair.psData.eLossInScint=0;
		pair.psData.timeScIn=-1.0;
//		bool foundEl=false;
//		bool foundPs=false;
		bool notFoundEl=true;
		bool notFoundPs=true;
		for (int istep = 0;istep<fBrDataPHCV->GetEntries();istep++) {
			GMCPHCVGeantStep *aStep = (GMCPHCVGeantStep *)fBrDataPHCV->At(istep);
			if ( aStep->GetfTrackID()==pair.elData.ids.TrkID ) {
//				std::cout<<"IStep "<<istep<<" Shell "<<aStep->GetfChamberNr()<<" time "<<aStep->GetfGlobalTime()<<" pos "<<aStep->GetfPos()->X()<<" "<<aStep->GetfPos()->Y()<<" "<<aStep->GetfPos()->Z()<<std::endl;
				if ( notFoundEl || pair.shellId==aStep->GetfChamberNr()) {
					if (pair.elData.timeScIn<0) {
						notFoundEl=false;
						pair.elData.timeScIn=aStep->GetfGlobalTime();
						pair.elData.vertScIn=*aStep->GetfPos();
						pair.elData.eLossInRad=pair.elData.eStart-TMath::Sqrt(aStep->GetfMomentum()->Mag2()+elMass2);
						pair.shellId=aStep->GetfChamberNr();
						//					std::cout<<"Theta "<<aStep->GetfMomentum()->Theta()<<" - "<<TMath::ATan(aStep->GetfMomentum()->Perp()/aStep->GetfMomentum()->Z())<<std::endl;
					}
					if ( (aStep->GetfGlobalTime()-pair.elData.timeScIn)<0.01 ) { //take only the steps of the first cross
						pair.elData.eLossInScint+=aStep->GetfEdep()+aStep->GetfNoIEdep();
						pair.elData.vertScOut=*aStep->GetfPosEnding();
						pair.elData.momInSc=*aStep->GetfMomentum();
						//					std::cout<<"Ending pos "<<aStep->GetfPosEnding()->X()<<" "<<aStep->GetfPosEnding()->Y()<<" "<<aStep->GetfPosEnding()->Z()<<std::endl;
					} /*else {
						foundEl=true;
				    }*/
				}
			} else if ( aStep->GetfTrackID()==pair.psData.ids.TrkID ) {
				if ( notFoundPs || pair.shellId==aStep->GetfChamberNr()) {
					if (pair.psData.timeScIn<0) {
						notFoundPs=false;
						pair.psData.timeScIn=aStep->GetfGlobalTime();
						pair.psData.vertScIn=*aStep->GetfPos();
						pair.psData.eLossInRad=pair.psData.eStart-TMath::Sqrt(aStep->GetfMomentum()->Mag2()+elMass2);
					}
					//				std::cout<<"IStep "<<istep<<" time "<<aStep->GetfGlobalTime()<<" pos "<<aStep->GetfPos()->X()<<" "<<aStep->GetfPos()->Y()<<" "<<aStep->GetfPos()->Z()<<std::endl;
					if ( (aStep->GetfGlobalTime()-pair.psData.timeScIn)<0.01 ) { //take only the steps of the first cross
						pair.psData.eLossInScint+=aStep->GetfEdep()+aStep->GetfNoIEdep();
						pair.psData.vertScOut=*aStep->GetfPosEnding();
						pair.psData.momInSc=*aStep->GetfMomentum();
					} /*else {
						foundPs=true;
					}*/
				}
			}
//			if (foundEl&&foundPs) { break; }
		}
	}
}

void GMCTPhotonConvStudy::FindPairDCHHits() {

	Int_t nhitsdch = fBrHitsDC->GetEntriesFast();

	for (int iPr=0; iPr<_pairs.size(); ++iPr) {
		pairProp &pair = _pairs[iPr];

		//sort traks DCH hits by tof
		std::vector<std::pair<double,int> > elHitvec;
		std::vector<std::pair<double,int> > psHitvec;
		for(int ihit=0;ihit<nhitsdch;ihit++){
			GMCDCHHit *ahit = (GMCDCHHit*) fBrHitsDC->At(ihit);
			//only hits beloging to primaries track
			if (pair.elData.ids.TrkID==ahit->GetfTrkIDofHit()) {
				elHitvec.push_back(std::pair<double,int >(ahit->GetfGlobalTime(),ihit));
			} else if (pair.psData.ids.TrkID==ahit->GetfTrkIDofHit()) {
				psHitvec.push_back(std::pair<double,int >(ahit->GetfGlobalTime(),ihit));
			}
		}
	    std::sort(elHitvec.begin(),elHitvec.end());
	    std::sort(psHitvec.begin(),psHitvec.end());

	    Int_t elSuperLay=-1;
	    if (_fDebug>=2) {
	    	std::cout<<"electron in shell "<<pair.shellId<<" time in Scint "<<pair.elData.timeScIn<<std::endl;
	    }
		for(int ihit=0;ihit<elHitvec.size();ihit++){
			if (_fDebug>=2) {
				GMCDCHHit *ahit = (GMCDCHHit*) fBrHitsDC->At(elHitvec[ihit].second);
				fGeometry->GetCellHandle()->SelectCellDet(ahit->GetfCellId());
				int tmpSuperLay=fGeometry->GetCellHandle()->GetSuperLayer();
				std::cout<<"DCH Hits id: "<<elHitvec[ihit].second<<" time "<<elHitvec[ihit].first<<" SpL "<<tmpSuperLay<<" pos "<<ahit->GetfEntranceX()<<" "<<ahit->GetfEntranceY()<<" "<<ahit->GetfEntranceZ()<<std::endl;
			}

			if ( elHitvec[ihit].first> pair.elData.timeScIn ) {
				pair.elData.dchHitsAllSL.push_back(elHitvec[ihit].second);
				GMCDCHHit *ahit = (GMCDCHHit*) fBrHitsDC->At(elHitvec[ihit].second);
				fGeometry->GetCellHandle()->SelectCellDet(ahit->GetfCellId());
				int tmpSuperLay=fGeometry->GetCellHandle()->GetSuperLayer();
				if (elSuperLay<0) {
					elSuperLay=tmpSuperLay;
					pair.elData.dchHits.push_back(elHitvec[ihit].second);
				} else {
					tmpSuperLay-=elSuperLay;
					if ( tmpSuperLay>0 && tmpSuperLay<_OSnSuperLayer[pair.shellId] ) {
						pair.elData.dchHits.push_back(elHitvec[ihit].second);
					}
				}
			}

		}
	    if (_fDebug>=2) {
	    	std::cout<<"Selected DCH nHits "<<pair.elData.dchHits.size()<<std::endl;
	    }

	    Int_t psSuperLay=-1;
	    if (_fDebug>=2) {
	    	std::cout<<"positron in shell "<<pair.shellId<<" time in Scint "<<pair.psData.timeScIn<<std::endl;
	    }
		for(int ihit=0;ihit<psHitvec.size();ihit++){
			if (_fDebug>=2) {
				GMCDCHHit *ahit = (GMCDCHHit*) fBrHitsDC->At(psHitvec[ihit].second);
				fGeometry->GetCellHandle()->SelectCellDet(ahit->GetfCellId());
				int tmpSuperLay=fGeometry->GetCellHandle()->GetSuperLayer();
				std::cout<<"DCH Hits id: "<<psHitvec[ihit].second<<" time "<<psHitvec[ihit].first<<" SpL "<<tmpSuperLay<<" pos "<<ahit->GetfEntranceX()<<" "<<ahit->GetfEntranceY()<<" "<<ahit->GetfEntranceZ()<<std::endl;
			}

			if ( psHitvec[ihit].first> pair.psData.timeScIn ) {
				pair.psData.dchHitsAllSL.push_back(psHitvec[ihit].second);
				GMCDCHHit *ahit = (GMCDCHHit*) fBrHitsDC->At(psHitvec[ihit].second);
				fGeometry->GetCellHandle()->SelectCellDet(ahit->GetfCellId());
				int tmpSuperLay=fGeometry->GetCellHandle()->GetSuperLayer();
				if (psSuperLay<0) {
					psSuperLay=tmpSuperLay;
					pair.psData.dchHits.push_back(psHitvec[ihit].second);
				} else {
					tmpSuperLay-=psSuperLay;
					if ( tmpSuperLay>0 && tmpSuperLay<_OSnSuperLayer[pair.shellId] ) {
						pair.psData.dchHits.push_back(psHitvec[ihit].second);
					}
				}
			}

		}
	    if (_fDebug>=2) {
	    	std::cout<<"Selected DCH nHits "<<pair.psData.dchHits.size()<<std::endl;
	    }

	}

}

void GMCTPhotonConvStudy::doPlot() {

	Int_t nGoodPhoton=0;
	double photonTheta=-1000;
	double photonPhi=-1000;
	for (int iPr=0; iPr<_pairs.size(); ++iPr) {
		++nGoodPhoton;
		pairProp &pair = _pairs[iPr];
		GMCGeantTrack* aMCTrack = ((GMCGeantTrack*)fBrDataTrk->At(_phPrps[pair.phId].ids.nTrk));
		photonTheta=aMCTrack->GetfMomentum()->Theta();
		photonPhi=aMCTrack->GetfMomentum()->Phi();
		GetHConvEnergy()->Fill(aMCTrack->GetfMomentum()->Mag());
		GetHPairEnergy()->Fill(pair.elData.eStart+pair.psData.eStart);
		double elEOutRad=pair.elData.eStart-pair.elData.eLossInRad;
		double psEOutRad=pair.psData.eStart-pair.psData.eLossInRad;
		double elEOutSct=elEOutRad-pair.elData.eLossInScint;
		double psEOutSct=psEOutRad-pair.psData.eLossInScint;
		double EPairRad=elEOutRad+psEOutRad;
		double EPairSct=elEOutSct+psEOutSct;
		GetHPairEnergyRad()->Fill(EPairRad);
		GetHPairEnergySct()->Fill(EPairSct);
		GetHPairEnergyRadDiff()->Fill(pair.shellId,EPairRad);
		GetHPairEnergySctDiff()->Fill(pair.shellId,EPairSct);
		//------------ The angle should be fixed for the curvature due to B field!!!!!!!! ------------
		double elThOutRad=pair.elData.momInSc.Theta()-pair.elData.momStart.Theta();
		double elPhiOutRad=pair.elData.momInSc.Phi()-pair.elData.momStart.Phi();
		double psThOutRad=pair.psData.momInSc.Theta()-pair.psData.momStart.Theta();
		double psPhiOutRad=pair.psData.momInSc.Phi()-pair.psData.momStart.Phi();
		double pairThOutRad=pair.elData.momInSc.Theta()+pair.psData.momInSc.Theta();
		pairThOutRad*=0.5;
		pairThOutRad-=photonTheta;
		double pairPhiOutRad=pair.elData.momInSc.Phi()+pair.psData.momInSc.Phi();
		pairPhiOutRad*=0.5;
		pairPhiOutRad-=photonPhi;
//		double pairThOutSct=pair.elData.mcMom0.Theta()+pair.psData.mcMom0.Theta();
//		pairThOutSct*=0.5;
//		pairThOutSct-=photonTheta;
//		double pairPhiOutSct=pair.elData.mcMom0.Phi()+pair.psData.mcMom0.Phi();
//		pairPhiOutSct*=0.5;
//		pairPhiOutSct-=photonPhi;
		GetHPairThetaRad()->Fill(pairThOutRad*1000.0);
		GetHThetaRad_El()->Fill(elThOutRad*1000.0);
		GetHThetaRad_Ps()->Fill(psThOutRad*1000.0);
		GetHPairPhiRad()->Fill(pairPhiOutRad*1000.0);
		GetHPhiRad_El()->Fill(elPhiOutRad*1000.0);
		GetHPhiRad_Ps()->Fill(psPhiOutRad*1000.0);
//		GetHPairThetaSct()->Fill(pairThOutSct*1000.0);
//		GetHPairPhiSct()->Fill(pairPhiOutSct*1000.0);
		//------------------------
		GetHConvHitPerShell()->Fill(pair.shellId);
		GetHConvProbPerShell()->Fill(pair.shellId);
		GetHGammaPerShell()->Fill(pair.shellId);
		GetHEnergyOut_El()->Fill(pair.shellId,elEOutSct);
		GetHElossInRadPerShell_El()->Fill(pair.shellId,pair.elData.eLossInRad);
		GetHElossInSctPerShell_El()->Fill(pair.shellId,pair.elData.eLossInScint);
		GetHEnergyOut_Ps()->Fill(pair.shellId,psEOutSct);
		GetHElossInRadPerShell_Ps()->Fill(pair.shellId,pair.psData.eLossInRad);
		GetHElossInSctPerShell_Ps()->Fill(pair.shellId,pair.psData.eLossInScint);
		int elDCHhits=pair.elData.dchHits.size();
//		if (elDCHhits>0) {
			GetHElHits()->Fill(elDCHhits);
			GetHElHitsDiff()->Fill(pair.shellId,elDCHhits);
//		}
		int psDCHhits=pair.psData.dchHits.size();
//		if (psDCHhits>0) {
			GetHPsHits()->Fill(psDCHhits);
			GetHPsHitsDiff()->Fill(pair.shellId,psDCHhits);
//		}
		if (elDCHhits>5&&psDCHhits>5) {
			GetHPairEnergyRec()->Fill(EPairSct);
			GetHPairEnergyRecDiff()->Fill(pair.shellId,EPairSct);
		}

		double ERecPair0=0;
		double ERecPair=0;
		double mcERecPair0=0;
		if (pair.elData.fitted) {
			ERecPair0+=TMath::Sqrt(pair.elData.recMom0.Mag2()+elMass2);
			mcERecPair0+=TMath::Sqrt(TMath::Power(pair.elData.mcMom0,2)+elMass2);
			if (pair.elData.extrap) { ERecPair+=TMath::Sqrt(pair.elData.recMom.Mag2()+elMass2); }
		}
		if (pair.psData.fitted) {
			ERecPair0+=TMath::Sqrt(pair.psData.recMom0.Mag2()+elMass2);
			mcERecPair0+=TMath::Sqrt(TMath::Power(pair.psData.mcMom0,2)+elMass2);
			if (pair.psData.extrap) ERecPair+=TMath::Sqrt(pair.psData.recMom.Mag2()+elMass2);
		}
		if (pair.elData.fitted||pair.psData.fitted) {
			GetHRecPairEnergy()->Fill(ERecPair0);
			GetHRecPairEnergyDiff()->Fill(pair.shellId,ERecPair0);
			GetHRecPairEnergyDebg()->Fill(mcERecPair0);
			if (pair.elData.extrap||pair.psData.extrap) {
				GetHRecPairEnergyS()->Fill(ERecPair);
				GetHRecPairEnergySDiff()->Fill(pair.shellId,ERecPair);
			}
		}
		if (pair.elData.fitted&&pair.psData.fitted) {
			GetHRecPair2EnergyDiff()->Fill(pair.shellId,ERecPair0);
			if (pair.elData.extrap&&pair.psData.extrap) {
				GetHRecPair2EnergySDiff()->Fill(pair.shellId,ERecPair);
			}
			double elMomRes = pair.elData.mcMom0-pair.elData.recMom0.Mag();
			double psMomRes = pair.psData.mcMom0-pair.psData.recMom0.Mag();
			if (TMath::Abs(elMomRes)<0.2 && TMath::Abs(psMomRes)<0.2) {
				GetHRecPair2EnergyDiff_1()->Fill(pair.shellId,ERecPair0);
				if (pair.elData.extrap&&pair.psData.extrap) {
					GetHRecPair2EnergySDiff_1()->Fill(pair.shellId,ERecPair);
				}
			}
		}

		double EPairHit0=0;
		double EPairScnt=0;
		TVector3 Dxy;
		vertChek &vertc = _pairsInfo[iPr];
		if(vertc.nPart>0) {
			EPairHit0+=TMath::Sqrt(vertc.momIn0[0].Mag2()+elMass2);
			EPairScnt+=TMath::Sqrt(vertc.momIn[0].Mag2()+elMass2);
			Dxy=vertc.xyzIn[0]-vertc.xyzIn0[0];
			GetHPairVertZResHt0ScDebg()->Fill(pair.shellId,Dxy.Z());
			Dxy.SetZ(0.0);
			GetHPairVertXYResHt0ScDebg()->Fill(pair.shellId,Dxy.Mag());
			GetHPairMomReasHt0ScDebg()->Fill(pair.shellId,(vertc.momIn[0].Mag()-vertc.momIn0[0].Mag()));
			if(vertc.nPart>1) {
				EPairHit0+=TMath::Sqrt(vertc.momIn0[1].Mag2()+elMass2);
				EPairScnt+=TMath::Sqrt(vertc.momIn[1].Mag2()+elMass2);
				Dxy=vertc.xyzIn[1]-vertc.xyzIn0[1];
				GetHPairVertZResHt0ScDebg()->Fill(pair.shellId,Dxy.Z());
				Dxy.SetZ(0.0);
				GetHPairVertXYResHt0ScDebg()->Fill(pair.shellId,Dxy.Mag());
				GetHPairMomReasHt0ScDebg()->Fill(pair.shellId,(vertc.momIn[1].Mag()-vertc.momIn0[1].Mag()));
			}
		}
		GetHPairEnergyHit0Debg()->Fill(pair.shellId,EPairHit0);
		GetHPairEnergyScntDebg()->Fill(pair.shellId,EPairScnt);

	}

	double pairThOutSct=-1000.0;
	double pairPhiOutSct=-1000.0;
	for (int iTrkPM=0; iTrkPM<_trkPairMap.size(); ++iTrkPM) {
	  vertChek &vertc = _pairsInfo[_trkPairMap[iTrkPM].first];
	  if (_trkPairMap[iTrkPM].second==0) {
		  pairThOutSct=vertc.momIn0[vertc.nPart].Theta();
		  pairPhiOutSct=vertc.momIn0[vertc.nPart].Phi();
	  } else {
		  pairThOutSct+=vertc.momIn0[vertc.nPart].Theta();
		  pairPhiOutSct+=vertc.momIn0[vertc.nPart].Phi();
		  pairThOutSct*=0.5;
		  pairThOutSct-=_phPrps[_pairs[_trkPairMap[iTrkPM].first].phId].mom.Theta();
		  pairPhiOutSct*=0.5;
		  pairPhiOutSct-=_phPrps[_pairs[_trkPairMap[iTrkPM].first].phId].mom.Phi();

		  GetHPairThetaSct()->Fill(pairThOutSct*1000.0);
		  GetHPairPhiSct()->Fill(pairPhiOutSct*1000.0);
	  }
	}

	for (int iph=0; iph<_phPrps.size(); ++iph) {
		bool noPair=true;
		for (int iPr=0; iPr<_pairs.size(); ++iPr) {
			if (_pairs[iPr].phId==iph) {
				noPair=false;
				break;
			}
		}
		if (noPair) {
			bool crossAShell=false;
			GMCGeantTrack* aMCTrack = ((GMCGeantTrack*)fBrDataTrk->At(_phPrps[iph].ids.nTrk));
			double phTheta=TMath::Abs(TMath::ATan(aMCTrack->GetfMomentum()->Perp()/aMCTrack->GetfMomentum()->Z()));
			for (int iShl=0; iShl<_nShells; ++iShl) {
				if (phTheta>_sTheta[iShl]) {
					crossAShell=true;
					GetHGammaPerShell()->Fill(iShl);
				}
			}
			if (crossAShell) {
				++nGoodPhoton;
				photonTheta=aMCTrack->GetfMomentum()->Theta();
			}
		}
	}
	if (nGoodPhoton>0) { GetHThetaGamma()->Fill(photonTheta*TMath::RadToDeg()); }
}

void GMCTPhotonConvStudy::FitPairs() {

	FindFittableTracks();
	for(int i=0;i<gAnalyzer->GetRecoTracksSize();++i){
		FitTrack(i);
	}

}

void GMCTPhotonConvStudy::FindFittableTracks(){

	_trkPairMap.clear();
	_pairsInfo.clear();

	int nFtTrk=0;
	for (int iPr=0; iPr<_pairs.size(); ++iPr) {
		pairProp &pair = _pairs[iPr];
		if (pair.elData.dchHitsAllSL.size()>5) { ++nFtTrk; }
		if (pair.psData.dchHitsAllSL.size()>5) { ++nFtTrk; }
		_pairsInfo.push_back(vertChek());
		_pairsInfo.back().nPart=0;
	}
	gAnalyzer->SetRecoTracksSize(nFtTrk);

//	  for(int i=0;i<nFtTrk;i++){
	int i=-1;
	for (int iPr=0; iPr<_pairs.size(); ++iPr) {
		pairProp &pair = _pairs[iPr];
		for (int iPart=0; iPart<2; ++iPart) {
			partData *tpart;
			if (iPart==0) { tpart = &pair.elData; }
			else  { tpart = &pair.psData; }
			if (tpart->dchHitsAllSL.size()>5) {
				++i;
				_trkPairMap.push_back(std::pair<int,int>(iPr,iPart));
				GMCRecoTracks *aTrk = gAnalyzer->GetRecoTracksAt(i);
				aTrk->SetTrkID(tpart->ids.TrkID);
				aTrk->Getcov()->ResizeTo(6,6);

				unsigned int ntrkhits=tpart->dchHitsAllSL.size()+1;//+1 for pair vertex
				aTrk->Setnhits(ntrkhits);
				aTrk->SethitindexSize(ntrkhits);
				aTrk->SetdetidSize(ntrkhits);
				aTrk->SetIsFitted(false);
				aTrk->Getcov()->ResizeTo(6,6);

				//already sorted by tof

				int detid=1;
				aTrk->SetdetidAt(0,detid);
				aTrk->SethitindexAt(0,0);
				for(unsigned int ihit=1;ihit<ntrkhits;++ihit){
					detid=0;
					aTrk->SetdetidAt(ihit,detid);
					aTrk->SethitindexAt(ihit,tpart->dchHitsAllSL[ihit-1]);
				}

				if (_fitOnlyFst) {
					//fit only first turn
					double trkdir_prev=1.;
					for(unsigned int ihit=1;ihit<ntrkhits;++ihit){

						TVector3 xyz,k;
//						GMCDCHHit *aHit = (GMCDCHHit*) fBrHitsDC->At(tpart->dchHitsAllSL[ihit-1]);
						GMCDCHHit *aHit = (GMCDCHHit*) fBrHitsDC->At(aTrk->GethitindexAt(ihit));

						xyz=TVector3(aHit->GetfEntranceX(),aHit->GetfEntranceY(),0);//aHit->GetfEntranceZ());
						k=TVector3(aHit->GetfEntranceMomX(),aHit->GetfEntranceMomY(),0);//aHit->GetfEntranceMomZ());

						double trkdir=xyz.Dot(k)/xyz.Mag()/k.Mag();
						if(trkdir_prev<=0&&trkdir>0){
							aTrk->Setnhits(ihit-1);
							break;
						}
						trkdir_prev=trkdir;
					}
				}

			}//close n hits>5
		}//close part
	}//close pair

}

bool GMCTPhotonConvStudy::FitTrack(int iTrk) {

	GMCRecoTracks *aTrack = gAnalyzer->GetRecoTracksAt(iTrk);
	  int nhits = aTrack->Getnhits();

	  pairProp &pair = _pairs[_trkPairMap[iTrk].first];
	  partData *tpart;
	  int pdgid=11;
	  if (_trkPairMap[iTrk].second==1) {
		  pdgid=-11;
		  tpart=&pair.psData;
	  } else {
		  tpart=&pair.elData;
	  }

//	  if (nhits < 6) {
//	    printf(" ****  !!! Event not reconstructable: number of hits forr signal track less than 6!\n");
//	    return false;
//	  }

	  int trkid=0;
	  TVector3 xyzhit0,momhit0;
	  GMCDCHHit *ahit0 = (GMCDCHHit*) fBrHitsDC->At(aTrack->GethitindexAt(1));
	  xyzhit0=0.1*TVector3(ahit0->GetfEntranceX(),ahit0->GetfEntranceY(),ahit0->GetfEntranceZ()); // mm -> cm
	  momhit0=0.001*TVector3(ahit0->GetfEntranceMomX(),ahit0->GetfEntranceMomY(),ahit0->GetfEntranceMomZ()); // MeV -> GeV
//	  trkid=ahit0->GetfTrkIDofHit();
//	  xyzhit0=0.1*(tpart->vertScIn+tpart->vertScOut)*0.5;
//	  momhit0=0.001*tpart->momInSc;
	  TVector3 xyzInSc=0.1*(tpart->vertScIn+tpart->vertScOut)*0.5;
	  TVector3 momInSc=0.001*tpart->momInSc;
	  trkid=tpart->ids.TrkID;
//	  std::cout<<"momhit0 "<<momhit0.X()<<" "<<momhit0.Y()<<" "<<momhit0.Z()<<std::endl;
//	  std::cout<<"momInSc "<<momInSc.X()<<" "<<momInSc.Y()<<" "<<momInSc.Z()<<std::endl;

	  vertChek &vertc = _pairsInfo[_trkPairMap[iTrk].first];
	  vertc.momIn[vertc.nPart]=momInSc*1000.0;
	  vertc.momIn0[vertc.nPart]=momhit0*1000.0;
	  vertc.xyzIn[vertc.nPart]=xyzInSc*10.0;
	  vertc.xyzIn0[vertc.nPart]=xyzhit0*10.0;
	  vertc.nPart++;

	  //init values
	  TVector3 xyz0=xyzhit0;//fVert;
	  TVector3 pdir0=momhit0;
	  pdir0.SetMag(1.);
//	  TVector3 pmom0=momhit0;//
	  TVector3 pmom0=0.001*tpart->momStart;//

	  //Do initialization of initial covariance matrix with sigma_p,sigma_angle, 1cm position
	  TMatrixDSym Cov(6); Cov.Zero();

	  Cov[0][0] = Cov[1][1] =Cov[2][2] = 0.1*0.1;//centimeters
	  Cov[3][3] = Cov[4][4] =Cov[5][5] = 0.001*0.001;//0.001*0.001;//GeV

	  //-----------------------------------
	  //init positron track with parameters from first State Vector and predefined Covariance matrix
	  //-----------------------------------
//	  pairProp &pair = _pairs[_trkPairMap[iTrk].first];
//	  partData *tpart;
//	  int pdgid=11;
//	  if (_trkPairMap[iTrk].second==1) {
//		  pdgid=-11;
//		  tpart=&pair.psData;
//	  } else {
//		  tpart=&pair.elData;
//	  }

	  //
	  if(_fDebug)
	    std::cout<<"set charge to pdgid ="<<pdgid<<std::endl;
	  AbsTrackRep *rep = new RKTrackRep(pdgid);
	  MeasuredStateOnPlane stateRef(rep);
//	  rep->setPosMomCov(stateRef,xyz0,pmom0,Cov);
	  rep->setPosMomCov(stateRef,xyz0,momhit0,Cov);
//	  stateRef.setTime(0.);//or Pair Vertex time?
	  stateRef.setTime(tpart->timeScIn);//or Pair Vertex time?

	  double sigmaVtime=GetSP()->GetsigmaVtime();
	  //init genfit track from initial state
	  TVectorD seedState(6);
	  TMatrixDSym seedCov(6);
	  rep->get6DStateCov(stateRef, seedState, seedCov);
	  Track fitTrack(rep,seedState, seedCov);
//	  fitTrack.setTimeSeed(0.);//or Pair Vertex time?
	  fitTrack.setTimeSeed(tpart->timeScIn+gRandom->Gaus()*sigmaVtime);//or Pair Vertex time?


	  //-----------------------------------------------
	  //fill hit information
	  //-----------------------------------------------
	  int nid=0;

	  //add vertex constrain
	  double sigmaDVertX=GetSP()->GetsigmaDVertX();
	  double sigmaDVertY=GetSP()->GetsigmaDVertY();
	  double sigmaDVertZ=GetSP()->GetsigmaDVertZ();
//	  if(sigmaDVertX>0){
	    TVectorD hitCoords(3);
	    TMatrixDSym hitCov(3);
	    hitCoords(0)=xyz0.X()+gRandom->Gaus()*sigmaDVertX;
	    hitCoords(1)=xyz0.Y()+gRandom->Gaus()*sigmaDVertY;
	    hitCoords(2)=xyz0.Z()+gRandom->Gaus()*sigmaDVertZ;
	    hitCov(0,0) = sigmaDVertX*sigmaDVertX;
	    hitCov(1,1) = sigmaDVertY*sigmaDVertY;
	    hitCov(2,2) = sigmaDVertZ*sigmaDVertZ;
	    genfit::SpacepointMeasurement* measurement = new genfit::SpacepointMeasurement(hitCoords, hitCov, 10, 100000, NULL);
	    fitTrack.insertMeasurement(measurement,nid++);
	    if(_fDebug)
	      std::cout<<" add vertex constrain " <<hitCoords(0)<<" "<<hitCoords(1)<<" "<<hitCoords(2)<< " with +- "<<sigmaDVertX<<"  "<<sigmaDVertY<<"  "<<sigmaDVertZ<<std::endl;
//	  }

	  int nhitsdch=0;

	  for(int ihit=1;ihit<nhits;++ihit){

		  nhitsdch++;
		  GMCDCHHit *ahit =(GMCDCHHit*) fBrHitsDC->At(aTrack->GethitindexAt(ihit));

		  Int_t nwire=ahit->GetfCellId();

		  fGeometry->GetCellHandle()->SelectCellDet(nwire);
		  const CLHEP::Hep3Vector& mid   = fGeometry->GetCellHandle()->GetCellCenter();
		  const CLHEP::Hep3Vector& w     = fGeometry->GetCellHandle()->GetCellDirection();

		  TVector3 wire_pos(mid.x(),mid.y(),mid.z());
		  TVector3 wire_dir(w.x(),w.y(),w.z());

		  Double_t b_meas = ahit->GetfImpact();

		  //fill mesuarements
		  TVector3 p0 = 0.1*wire_pos;
		  TVector3 w_axis = wire_dir;
		  double wnorm=w_axis.Mag();
		  TVectorD hitCoords(8);
		  TMatrixDSym hitCov(8);
		  //fill wire ends
		  for(int i=0;i<3;i++){
			  hitCoords(i)=(p0+100*w_axis)(i);
			  hitCoords(i+3)=(p0-100*w_axis)(i);
		  }
		  //mesuared values dist,Z:
		  double sigmab=GetSP()->GetSigmaRPhi();
		  hitCoords(6) = b_meas*0.1+gRandom->Gaus()*sigmab;
		  //z mesuarment are relative to wire1 in direction of wire2
		  hitCoords(7) = 100.*wnorm;
		  hitCov(6,6) = sigmab*sigmab;
		  hitCov(7,7) = 100*100;
		  WireMeasurement *whit;
		  if(1==0){//use Z
			  whit=new WirePointMeasurement(hitCoords, hitCov,0,ihit,nullptr);
		  }else{
			  whit=new WireMeasurement(hitCoords, hitCov,0,ihit,nullptr);
		  }
		  whit->setLeftRightResolution(0);//1>0?1:-1);
		  fitTrack.insertMeasurement(whit,nid++);

		  if(_fDebug>1){
			  std::cout<<"add "<<ihit<<" wire "<<nwire<<" xywire "<<p0.X()<<" "<<p0.Y()<<" "<<p0.z()
	            		 <<" bl "<<hitCoords(6)<<" sigma "<<sqrt(hitCov(6,6))<<" "<<sqrt(hitCov(7,7))<<" xyz "
						 <<0.1*ahit->GetfEntranceX()<<" "<<0.1*ahit->GetfEntranceY()<<" "<<0.1*ahit->GetfEntranceZ()<<" T "<<ahit->GetfGlobalTime()<<std::endl;
		  }
	  }

	  // ---- end of hit info filling ------------------------

	  assert(fitTrack.checkConsistency());
	  genfit::FitStatus* fitStatus=fitTrack.getFitStatus(rep);
	  if(_fDebug){
	    std::cout<<" init chi2 "<<fitStatus->getChi2()<<" nfailed "<<fitStatus->getNFailedPoints()
	             <<" P= "<<rep->getMomMag(stateRef)*rep->getCharge(stateRef)<<" +- "<<sqrt(rep->getMomVar(stateRef))
	         <<" nhits "<<nid<<" from "<<nhits<<" dch "<<nhitsdch<<std::endl;
	    std::cout<<"init state "<<xyz0<<" mom hit0 "<<momhit0<<" mom Start "<<pmom0<<std::endl;
	  }

	  //choose different fitter
	  genfit::AbsKalmanFitter* fitter = 0;
	  switch(0/*GetSP()->GetKalmanType()*/){
	  case 0:
	    fitter=new DAF();
	    break;
	  case 1:
	    fitter=new KalmanFitterRefTrack(20);
	    break;
	  case 2:
	    fitter=new KalmanFitter(20);
	    break;
	  }
	  /*
	  if(GetSP()->GetKalmanType()>0){
	    //use LR from patter recognition in case of usual KalmanFitter
	    fitter->setMultipleMeasurementHandling(genfit::weightedClosestToPredictionWire);
	  }*/

	  if(_fDebug>20){
	    fitter->setDebugLvl(10);
	    rep->setDebugLvl(10);
	  }

	  //-----------------------------------
	  // FIT IT
	  //-----------------------------------

	  try{
	    fitter->processTrack(&fitTrack);
	  }catch(Exception& e){
	    if(_fDebug) std::cout<<"on fitting "<<e.what()<<std::endl;
	  }
	  fitStatus=fitTrack.getFitStatus(rep);
	  bool fitstatus=fitStatus->isFitConverged()&&(fitStatus->getNFailedPoints()==0);
	  if(_fDebug) fitStatus->Print();


	  TVector3 pos,mom;
	  TMatrixDSym cov(6);
	  TVector3 pos2,mom2;
	  TMatrixDSym cov2(6);
	  double pmom=0;
	  double tof=0;
	  double sigmap=0.;

	  //-----------------------------------
	  // fill info from first hit
	  //-----------------------------------

	  try{
	    const MeasuredStateOnPlane& stFirst=fitTrack.getFittedState();
	    pmom=rep->getMomMag(stFirst)*rep->getCharge(stFirst);
	    stFirst.getPosMomCov(pos,mom,cov);
	    tof=stFirst.getTime();
	  }catch(Exception& e){
	    if(_fDebug) std::cout<<"on getting state "<<e.what()<<std::endl;
	  }


	  if(_fDebug)
	    std::cout<<" chi2 "<<fitStatus->getChi2()<<" nfailed "<<fitStatus->getNFailedPoints()
	             <<" P= "<<pmom<<" fitok="<<fitstatus<<std::endl;

	  if(_fDebug){std::cout<<"fitted xyz "<<pos<<" mom "<<mom<<std::endl;}

	  //-----------------------------------
	  //set states on hits
	  //-----------------------------------

	  unsigned int numhits = fitTrack.getNumPoints();

	  int ngoodhits=0;
	  int ngoodhitsdch=0;
	  double sumddist=0;

	  aTrack->SetStateVectorSize(nhits);
	  aTrack->GetSkipped().resize(nhits,true);

	  if(fitstatus){
	    try{
	      for(unsigned int jhit=0;jhit<numhits;jhit++){
	        TrackPoint* point = fitTrack.getPoint(jhit);
	        int ihit=point->getRawMeasurement(0)->getHitId();
	        int detid=point->getRawMeasurement(0)->getDetId();
	        //skip virtual hit
	    //        if(ihit>=1000) continue;

	        //skip point without fitted information
	        if(!point->hasFitterInfo(rep)) continue;

	        KalmanFitterInfo* fi = static_cast<KalmanFitterInfo*>(point->getFitterInfo(rep));
	        //get weights for LR combinations
	        std::vector<double> dafWeights = fi->getWeights();

	        //maximum weigth in fitting from LR positions
	        int imaxdaf=std::distance(dafWeights.begin(),
	                                  std::max_element(dafWeights.begin(),dafWeights.end()));
	        double maxdaf=dafWeights[imaxdaf];


	    //fill histogram with final chi2 per hit
	    const MeasurementOnPlane& residual = fi->getResidual(imaxdaf, false, false);
	    const TVectorD& resid(residual.getState());

	        //dont fill StateVector forr skipped hits during fitting(in normal case hit must have weight=1.)
	        if(maxdaf<0.5) {
	          if(_fDebug>9) std::cout<<"skip point "<<std::endl;
	          continue;
	        }

	        //fill biased best StateVector on hit
	        // (averaged from forward-backward propagation)
	    const MeasuredStateOnPlane& state=fi->getFittedState(true);
	    state.getPosMomCov(pos,mom,cov);

	    //unbiased
	    const MeasuredStateOnPlane& state2=fi->getFittedState(false);
	    state2.getPosMomCov(pos2,mom2,cov2);

	    if(detid==0){
	      //average T0 shift over hits
	      sumddist+=resid(0)*(imaxdaf?1:-1);
	          ngoodhitsdch++;
	        }/*else if(detid==1){
	          ngoodhitssvx++;
	        }else if(detid==2){
	          ngoodhitspshw++;
	        }*/

	    if(detid<10){ //wire or svx or pshw hit
	      aTrack->SetSkippedAt(ihit,false);
	          *aTrack->GetStateVectorAt(ihit)=pos;
	      ngoodhits++;
	          if(detid==0){
	            GMCDCHHit *ahit = (GMCDCHHit*) fBrHitsDC->At(aTrack->GethitindexAt(ihit));
	            if(_fDebug>9)
	              std::cout<<detid<<" "<<ihit<<" track aver "<<state.getState()(3)<<" mes "<<0.1*ahit->GetfImpact()<<" dR not biased "<<resid(0)
	                  <<" biased "<<0.1*ahit->GetfImpact()-fabs(state.getState()(3))<<" trk xyz "<<pos<<" mom "<<mom.Y()<<std::endl;
	            GetHResidualUnbiased()->Fill(10*fabs(state.getState()(3)),10*resid(0));
	            GetHResidualBiased()->Fill(10*fabs(state.getState()(3)),ahit->GetfImpact()-10*fabs(state.getState()(3)));
	          }/*else if(detid==1){
	            GMCSVXHit *ahit = (GMCSVXHit*) fBrHitsSVX->At(aTrack->GethitindexAt(ihit));
	            if(_fDebug>9)
	              std::cout<<detid<<" "<<ihit<<" track aver "<<state.getState()(3)<<" "<<state.getState()(4)<<" exact mes 0 0 dy,dz not biased "<<resid(0)<<" "<<resid(1)
	                  <<" trk xyz "<<pos<<" mom "<<mom.Y()<<std::endl;
	          }else if(detid==2){
	            GMCPSHWHit *ahit = (GMCPSHWHit*) fBrHitsPSHW->At(aTrack->GethitindexAt(ihit));
	            if(_fDebug>9)
	              std::cout<<detid<<" "<<ihit<<" track aver "<<state.getState()(3)<<" "<<state.getState()(4)<<" exact mes 0 0 dy,dz not biased "<<resid(0)<<" "<<resid(1)
	                  <<" trk xyz "<<pos<<" mom "<<mom.Y()<<std::endl;
	          }*/
	    }
	    // state.Print();
	    // cov.Print();
	    // state2.Print();
	    // cov2.Print();
	        if(ihit==aTrack->GethitindexAt(1))
	          if(IsHMomentumActive()) {
	        	  GetHMomentum()->Fill(1000.0*momhit0.Mag(),mom.Mag()/momhit0.Mag()-1);
	    		  tpart->fitted=true;
	    		  tpart->recMom0=mom*1000.0;
	    		  tpart->mcMom0=momhit0.Mag()*1000.0;
	    	      if (pdgid==11) GetHMomRes_El()->Fill(momhit0.Mag()*1000.0,(mom.Mag()-momhit0.Mag())*1000.0);
	    	      if (pdgid==-11) GetHMomRes_Ps()->Fill(momhit0.Mag()*1000.0,(mom.Mag()-momhit0.Mag())*1000.0);
	          }
//	          if(IsHMomentumActive()) GetHMomentum()->Fill(pmom0.Mag(),mom.Mag()/pmom0.Mag()-1);
	    if(_fDebug>9) {
	          std::cout<<jhit<<" detid "<<detid<<" id "<<ihit<<" at hit "<<pos<<" "<<mom<<std::endl;
	        }
	      }
	    }catch(Exception& e){
	      if(_fDebug) std::cout<<"on getting points "<<e.what()<<std::endl;
	      fitstatus=false;
	    }
	    sumddist/=ngoodhitsdch;
	    if(_fDebug) std::cout<<" filtrated hits "<<numhits-ngoodhits<<" average ddist "<<sumddist/(0.4/150e-9)<<std::endl;
	  }


	  //-----------------------------------
	  //propagate to decay vertex
	  //-----------------------------------
	  if(fitstatus && _doExtrapol){

	    //force backward propagation(will be working forr only finite plane
	    rep->setPropDir(-1);
	    //take first fitted point
	    TrackPoint* tp = fitTrack.getPointWithFitterInfo(0);
	    if (tp == NULL) {std::cout << "Track has no TrackPoint with fitterInfo!(but fitstatus ok?)"<<std::endl;}
	    KalmanFitterInfo* fi = static_cast<KalmanFitterInfo*>(tp->getFitterInfo(rep));
	    MeasuredStateOnPlane state;

	    //extrapolate rep to "decay vertex" plane
	    bool isDecVert=true;
	    try{
	      state=fi->getFittedState(true);
//	      TVector3 vxt(0,0,0);
//	      TVector3 norm(0,0,1);
	      TVector3 vxt=pair.vert*0.1;
	      //!FIXme must be changed using extrapolate
	      TVector3 norm(vxt.X(),vxt.Y(),0);
	      norm.SetMag(1);
	      vxt-=norm*0.1*(_sThick[pair.shellId]-1.0);  //1mm "down"
	      rep->extrapolateToLine(state,vxt,norm);
//	      TVector3 vtxPoca;
//	      TVector3 vtxDirInPoca;
//	      rep->extrapolateToPoint(vxt,vtxPoca,vtxDirInPoca);
	    }catch(Exception& e){
	      if(_fDebug) std::cout<<"on extrapolation before conv shell "<<e.what()<<std::endl;
	      isDecVert=false;
	    }
	    double tof_target=1e9;
	    if(isDecVert){
	      tof_target=state.getTime();
	      state.getPosMomCov(pos,mom,cov);
	      double momrec=mom.Mag();
	      sigmap=momrec*momrec*TMath::Sqrt(state.getCov()(0,0));

	      tpart->extrap=true;
	      tpart->recMom=mom*1000.0;
//	      if (pdgid==11) GetHMomRes_El()->Fill(pmom0.Mag()*1000.0,(mom.Mag()-pmom0.Mag())*1000.0);
//	      if (pdgid==-11) GetHMomRes_Ps()->Fill(pmom0.Mag()*1000.0,(mom.Mag()-pmom0.Mag())*1000.0);

	    }
	    if(_fDebug)
	      std::cout<<"vertex chi2 "<<fitStatus->getChi2()<<" ndf "<<fitStatus->getNdf()
	           <<" nhits "<<nhits<<" dch "<<nhitsdch/*<<" svx "<<nhitssvx<<" pshw "<<nhitspshw*/<<" nfailed "<<fitStatus->getNFailedPoints()
	               <<" P= "<<mom.Mag()*rep->getCharge(state)<<" +- "<<sigmap<<" fitok="<<fitstatus
	           <<" pos "<<pos<<" mom "<<mom<<std::endl;
	  }

	  aTrack->Setx0(pos.X()*10);
	  aTrack->Seterr_x0(sqrt(cov(0,0))*10);
	  aTrack->Sety0(pos.Y()*10);
	  aTrack->Seterr_y0(sqrt(cov(1,1))*10);
	  aTrack->Setz0(pos.Z()*10);
	  aTrack->Seterr_z0(sqrt(cov(2,2))*10);
	  aTrack->Settheta(mom.Theta());
	  aTrack->Seterr_theta(0.);
	  aTrack->Setphi(mom.Phi());
	  aTrack->Seterr_phi(0.);
	  aTrack->SetMomentum(mom.Mag());
	  aTrack->SetErr_Momentum(sigmap);
	  aTrack->Setmom(mom);
	  aTrack->Getcov()->ResizeTo(6,6);
	  aTrack->Setcov(cov);

	  //fill final information
	  aTrack->Setnhits(nhits);
	  aTrack->Setnhitsdch(nhitsdch);
//	  aTrack->Setnhitssvx(nhitssvx);
//	  aTrack->Setnhitspshw(nhitspshw);
	  aTrack->Setchi2(fitStatus->getChi2());
	  aTrack->Setdof(fitStatus->getNdf());
	  aTrack->Setngoodhits(ngoodhits);
	  aTrack->Setngoodhitsdch(ngoodhitsdch);
//	  aTrack->Setngoodhitssvx(ngoodhitssvx);
//	  aTrack->Setngoodhitspshw(ngoodhitspshw);
	  aTrack->SetIsFitted(fitstatus);


	  delete fitter;

	  return true;

}
