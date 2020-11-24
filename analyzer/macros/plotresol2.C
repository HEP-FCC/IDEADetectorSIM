TH1D* stablefit(TH2* h2){
  h2->FitSlicesY();
  TH1D *hsig=(TH1D*)gROOT->FindObject(Form("%s_2",h2->GetName()));
  for(int i=0;i<h2->GetNbinsX();i++){
    TH1D *h1=h2->ProjectionY("_py",i+1,i+1);
    double rms=1e30;
    for(int ii=0;ii<3;ii++){
      rms=h1->GetRMS();
      h1->SetAxisRange(-5*rms,5*rms);
    }
    if(h1->Integral()==0) continue;
    double nbins=h1->GetRMS()/h1->GetBinWidth(1);
    if(nbins>10) {
      h1->RebinX(nbins/10);
      h1->SetAxisRange(-5*rms,5*rms);
    }
    h1->Fit("gaus","0q");
    rms=h1->GetFunction("gaus")->GetParameter(2);
    h1->SetAxisRange(-5*rms,5*rms);
    h1->Fit("gaus","0q");
    if(fabs(h1->GetFunction("gaus")->GetParError(2)/h1->GetFunction("gaus")->GetParameter(2))<0.5){
      hsig->SetBinContent(i+1,h1->GetFunction("gaus")->GetParameter(2));
      hsig->SetBinError(i+1,h1->GetFunction("gaus")->GetParError(2));
    }
  }
  return hsig;
}

 void BinLogX(TH1*h)
{
  TAxis *axis = h->GetXaxis();
  int bins = axis->GetNbins();
  
  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = TMath::Log(to/from)/TMath::Log(10.)/bins;
  Axis_t *new_bins = new Axis_t[bins + 1];
  
  for (int i = 0; i <= bins; i++) {
    new_bins[i] = from*TMath::Power(10, i * width); 
  }
  axis->Set(bins, new_bins);
  delete [] new_bins;
}

