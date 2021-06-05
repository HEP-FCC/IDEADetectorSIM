// Include files
#include <iostream>
#include <vector>
#include <math.h>

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TKey.h"
#include "TTree.h"
#include "TBranch.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "Riostream.h"
#include "TMatrixTSym.h"

#include "GEOMSRVC/include/GMCG4Particle.hh"
#include "GEOMSRVC/include/GMCG4TrackerHit.hh"
#include "generated/GMCRecoTracks.h"
#include "ROMETreeInfo.h"

#include "DD4hep/DetType.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"
#include "edm4hep/TrackCollection.h"

// using namespace std;
int main(int argc,char** argv) 
{

  std::cout << "convert tracks" << std::endl;

  gSystem->Load("$PRJBASE/simulation/build/lib/libGMCG4ClassesDict");
  gSystem->Load("$PRJBASE/analyzer/GMC/obj/libGMCAnalyzer");

  if(argc<2) std::cout << "Missing name of the file to read!" << std::endl;

  TFile fo(argv[1]);

  // -------------
  TTree* RecoData = ((TTree*) fo.Get("RecoData"));
  if (RecoData) RecoData->SetCacheSize();
  TClonesArray* RecoDataRecoTracks = new TClonesArray("GMCRecoTracks");
  TBranch *branchRecoDataRecoTracks = RecoData->GetBranch("RecoTracks");
  if(branchRecoDataRecoTracks) branchRecoDataRecoTracks->SetAddress(&RecoDataRecoTracks);
  // -------------

  int nevents = branchRecoDataRecoTracks->GetEntries();  
  std::cout << "events in tree " << nevents << std::endl; 
  
  // ------------------------------------------------
  // output file name construction
  int fOutNum=1;
  if (argc==3) { fOutNum = TString(argv[2]).Atoi(); }
  else {
    TString fIn(argv[1]);
    fIn.ReplaceAll(".root","");
    fIn.ReplaceAll("tracks","");
    if (fIn.IsDec()) { fOutNum = fIn.Atoi(); }
  }
  
  // output file ------------------------------------------
  TFile fOutput(Form("EDMTracks%05d.root",fOutNum),"RECREATE");

  // create a new podio::EventStore, linked to a podio::ROOTWriter,
  // to write the collections on the output file
  std::string filename = fOutput.GetName();

  podio::EventStore * l_evtstore = new podio::EventStore();
  podio::ROOTWriter * l_writer = new podio::ROOTWriter(filename, l_evtstore);
    
  std::cout << "filename  " << filename << std::endl;
  std::cout << "evt store " << l_evtstore << std::endl;
  std::cout << "writer    " << l_writer << std::endl;

  // from https://github.com/key4hep/EDM4hep/blob/master/edm4hep.yaml
  // tracks
  edm4hep::TrackCollection *recoTracks = new edm4hep::TrackCollection();
  l_evtstore->registerCollection("recoTracks",recoTracks);
  l_writer->registerForWrite("recoTracks");


  // ---------------
  for(int ievt = 0; ievt < nevents; ievt++) {

    branchRecoDataRecoTracks->GetEntry(ievt);

    int ntracks = RecoDataRecoTracks->GetEntries();
    // std::cout << "ntracks " << ntracks << std::endl;
    
    for(int itrk = 0; itrk < ntracks; itrk++) {
      GMCRecoTracks* track = (GMCRecoTracks*) RecoDataRecoTracks->At(itrk);

      // get track variables GMCRecoTracks
      Int_t trackID = track->GetTrkID();
      Double_t x0 = track->Getx0();
      Double_t y0 = track->Gety0();
      Double_t z0 = track->Getz0();
      Double_t err_x0 = track->Geterr_x0();
      Double_t err_y0 = track->Geterr_y0();
      Double_t err_z0 = track->Geterr_z0();
      Double_t theta =  track->Gettheta();
      Double_t phi = track->Getphi();
      Double_t err_theta = track->Geterr_theta();
      Double_t err_phi = track->Geterr_phi();
      Double_t momentum = track->GetMomentum();
      Double_t err_momentum = track->GetErr_Momentum();
      TVector3 *mom = track->Getmom();
      TMatrixTSym<double> *cov = track->Getcov();
      std::vector<int> hitindex = track->Gethitindex();
      std::vector<int> detid = track->Getdetid();
      std::vector<bool> skipped = track->GetSkipped();
      TClonesArray *statevector = track->GetStateVector();
      Int_t nhits = track->Getnhits();
      Int_t nhitsdch = track->Getnhitsdch();
      Int_t nhitssvx = track->Getnhitssvx();
      Int_t nhitspshw = track->Getnhitspshw();
      Int_t ngoodhits = track->Getngoodhits();
      Int_t ngoodhitsdch = track->Getngoodhitsdch();
      Int_t ngoodhitssvx = track->Getngoodhitssvx();
      Int_t ngoodhitspshw = track->Getngoodhitspshw();
      Double_t chi2 = track->Getchi2();
      Int_t dof = track->Getdof();
      Bool_t isFitted = track->GetIsFitted();
      
      // track ID
      std::cout << "trackid " << trackID << std::endl;

      // detector ID 
      // int subDetectorHitNumbers CHECK
      // number of hits in particular subdetectors. Check/set collection variable TrackSubdetectorNames for decoding the indices
      std::cout << "podio " << l_track.getSubDetectorHitNumbers().size() << std::endl;
      std::cout << "nhits " << nhits << " nhitsdch " << nhitsdch << " nhitssvx " << nhitssvx << " nhitspshw " << nhitspshw << std::endl;
      // std::cout << "hitindex " << hitindex.size() << " detid " << detid.size() << std::endl;
      // for(int ihit = 0; ihit < hitindex.size(); ihit++) {
      //	std::cout << "hit " << hitindex.at(ihit) << " detector id " << detid.at(ihit) << std::endl;
      // }

      // standalone DETID 1=SVX  0=DCH 2=PSHW
      // DD4hep:
      dd4hep::DetType typesvx( dd4hep::DetType::TRACKER | dd4hep::DetType::VERTEX | dd4hep::DetType::PIXEL ) ;         // DETID 1
      dd4hep::DetType typedch( dd4hep::DetType::TRACKER | dd4hep::DetType::GASEOUS | dd4hep::DetType::WIRE ) ;         // DETID 0
      dd4hep::DetType typepshw( dd4hep::DetType::TRACKER | dd4hep::DetType::GASEOUS | dd4hep::DetType::CALORIMETER ) ; // DETID 2
      /**  int nhits_in_vxd = l_track.subdetectorHitNumbers()[0];
          if (nhits_in_vxd > 0) Track->setTypeBit( lcio::ILDDetID::VXD ) ;**/

      // --------------------------------------------------------------------
      
      // state vector
      std::cout << "state vector " << statevector->GetEntries() << std::endl;

      
      // convert to EDM track ------------------------------------------------
      auto l_track = recoTracks->create();
      //flagword that defines the type of track.Bits 16-31 are used internally CHECK
      l_track.setType(12345);
      //Chi^2 of the track fit
      l_track.setChi2(chi2);
      //number of degrees of freedom of the track fit 
      l_track.setNdf(dof);
      //dEdx of the track CHECK
      float dEdx = 0;
      //error of dEdx CHECK
      float dEdxError = 0;
      //radius of the innermost hit that has been used in the track fit CHECK
      float radiusOfInnermostHit = 0;


      edm4hep::TrackState l_state;
      // for use with At{Other|IP|FirstHit|LastHit|Calorimeter|Vertex}|LastLocation CHECK
      int location = 0;

      // transverse impact parameter CHECK
      float D0 =TMath::Sqrt( x0*x0 + y0*y0 );
      // azimuthal angle CHECK
      float phi0 = phi;
      // is the signed curvature of the track in [1/mm] CHECK
      // pT [GeV/c] = 0.3 * z * B [T] * R [m]
      int charge = -1; // CHECK
      double mag_field = 3; // CHECK
      double radius = 1e3 * momentum / (0.3 * charge * mag_field); // mm
      float omega = 1./radius;
      // longitudinal impact parameter CHECK
      float Z0 = z0;
      // lambda is the dip angle of the track in r-z CHECK
      float tanLambda = TMath::Tan(TMath::Pi()*0.5 - theta);

      // Reference point of the track parameters
      // e.g. the origin at the IP
      edm4hep::Vector3f referencePoint(0,0,0); // CHECK

      std::array<float, 15> covMatrix // lower triangular covariance matrix of the track parameters.  the order of parameters is  d0, phi, omega, z0, tan(lambda). the array is a row-major flattening of the matrix.

	  
      
      /**
    VectorMembers:
    - edm4hep::TrackState trackStates  //track states
    OneToManyRelations:
      - edm4hep::TrackerHit trackerHits  //hits that have been used to create this track
      - edm4hep::Track tracks            //tracks (segments) that have been combined to create this tr
      **/
    }
  }
  // --------------------
  
  
  // for each event write output
  if (l_writer != NULL)   l_writer->writeEvent();
  if (l_evtstore != NULL) l_evtstore->clearCollections();


  
  // end of run
  // fOutput.Write();
  // fOutput.Close();

  l_writer->finish();
  
  
}
