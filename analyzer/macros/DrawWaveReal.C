void DrawWaveReal(TString fName="MCHits00001.root") {

  TFile f(fName.Data());
  
  TTree *tr = (TTree *)f.Get("RawData");
  
  Int_t nEvents = tr->GetEntries();
  
  cout<<"Number of events Stored --> "<<nEvents<<endl;
  
  GMCEvent *EvData;

  tr->SetBranchAddress("EventData",&EvData);
  
  
  TGraph *grL;
  TGraph *grR;
  TCanvas *myC = new TCanvas();
  //  TCanvas *mC2 = new TCanvas();
  TH2F *modupix0 = new TH2F("h001","Modupix nr 0",256,-0.5,255.5,256,-0.5,255.5);
  TH2F *modupix1 = new TH2F("h002","Modupix nr 1",256,-0.5,255.5,256,-0.5,255.5);

  for (int ie=0;ie<nEvents;ie++) {

    myC->Clear();
    myC->Divide(2,2);

    
    tr->GetEntry(ie);
    modupix0->Reset();
    modupix1->Reset();

    Int_t nrOfWaves = EvData->GetOscWaveformSize();

    cout<<"In event "<<ie<<" --> "<<nrOfWaves<<" waveforms"<<endl;
    
    //primo modupix
    for (int ipx=0;ipx<EvData->GetModuPix0Size();ipx++) {
      GMCMPXHit *ah = (GMCMPXHit *)EvData->GetModuPix0At(ipx);
      modupix0->Fill(ah->GetfnrXpixel(),ah->GetfnrXpixel());
    }

    //secondo modupix
    for (int ipx=0;ipx<EvData->GetModuPix1Size();ipx++) {
      GMCMPXHit *ah = (GMCMPXHit *)EvData->GetModuPix1At(ipx);
      modupix1->Fill(ah->GetfnrXpixel(),ah->GetfnrXpixel());
    }

    myC->cd(1);
    modupix0->SetMarkerStyle(21);
    modupix0->DrawCopy();

    myC->cd(2);
    modupix1->SetMarkerStyle(21);
    modupix1->DrawCopy();


    Waveform *wave = (Waveform *)EvData->GetOscWaveformAt(0); 

    
    Double_t *sign = wave->GetAmpl();
    Double_t *time = wave->GetTime();
    
    grL = new TGraph(wave->GetNPoints(),time,sign);
    grL->SetTitle(Form("Waveform Event %d, ch %d",ie,0));  
    
    myC->cd(3);
    grL->Draw("al");
    ((TAxis *)grL->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [mV]");

    wave = (Waveform *)EvData->GetOscWaveformAt(1); 
    
    sign = wave->GetAmpl();
    time = wave->GetTime();
    grR = new TGraph(wave->GetNPoints(),time,sign);  
    grR->SetTitle(Form("Waveform Event %d, ch %d",ie,1));  

    myC->cd(4);
    grR->Draw("al");
    ((TAxis *)grR->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)grR->GetYaxis())->SetTitle("Amplitude [mV]");
    
    myC->Update();
    myC->WaitPrimitive();
  



  }
  
}
