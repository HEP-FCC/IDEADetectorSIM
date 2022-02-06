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
// $Id: readHits.cc 68025 2013-03-13 13:43:46Z gcosmo $
//
/// \file persistency/P01/readHits.cc
/// \brief Main program of the persistency/P01 example
//
// Include files

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TKey.h"
#include "TTree.h"
#include "TBranch.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "Riostream.h"
#include "generated/GMCDCGeantStep.h"
#include "generated/GMCPXGeantStep.h"
#include "generated/GMCSVXGeantStep.h"
#include "generated/GMCPSHWGeantStep.h"
#include "generated/GMCPHCVGeantStep.h"
#include "generated/GMCPHCVRadGeantStep.h"
#include "generated/GMCGeantTrack.h"
#include "ROMETreeInfo.h"
#include "GEOMSRVC/include/GMCG4Particle.hh"
#include "GEOMSRVC/include/GMCG4TrackerHit.hh"

using namespace std;

int main(int argc,char** argv) 
{

  std::cout<< "************** MAIN ****************" << std::endl;

  // initialize ROOT
  TSystem ts;
  gSystem->Load("$PRJBASE/simulation/build/lib/libGMCG4ClassesDict");

  if(argc<2) cout << "Missing name of the file to read!" << endl;
 
  TFile fo(argv[1]);
   
  TString br1("MCStep");
  TString br2("MCTracks");


  TTree *a;
  TTree *b;
  
  std::vector<GMCG4TrackerHit*> *hitsch = new std::vector<GMCG4TrackerHit*>();
  std::vector<GMCG4TrackerHit*> *hitspx = new std::vector<GMCG4TrackerHit*>();
  std::vector<GMCG4TrackerHit*> *hitssvx = new std::vector<GMCG4TrackerHit*>();
  std::vector<GMCG4TrackerHit*> *hitspshw = new std::vector<GMCG4TrackerHit*>();
  std::vector<GMCG4TrackerHit*> *hitsphcv = new std::vector<GMCG4TrackerHit*>();
  std::vector<GMCG4TrackerHit*> *hitsphcvrd = new std::vector<GMCG4TrackerHit*>();
  std::vector<GMCG4Particle*> *tracks = new std::vector<GMCG4Particle*>();
  bool hitChIsPresent=false;
  bool hitPxIsPresent=false;
  bool hitSVXIsPresent=false;
  bool hitPSHWIsPresent=false;
  bool hitPHCVIsPresent=false;
  bool hitPHCVRadIsPresent=false;

  fo.GetListOfKeys()->Print();
    
  TIter next(fo.GetListOfKeys());
  TKey *key = NULL;

  //double tot_en;
  while ((key=(TKey*)next()))
  {
    
    if (br1.CompareTo(key->GetName()) == 0) {
     
      fo.GetObject(key->GetName(), a);
      if (a->FindBranch("HitsStepCh")!=0x0) {
        hitChIsPresent=true;
        a->SetBranchAddress("HitsStepCh",&hitsch);
        cout<<"Found DCH hits"<<endl;
      }
      if (a->FindBranch("HitsStepPx")!=0x0) {
        hitPxIsPresent=true;
        a->SetBranchAddress("HitsStepPx",&hitspx);
        cout<<"Found Px hits"<<endl;
      }
      if (a->FindBranch("SVXHitsStepCh")!=0x0) {
        hitSVXIsPresent=true;
        a->SetBranchAddress("SVXHitsStepCh",&hitssvx);
        cout<<"Found SVX hits"<<endl;
      }
      if (a->FindBranch("PSHWHitsStepCh")!=0x0) {
        hitPSHWIsPresent=true;
        a->SetBranchAddress("PSHWHitsStepCh",&hitspshw);
        cout<<"Found PSHW hits"<<endl;
      }
      if (a->FindBranch("PHCVHitsStepCh")!=0x0) {
        hitPHCVIsPresent=true;
        a->SetBranchAddress("PHCVHitsStepCh",&hitsphcv);
        cout<<"Found PHCV hits"<<endl;
      }
      if (a->FindBranch("PHCVHitsStepChRad")!=0x0) {
        hitPHCVRadIsPresent=true;
        a->SetBranchAddress("PHCVHitsStepChRad",&hitsphcvrd);
        cout<<"Found PHCV Radiator hits"<<endl;
      }
      
  cout << "Collection: " << a->GetName() << endl;
  cout << "Number of events: " << a->GetEntries() << endl;


    } 
    
    if (br2.CompareTo(key->GetName()) == 0) {

      fo.GetObject(key->GetName(), b);
    
      b->SetBranchAddress("Tracks",&tracks);
  cout << "Collection: " << b->GetName() << endl;
  cout << "Number of events: " << b->GetEntries() << endl;
  
    
    }
  }
  
  int fOutNum=1;
  if (argc==3) { fOutNum = TString(argv[2]).Atoi(); }
  else {
    TString fIn(argv[1]);
    fIn.ReplaceAll(".root","");
    fIn.ReplaceAll("hits","");
    if (fIn.IsDec()) { fOutNum = fIn.Atoi(); }
  }

  TFile fOutput(Form("MCData%05d.root",fOutNum),"RECREATE");

  TTree *tr = new TTree("MCData","MCData");

  ROMETreeInfo fInfo;
  tr->Branch("Info",&fInfo);
  fInfo.SetRunNumber(fOutNum);

  TClonesArray *myBr_trk = new TClonesArray("GMCGeantTrack",0);
  TClonesArray &ptrBr_trk = *myBr_trk;
  tr->Branch("MCTracks",&myBr_trk);

  TClonesArray *myBr_dch = new TClonesArray("GMCDCGeantStep",0);
  TClonesArray &ptrBr_dch = *myBr_dch;
  if (hitChIsPresent) { tr->Branch("MCDCStep",&myBr_dch); }

  TClonesArray *myBr_mpx = new TClonesArray("GMCPXGeantStep",0);
  TClonesArray &ptrBr_mpx = *myBr_mpx;
  if (hitPxIsPresent) { tr->Branch("MCPXStep",&myBr_mpx); }

  TClonesArray *myBr_svx = new TClonesArray("GMCSVXGeantStep",0);
  TClonesArray &ptrBr_svx = *myBr_svx;
  if (hitSVXIsPresent) { tr->Branch("MCSVXStep",&myBr_svx); }

  TClonesArray *myBr_pshw = new TClonesArray("GMCPSHWGeantStep",0);
  TClonesArray &ptrBr_pshw = *myBr_pshw;
  if (hitPSHWIsPresent) { tr->Branch("MCPSHWStep",&myBr_pshw); }

  TClonesArray *myBr_phcv = new TClonesArray("GMCPHCVGeantStep",0);
  TClonesArray &ptrBr_phcv = *myBr_phcv;
  if (hitPHCVIsPresent) { tr->Branch("MCPHCVStep",&myBr_phcv); }

  TClonesArray *myBr_phcvrd = new TClonesArray("GMCPHCVRadGeantStep",0);
  TClonesArray &ptrBr_phcvrd = *myBr_phcvrd;
  if (hitPHCVRadIsPresent) { tr->Branch("MCPHCVRadStep",&myBr_phcvrd); }

  for (int i=0;i<b->GetEntries();i++) {

	   b->GetEntry(i);

	   a->GetEntry(i);

           fInfo.SetEventNumber(i);
     int ntracks = tracks->size();
     ptrBr_trk.Clear();
     if (i%50==0) cout <<"Ievent = "<<i<<"  nr. of tracks "<<ntracks<<endl;
     for (int k=0;k<ntracks;k++) { 
       
       G4ThreeVector start = tracks->at(k)->GetPosStart();
       TVector3 vec_a(start.x(),start.y(),start.z());
       G4ThreeVector end = tracks->at(k)->GetPosEnd();
       TVector3 vec_b(end.x(),end.y(),end.z());
       G4ThreeVector mom = tracks->at(k)->GetMomentum();
       TVector3 vec_c(mom.x(),mom.y(),mom.z());
       
       new(ptrBr_trk[k]) GMCGeantTrack(tracks->at(k)->GetTrackID(),
				       tracks->at(k)->GetParentID(),
				       tracks->at(k)->GetParticleName(),
				       tracks->at(k)->GetParticleCharge(),
				       tracks->at(k)->GetPDGCode());

       ((GMCGeantTrack*)ptrBr_trk[k])->SetfPosStart(vec_a);
       ((GMCGeantTrack*)ptrBr_trk[k])->SetfPosEnd(vec_b);
       ((GMCGeantTrack*)ptrBr_trk[k])->SetfMomentum(vec_c);
     }

     if (hitChIsPresent) {
       int nhits = hitsch->size();
       ptrBr_dch.Clear();
       for (int k=0;k<nhits;k++) {

         G4ThreeVector s_pos = hitsch->at(k)->GetPos();
         TVector3 vec_p(s_pos.x(),s_pos.y(),s_pos.z());
         G4ThreeVector q_pos = hitsch->at(k)->GetPosEnding();
         TVector3 vec_q(q_pos.x(),q_pos.y(),q_pos.z());
         G4ThreeVector mom = hitsch->at(k)->GetMomentum();
         TVector3 vec_s(mom.x(),mom.y(),mom.z());

         new(ptrBr_dch[k]) GMCDCGeantStep(hitsch->at(k)->GetTrackID(),
             hitsch->at(k)->GetChamberNb(),
             hitsch->at(k)->GetEdep(),
             hitsch->at(k)->GetNoIEdep(),
             hitsch->at(k)->GetGlobalTime(),
             hitsch->at(k)->GetProperTime(),
             hitsch->at(k)->GetStepLength(),
             hitsch->at(k)->GetProcessCode() );
         ((GMCDCGeantStep*)ptrBr_dch[k])->SetfPos(vec_p);
         ((GMCDCGeantStep*)ptrBr_dch[k])->SetfPosEnding(vec_q);
         ((GMCDCGeantStep*)ptrBr_dch[k])->SetfMomentum(vec_s);

       }
     }
     
     if (hitPxIsPresent) {
       int mhits = hitspx->size();
       ptrBr_mpx.Clear();
       for (int k=0;k<mhits;k++) {

         G4ThreeVector s_pos = hitspx->at(k)->GetPos();
         TVector3 vec_r(s_pos.x(),s_pos.y(),s_pos.z());
         G4ThreeVector u_pos = hitspx->at(k)->GetPosEnding();
         TVector3 vec_u(u_pos.x(),u_pos.y(),u_pos.z());
         G4ThreeVector mom = hitspx->at(k)->GetMomentum();
         TVector3 vec_t(mom.x(),mom.y(),mom.z());
         new(ptrBr_mpx[k]) GMCPXGeantStep(hitspx->at(k)->GetTrackID(),
             hitspx->at(k)->GetChamberNb(),
             hitspx->at(k)->GetEdep(),
             hitspx->at(k)->GetNoIEdep(),
             hitspx->at(k)->GetGlobalTime(),
             hitspx->at(k)->GetProperTime(),
             hitspx->at(k)->GetStepLength(),
             hitspx->at(k)->GetProcessCode() );
         ((GMCDCGeantStep*)ptrBr_mpx[k])->SetfPos(vec_r);
         ((GMCDCGeantStep*)ptrBr_mpx[k])->SetfPosEnding(vec_u);
         ((GMCDCGeantStep*)ptrBr_mpx[k])->SetfMomentum(vec_t);
       }
     }

     if (hitSVXIsPresent) {
       int nhits = hitssvx->size();
       ptrBr_svx.Clear();
       for (int k=0;k<nhits;k++) {

         G4ThreeVector s_pos = hitssvx->at(k)->GetPos();
         TVector3 vec_p(s_pos.x(),s_pos.y(),s_pos.z());
         G4ThreeVector q_pos = hitssvx->at(k)->GetPosEnding();
         TVector3 vec_q(q_pos.x(),q_pos.y(),q_pos.z());
         G4ThreeVector mom = hitssvx->at(k)->GetMomentum();
         TVector3 vec_s(mom.x(),mom.y(),mom.z());

         new(ptrBr_svx[k]) GMCSVXGeantStep(hitssvx->at(k)->GetTrackID(),
             hitssvx->at(k)->GetChamberNb(),
             hitssvx->at(k)->GetChannelNb(),
             hitssvx->at(k)->GetEdep(),
             hitssvx->at(k)->GetNoIEdep(),
             hitssvx->at(k)->GetGlobalTime(),
             hitssvx->at(k)->GetProperTime(),
             hitssvx->at(k)->GetStepLength(),
             hitssvx->at(k)->GetProcessCode() );

         
         ((GMCSVXGeantStep*)ptrBr_svx[k])->SetfPos(vec_p);
         ((GMCSVXGeantStep*)ptrBr_svx[k])->SetfPosEnding(vec_q);
         ((GMCSVXGeantStep*)ptrBr_svx[k])->SetfMomentum(vec_s);
         
       }
     }

     if (hitPSHWIsPresent) {
       int nhits = hitspshw->size();
       ptrBr_pshw.Clear();
       for (int k=0;k<nhits;k++) {

         G4ThreeVector s_pos = hitspshw->at(k)->GetPos();
         TVector3 vec_p(s_pos.x(),s_pos.y(),s_pos.z());
         G4ThreeVector q_pos = hitspshw->at(k)->GetPosEnding();
         TVector3 vec_q(q_pos.x(),q_pos.y(),q_pos.z());
         G4ThreeVector mom = hitspshw->at(k)->GetMomentum();
         TVector3 vec_s(mom.x(),mom.y(),mom.z());

         new(ptrBr_pshw[k]) GMCPSHWGeantStep(hitspshw->at(k)->GetTrackID(),
             hitspshw->at(k)->GetChamberNb(),
             hitspshw->at(k)->GetChannelNb(),
             hitspshw->at(k)->GetEdep(),
             hitspshw->at(k)->GetNoIEdep(),
             hitspshw->at(k)->GetGlobalTime(),
             hitspshw->at(k)->GetProperTime(),
             hitspshw->at(k)->GetStepLength(),
             hitspshw->at(k)->GetProcessCode() );
         ((GMCPSHWGeantStep*)ptrBr_pshw[k])->SetfPos(vec_p);
         ((GMCPSHWGeantStep*)ptrBr_pshw[k])->SetfPosEnding(vec_q);
         ((GMCPSHWGeantStep*)ptrBr_pshw[k])->SetfMomentum(vec_s);

       }
     }

     if (hitPHCVIsPresent) {
       int nhits = hitsphcv->size();
       ptrBr_phcv.Clear();
       for (int k=0;k<nhits;k++) {

         G4ThreeVector s_pos = hitsphcv->at(k)->GetPos();
         TVector3 vec_p(s_pos.x(),s_pos.y(),s_pos.z());
         G4ThreeVector q_pos = hitsphcv->at(k)->GetPosEnding();
         TVector3 vec_q(q_pos.x(),q_pos.y(),q_pos.z());
         G4ThreeVector mom = hitsphcv->at(k)->GetMomentum();
         TVector3 vec_s(mom.x(),mom.y(),mom.z());

         new(ptrBr_phcv[k]) GMCPHCVGeantStep(hitsphcv->at(k)->GetTrackID(),
             hitsphcv->at(k)->GetChamberNb(),
             hitsphcv->at(k)->GetChannelNb(),
             hitsphcv->at(k)->GetEdep(),
             hitsphcv->at(k)->GetNoIEdep(),
             hitsphcv->at(k)->GetGlobalTime(),
             hitsphcv->at(k)->GetProperTime(),
             hitsphcv->at(k)->GetStepLength(),
             hitsphcv->at(k)->GetProcessCode() );
         ((GMCPHCVGeantStep*)ptrBr_phcv[k])->SetfPos(vec_p);
         ((GMCPHCVGeantStep*)ptrBr_phcv[k])->SetfPosEnding(vec_q);
         ((GMCPHCVGeantStep*)ptrBr_phcv[k])->SetfMomentum(vec_s);

       }
     }

     if (hitPHCVRadIsPresent) {
       int nhits = hitsphcvrd->size();
       ptrBr_phcvrd.Clear();
       for (int k=0;k<nhits;k++) {

         G4ThreeVector s_pos = hitsphcvrd->at(k)->GetPos();
         TVector3 vec_p(s_pos.x(),s_pos.y(),s_pos.z());
         G4ThreeVector q_pos = hitsphcvrd->at(k)->GetPosEnding();
         TVector3 vec_q(q_pos.x(),q_pos.y(),q_pos.z());
         G4ThreeVector mom = hitsphcvrd->at(k)->GetMomentum();
         TVector3 vec_s(mom.x(),mom.y(),mom.z());

         new(ptrBr_phcvrd[k]) GMCPHCVRadGeantStep(hitsphcvrd->at(k)->GetTrackID(),
             hitsphcvrd->at(k)->GetChamberNb(),
             hitsphcvrd->at(k)->GetChannelNb(),
             hitsphcvrd->at(k)->GetEdep(),
             hitsphcvrd->at(k)->GetNoIEdep(),
             hitsphcvrd->at(k)->GetGlobalTime(),
             hitsphcvrd->at(k)->GetProperTime(),
             hitsphcvrd->at(k)->GetStepLength(),
             hitsphcvrd->at(k)->GetProcessCode() );
         ((GMCPHCVRadGeantStep*)ptrBr_phcvrd[k])->SetfPos(vec_p);
         ((GMCPHCVRadGeantStep*)ptrBr_phcvrd[k])->SetfPosEnding(vec_q);
         ((GMCPHCVRadGeantStep*)ptrBr_phcvrd[k])->SetfMomentum(vec_s);

       }
     }

     tr->Fill();
  }

  tr->Write();
  fOutput.Close();
  
}


