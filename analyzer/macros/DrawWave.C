void DrawWave(TString fName="MCHits00001.root") {

  TFile f(fName.Data());

TTree *tr = (TTree *)f.Get("MCHits");

Int_t nEvents = tr->GetEntries();

cout<<"Number of events Stored --> "<<nEvents<<endl;

TClonesArray *W_array = new TClonesArray("GMCWaveformData");
tr->SetBranchAddress("DCWaveforms",&W_array);

Double_t *time = new Double_t [1600];
for (int i=0;i<1600;i++) time[i] = i*0.5;
 
TGraph *grL;
TGraph *grR;
TCanvas *myC = new TCanvas();
 
for (int ie=0;ie<nEvents;ie++) {

  tr->GetEntry(ie);
  Int_t nrOfWaves = W_array->GetEntries();

  cout<<"In event "<<ie<<" --> "<<nrOfWaves<<" waveforms"<<endl;
  
  for (int iw=0;iw<nrOfWaves;iw++) {

myC->Clear();
    myC->Divide(1,2);
    
    GMCWaveformData *wave = (GMCWaveformData *)W_array->At(iw); 
    Double_t *signL = wave->GetfSignalL();
    Double_t *signR = wave->GetfSignalR();
    Int_t ntubes = wave->GetfNrOfTube();

    grL = new TGraph(1600,time,signL);
    grL->SetTitle(Form("Waveform Event %d, tube %d, side Left",ie+1,ntubes));  
    grR = new TGraph(1600,time,signR);  
    grR->SetTitle(Form("Waveform Event %d, tube %d, side Right",ie+1,ntubes));  

    myC->cd(1);
    grL->Draw("al");
    ((TAxis *)grL->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [mV]");

    myC->cd(2);
    grR->Draw("al");
    ((TAxis *)grR->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)grR->GetYaxis())->SetTitle("Amplitude [mV]");
 
    myC->Update();
    myC->WaitPrimitive();
  }

}
 
}
