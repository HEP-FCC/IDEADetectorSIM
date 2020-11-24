

TCanvas *cV;
TGraph *grInv;
TGraph *grDWave;
TGraph *grDer;
TGraph *grRise;
TLine *lnWav;
TLine *lnDer;
TLine *lnRise;
Float_t Threshold = 0.5;

Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t *pkHgt, Int_t *pkPos);


void PeakFind(TString fName="MCHits00001.root", Int_t iev=0, Int_t nrise=2) {

  TFile f(fName.Data());

TTree *tr = (TTree *)f.Get("MCHits");

Int_t nEvents = tr->GetEntries();

cout<<"Number of events Stored --> "<<nEvents<<endl;

TClonesArray *W_array = new TClonesArray("GMCWaveformData");
tr->SetBranchAddress("DCWaveforms",&W_array);

TClonesArray *Hits = new TClonesArray("GMCDCHHit");
tr->SetBranchAddress("DCHits",&Hits);


  Int_t tmpPkPos[100];
  Float_t tmpPkHgt[100];

  Int_t nBin = 1600;
  cV = new TCanvas("cV");
  cV->Divide(1,4);
  grInv = new TGraph(nBin);
  grDWave = new TGraph(nBin);
  grDer = new TGraph(nBin);
  grRise = new TGraph(nBin);
  grDer->SetMarkerColor(kBlue);
  grDer->SetLineColor(kBlue);
  grRise->SetMarkerColor(kRed);
  grRise->SetLineColor(kRed);
  lnWav = new TLine();
  lnDer = new TLine();
  lnRise = new TLine();
  lnDer->SetLineColor(kBlue);
  lnRise->SetLineColor(kRed);


Double_t *time = new Double_t [nBin];
for (int i=0;i<nBin;i++) time[i] = i*0.5;
 
TGraph *grL;
TGraph *grR;
//TCanvas *myC = new TCanvas();

for (;iev<nEvents;iev++) {

  tr->GetEntry(iev);

  Int_t nrOfWaves = W_array->GetEntries();

  cout<<"In event "<<iev<<" --> "<<nrOfWaves<<" waveforms"<<endl;
  
  for (int iw=0;iw<nrOfWaves;iw++) {

//    myC->Clear();
//    myC->Divide(1,4);
    cout<<"Numero di clustr generato "<<((GMCDCHHit *)Hits->At(iw))->GetfNrClusters()<<endl;
    
    GMCWaveformData *wave = (GMCWaveformData *)W_array->At(iw); 
    Double_t *signL = wave->GetfSignalL();
    Double_t *signR = wave->GetfSignalR();
    Int_t ntubes = wave->GetfNrOfTube();

    Int_t nFndPks = findPeaks(signL,nBin,nrise,tmpPkHgt,tmpPkPos);

   cout<<"Npkeas "<<nFndPks<<endl;

    grL = new TGraph(nBin,time,signL);
    grL->SetTitle(Form("Waveform Event %d, tube %d, side Left",iev+1,ntubes));  
//    grR = new TGraph(2500,time,signR);  
//    grR->SetTitle(Form("Waveform Event %d, tube %d, side Right",ie+1,ntubes));  

    cV->cd(4);
    grL->Draw("al");
    ((TAxis *)grL->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [mV]");
    //   ((TAxis *)grL->GetXaxis())->SetRangeUser(300.,600.);
    TMarker *marks = new TMarker[nFndPks];
    for (int ip=0; ip<nFndPks; ++ip) {
      if (fabs(signL[tmpPkPos[ip]]) > 5.*Threshold) {
        marks[ip].SetMarkerStyle(22);
        marks[ip].SetMarkerColor(kRed);
        marks[ip].SetX(time[tmpPkPos[ip]]);
        marks[ip].SetY(signL[tmpPkPos[ip]]);
        marks[ip].Draw();
        cout<<"pk "<<ip<<" at "<<time[tmpPkPos[ip]]<<" ampl "<<signL[tmpPkPos[ip]]<<endl;
      }
    }

//    myC->cd(2);
//    grR->Draw("al");
//    ((TAxis *)grR->GetXaxis())->SetTitle("Time [ns]");
//    ((TAxis *)grR->GetYaxis())->SetTitle("Amplitude [mV]");
 
    cV->Update();
    cV->WaitPrimitive();
    
    delete grL;
    delete [] marks;

  }

}

  delete grInv;
  delete grDWave;
  delete grDer;
  delete grRise;
  delete lnDer;
  delete lnRise;
  delete cV;
 
}


//__________________________________________________________
Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t *pkHgt, Int_t *pkPos) {

  //  Int_t nrise=1;
  Int_t checkUpTo=1;//1; //
  if(checkUpTo>nrise) { checkUpTo=nrise; }
  Float_t sig=0.5e-3;
  Float_t *sigd = new Float_t[nrise];
  Float_t *sigd_2 = new Float_t[nrise];
  
  TH1F tmpWdist("tmpWdist","",1000,-5.,5.);
  for (int i=10; i<500; ++i) { tmpWdist.Fill(data[i]); }
  sig = tmpWdist.GetRMS();
  Float_t mean = tmpWdist.GetMean();
  
  cout<<"Signal Off "<<mean<<" noise "<<sig<<endl;
  
  for (int ir=checkUpTo; ir<=nrise; ++ir) {
    int irId = ir-1;
    sigd[irId]=2.4495*sig/((float)(2*ir+1));
    sigd_2[irId]=1.414*sigd[irId];//%0
    sigd_2[irId]*=3.;   //5.0;
    sigd[irId]*=3.5;//% stava a 3
    // better comb 4-3, 2-4
    //cout<<"iRise "<<ir<<" sgm "<<sigd[irId]<<" sgm_2 "<<sigd_2[irId]<<endl;
  }
//  sig*=4.0*1.414;  //stava a 4

  Threshold = sig;
  sig *= 3.;

  lnWav->SetX1(0);
  lnWav->SetY1(sig);
  lnWav->SetX2(nPnts);
  lnWav->SetY2(sig);
  //sig*=1.414;

  grInv->Set(nPnts);
  grDWave->Set(nPnts);
  grDer->Set(nPnts);
  grRise->Set(nPnts);

  Float_t *wave = new Float_t[nPnts];
  Float_t **riseMeas = new Float_t*[nPnts];// (r,nrise);
  Float_t **deri = new Float_t*[nPnts];// zeros(r,nrise);
  for (int ip=0; ip<nPnts; ++ip) {
    wave[ip]=-1.0*(data[ip]-mean);
    riseMeas[ip]=new Float_t [nrise];
    deri[ip]=new Float_t [nrise];
    for (int ir=0; ir<nrise; ++ir) {
      riseMeas[ip][ir]=0.0;
      deri[ip][ir]=0.0;
    }
    grInv->SetPoint(ip,ip,wave[ip]);
  }
  cV->cd(1);
  grInv->Draw("AL");
  
  //Int_t tmpPkPos[maxNPks];
  //Float_t tmpPkHgt[maxNPks];
  Int_t nPks=0;
  Int_t last=-nrise;
  for (int i=nrise+1; i<nPnts-1; ++i) {
    for (int ir=nrise; ir>=checkUpTo; --ir) {
      int irId = ir-1;
      deri[i][irId] = (2.0*wave[i]-wave[i-ir]-wave[i-ir-1])/((float)(2*ir+1));
      riseMeas[i][irId] = (deri[i][irId]-deri[i-ir][irId]);
      
      if ( deri[i][irId]>=sigd[irId] && (deri[i][irId]-deri[i-ir][irId])>sigd_2[irId] && (wave[i]-wave[i-ir])> 2.*sig ) {
        //%if globFlag
        if ((i-last)>ir /*%1*/) {
            pkHgt[nPks]=-1.0*(wave[i]-mean);
            pkPos[nPks]=i;
            ++nPks;
            last=i;
        } else {
            pkHgt[nPks-1]=-1.0*(wave[i]-mean);
            pkPos[nPks-1]=i;
            last=i;
        }
      }
      if(nPks==100) {break;}
    }
    if(nPks==100) {break;}
  }

  for (int ip=0; ip<nPnts; ++ip) {
    if (ip>=nrise) { grDWave->SetPoint(ip,ip,wave[ip]-wave[ip-nrise]); }
    else { grDWave->SetPoint(ip,ip,0); }
    grDer->SetPoint(ip,ip,deri[ip][nrise-1]);
    grRise->SetPoint(ip,ip,riseMeas[ip][nrise-1]);
  }
  lnDer->SetX1(0);
  lnDer->SetY1(sigd[nrise-1]);
  lnDer->SetX2(nPnts);
  lnDer->SetY2(sigd[nrise-1]);
  lnRise->SetX1(0);
  lnRise->SetY1(sigd_2[nrise-1]);
  lnRise->SetX2(nPnts);
  lnRise->SetY2(sigd_2[nrise-1]);
  cV->cd(2);
  grDWave->Draw("AL");
  lnWav->Draw("same");
  cV->cd(3);
  grDer->Draw("AL");
  grRise->Draw("Lsame");
  lnDer->Draw("same");
  lnRise->Draw("same");

  delete [] sigd;
  delete [] sigd_2;
  delete [] wave;
  for (int ip=0; ip<nPnts; ++ip) {
    delete [] riseMeas[ip];
    delete [] deri[ip];
  }
  delete [] riseMeas;
  delete [] deri;
    
  return nPks;
}