void plotresol2(int nrun=500,std::string runs="0051[0-1]",const char* opt="",const char* path="data",const char* recopt=""){
  // int nrun=500;
  // std::string runs="0051[0-1]";

  //int nrun=200;
  //std::string runs="002[0-2][0-9]";
   
  //  const char* path="data";
  double pmin=1;
  double pmax=150;
  bool islogx=true;
  
//  TCut simTrks("RecoTracks.TrkID==1");
//  TCut mcTrks("MCTracks.fTrackID==1");
//  TCut fittedTrks("RecoTracks.IsFitted==1");
//  TCut qualityCut("RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof<25.");


  //  TFile *fRec = TFile::Open(Form("data/RecoData%.05d.root",nrun));
  //  TTree *RecoData = (TTree*)fRec->Get("RecoData");
  //RecoData->AddFriend("MCHits",Form("data/MCHits%.05d.root",nrun));

  const char *simfile=runs==""?Form("%s/MCHits%05i.root",path,nrun):Form("%s/MCHits%s.root",path,runs.c_str());
  const char *recfile=runs==""?Form("%s/RecoData%05i%s.root",path,nrun,recopt):Form("%s/RecoData%s%s.root",path,runs.c_str(),recopt);

  
  TChain *chtreerec=new TChain("RecoData","RecoData");
  TChain *chtreesim=new TChain("MCHits","MCHits");
  chtreerec->Add(recfile);
  chtreesim->Add(simfile);
  chtreerec->AddFriend(chtreesim);
  
  TTree *RecoData =chtreerec;
  
  TFile *fOut = new TFile(Form("RecoPlots%.05d%s.root",nrun,recopt),"recreate");


  
  TH1F *hNChi2= new TH1F("hNChi2","Reconstructed Tracks Chi2 over nDof",1000,0,10);
  TH1F *hNHits= new TH1F("hNHits","Number of dch hits per tracks;# hits; entries",300,0,300);
  TH1F *hNgoodHits= new TH1F("hNgoodHits","Number of good dch hits per fitted tracks;# hits; entries",300,0,300);

  TH1F *hNHitsSVX= new TH1F("hNHitsSVX","Number of svx hits per tracks;# hits; entries",30,0,30);
  TH1F *hNgoodHitsSVX= new TH1F("hNgoodHitsSVX","Number of good svx hits per fitted tracks;# hits; entries",30,0,30);
  TH1F *hNHitsPSHW= new TH1F("hNHitsPSHW","Number of pshw hits per tracks;# hits; entries",10,0,10);
  TH1F *hNgoodHitsPSHW= new TH1F("hNgoodHitsPSHW","Number of good pshw hits per fitted tracks;# hits; entries",10,0,10);

  TH1F *hMomRes= new TH1F("hMomRes","Momentum Resolution;#Delta P [MeV/c]",500,-5,5);
  TH1F *hMomPtRes= new TH1F("hMomPtRes","Momentum Resolution;#Delta Pt [MeV/c]",500,-5,5);
  TH1F *hThetaRes= new TH1F("hThetaRes","Theta Resolution;#Delta#theta [mrad]",500,-50,50);
  TH1F *hPhiRes= new TH1F("hPhiRes","Phi Resolution;#Delta#phi [mrad]",500,-50,50);
  TH1F *hdRes= new TH1F("hdRes","R-phi vtx Resolution;#Delta r-phi vtx [mm]",500,-10.0,10.0);
  TH1F *hd0Res= new TH1F("hd0Res","d0 vtx Resolution;#Delta d0 vtx [mm]",250,0.0,10.0);
  TH1F *hzRes= new TH1F("hzRes","Z vtx Resolution;#Delta Z vtx [mm]",500,-10.0,10.0);

  TH2F *hMomRes2= new TH2F("hMomRes2","Momentum Resolution;Momentum, MeV/c;#Delta P/P",100,pmin,pmax,2000,-0.05,0.05);
  TH2F *hMomPtRes2= new TH2F("hMomPtRes2","Momentum Resolution;Transverse Momentum, MeV/c;#Delta Pt/Pt",100,pmin,pmax,2000,-0.05,0.05);
  TH2F *hMomPt2Res2= new TH2F("hMomPt2Res2","Momentum Resolution;Transverse Momentum, MeV/c;#Delta Pt/Pt^{2}",100,pmin,pmax,2000,-0.005,0.005);
  TH2F *hThetaRes2= new TH2F("hThetaRes2","Theta Resolution;Momentum, MeV/c;#theta,rad",100,pmin,pmax,2000,-0.01,0.01);
  TH2F *hPhiRes2= new TH2F("hPhiRes2","Phi Resolution;Momentum, MeV/c;#phi,rad",100,pmin,pmax,2000,-0.01,0.01);

  TH2F *hMomvsThRes2= new TH2F("hMomvsThRes2","Momentum Resolution;cos(#theta);#Delta P/P",100,-1,1,2000,-0.05,0.05);
  TH2F *hMomPtvsThRes2= new TH2F("hMomPtvsThRes2","Momentum Resolution;cos(#theta);#Delta Pt/Pt",100,-1,1,2000,-0.05,0.05);
  TH2F *hThetavsThRes2= new TH2F("hThetavsThRes2","Theta Resolution;cos(#theta);#theta,rad",100,-1,1,2000,-0.01,0.01);
  TH2F *hPhivsThRes2= new TH2F("hPhivsThRes2","Phi Resolution;cos(#theta);#phi,rad",100,-1,1,2000,-0.01,0.01);

  TH2F *hNHitsvsThRes2= new TH2F("hNHitsvsThRes2","Number of dch hits per tracks;cos(#theta);# hits; rad",100,-1,1,300,0,300);
  TH2F *hNgoodHitsvsThRes2= new TH2F("hNgoodHitsvsThRes2","Number of good dch hits per fitted tracks;cos(#theta);# hits; rad",100,-1,1,300,0,300);

  TH2F *hNHitsSVXvsThRes2= new TH2F("hNHitsSVXvsThRes2","Number of svx hits per tracks;cos(#theta);# hits; rad",100,-1,1,30,0,30);
  TH2F *hNgoodHitsSVXvsThRes2= new TH2F("hNgoodHitsSVXvsThRes2","Number of good svx hits per fitted tracks;cos(#theta);# hits; rad",100,-1,1,30,0,30);
  TH2F *hNHitsPSHWvsThRes2= new TH2F("hNHitsPSHWvsThRes2","Number of pshw hits per tracks;cos(#theta);# hits; rad",100,-1,1,10,0,10);
  TH2F *hNgoodHitsPSHWvsThRes2= new TH2F("hNgoodHitsPSHWvsThRes2","Number of good pshw hits per fitted tracks;cos(#theta);# hits; rad",100,-1,1,10,0,10);

  TH2F *hdRes2= new TH2F("hdRes2","R-phi vtx Resolution;Momentum, MeV/c;r-phi vtx,#mu m",100,0,pmax,10000,-500.0,500.0);
  TH2F *hd0Res2= new TH2F("hd0Res2","d0 vtx Resolution;Momentum, MeV/c;d0 vtx,#mu m",100,0,pmax,10000,-500.0,500.0);
  TH2F *hzRes2= new TH2F("hzRes2","Z vtx Resolution;Momentum, MeV/c;Z vtx,#mu m",100,0,pmax,10000,-500.0,500.0);
  if(1==1&&islogx){
    BinLogX(hMomRes2);
    BinLogX(hMomPtRes2);
    BinLogX(hMomPt2Res2);
    BinLogX(hThetaRes2);
    BinLogX(hPhiRes2);
  }

  TClonesArray *recs = new TClonesArray("GMCRecoTracks");
  RecoData->SetBranchAddress("RecoTracks",&recs);
  TClonesArray *trcks = new TClonesArray("GMCGeantTrack");
  RecoData->SetBranchAddress("MCTracks",&trcks);

  bool simTrks, mcTrks, fittedTrks, qualityCut;
  for (int iev=0; iev<RecoData->GetEntries(); ++iev) {
    RecoData->GetEntry(iev);
    GMCGeantTrack *mcTrk;
    mcTrks=false;
    for (int imctrk=0; imctrk<trcks->GetEntriesFast(); ++imctrk) {
      mcTrk = (GMCGeantTrack *) trcks->At(imctrk);
      if ( mcTrk->GetfTrackID()==1 ) { mcTrks=true; break; }
    }
    simTrks=fittedTrks=qualityCut=false;
    GMCRecoTracks *recTrk;
    for (int irectrk=0; irectrk<recs->GetEntriesFast(); ++irectrk) {
      recTrk = (GMCRecoTracks *) recs->At(irectrk);
      if (recTrk->GetTrkID()==1) { simTrks=true; break; }
    }
    if (simTrks && recTrk->Getdof()>0) {
      hNChi2->Fill(recTrk->Getchi2()/recTrk->Getdof());
      hNHits->Fill(recTrk->Getnhitsdch());
      hNHitsSVX->Fill(recTrk->Getnhitssvx());
      hNHitsPSHW->Fill(recTrk->Getnhitspshw());
      fittedTrks = recTrk->GetIsFitted();
      if (fittedTrks) {
        hNgoodHits->Fill(recTrk->Getngoodhitsdch());
        hNgoodHitsSVX->Fill(recTrk->Getngoodhitssvx());
        hNgoodHitsPSHW->Fill(recTrk->Getngoodhitspshw());

        qualityCut=recTrk->Getchi2()/recTrk->Getdof()<5;

        if (qualityCut&&mcTrks) {
          Double_t simMom = mcTrk->GetfMomentum()->Mag();//*0.001;
          Double_t simPt = mcTrk->GetfMomentum()->Pt();//*0.001;
          Double_t simCosth = mcTrk->GetfMomentum()->CosTheta();

          Double_t recMom = recTrk->GetMomentum()*1000.0;
          Double_t recPt = recTrk->Getmom()->Pt()*1000.0;

          if (simMom>0.0) hMomRes->Fill(recMom-simMom);
          if (simPt!=0.0) hMomPtRes->Fill(recPt-simPt);
          hThetaRes->Fill((recTrk->Gettheta()-mcTrk->GetfMomentum()->Theta())*1000.0);
          hPhiRes->Fill((recTrk->Getphi()-mcTrk->GetfMomentum()->Phi())*1000.0);
          if (recTrk->GetMomentum()>0.0) hdRes->Fill( ( (recTrk->Getx0()*recTrk->Getmom()->Y()-recTrk->Gety0()*recTrk->Getmom()->X())*1000.0/recTrk->GetMomentum() )*0.001 );
          if (recTrk->GetMomentum()>0.0) hd0Res->Fill(TMath::Sqrt(recTrk->Getx0()*recTrk->Getx0()+recTrk->Gety0()*recTrk->Gety0()+recTrk->Getz0()*recTrk->Getz0())/**1000.0*/);
          hzRes->Fill(recTrk->Getz0()/**1000.0*/);

          if (simMom>0.0) hMomRes2->Fill(simMom,recMom/simMom-1.0);
          if (simPt!=0.0) hMomPtRes2->Fill(simPt,recPt/simPt-1.0);
          if (simPt!=0.0) hMomPt2Res2->Fill(simPt,(recPt-simPt)/(simPt*simPt));
          hThetaRes2->Fill(simMom,recTrk->Gettheta()-mcTrk->GetfMomentum()->Theta());
          hPhiRes2->Fill(simMom,recTrk->Getphi()-mcTrk->GetfMomentum()->Phi());
          if (simMom>0.0) hMomvsThRes2->Fill(simCosth,recMom/simMom-1.0);
          if (simPt!=0.0) hMomPtvsThRes2->Fill(simCosth,recPt/simPt-1.0);
          hThetavsThRes2->Fill(simCosth,recTrk->Gettheta()-mcTrk->GetfMomentum()->Theta());
          hPhivsThRes2->Fill(simCosth,recTrk->Getphi()-mcTrk->GetfMomentum()->Phi());

          hNHitsvsThRes2->Fill(simCosth,recTrk->Getnhitsdch());
          hNgoodHitsvsThRes2->Fill(simCosth,recTrk->Getngoodhitsdch());
          hNHitsSVXvsThRes2->Fill(simCosth,recTrk->Getnhitssvx());
          hNgoodHitsSVXvsThRes2->Fill(simCosth,recTrk->Getngoodhitssvx());
          hNHitsPSHWvsThRes2->Fill(simCosth,recTrk->Getnhitspshw());
          hNgoodHitsPSHWvsThRes2->Fill(simCosth,recTrk->Getngoodhitspshw());

          if (recTrk->GetMomentum()>0.0) hdRes2->Fill(simMom,(recTrk->Getx0()*recTrk->Getmom()->Y()-recTrk->Gety0()*recTrk->Getmom()->X())*1000.0/recTrk->GetMomentum());
          if (recTrk->GetMomentum()>0.0) hd0Res2->Fill(simMom,TMath::Sqrt(recTrk->Getx0()*recTrk->Getx0()+recTrk->Gety0()*recTrk->Gety0()+recTrk->Getz0()*recTrk->Getz0())*1000.0);
          hzRes2->Fill(simMom,recTrk->Getz0()*1000.0);
        }

      }
    }

//    cout<<"Iev "<<iev<<" mcTrks "<<mcTrks<<" simTrks "<< simTrks<<" fittedTrks "<<fittedTrks <<" qualityCut "<<qualityCut <<endl;

    if (iev%500==0) {cout<<"Processing ev "<<iev<<endl;}
  }


//  RecoData->Draw("RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof>>hNChi2",simTrks);
//  RecoData->Draw("RecoTracks.nhitsdch>>hNHits",simTrks);
//  RecoData->Draw("RecoTracks.ngoodhitsdch>>hNgoodHits",simTrks+fittedTrks);
//  RecoData->Draw("RecoTracks.nhitssvx>>hNHitsSVX",simTrks);
//  RecoData->Draw("RecoTracks.ngoodhitssvx>>hNgoodHitsSVX",simTrks+fittedTrks);
//  RecoData->Draw("RecoTracks.nhitspshw>>hNHitsPSHW",simTrks);
//  RecoData->Draw("RecoTracks.ngoodhitspshw>>hNgoodHitsPSHW",simTrks+fittedTrks);

//  RecoData->Draw("RecoTracks.Momentum/MCHits.MCTracks.fMomentum.Mag()/0.001-1:MCHits.MCTracks.fMomentum.Mag()*0.001>>hMomRes2",simTrks+fittedTrks+qualityCut+mcTrks);
//  RecoData->Draw("RecoTracks.mom.Pt()/MCHits.MCTracks.fMomentum.Pt()/0.001-1:MCHits.MCTracks.fMomentum.Pt()*0.001>>hMomPtRes2",simTrks+fittedTrks+qualityCut+mcTrks);

//  RecoData->Draw("RecoTracks.theta-MCHits.MCTracks.fMomentum.Theta():MCHits.MCTracks.fMomentum.Mag()*0.001>>hThetaRes2",simTrks+fittedTrks+qualityCut+mcTrks);
//  RecoData->Draw("RecoTracks.phi-MCHits.MCTracks.fMomentum.Phi():MCHits.MCTracks.fMomentum.Mag()*0.001>>hPhiRes2",simTrks+fittedTrks+qualityCut+mcTrks);

//  RecoData->Draw("RecoTracks.Momentum/MCHits.MCTracks.fMomentum.Mag()/0.001-1:MCHits.MCTracks.fMomentum.CosTheta()>>hMomvsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);
//  RecoData->Draw("RecoTracks.mom.Pt()/MCHits.MCTracks.fMomentum.Pt()/0.001-1:MCHits.MCTracks.fMomentum.CosTheta()>>hMomPtvsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);

//  RecoData->Draw("RecoTracks.theta-MCHits.MCTracks.fMomentum.Theta():MCHits.MCTracks.fMomentum.CosTheta()>>hThetavsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);
//  RecoData->Draw("RecoTracks.phi-MCHits.MCTracks.fMomentum.Phi():MCHits.MCTracks.fMomentum.CosTheta()>>hPhivsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);

//  RecoData->Draw("(RecoTracks.x0*RecoTracks.mom.Y()-RecoTracks.y0*RecoTracks.mom.X())*1000/RecoTracks.Momentum:MCHits.MCTracks.fMomentum.Mag()*0.001>>hdRes2",simTrks+fittedTrks+qualityCut+mcTrks);
//  RecoData->Draw("RecoTracks.z0*1000:MCHits.MCTracks.fMomentum.Mag()*0.001>>hzRes2",simTrks+fittedTrks+qualityCut+mcTrks);

  TCanvas* c1=new TCanvas("chit","",800,800);
  c1->Divide(2,2,0.001);
  c1->cd(1)->SetLogy();
  hNChi2->Draw();

  c1->cd(2);
  hNgoodHits->SetLineColor(kRed);
  hNgoodHits->Draw();
  hNHits->Draw("same");
  c1->cd(3);
  hNgoodHitsSVX->SetLineColor(kRed);
  hNgoodHitsSVX->Draw();
  hNHitsSVX->Draw("same");
  c1->cd(4);
  hNgoodHitsPSHW->SetLineColor(kRed);
  hNgoodHitsPSHW->Draw();
  hNHitsPSHW->Draw("same");

  //  gROOT->LoadMacro("/data/comet/macros/stablefit.h");
  gStyle->SetOptStat(0);

  TCanvas *cres=new TCanvas("cres","",800,800);
  cres->Divide(2,2,0.001);
  TVirtualPad *p=cres->cd(1);
  TH1D* hMomRes2_2=stablefit(hMomRes2);
  hMomRes2_2->Draw();
  hMomRes2_2->SetTitle("Momentum Resolution;Momentum, MeV/c;#sigma P/P");
  if(islogx) p->SetLogx();
  hMomRes2_2->SetAxisRange(1e-4,0.01,"Y");
  p->SetLogy();

  p=cres->cd(2);
  TH1D* hMomPtRes2_2=stablefit(hMomPtRes2);
  hMomPtRes2_2->Draw();
  hMomPtRes2_2->SetTitle("Transverse Momentum Resolution;Transverse Momentum, MeV/c;#sigma P_{tr}/P_{tr}");
  if(islogx) p->SetLogx();
  hMomPtRes2_2->SetAxisRange(1e-4,0.01,"Y");
  p->SetLogy();

  p=cres->cd(3);
  TH1D* hThetaRes2_2=stablefit(hThetaRes2);
  TH1D* hPhiRes2_2=stablefit(hPhiRes2);
  hThetaRes2_2->Draw();
  hPhiRes2_2->Draw("same");
  hPhiRes2_2->SetLineColor(2);
  hThetaRes2_2->SetTitle("Theta Resolution;Momentum, MeV/c;#sigma #theta, rad");
  hPhiRes2_2->SetTitle("Phi Resolution;Momentum, MeV/c;#sigma #phi, rad");

  hThetaRes2_2->SetTitle("Angle Resolution;Momentum, MeV/c;#sigma angle, rad");
  TLegend *leg=new TLegend(0.7,0.7,0.85,0.8);
  leg->AddEntry(hThetaRes2_2,"theta");
  leg->AddEntry(hPhiRes2_2,"phi");
  leg->Draw();
  if(islogx) p->SetLogx();
  p->SetLogy();
  hThetaRes2_2->SetAxisRange(1e-5,1e-2,"Y");

  
  cres->cd(4)->SetLogy();
  TH1D* hdRes2_2=stablefit(hdRes2);
  TH1D* hd0Res2_2=stablefit(hd0Res2);
  TH1D* hzRes2_2=stablefit(hzRes2);
  hdRes2_2->Draw();
  hd0Res2_2->Draw("same");
  hzRes2_2->Draw("same");
  hzRes2_2->SetLineColor(2);
  hdRes2_2->SetTitle("R-phi vtx Resolution;Momentum, MeV/c;#sigma r-phi vtx,#mu m");
  hd0Res2_2->SetTitle("d0 vtx Resolution;Momentum, MeV/c;#sigma d0 vtx,#mu m");
  hzRes2_2->SetTitle("Z vtx Resolution;Momentum, MeV/c;#sigma z vtx,#mu m");

  leg=new TLegend(0.7,0.7,0.85,0.8);
  leg->AddEntry(hdRes2_2,"vtx r-phi");
  leg->AddEntry(hd0Res2_2,"vtx d0");
  leg->AddEntry(hzRes2_2,"vtx z");
  leg->Draw();
  if(islogx) p->SetLogx();
  p->SetLogy();
  //  hMomPtRes2_2->SetAxisRange(1e-5,1e-2);

  TCanvas *cresth=new TCanvas("cresvsth","",800,800);
  TH1D* hMomvsThRes2_2=stablefit(hMomvsThRes2);
  TH1D* hMomPtvsThRes2_2=stablefit(hMomPtvsThRes2);
  TH1D* hThetavsThRes2_2=stablefit(hThetavsThRes2);
  TH1D* hPhivsThRes2_2=stablefit(hPhivsThRes2);
  hMomvsThRes2_2->SetTitle("");
  hMomvsThRes2_2->Draw();
  hMomPtvsThRes2_2->Draw("same");
  hThetavsThRes2_2->Draw("same");
  hPhivsThRes2_2->Draw("same");
  hMomvsThRes2_2->SetLineColor(kRed);
  hMomPtvsThRes2_2->SetLineColor(kBlack);
  hPhivsThRes2_2->SetLineColor(kGreen);
  hThetavsThRes2_2->SetLineColor(kBlue);
  hMomvsThRes2_2->SetAxisRange(1e-5,1e-1,"Y");
  leg=new TLegend(0.55,0.7,0.8,0.9);
  leg->AddEntry(hMomvsThRes2_2,"#sigma P/P");
  leg->AddEntry(hMomPtvsThRes2_2,"#sigma Pt/Pt");
  leg->AddEntry(hPhivsThRes2_2,"#sigma phi");
  leg->AddEntry(hThetavsThRes2_2,"#sigma theta");

  leg->Draw();

  TH1D* hNHitsvsThRes2_2 = stablefit(hNHitsvsThRes2);
  TH1D* hNgoodHitsvsThRes2_2 = stablefit(hNgoodHitsvsThRes2);
  TH1D* hNHitsSVXvsThRes2_2 = stablefit(hNHitsSVXvsThRes2);
  TH1D* hNgoodHitsSVXvsThRes2_2 = stablefit(hNgoodHitsSVXvsThRes2);
  TH1D* hNHitsPSHWvsThRes2_2 = stablefit(hNHitsPSHWvsThRes2);
  TH1D* hNgoodHitsPSHWvsThRes2_2 = stablefit(hNgoodHitsPSHWvsThRes2);
  
  cresth->SetLogy();
  
  TCanvas *cres2=new TCanvas("cres2","",800,800);
  TH1D* hMomPt2Res2_2=stablefit(hMomPt2Res2);
  hMomPt2Res2_2->Draw();
  hMomPt2Res2_2->SetTitle("Transverse Momentum Resolution;Transverse Momentum, MeV/c;#sigma P_{tr}/P_{tr}^{2}");
  if(islogx) cres2->SetLogx();
  hMomPt2Res2_2->SetAxisRange(1e-6,1e-4,"Y");
  cres2->SetLogy();

  c1->Print(Form("nhit%s.png",opt));
  cres->Print(Form("res%s.png",opt));
  cresth->Print(Form("resvsth%s.png",opt));

  c1->Write();
  cres->Write();
  cresth->Write();
  cres2->Write();

  gStyle->SetOptStat(11220);
  gStyle->SetOptFit(1);

  TF1 *fdg= new TF1("fdg","[0]*([5]*TMath::Gaus(x,[1],[2],1)+(1-[5])*TMath::Gaus(x,[3],[4],1))",-1000,1000);
  fdg->SetNpx(1000);
  fdg->SetParLimits(0,0,1e+6);
  fdg->SetParLimits(5,0,1);
  fdg->SetParNames("Const","#mu_{core}","#sigma_{core}","#mu_{tail}","#sigma_{tail}","frac_{core}");

  TCanvas *cresMomLin=new TCanvas("cresMomLin","",800,800);
//  cresMomLin->Divide(2,1,0.001);
  p=cresMomLin->cd();
  hMomRes->Draw();
  fdg->SetParameter(0,hMomRes->GetEntries()*hMomRes->GetBinWidth(0));
  fdg->SetParameter(1,hMomRes->GetMean());
  fdg->SetParameter(2,hMomRes->GetStdDev()/2.0);
  fdg->SetParameter(3,hMomRes->GetMean());
  fdg->SetParameter(4,hMomRes->GetStdDev()*3.0);
  fdg->SetParameter(5,0.8);
  hMomRes->Fit(fdg);
//  if(islogx) p->SetLogx();
  p->SetLogy();

  TCanvas *cresMomPtLin=new TCanvas("cresMomPtLin","",800,800);
  p=cresMomPtLin->cd();
  hMomPtRes->Draw();
  fdg->SetParameter(0,hMomPtRes->GetEntries()*hMomPtRes->GetBinWidth(0));
  fdg->SetParameter(1,hMomPtRes->GetMean());
  fdg->SetParameter(2,hMomPtRes->GetStdDev()/2.0);
  fdg->SetParameter(3,hMomPtRes->GetMean());
  fdg->SetParameter(4,hMomPtRes->GetStdDev()*3.0);
  fdg->SetParameter(5,0.8);
  hMomPtRes->Fit(fdg);
//  hMomPtRes->Fit("gaus");
//  if(islogx) p->SetLogx();
  p->SetLogy();

  TCanvas *cresLin=new TCanvas("cresLin","",960,960);
  cresLin->Divide(2,2,0.001);
  p=cresLin->cd(1);
  hThetaRes->Draw();
  fdg->SetParameter(0,hThetaRes->GetEntries()*hThetaRes->GetBinWidth(0));
  fdg->SetParameter(1,hThetaRes->GetMean());
  fdg->SetParameter(2,hThetaRes->GetStdDev()/2.0);
  fdg->SetParameter(3,hThetaRes->GetMean());
  fdg->SetParameter(4,hThetaRes->GetStdDev()*3.0);
  fdg->SetParameter(5,0.8);
  hThetaRes->Fit(fdg);
//  hThetaRes->Fit("gaus");
//  if(islogx) p->SetLogx();
  p->SetLogy();

  p=cresLin->cd(2);
  hPhiRes->Draw();
  fdg->SetParameter(0,hPhiRes->GetEntries()*hPhiRes->GetBinWidth(0));
  fdg->SetParameter(1,hPhiRes->GetMean());
  fdg->SetParameter(2,hPhiRes->GetStdDev()/2.0);
  fdg->SetParameter(3,hPhiRes->GetMean());
  fdg->SetParameter(4,hPhiRes->GetStdDev()*3.0);
  fdg->SetParameter(5,0.8);
  hPhiRes->Fit(fdg);
//  hPhiRes->Fit("gaus");
//  if(islogx) p->SetLogx();
  p->SetLogy();

  p=cresLin->cd(3);
  hdRes->Draw();
  fdg->SetParameter(0,hdRes->GetEntries()*hdRes->GetBinWidth(0));
  fdg->SetParameter(1,hdRes->GetMean());
  fdg->SetParameter(2,hdRes->GetStdDev()/2.0);
  fdg->SetParameter(3,hdRes->GetMean());
  fdg->SetParameter(4,hdRes->GetStdDev()*3.0);
  fdg->SetParameter(5,0.8);
  hdRes->Fit(fdg);
//  hdRes->Fit("gaus");
//  if(islogx) p->SetLogx();
  p->SetLogy();

  p=cresLin->cd(4);
  hzRes->Draw();
  fdg->SetParameter(0,hzRes->GetEntries()*hzRes->GetBinWidth(0));
  fdg->SetParameter(1,hzRes->GetMean());
  fdg->SetParameter(2,hzRes->GetStdDev()/2.0);
  fdg->SetParameter(3,hzRes->GetMean());
  fdg->SetParameter(4,hzRes->GetStdDev()*3.0);
  fdg->SetParameter(5,0.8);
  hzRes->Fit(fdg);
//  hzRes->Fit("gaus");
//  if(islogx) p->SetLogx();
  p->SetLogy();

  cresMomLin->Print(Form("linMomRes%s.png",opt));
  cresMomPtLin->Print(Form("linMomPtRes%s.png",opt));
  cresLin->Print(Form("linRes%s.png",opt));

  cresMomLin->Write();
  cresMomPtLin->Write();
  cresLin->Write();
  fOut->Write();
}
