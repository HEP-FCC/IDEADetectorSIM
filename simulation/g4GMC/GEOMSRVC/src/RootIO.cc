//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file persistency/P01/src/RootIO.cc
/// \brief Implementation of the RootIO class
//
// $Id: RootIO.cc 82130 2014-06-11 09:26:44Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RootIO.hh"

#include <sstream>
#include <memory>

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

#include "Riostream.h"
#include "TSystem.h"

#include "DRCaloIO.hh"

#include "GMCG4PodioManager.hh"

//#include "TClonesArray.h"
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

static RootIO* instance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RootIO::RootIO(Int_t runN, TString outFold, bool hasDRFPIC):fHasDRFPIC(hasDRFPIC),fNevents(0),fMCStep(NULL),fMCTracks(NULL){

  // initialize ROOT
  TSystem ts;
  gSystem->Load("libGMCG4ClassesDict");

  //gDebug = 1;

  fFile = new TFile(Form("%s/hits%.5d.root",outFold.Data(),runN),"RECREATE");
  CreateTreeObject();
  GMCG4PodioManager * l_podioManager = GMCG4PodioManager::Instance();
  l_podioManager->SetFilePrefix(Form("%s/simhits_podio",outFold.Data()));
  l_podioManager->SetFileSuffix(Form("%.5d.root",runN));
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::CreateTreeObject() {

 fFile->cd();

 fMCStep =  new TTree("MCStep","MCStep");
 fMCStep->SetAutoFlush(10);
 fMCTracks =  new TTree("MCTracks","MCTracks");
 fMCTracks->SetAutoFlush(10);

 if (fHasDRFPIC) {
	 fDRCalo = new TTree("B4", "edep");
	 fDRCalo->SetAutoFlush(10);
 }
 
// fMCStep->Branch("HitsStepCh","std::vector<GMCG4TrackerHit*>",&fHitsVectorCh);
// fMCStep->Branch("HitsStepPx","std::vector<GMCG4TrackerHit*>",&fHitsVectorPx);

 fMCTracks->Branch("Tracks","std::vector<GMCG4Particle*>",&fTracks);
// fMCTracks->Branch("Tracks",&fTracks);
 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::CreateMCStepBranches(std::string key, const char *branchName) {

  std::vector<GMCG4TrackerHit*> &HitsVector = fHitsVector[key];
  fMCStep->Branch(branchName,"std::vector<GMCG4TrackerHit*>",&HitsVector);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::CreateDRCaloBranches() {
	if (fHasDRFPIC) {
		drc::DRCaloIO* clIo = drc::DRCaloIO::GetInstance();

		fDRCalo->Branch("Energyem",&clIo->GetEnergyem(),"Energyem/D");
		fDRCalo->Branch("EnergyScin",&clIo->GetEnergyScin(),"EnergyScin/D");
		fDRCalo->Branch("EnergyCher",&clIo->GetEnergyCher(),"EnergyCher/D");
		fDRCalo->Branch("NofCherenkovDetected",&clIo->GetNofCherenkovDetected(),"NofCherenkovDetected/I");
		fDRCalo->Branch("EnergyTot",&clIo->GetEnergyTot(),"EnergyTot/D");
		fDRCalo->Branch("PrimaryParticleEnergy",&clIo->GetPrimaryParticleEnergy(),"PrimaryParticleEnergy/D");
//		fDRCalo->Branch("PrimaryParticleName","G4String",&clIo->GetPrimaryParticleName());
		fDRCalo->Branch("PrimaryParticleName",const_cast<char*>(clIo->GetPrimaryParticleName().data() ),"PrimaryParticleName/C");
		fDRCalo->Branch("neutrinoleakage",&clIo->Getneutrinoleakage(),"neutrinoleakage/D");
		fDRCalo->Branch("leakage",&clIo->Getleakage(),"leakage/D");
		fDRCalo->Branch("VectorSignalsR","std::vector<double>",&clIo->GetVectorSignalsR());
		fDRCalo->Branch("VectorSignalsL","std::vector<double>",&clIo->GetVectorSignalsL());
		fDRCalo->Branch("VectorSignalsCherR","std::vector<double>",&clIo->GetVectorSignalsCherR());
		fDRCalo->Branch("VectorSignalsCherL","std::vector<double>",&clIo->GetVectorSignalsCherL());
		fDRCalo->Branch("VectorL","std::vector<double>", &clIo->GetVectorL());
		fDRCalo->Branch("VectorR","std::vector<double>", &clIo->GetVectorR());
		fDRCalo->Branch("VectorL_loop","std::vector<double>", &clIo->GetVectorL_loop());
		fDRCalo->Branch("VectorR_loop","std::vector<double>", &clIo->GetVectorR_loop());

		clIo->CreateEdm4HepCollections();
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RootIO::~RootIO(){
	if (instance!=0) {
		delete instance;
		instance=0;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RootIO* RootIO::GetInstance(Int_t runN, TString outFold, bool hasDRFPIC){

  if (instance == 0 ){

    instance = new RootIO(runN,outFold,hasDRFPIC);
  }
  
  return instance;

}

//////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//void RootIO::PutMCStepHitCh(std::vector<GMCG4TrackerHit*> &mcHit) {
//
// fHitsVectorCh.clear();
// fHitsVectorCh = mcHit;
//
//}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//void RootIO::PutMCStepHitPx(std::vector<GMCG4TrackerHit*> &mcHit) {
//
// fHitsVectorPx.clear();
// fHitsVectorPx = mcHit;
//
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::PutMCStepHit(std::string key, std::vector<GMCG4TrackerHit*> &mcHit) {

  fHitsVector[key].clear();
  fHitsVector[key] = mcHit;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::PutMCTracks(std::vector<GMCG4Particle*> &mctrk) {
//void RootIO::PutMCTracks(TClonesArray &mctrk) {

  for (auto p : fTracks){
    if (p != NULL) delete p;
  }
 fTracks.clear();
 fTracks = mctrk;
 
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::FillEvent(){

 fMCStep->Fill();
 fMCTracks->Fill();
 if (fHasDRFPIC) {
	 fDRCalo->Fill();
 }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::Close(){

  fFile->cd();
  fMCStep->Write();
  fMCTracks->Write(); 
  if (fHasDRFPIC) {
 	 fDRCalo->Write();
  }
  fFile->Close();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
