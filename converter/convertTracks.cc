// Include files
#include <iostream>
#include <vector>
#include <math.h>

#include "TROOT.h"
#include "TNtuple.h"
#include "TH1F.h"
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
  bool debug=false;
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

  // ntuple-output file ---------------------------
  TFile *fTest = new TFile("test.root", "RECREATE");
  TNtuple *rome_tree = new TNtuple("rome_tree", "variables from ROME", "eventid:trackid:x0:y0:z0:px:py:pz:err_x0:err_y0:err_z0:err_px:err_py:err_pz");
  TNtuple *edm4hep_tree = new TNtuple("edm4hep_tree", "varibles for EDM4Hep", "eventid:trackid:k:phi:rho:cotg:z0:err_k:err_phi:err_rho:err_cotg:err_z0");

  // ----------------------------------------------
  // output file name construction
  int fOutNum=1;
  if (argc==3) { fOutNum = TString(argv[2]).Atoi(); }
  else {
    TString fIn(argv[1]);
    fIn.ReplaceAll(".root","");
    fIn.ReplaceAll("tracks","");
    if (fIn.IsDec()) { fOutNum = fIn.Atoi(); }
  }
  
  // edm4hep output file ------------------------------------------
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
    std::cout << "****************** EVENT " << ievt << std::endl;
    branchRecoDataRecoTracks->GetEntry(ievt);

    int ntracks = RecoDataRecoTracks->GetEntries();
    if(debug==true) std::cout << "ntracks " << ntracks << std::endl;
    
    for(int itrk = 0; itrk < ntracks; itrk++) {
      if(debug==true) std::cout << "-------- TRACK " << itrk << "/" << ntracks << std::endl;

      GMCRecoTracks* track = (GMCRecoTracks*) RecoDataRecoTracks->At(itrk);

      // get track variables GMCRecoTracks
      Int_t trackID = track->GetTrkID();
      Double_t x0 = track->Getx0(); // [mm]
      Double_t y0 = track->Gety0(); // [mm]
      Double_t z0 = track->Getz0(); // [mm]
      Double_t err_x0 = track->Geterr_x0(); // [mm]
      Double_t err_y0 = track->Geterr_y0(); // [mm]
      Double_t err_z0 = track->Geterr_z0(); // [mm]
      Double_t theta =  track->Gettheta(); // [rad]
      Double_t phi = track->Getphi(); // [rad]
      Double_t err_theta = track->Geterr_theta(); // put to 0 by hand
      Double_t err_phi = track->Geterr_phi(); // put to 0 by hand 
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

      if(statevector->GetEntries() != hitindex.size()) std::cout << "ERROR, stateVector has size " << statevector->GetEntries() << " and hitlist has size " << hitindex.size() << std::endl;
      
      // fill ROME tree for test ntuple output (not needed for edm4hep
      rome_tree->Fill(ievt,trackID,x0,y0,z0,mom->X(),mom->Y(),mom->Z(),
                      10*TMath::Sqrt(cov->GetMatrixArray()[0]),10*TMath::Sqrt(cov->GetMatrixArray()[7]),10*TMath::Sqrt(cov->GetMatrixArray()[14]),
                      TMath::Sqrt(cov->GetMatrixArray()[21]),TMath::Sqrt(cov->GetMatrixArray()[28]),TMath::Sqrt(cov->GetMatrixArray()[35]));
            
      // printouts
      if(debug==true) {
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "trackid " << trackID << std::endl;
	std::cout << "x0, y0, z0 " << x0 << " " << y0 << " " << z0 << std::endl;
	std::cout << "phi rad " << phi << " -> deg " << phi * TMath::RadToDeg() << std::endl;
	std::cout << "theta rad " << theta << " -> deg " << theta * TMath::RadToDeg() <<std::endl;
	std::cout << "momentum module " << momentum << " " << mom->Mag() << std::endl;
	mom->Print();

	std::cout << "ROME errors on angles (rad) - set to 0 by hand in ROME " << err_theta << " " << err_phi << std::endl;
	std::cout << "ROME errors on pos [mm] " << err_x0 << " " << err_y0 << " " << err_z0 << " error momentum " << err_momentum << std::endl;
	std::cout << "-> err*err on pos [mm*mm] = " << err_x0*err_x0 << " " << err_y0*err_y0 << " " << err_z0*err_z0 << " on momentum = " << err_momentum*err_momentum << std::endl;
	std::cout << "matrix 6 x 6, pos [cm*cm] - mom [GeV*GeV]" << std::endl;
	cov->Print();
      }
      
      /*
	compute helix parameters
	k, phi, rho, cotg(theta), z_0)
      */
      
      int charge = -1;                                                // CHECK should be reconstructed by ROME
      double mag_field = 2;                                           // [T] // CHECK should be read from geometry
      // pT [GeV/c] = 0.3 * z * B [T] * R [m]
      double radius = 1e3 * mom->Perp() / (0.3 * charge * mag_field); // [mm]
      double k = charge / radius;                                     // [mm^-1] signed curvature
      // double phi                                                   // [rad], already defined
      double rho = charge * TMath::Sqrt( x0*x0 + y0*y0 );             // [mm] signed (? CHECK) closest distance
      double cotg = 1./TMath::Tan(theta);                             // theta [rad], already defined
      double z_0 = z0;                                                // [mm]
      
      /* Jacobian matrix 5 x 6
	 D(k, phi, rho, cotg(theta), z_0)
	 ---------------------------------
	 D(x, y, z, p_x, p_y, p_z) 
	 element 0, 3 = Dk / Dp_x
	 element 0, 4 = Dk / Dp_y
	 element 1, 3 = Dphi / Dp_x
	 element 1, 4 = Dphi / Dp_y
	 element 2, 0 = Drho / Dx 
	 element 2, 1 = Drho / Dy
	 element 3, 3 = Dcotg(theta) / Dp_x
	 element 3, 4 = Dcotg(theta) / Dp_y
	 element 3, 5 = Dcotg(theta) / Dp_z
	 element 4, 2 = Dz_0 / Dz            
      */
      int nrow = 5;
      int ncol = 6;
      int counter = 0;
      TMatrixT<double> J_from_MARS_to_helix(nrow, ncol);
      for(int irow = 0; irow < nrow; irow++) for(int icol = 0; icol < ncol; icol++) J_from_MARS_to_helix[irow][icol] = 0.0;
      
      J_from_MARS_to_helix[0][3] = -k * mom->X() / (mom->Perp() *  mom->Perp());
      J_from_MARS_to_helix[0][4] = -k * mom->Y() / (mom->Perp() *  mom->Perp());
      J_from_MARS_to_helix[1][3] = - mom->Y() / (mom->Perp() *  mom->Perp());
      J_from_MARS_to_helix[1][4] =   mom->X() / (mom->Perp() *  mom->Perp());
      J_from_MARS_to_helix[2][0] = x0 / TMath::Sqrt( x0*x0 + y0*y0 );
      J_from_MARS_to_helix[2][1] = y0 / TMath::Sqrt( x0*x0 + y0*y0 );
      J_from_MARS_to_helix[3][3] = - cotg * mom->X() / (mom->Perp() *  mom->Perp());
      J_from_MARS_to_helix[3][4] = - cotg * mom->Y() / (mom->Perp() *  mom->Perp());
      J_from_MARS_to_helix[3][5] = 1. / mom->Perp() ;
      J_from_MARS_to_helix[4][2] = 1.;

      if(debug==true) std::cout << "transformation matrix" << std::endl;
      if(debug==true) J_from_MARS_to_helix.Print();

      /*
	original TMatrixTSym (6 x 6) cov: positions are in [cm] -> must be converted
	x_x  x_y  x_z  x_px  x_py x_pz
	y_x  y_y  y_z  y_px  y_py y_pz
	z_x  z_y  z_z  z_px  z_py z_pz
	px_x px_y px_z px_px px_py px_pz
	py_x py_y py_z py_px py_py py_pz
	pz_x pz_y pz_z pz_px pz_py pz_pz                 
      */
      TMatrixTSym<double> cov_mm(6);
      counter = 0;
      for(int irow = 0; irow < 6; irow++) {
	for(int icol = 0; icol < 6; icol++) {
	  double variance = cov->GetMatrixArray()[counter];
	  if(irow<3  && icol<3) variance *= 100; // [cm^2] -> [mm^2] 
	  else if(irow>=3 && icol<3) variance *= 10; // [cm] -> [mm]
	  else if(irow<3 && icol>=3) variance *= 10; // [cm] -> [mm]
	  cov_mm[irow][icol] = variance;
	  counter++;
	}
      }

      // covariance helix (5 x 5) = J (5 x 6) cov_mm(6 x 6) JT (6 x 5)
      // J * cov
      TMatrixT<double> J_cov(5,6);
      J_cov.Mult(J_from_MARS_to_helix, cov_mm);
      // if(debug==true) J_cov.Print();                                                                                                        

      TMatrixT<double> J_cov_JT(5,5);
      J_cov_JT.MultT(J_cov, J_from_MARS_to_helix);
      // if(debug==true) J_cov_JT.Print();

      if(debug==true) std::cout << "covariance helix (k, phi, rho, cotg(theta), z_0)" << std::endl;
      if(debug==true) J_cov_JT.Print();
      
      // fill edm4hep tree for test ntuple output (not needed for edm4hep)
      edm4hep_tree->Fill(ievt,trackID,k,phi,rho,cotg,z0,
			 TMath::Sqrt(J_cov_JT[0][0]),TMath::Sqrt(J_cov_JT[1][1]),TMath::Sqrt(J_cov_JT[2][2]),TMath::Sqrt(J_cov_JT[3][3]),TMath::Sqrt(J_cov_JT[4][4]));
      
      // std::cout << "cotangente " << cotg << " theta [deg] = " << TMath::RadToDeg() * TMath::ATan(1./cotg) << std::endl;
      // std::cout << "momentum mag " << mom->Mag() << " px,py,pz " << mom->X() << " " << mom->Y() << " " << mom->Z() << " perp " << mom->Perp()  << std::endl;
      // std::cout << "variance "<< TMath::Sqrt(J_cov_JT[3][3]) << std::endl;

      // HITLIST  --------------------------------------------------------------------------      
      // detector ID 
      // int subDetectorHitNumbers CHECK
      // number of hits in particular subdetectors. Check/set collection variable TrackSubdetectorNames for decoding the indices
      // if(debug==true) std::cout << "podio " << l_track.getSubDetectorHitNumbers().size() << std::endl;
 
      // standalone DETID 1=SVX  0=DCH 2=PSHW
      // DD4hep:
      dd4hep::DetType typesvx( dd4hep::DetType::TRACKER | dd4hep::DetType::VERTEX | dd4hep::DetType::PIXEL ) ;         // DETID 1
      dd4hep::DetType typedch( dd4hep::DetType::TRACKER | dd4hep::DetType::GASEOUS | dd4hep::DetType::WIRE ) ;         // DETID 0
      dd4hep::DetType typepshw( dd4hep::DetType::TRACKER | dd4hep::DetType::GASEOUS | dd4hep::DetType::CALORIMETER ) ; // DETID 2
      /*
	int nhits_in_vxd = l_track.subdetectorHitNumbers()[0];
	if (nhits_in_vxd > 0) Track->setTypeBit( lcio::ILDDetID::VXD ) ;
      */

      /*
        nhits = nhitsdch + nhitssvx + nhitspshw
        ngoodhits = ngoodhitsdch + ngoodhitssvx + ngoodhitspshw
        nhits = hitindex.size() = detid.size() = skipped.size() = statevector->GetEntries()
        ngoodhits = nhits - "n(skipped==1)"
      */

      // -------------------------------------------------------------------
      // state vector
      counter=0;
      // radius @ innermost state
      double tmpradius = 10000; // initialized to extremely big value by choice [mm]
      for(int ihit = 0; ihit < statevector->GetEntries(); ihit++) {
	
	// CHECK fixed bug 2022-12-02
	// if(skipped.at(ihit)) continue;
	try { 
	  bool isthere = skipped.at(ihit);
	  if(isthere==true) continue;
	} 
	catch(std::out_of_range) {
	  std::cout << "event " << ievt << "out of range error because skipped size is " << skipped.size() << std::endl;
	}

	TVector3 *state = (TVector3*) statevector->At(ihit);
	if(state==NULL) continue;
	// std::cout << state << " " << counter << " " << "hit " << hitindex.at(ihit) << " detector id " << detid.at(ihit) << " is skipped " << skipped.at(ihit)
	//  	  << " -> position [cm] = state x " << state->X() << " y " << state->Y() << " z " << state->Z() << std::endl;
	*state *= 10.; // [cm] to [mm]
	if(state->Perp() < tmpradius) tmpradius = state->Perp();
	counter++;
      }
     
      // --------------------------------------------------------------------
      // convert to EDM track
      auto l_track = recoTracks->create();

      // flagword that defines the type of track.Bits 16-31 are used internally CHECK
      l_track.setType(12345);
      // Chi^2 of the track fit
      l_track.setChi2(chi2);
      // number of degrees of freedom of the track fit 
      l_track.setNdf(dof);
      // dEdx of the track CHECK
      float dEdx = 0;
      // error of dEdx CHECK
      float dEdxError = 0;
      // radius of the innermost hit that has been used in the track fit CHECK
      float radiusOfInnermostHit = tmpradius;

      // track state vector
      edm4hep::TrackState l_state;
      // for use with At{Other|IP|FirstHit|LastHit|Calorimeter|Vertex}|LastLocation CHECK
      l_state.location = l_state.AtIP;
      // CHECK ----------------------------------------------------------------
      /* all the following things must be checked */
      // ---- parameters ---------------
      // d0, transverse impact parameter [mm] 
      l_state.D0 = TMath::Sqrt( x0*x0 + y0*y0 );
      // phi, azimuthal angle [rad]
      l_state.phi = phi;
      // omega, signed curvature of the track in [1/mm]
      l_state.omega = k;
      // z_0, longitudinal impact parameter [mm]
      // already defined before
      // lambda is the dip angle of the track in r-z 
      l_state.tanLambda = TMath::Tan(TMath::Pi()*0.5 - theta);
      
      // Reference point of the track parameters
      // e.g. the origin at the IP
      edm4hep::Vector3f referencePoint(0,0,0);  // CHECK

      // lower triangular covariance matrix of the track parameters.
      // the order of parameters is  d0, phi, omega, z0, tan(lambda).
      // the array is a row-major flattening of the matrix. 
      std::array<float, 15> covMatrix; 

      /* 
	 0=00 1=10 2=11 3=20 4=21 5=22 6=30 7=31 8=32 9=33 10=40 11=41 12=42 13=43 14=44 
      */
      covMatrix[0] = J_cov_JT[0][0];
      covMatrix[1] = J_cov_JT[1][0];
      covMatrix[2] = J_cov_JT[1][1];
      covMatrix[3] = J_cov_JT[2][0];
      covMatrix[4] = J_cov_JT[2][1];
      covMatrix[5] = J_cov_JT[2][2];
      covMatrix[6] = J_cov_JT[3][0];
      covMatrix[7] = J_cov_JT[3][1];
      covMatrix[8] = J_cov_JT[3][2];
      covMatrix[9] = J_cov_JT[3][3];
      covMatrix[10] = J_cov_JT[4][0];
      covMatrix[11] = J_cov_JT[4][1];
      covMatrix[12] = J_cov_JT[4][2];
      covMatrix[13] = J_cov_JT[4][3];
      covMatrix[14] = J_cov_JT[4][4];

      l_state.covMatrix = covMatrix;

      /*  
	  VectorMembers:
	  - edm4hep::TrackState trackStates  //track states
       */
      l_track.addToTrackStates(l_state);
      
      /* not filled
	 OneToManyRelations:
	 - edm4hep::TrackerHit trackerHits  //hits that have been used to create this track
	 - edm4hep::Track tracks            //tracks (segments) that have been combined to create this tr
      */
    }
    
  
    // for each event write output
    if (l_writer != NULL)   l_writer->writeEvent();
    if (l_evtstore != NULL) l_evtstore->clearCollections();
  }

  fTest->Write();
  fTest->Close();
    
  // end of run
  fOutput.Write();
  fOutput.Close();

  l_writer->finish();
  
  
}
