

void TrackResol(TString foldList="0;1", TString foldBase="../../data/IDEA", Int_t runN=0){

  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(1111);

  TObjArray *FoldList=foldList.Tokenize(";");
  TString frunN=Form("%.05d.root",runN);
  int nFold=FoldList->GetEntries();

  TF1 fGaus("fGaus","gaus");
  fGaus.SetNpx(1000);

//  TCut simTrks("RecoTracks.TrkID==0");
  TCut simTrks("");
//  TCut mcTrks("MCTracks.fParentID==1&&(MCTracks.fPDGCode==-11||MCTracks.fPDGCode==11)");
  TCut mcTrks("MCTracks.fTrackID==RecoTracks.TrkID");
  TCut fittedTrks("RecoTracks.IsFitted==1");
  TCut qualityCut("RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof>0.5&&RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof<2.1");

  Double_t *sgmP = new Double_t[nFold];
  Double_t *erSgmP = new Double_t[nFold];
  Double_t *sgmTheta = new Double_t[nFold];
  Double_t *erSgmTheta = new Double_t[nFold];
  Double_t *sgmPhi = new Double_t[nFold];
  Double_t *erSgmPhi = new Double_t[nFold];
  Double_t *genMom = new Double_t[nFold];
  Double_t *erGenMom = new Double_t[nFold];

//  TClonesArray fBrDataTrk = new TClonesArray("GMCGeantTrack",0);

  for (Int_t ifl=0; ifl<nFold; ++ifl) {
    TString fFold = ((TObjString*)FoldList->At(ifl))->GetString();
    cout<<"Analyzing folder "<<fFold<<endl;
    TString fMCName = foldBase+"/"+fFold+"/"+"MCData"+frunN;
    TString fRCName = foldBase+"/"+fFold+"/"+"RecoData"+frunN;
    TString fOutName = foldBase+"/"+fFold+"/"+"RecoPltos"+frunN;

    TFile *fRec = TFile::Open(fRCName.Data());
    //TFile *fMc = TFile::Open(fMCName.Data());

    TFile *fOut = new TFile(fOutName.Data(),"recreate");

    TTree *RecoData = (TTree*)fRec->Get("RecoData");
    //TTree *MCData = (TTree*)fMC->Get("MCData");
    RecoData->AddFriend("MCData",fMCName.Data());
//    RecoData->SetBranchAddress("MCTracks",&fBrDataTrk);


    TCanvas *tmpCv1=new TCanvas();
    tmpCv1->cd();
//    TCanvas tmpCv1;
//    tmpCv1.cd();

    TH1F *hMomTemp = new TH1F("hMomTemp","",500,0,100);
    RecoData->Draw("MCData.MCTracks.fMomentum.Mag()>>hMomTemp",mcTrks);
    genMom[ifl] = hMomTemp->GetMean();
    erGenMom[ifl] = hMomTemp->GetMeanError();
    hMomTemp->Draw();


    TCanvas *tmpCv0=new TCanvas();
    tmpCv0->cd();
    TH1F *hNChi2= new TH1F("hNChi2","Reconstructed Tracks Chi2 over nDof",100,0,10);
    RecoData->Draw("RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof>>hNChi2",simTrks);
//    tmpCv1.SetLogy();
//    hNChi2->Fit(&fGaus);

    TCanvas *tmpCv2=new TCanvas();
    tmpCv2->cd();
//    TCanvas tmpCv2;
//    tmpCv2.cd();
    TH1F *hNHits= new TH1F("hNHits","Number of hits per tracks;# hits; entries",100,100,200);
    RecoData->Draw("RecoTracks.nhits>>hNHits",simTrks);
    TH1F *hNgoodHits= new TH1F("hNgoodHits","Number of good hits per fitted tracks;# hits; entries",100,100,200);
    RecoData->Draw("RecoTracks.ngoodhits>>hNgoodHits",simTrks+fittedTrks);

    hNgoodHits->SetLineColor(kRed);
    hNgoodHits->Draw();
    hNHits->Draw("same");

    TCanvas *tmpCv3=new TCanvas();
    tmpCv3->cd();
//    TCanvas tmpCv3;
//    tmpCv3.cd();
    TH1F *hTmpMomRes= new TH1F("hTmpMomRes","Momentum Resolution",1000,-100,100);
    RecoData->Draw("1000.0*RecoTracks.Momentum-MCData.MCTracks.fMomentum.Mag()>>hTmpMomRes",simTrks+fittedTrks+qualityCut+mcTrks);

//    Float_t *tmpMRes = hTmpMomRes->GetArray();
//    Double_t momResMin=0.0, momResMax=0.0;
//    for (int ib=1; ib<hTmpMomRes->GetNbinsX()+1; ++ib ) {
//      if (tmpMRes[ib]>0.0) {
//        momResMin = hTmpMomRes->GetBinCenter(ib);
//        break;
//      }
//    }
//    for (int ib=hTmpMomRes->GetNbinsX(); ib>0; --ib ) {
//      if (tmpMRes[ib]>0.0) {
//        momResMax = hTmpMomRes->GetBinCenter(ib);
//        break;
//      }
//    }
//    cout<<"--------- "<<momResMin<<" "<<momResMax<<endl;
//    Double_t momRange=momResMax*5;
//    //if (-momResMin>momRange) momRange=-momResMin;
//    TH1F *hMomRes= new TH1F("hMomRes","Momentum Resolution",500,-momRange,momRange);
//    RecoData->Draw("1000.0*RecoTracks.Momentum-MCData.MCTracks.fMomentum.Mag()>>hMomRes",simTrks+fittedTrks+qualityCut+mcTrks);
//    fGaus.SetParameter(1,0);
//    fGaus.SetParameter(2,1e-4);
//    hMomRes->Fit(&fGaus);
//
//    sgmP[ifl]=fGaus.GetParameter(2);
//    erSgmP[ifl]=fGaus.GetParError(2);
//
//    TCanvas tmpCv4;
//    tmpCv4.cd();
//    TH1F *hThetaRes= new TH1F("hThetaRes","Theta Resolution",200,-0.01,0.01);
//    RecoData->Draw("RecoTracks.theta-MCData.MCTracks.fMomentum.Theta()>>hThetaRes",simTrks+fittedTrks+qualityCut+mcTrks);
//    hThetaRes->Fit(&fGaus);
//
//    sgmTheta[ifl]=fGaus.GetParameter(2);
//    erSgmTheta[ifl]=fGaus.GetParError(2);
//
//    TCanvas tmpCv5;
//    tmpCv5.cd();
//    TH1F *hPhiRes= new TH1F("hPhiRes","Phi Resolution",200,-0.01,0.01);
//    RecoData->Draw("RecoTracks.phi-MCData.MCTracks.fMomentum.Phi()>>hPhiRes",simTrks+fittedTrks+qualityCut+mcTrks);
//    hPhiRes->Fit(&fGaus);
//
//    sgmPhi[ifl]=fGaus.GetParameter(2);
//    erSgmPhi[ifl]=fGaus.GetParError(2);
//
//    //---------------------
//
//    hNChi2->Write();
//    tmpCv1.Write();
//
//    hNHits->Write();
//    hNgoodHits->Write();
//    tmpCv2.Write();
//
//    hMomRes->Write();
//    tmpCv3.Write();
//
//    hThetaRes->Write();
//    tmpCv4.Write();
//
//    hPhiRes->Write();
//    tmpCv5.Write();
//
//    //--------------------------
//
//    hMomTemp->Delete();
//    hTmpMomRes->Delete();
//    hNChi2->Delete();
//    hNHits->Delete();
//    hNgoodHits->Delete();
//    hMomRes->Delete();
//    hThetaRes->Delete();
//    hPhiRes->Delete();
//
//    fOut->Close();
//    fOut->Delete();


    //delete fOut;
    //delete hNChi2;

  }

//  TString fOutName = foldBase+"/GlobalPlots.root";
//  TFile fOut1(fOutName.Data(),"recreate");
//
//  TCanvas tmpCv;
//  for (int ifl=0; ifl<nFold; ++ifl) {
//    sgmP[ifl]/=genMom[ifl];
//    erSgmP[ifl]/=genMom[ifl];
//  }
//
//  TGraphErrors momResTrnd(nFold,genMom,sgmP,erGenMom,erSgmP);
//  momResTrnd.SetMarkerStyle(8);
//  momResTrnd.Draw("ap");
//  momResTrnd.SetTitle("Momentum resolution trend");
//  momResTrnd.GetXaxis()->SetTitle("p [MeV]");
//  momResTrnd.GetYaxis()->SetTitle("#sigma p/p");
//  tmpCv.Update();
//
//  momResTrnd.Write();
//  tmpCv.Write();
//
//  TCanvas tmpCv2;
//  TGraphErrors thetaResTrnd(nFold,genMom,sgmTheta,erGenMom,erSgmTheta);
//  thetaResTrnd.SetMarkerStyle(8);
//  thetaResTrnd.Draw("ap");
//  thetaResTrnd.SetTitle("Theta resolution trend");
//  thetaResTrnd.GetXaxis()->SetTitle("p [MeV]");
//  thetaResTrnd.GetYaxis()->SetTitle("#sigma #Theta [rad]");
//  tmpCv2.Update();
//
//  momResTrnd.Write();
//  tmpCv2.Write();
//
//  TCanvas tmpCv3;
//  TGraphErrors phiResTrnd(nFold,genMom,sgmPhi,erGenMom,erSgmPhi);
//  phiResTrnd.SetMarkerStyle(8);
//  phiResTrnd.Draw("ap");
//  phiResTrnd.SetTitle("Phi resolution trend");
//  phiResTrnd.GetXaxis()->SetTitle("p [MeV]");
//  phiResTrnd.GetYaxis()->SetTitle("#sigma #Phi [rad]");
//  tmpCv3.Update();
//
//  phiResTrnd.Write();
//  tmpCv3.Write();
//
//  fOut1.Close();

}
