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


//#include "TClonesArray.h"
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

static RootIO* instance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RootIO::RootIO(Int_t runN, TString outFold):fNevents(0),fMCStep(NULL),fMCTracks(NULL){

  // initialize ROOT
  TSystem ts;
  gSystem->Load("libGMCG4ClassesDict");

  //gDebug = 1;

  fFile = new TFile(Form("%s/hits%.5d.root",outFold.Data(),runN),"RECREATE");
  CreateTreeObject();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::CreateTreeObject() {

 fMCStep =  new TTree("MCStep","MCStep");
 fMCTracks =  new TTree("MCTracks","MCTracks");
 
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
RootIO::~RootIO(){
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RootIO* RootIO::GetInstance(Int_t runN, TString outFold){

  if (instance == 0 ){

    instance = new RootIO(runN,outFold);
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

 fTracks.clear();
 fTracks = mctrk;
 
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::FillEvent(){

 fMCStep->Fill();
 fMCTracks->Fill();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RootIO::Close(){

  fMCStep->Write();
  fMCTracks->Write(); 
  fFile->Close();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
