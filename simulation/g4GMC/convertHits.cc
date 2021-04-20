// Include files

#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TKey.h"
#include "TTree.h"
#include "TBranch.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "Riostream.h"

//
// #include "generated/GMCDCGeantStep.h"
// #include "generated/GMCPXGeantStep.h"
// #include "generated/GMCSVXGeantStep.h"
// #include "generated/GMCPSHWGeantStep.h"
// #include "generated/GMCPHCVGeantStep.h"
// #include "generated/GMCPHCVRadGeantStep.h"
// #include "generated/GMCGeantTrack.h"
// #include "ROMETreeInfo.h"

#include "GEOMSRVC/include/GMCG4Particle.hh"
#include "GEOMSRVC/include/GMCG4TrackerHit.hh"
//#include "SVX/include/ROGeometryHandle.hh"

#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"
#include "edm4hep/SimTrackerHitCollection.h"

// using namespace std;

int main(int argc,char** argv) 
{

  std::cout << "convert hits" << std::endl;

  //  gSystem->Load("$PRJBASE/simulation/g4GMC/lib/libGMCG4ClassesDict");
  gSystem->Load("$PRJBASE/simulation/g4GMC/build/lib/libGMCG4ClassesDict");
  if(argc<2) std::cout << "Missing name of the file to read!" << std::endl;

  TFile fo(argv[1]);

  TString br1("MCStep");
  TString br2("MCTracks");

  TTree *hit_tree;
  TTree *track_tree;

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

	fo.GetObject(key->GetName(), hit_tree);
	if (hit_tree->FindBranch("HitsStepCh")!=0x0) {
	  hitChIsPresent=true;
	  hit_tree->SetBranchAddress("HitsStepCh",&hitsch);
	  std::cout<<"Found DCH hits"<<std::endl;
	}
	if (hit_tree->FindBranch("HitsStepPx")!=0x0) {
	  hitPxIsPresent=true;
	  hit_tree->SetBranchAddress("HitsStepPx",&hitspx);
	  std::cout<<"Found Px hits"<<std::endl;
	}
	if (hit_tree->FindBranch("SVXHitsStepCh")!=0x0) {
	  hitSVXIsPresent=true;
	  hit_tree->SetBranchAddress("SVXHitsStepCh",&hitssvx);
	  std::cout<<"Found SVX hits"<<std::endl;
	}
	if (hit_tree->FindBranch("PSHWHitsStepCh")!=0x0) {
	  hitPSHWIsPresent=true;
	  hit_tree->SetBranchAddress("PSHWHitsStepCh",&hitspshw);
	  std::cout<<"Found PSHW hits"<<std::endl;
	}
	if (hit_tree->FindBranch("PHCVHitsStepCh")!=0x0) {
	  hitPHCVIsPresent=true;
	  hit_tree->SetBranchAddress("PHCVHitsStepCh",&hitsphcv);
	  std::cout<<"Found PHCV hits"<<std::endl;
	}
	if (hit_tree->FindBranch("PHCVHitsStepChRad")!=0x0) {
	  hitPHCVRadIsPresent=true;
	  hit_tree->SetBranchAddress("PHCVHitsStepChRad",&hitsphcvrd);
	  std::cout<<"Found PHCV Radiator hits"<<std::endl;
	}

	std::cout << "Collection: " << hit_tree->GetName() << std::endl;
	std::cout << "Number of events: " << hit_tree->GetEntries() << std::endl;

      }
      
      if (br2.CompareTo(key->GetName()) == 0) {
	fo.GetObject(key->GetName(), track_tree);
	track_tree->SetBranchAddress("Tracks",&tracks);
	std::cout << "Collection: " << track_tree->GetName() << std::endl;
	std::cout << "Number of events: " << track_tree->GetEntries() << std::endl;
      }
    }

  std::cout << "DCH hit     " << hitChIsPresent << std::endl;
  std::cout << "Px hit      " <<  hitPxIsPresent << std::endl;
  std::cout << "SVX hit     " <<  hitSVXIsPresent << std::endl;
  std::cout << "PSHW hit    " <<  hitPSHWIsPresent << std::endl;
  std::cout << "PHCV hit    " <<  hitPHCVIsPresent << std::endl;
  std::cout << "PHCV hit    " <<  hitPHCVRadIsPresent << std::endl;
 
  
  
  // ------------------------------------------------
  // output file name construction
  int fOutNum=1;
  if (argc==3) { fOutNum = TString(argv[2]).Atoi(); }
  else {
    TString fIn(argv[1]);
    fIn.ReplaceAll(".root","");
    fIn.ReplaceAll("hits","");
    if (fIn.IsDec()) { fOutNum = fIn.Atoi(); }
  }

  // output file ------------------------------------------
  TFile fOutput(Form("MCData%05d.root",fOutNum),"RECREATE");

  // create a new podio::EventStore, linked to a podio::ROOTWriter,
  // to write the collections on the output file
  std::string filename = fOutput.GetName();
  podio::EventStore * l_evtstore = new podio::EventStore();
  podio::ROOTWriter * l_writer = new podio::ROOTWriter(filename, l_evtstore);
  std::cout << "filename  " << filename << std::endl;
  std::cout << "evt store " << l_evtstore << std::endl;
  std::cout << "writer    " << l_writer << std::endl;

  // from https://github.com/key4hep/EDM4hep/blob/master/edm4hep.yaml
  // DCH
  edm4hep::SimTrackerHitCollection *s_dchtrackerHits = new edm4hep::SimTrackerHitCollection();
  l_evtstore->registerCollection("S_DCHtrackerHits",s_dchtrackerHits);
  l_writer->registerForWrite("S_DCHtrackerHits");
  // SVX
  edm4hep::SimTrackerHitCollection *s_svxtrackerHits = new edm4hep::SimTrackerHitCollection();
  l_evtstore->registerCollection("S_SVXtrackerHits",s_svxtrackerHits);
  l_writer->registerForWrite("S_SVXtrackerHits");
  // PSHW
  edm4hep::SimTrackerHitCollection *s_pshwtrackerHits = new edm4hep::SimTrackerHitCollection();
  l_evtstore->registerCollection("S_PSHWtrackerHits",s_pshwtrackerHits);
  l_writer->registerForWrite("S_PSHWtrackerHits");

  
  // event loop -------------------
  int nevt = hit_tree->GetEntries();
  std::cout << "nof events " << nevt << std::endl;
  for(int ievt=0; ievt<nevt; ievt++) {

    //    track_tree->GetEntry(ievt);
    hit_tree->GetEntry(ievt);

    // DCH ---------------------------------
    if (hitChIsPresent) {
      int nhits = hitsch->size();
      std::cout << "event " << ievt << " has nof DCH hits " << nhits << std::endl;

      // loop on DCH hits
      for (int ihit=0; ihit < nhits; ihit++) {

	// pre step pos
	G4ThreeVector s_pos = hitsch->at(ihit)->GetPos();
	// three-momentum
	G4ThreeVector mom = hitsch->at(ihit)->GetMomentum();
	// chamber number
	int chamberNb = hitsch->at(ihit)->GetChamberNb();
	// deposited energy
	double edep   = hitsch->at(ihit)->GetEdep();
	// step length
	double step_length = hitsch->at(ihit)->GetStepLength();
	// proper time 
	double pro_time = hitsch->at(ihit)->GetProperTime();

	// additional Get methods -------------------
	// track ID
	int trackID   = hitsch->at(ihit)->GetTrackID();
	// channel number (NOT USED)
	int channelNb = hitsch->at(ihit)->GetChannelNb();
	// deposited energy, not from ionization
	double edep_noion = hitsch->at(ihit)->GetNoIEdep();
	// global time 
	double glo_time = hitsch->at(ihit)->GetGlobalTime();
	// post step pos
	G4ThreeVector q_pos = hitsch->at(ihit)->GetPosEnding();
	// GetProcessCode 
	G4String proc_code = hitsch->at(ihit)->GetProcessCode();

	// CHECK from SVX/include/ROGeometryHandle.hh
	unsigned long cellID = chamberNb;

	// convert to EDM DCH hit ............................ !! CHECK all the variables and units !!
	auto l_hit = s_dchtrackerHits->create();
	
	// unsigned long long cellID / ID of the sensor that created this hit
        l_hit.setCellID(cellID); 
	// float EDep / energy deposited in the hit [GeV]
	l_hit.setEDep(edep);
	// float time / proper time of the hit in the lab frame in [ns]
	l_hit.setTime(pro_time);
	// float pathLength / path length of the particle in the sensitive material that resulted in this hit
	l_hit.setPathLength(step_length);
	// int quality / quality bit flag
	// CHECK does it exist?
	// edm4hep::Vector3d position / the hit position in [mm].
        edm4hep::Vector3d position(s_pos.x(), s_pos.y(), s_pos.z());
	l_hit.setPosition(position);
	// edm4hep::Vector3f momentum / the 3-momentum of the particle at the hits position in [GeV]
	edm4hep::Vector3f momentum(mom.x(), mom.y(), mom.z());
	l_hit.setMomentum(momentum);

	//	std::cout << "ihit " << ihit << std::endl;
      }
    }

    // SVX
    if (hitSVXIsPresent) {
      int nhits = hitssvx->size();
      std::cout << "event " << ievt << " has nof SVX hits " << nhits << std::endl;

      // loop on SVX hits
      for (int ihit=0; ihit < nhits; ihit++) {

	// pre step pos CHECK
	G4ThreeVector s_pos = hitssvx->at(ihit)->GetPos();
	// post step pos CHECK
	G4ThreeVector q_pos = hitssvx->at(ihit)->GetPosEnding();
	// three-momentum
	G4ThreeVector mom = hitssvx->at(ihit)->GetMomentum();
	// track ID
	int trackID   = hitssvx->at(ihit)->GetTrackID();
	// channel number
	int channelNb = hitssvx->at(ihit)->GetChannelNb();
	// chamber number
	int chamberNb = hitssvx->at(ihit)->GetChamberNb();
	// deposited energy
	double edep   = hitssvx->at(ihit)->GetEdep();
	// deposited energy, not from ionization
	double edep_noion = hitssvx->at(ihit)->GetNoIEdep();
	// global time ?? CHECK	
	double glo_time = hitssvx->at(ihit)->GetGlobalTime();
	// proper time ?? CHECK
	double pro_time = hitssvx->at(ihit)->GetProperTime();
	// step length
	double step_length = hitssvx->at(ihit)->GetStepLength();
	// GetProcessCode ?? CHECK
	G4String proc_code = hitssvx->at(ihit)->GetProcessCode();

        //std::cout << "proc_code " << proc_code << std::endl;

	// CHECK from SVX/include/ROGeometryHandle.hh
	unsigned long cellID = ((chamberNb/*+1*/)*1e+9) + channelNb;
	// std::cout << "SVX chamberNb " << chamberNb << " channelNb " << channelNb << " cellID " << cellID << std::endl;
	
		
	// convert to EDM SVX hit ............................ !! CHECK all the variables and units !!
	auto l_hit = s_svxtrackerHits->create();
	// unsigned long long cellID / ID of the sensor that created this hit
        l_hit.setCellID(cellID);
	// float EDep / energy deposited in the hit [GeV]
	l_hit.setEDep(edep);
	// float time / proper time of the hit in the lab frame in [ns]
	l_hit.setTime(pro_time);
	// float pathLength / path length of the particle in the sensitive material that resulted in this hit
	l_hit.setPathLength(step_length);
	// int quality / quality bit flag
	// CHECK does it exist?
	// edm4hep::Vector3d position / the hit position in [mm].
        edm4hep::Vector3d position(s_pos.x(), s_pos.y(), s_pos.z());
	l_hit.setPosition(position);
	// edm4hep::Vector3f momentum / the 3-momentum of the particle at the hits position in [GeV]
	edm4hep::Vector3f momentum(mom.x(), mom.y(), mom.z());
	l_hit.setMomentum(momentum);

	//	std::cout << "ihit " << ihit << std::endl;
      }
    }

    // PSHW
    if (hitPSHWIsPresent) {
      int nhits = hitspshw->size();
      std::cout << "event " << ievt << " has nof PSHW hits " << nhits << std::endl;

      // loop on PSHW hits
      for (int ihit=0; ihit < nhits; ihit++) {

	// pre step pos CHECK
	G4ThreeVector s_pos = hitspshw->at(ihit)->GetPos();
	// post step pos CHECK
	G4ThreeVector q_pos = hitspshw->at(ihit)->GetPosEnding();
	// three-momentum
	G4ThreeVector mom = hitspshw->at(ihit)->GetMomentum();
	// track ID
	int trackID   = hitspshw->at(ihit)->GetTrackID();
	// channel number
	int channelNb = hitspshw->at(ihit)->GetChannelNb();
	// chamber number
	int chamberNb = hitspshw->at(ihit)->GetChamberNb();
	// depoisted energy
	double edep   = hitspshw->at(ihit)->GetEdep();
	// deposited energy, not from ionization
	double edep_noion = hitspshw->at(ihit)->GetNoIEdep();
	// global time ?? CHECK	
	double glo_time = hitspshw->at(ihit)->GetGlobalTime();
	// proper time ?? CHECK
	double pro_time = hitspshw->at(ihit)->GetProperTime();
	// step length
	double step_length = hitspshw->at(ihit)->GetStepLength();
	// GetProcessCode ?? CHECK
	G4String proc_code = hitspshw->at(ihit)->GetProcessCode();

	// CHECK from SVX/include/ROGeometryHandle.hh
	unsigned long cellID = ((chamberNb/*+1*/)*1e+9) + channelNb;
	// std::cout << "PSHW chamberNb " << chamberNb << " channelNb " << channelNb << " cellID " << cellID << std::endl;

	
	// convert to EDM PSHW hit ............................ !! CHECK all the variables and units !!
	auto l_hit = s_pshwtrackerHits->create();
	// unsigned long long cellID / ID of the sensor that created this hit
        l_hit.setCellID(cellID);
	// float EDep / energy deposited in the hit [GeV]
	l_hit.setEDep(edep);
	// float time / proper time of the hit in the lab frame in [ns]
	l_hit.setTime(pro_time);
	// float pathLength / path length of the particle in the sensitive material that resulted in this hit
	l_hit.setPathLength(step_length);
	// int quality / quality bit flag
	// CHECK does it exist?
	// edm4hep::Vector3d position / the hit position in [mm].
        edm4hep::Vector3d position(s_pos.x(), s_pos.y(), s_pos.z());
	l_hit.setPosition(position);
	// edm4hep::Vector3f momentum / the 3-momentum of the particle at the hits position in [GeV]
	edm4hep::Vector3f momentum(mom.x(), mom.y(), mom.z());
	l_hit.setMomentum(momentum);

	//	std::cout << "ihit " << ihit << std::endl;
      }
    }

    
    // for each event write output
    if (l_writer != NULL)   l_writer->writeEvent();
    if (l_evtstore != NULL) l_evtstore->clearCollections();
  }

  
  // end of run
  // fOutput.Write();
  // fOutput.Close();

  l_writer->finish();
    

}
