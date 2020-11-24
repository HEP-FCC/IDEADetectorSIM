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

void plotresol(int nrun=500,std::string runs="0051[0-1]",const char* opt="",const char* path="data",const char* recopt=""){
  // int nrun=500;
  // std::string runs="0051[0-1]";

  //int nrun=200;
  //std::string runs="002[0-2][0-9]";
   
  //  const char* path="data";
  double pmin=0.1;
  double pmax=100;
  bool islogx=true;
  
  TCut simTrks("RecoTracks.TrkID==1");
  TCut mcTrks("MCTracks.fTrackID==1");
  TCut fittedTrks("RecoTracks.IsFitted==1");
  TCut qualityCut("RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof<25.");


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

  TH2F *hMomRes2= new TH2F("hMomRes2","Momentum Resolution;Momentum, GeV/c;#Delta P/P",100,pmin,pmax,2000,-0.05,0.05);
  TH2F *hMomPtRes2= new TH2F("hMomPtRes2","Momentum Resolution;Transverse Momentum, GeV/c;#Delta Pt/Pt",100,pmin,pmax,2000,-0.05,0.05);
  TH2F *hThetaRes2= new TH2F("hThetaRes2","Theta Resolution;Momentum, GeV/c;#theta,rad",100,pmin,pmax,2000,-0.01,0.01);
  TH2F *hPhiRes2= new TH2F("hPhiRes2","Phi Resolution;Momentum, GeV/c;#phi,rad",100,pmin,pmax,2000,-0.01,0.01);

  TH2F *hMomvsThRes2= new TH2F("hMomvsThRes2","Momentum Resolution;cos(#theta);#Delta P/P",100,-1,1,2000,-0.05,0.05);
  TH2F *hMomPtvsThRes2= new TH2F("hMomPtvsThRes2","Momentum Resolution;cos(#theta);#Delta Pt/Pt",100,-1,1,2000,-0.05,0.05);
  TH2F *hThetavsThRes2= new TH2F("hThetavsThRes2","Theta Resolution;cos(#theta);#theta,rad",100,-1,1,2000,-0.01,0.01);
  TH2F *hPhivsThRes2= new TH2F("hPhivsThRes2","Phi Resolution;cos(#theta);#phi,rad",100,-1,1,2000,-0.01,0.01);

  TH2F *hdRes2= new TH2F("hdRes2","R-phi vtx Resolution;Momentum, GeV/c;r-phi vtx,#mu m",100,0,pmax,10000,-500.0,500.0);
  TH2F *hzRes2= new TH2F("hzRes2","Z vtx Resolution;Momentum, GeV/c;Z vtx,#mu m",100,0,pmax,10000,-500.0,500.0);
  if(1==1&&islogx){
    BinLogX(hMomRes2);
    BinLogX(hMomPtRes2);
    BinLogX(hThetaRes2);
    BinLogX(hPhiRes2);
  }

  RecoData->Draw("RecoTracks.RecoTracks.chi2/RecoTracks.RecoTracks.dof>>hNChi2",simTrks);
  RecoData->Draw("RecoTracks.nhitsdch>>hNHits",simTrks);
  RecoData->Draw("RecoTracks.ngoodhitsdch>>hNgoodHits",simTrks+fittedTrks);
  RecoData->Draw("RecoTracks.nhitssvx>>hNHitsSVX",simTrks);
  RecoData->Draw("RecoTracks.ngoodhitssvx>>hNgoodHitsSVX",simTrks+fittedTrks);
  RecoData->Draw("RecoTracks.nhitspshw>>hNHitsPSHW",simTrks);
  RecoData->Draw("RecoTracks.ngoodhitspshw>>hNgoodHitsPSHW",simTrks+fittedTrks);

  RecoData->Draw("RecoTracks.Momentum/MCHits.MCTracks.fMomentum.Mag()/0.001-1:MCHits.MCTracks.fMomentum.Mag()*0.001>>hMomRes2",simTrks+fittedTrks+qualityCut+mcTrks);
  RecoData->Draw("RecoTracks.mom.Pt()/MCHits.MCTracks.fMomentum.Pt()/0.001-1:MCHits.MCTracks.fMomentum.Pt()*0.001>>hMomPtRes2",simTrks+fittedTrks+qualityCut+mcTrks);

  RecoData->Draw("RecoTracks.theta-MCHits.MCTracks.fMomentum.Theta():MCHits.MCTracks.fMomentum.Mag()*0.001>>hThetaRes2",simTrks+fittedTrks+qualityCut+mcTrks);
  RecoData->Draw("RecoTracks.phi-MCHits.MCTracks.fMomentum.Phi():MCHits.MCTracks.fMomentum.Mag()*0.001>>hPhiRes2",simTrks+fittedTrks+qualityCut+mcTrks);

  RecoData->Draw("RecoTracks.Momentum/MCHits.MCTracks.fMomentum.Mag()/0.001-1:MCHits.MCTracks.fMomentum.CosTheta()>>hMomvsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);
  RecoData->Draw("RecoTracks.mom.Pt()/MCHits.MCTracks.fMomentum.Pt()/0.001-1:MCHits.MCTracks.fMomentum.CosTheta()>>hMomPtvsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);

  RecoData->Draw("RecoTracks.theta-MCHits.MCTracks.fMomentum.Theta():MCHits.MCTracks.fMomentum.CosTheta()>>hThetavsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);
  RecoData->Draw("RecoTracks.phi-MCHits.MCTracks.fMomentum.Phi():MCHits.MCTracks.fMomentum.CosTheta()>>hPhivsThRes2",simTrks+fittedTrks+qualityCut+mcTrks);

  RecoData->Draw("(RecoTracks.x0*RecoTracks.mom.Y()-RecoTracks.y0*RecoTracks.mom.X())*1000/RecoTracks.Momentum:MCHits.MCTracks.fMomentum.Mag()*0.001>>hdRes2",simTrks+fittedTrks+qualityCut+mcTrks);
  RecoData->Draw("RecoTracks.z0*1000:MCHits.MCTracks.fMomentum.Mag()*0.001>>hzRes2",simTrks+fittedTrks+qualityCut+mcTrks);

  TCanvas* c1=new TCanvas("chit");
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
  
  TCanvas *cres=new TCanvas("cres");
  cres->Divide(2,2,0.001);
  TVirtualPad *p=cres->cd(1);
  TH1D* hMomRes2_2=stablefit(hMomRes2);
  hMomRes2_2->Draw();
  hMomRes2_2->SetTitle("Momentum Resolution;Momentum, GeV/c;#sigma P/P");
  if(islogx) p->SetLogx();
  hMomRes2_2->SetAxisRange(1e-4,0.01,"Y");
  p->SetLogy();

  p=cres->cd(2);
  TH1D* hMomPtRes2_2=stablefit(hMomPtRes2);
  hMomPtRes2_2->Draw();
  hMomPtRes2_2->SetTitle("Transverse Momentum Resolution;Transverse Momentum, GeV/c;#sigma P_{tr}/P_{tr}");
  if(islogx) p->SetLogx();
  hMomPtRes2_2->SetAxisRange(1e-4,0.01,"Y");
  p->SetLogy();

  p=cres->cd(3);
  TH1D* hThetaRes2_2=stablefit(hThetaRes2);
  TH1D* hPhiRes2_2=stablefit(hPhiRes2);
  hThetaRes2_2->Draw();
  hPhiRes2_2->Draw("same");
  hPhiRes2_2->SetLineColor(2);
  hThetaRes2_2->SetTitle("Theta Resolution;Momentum, GeV/c;#sigma #theta, rad");
  hPhiRes2_2->SetTitle("Phi Resolution;Momentum, GeV/c;#sigma #phi, rad");

  hThetaRes2_2->SetTitle("Angle Resolution;Momentum, GeV/c;#sigma angle, rad");
  TLegend *leg=new TLegend(0.7,0.7,0.85,0.8);
  leg->AddEntry(hThetaRes2_2,"theta");
  leg->AddEntry(hPhiRes2_2,"phi");
  leg->Draw();
  if(islogx) p->SetLogx();
  p->SetLogy();
  hThetaRes2_2->SetAxisRange(1e-5,1e-2,"Y");

  
  cres->cd(4)->SetLogy();
  TH1D* hdRes2_2=stablefit(hdRes2);
  TH1D* hzRes2_2=stablefit(hzRes2);
  hdRes2_2->Draw();
  hzRes2_2->Draw("same");
  hzRes2_2->SetLineColor(2);
  hdRes2_2->SetTitle("R-phi vtx Resolution;Momentum, GeV/c;#sigma r-phi vtx,#mu m");
  hzRes2_2->SetTitle("Z vtx Resolution;Momentum, GeV/c;#sigma z vtx,#mu m");

  leg=new TLegend(0.7,0.7,0.85,0.8);
  leg->AddEntry(hdRes2_2,"vtx r-phi");
  leg->AddEntry(hzRes2_2,"vtx z");
  leg->Draw();
  if(islogx) p->SetLogx();
  p->SetLogy();
  //  hMomPtRes2_2->SetAxisRange(1e-5,1e-2);

  TCanvas *cresth=new TCanvas("cresvsth");
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
  
  cresth->SetLogy();
  
  c1->Write();
  cres->Write();
  cresth->Write();
  fOut->Write();
  
  c1->Print(Form("nhit%s.png",opt));
  cres->Print(Form("res%s.png",opt));
  cresth->Print(Form("resvsth%s.png",opt));
}
