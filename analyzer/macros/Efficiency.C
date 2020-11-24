void Efficiency(TString BandWidth="",TString FreqAcq="") {


  if (BandWidth.IsNull()) {
    printf("Error!!!  \n");
    return;
  }

  if (FreqAcq.IsNull()) {
    printf("Error!!!  \n");
    return;
  }


  Double_t BWidth[3];  //BandWidth in MHz
  Double_t Acquis[3];  //acquisition rate in ps

  TObjArray *strBwArr = BandWidth.Tokenize("-");
  TObjArray *strFqArr = FreqAcq.Tokenize("-");

  for (int i=0;i<strBwArr->GetEntries();i++) 
    BWidth[i] = TString(((TObjString *)strBwArr->At(i))->String()).Atof();

  for (int j=0;j<strFqArr->GetEntries();j++) 
    Acquis[j] = TString(((TObjString *)strFqArr->At(j))->String()).Atof();


  TGraphErrors *Efficiency[3];
  TGraphErrors *PercRecognizable[3];
  TGraphErrors *PercIdentified[3];
  TGraphErrors *PercWellDone[3];

  for (int j=0;j<strFqArr->GetEntries();j++) {
    cout<<" freq "<<Acquis[j]<<"  "<<strFqArr->GetEntries()<<endl;
    Efficiency[j] = new TGraphErrors(3);
    Efficiency[j]->SetTitle(Form("Efficincy @ %d ps BinWidth ",(int)Acquis[j]));
    PercRecognizable[j] = new TGraphErrors(3);
    PercRecognizable[j]->SetTitle(Form("\% of e- recognizable @ %d ps BinWidth ",(int)Acquis[j]));
    PercRecognizable[j]->SetName(Form("grafico%d",j+1));
    PercIdentified[j] = new TGraphErrors(3);
    PercIdentified[j]->SetTitle(Form("\% of peak associated to a e- @ %d ps BinWidth ",(int)Acquis[j]));
    PercWellDone[j] = new TGraphErrors(3);
    PercWellDone[j]->SetTitle(Form("\% of peak well associated @ %d ps BinWidth ",(int)Acquis[j]));


    for (int i=0;i<strBwArr->GetEntries();i++) {
      TFile fInput(Form("PeakAnalysisAt%dMHz_BinW%dps.root",
			(int)BWidth[i],(int)Acquis[j]));
      
      cout<<"Apro il file: "<<fInput.GetName()<<" -- "<<strBwArr->GetEntries()<<endl;
      TH1F *h1 = (TH1F*)((TCanvas *)fInput.Get("c1_n4"))->GetPrimitive("h102");
      Efficiency[j]->SetPoint(i,0.001*BWidth[i],h1->GetMean());
      Efficiency[j]->SetPointError(i,0.0001,h1->GetRMS()/sqrt(h1->GetEntries()));
      TH1F *h2 = (TH1F*)((TCanvas *)fInput.Get("c1_n18"))->GetPrimitive("h116");

      TH1F *h3 = (TH1F*)((TCanvas *)fInput.Get("c1_n13"))->GetPrimitive("h111");
      PercRecognizable[j]->SetPoint(i,0.001*BWidth[i],h3->GetMean()/12.22);
      PercRecognizable[j]->SetPointError(i,0.0001,h3->GetRMS()/sqrt(h3->GetEntries())/12.22);

      TH1F *h4 = (TH1F*)((TCanvas *)fInput.Get("c1_n15"))->GetPrimitive("h113");
      PercIdentified[j]->SetPoint(i,0.001*BWidth[i],h4->GetMean()/12.22);
      PercIdentified[j]->SetPointError(i,0.0001,h4->GetRMS()/sqrt(h4->GetEntries())/12.22);

      TH1F *h5 = (TH1F*)((TCanvas *)fInput.Get("c1_n17"))->GetPrimitive("h115");
      PercWellDone[j]->SetPoint(i,0.001*BWidth[i],h5->GetMean()/12.22);
      PercWellDone[j]->SetPointError(i,0.0001,h5->GetRMS()/sqrt(h5->GetEntries())/12.22);

      fInput.Close();
    }
  }


  TMultiGraph *gr_1 = new TMultiGraph();
  TMultiGraph *gr_2 = new TMultiGraph();
  TMultiGraph *gr_3 = new TMultiGraph();
  TMultiGraph *gr_4 = new TMultiGraph();

  Efficiency[0]->SetMarkerStyle(21);
  Efficiency[0]->SetMarkerColor(2);
  Efficiency[0]->SetLineColor(2);
  gr_1->Add(Efficiency[0]);
  PercRecognizable[0]->SetMarkerStyle(21);
  PercRecognizable[0]->SetMarkerColor(2);
  PercRecognizable[0]->SetLineColor(2);
  gr_2->Add(PercRecognizable[0]);
  PercIdentified[0]->SetMarkerStyle(21);
  PercIdentified[0]->SetMarkerColor(2);
  PercIdentified[0]->SetLineColor(2);
  gr_3->Add(PercIdentified[0]);
  PercWellDone[0]->SetMarkerStyle(21);
  PercWellDone[0]->SetMarkerColor(2);
  PercWellDone[0]->SetLineColor(2);
  gr_4->Add(PercWellDone[0]);

  /****************
  Efficiency[1]->SetMarkerStyle(22);
  Efficiency[1]->SetMarkerColor(3);
  Efficiency[1]->SetLineColor(3);
  gr_1->Add(Efficiency[1]);
  PercRecognizable[1]->SetMarkerStyle(22);
  PercRecognizable[1]->SetMarkerColor(3);
  PercRecognizable[1]->SetLineColor(3);
  gr_2->Add(PercRecognizable[1]);
  PercIdentified[1]->SetMarkerStyle(22);
  PercIdentified[1]->SetMarkerColor(3);
  PercIdentified[1]->SetLineColor(3);
  gr_3->Add(PercIdentified[1]);
  PercWellDone[1]->SetMarkerStyle(22);
  PercWellDone[1]->SetMarkerColor(3);
  PercWellDone[1]->SetLineColor(3);
  gr_4->Add(PercWellDone[1]);
 
  Efficiency[2]->SetMarkerStyle(23);
  Efficiency[2]->SetMarkerColor(4);
  Efficiency[2]->SetLineColor(4);
  gr_1->Add(Efficiency[2]);
  PercRecognizable[2]->SetMarkerStyle(23);
  PercRecognizable[2]->SetMarkerColor(4);
  PercRecognizable[2]->SetLineColor(4);
  gr_2->Add(PercRecognizable[2]);
  PercIdentified[2]->SetMarkerStyle(23);
  PercIdentified[2]->SetMarkerColor(4);
  PercIdentified[2]->SetLineColor(4);
  gr_3->Add(PercIdentified[2]);
  PercWellDone[2]->SetMarkerStyle(23);
  PercWellDone[2]->SetMarkerColor(4);
  PercWellDone[2]->SetLineColor(4);
  gr_4->Add(PercWellDone[2]);

  ****************************/
  TCanvas *myC = new TCanvas();
  gr_1->Draw("acp");
  myC->BuildLegend();

  /**************
  TCanvas *myC1 = new TCanvas();
  myC1->Divide(2,2);
  myC1->cd(1);
  gr_2->Draw("acp");
  gr_2->SetTitle("Percentage of e- recognizable; BandWidth [GHz]; Percentage [%]");
  ((TAxis *)gr_2->GetXaxis())->SetTitle("BandWidth [GHz]");
  ((TAxis *)gr_2->GetYaxis())->SetTitle("Percentage [%]");

  myC1->cd(2);
  gr_3->Draw("acp");
  gr_3->SetTitle("Percentage of peaks associated to 1e-; BandWidth [GHz]; Percentage [%]");
  ((TAxis *)gr_3->GetXaxis())->SetTitle("BandWidth [GHz]");
  ((TAxis *)gr_3->GetYaxis())->SetTitle("Percentage [%]");

  myC1->cd(3);
  gr_4->Draw("acp");
  gr_4->SetTitle("Percentage of peak well associated to 1e-; BandWidth [GHz]; Percentage [%]");
  ((TAxis *)gr_4->GetXaxis())->SetTitle("BandWidth [GHz]");
  ((TAxis *)gr_4->GetYaxis())->SetTitle("Percentage [%]");

  myC1->cd(4);
  TLegend *leg = new TLegend(0.1,0.25,0.9,0.75);
  leg->AddEntry((TObject *)0,"Normalized to 12.22 e-","");
  leg->AddEntry(PercRecognizable[0],"BinWidth 400 ps","lep");
  leg->AddEntry(PercRecognizable[1],"BinWidth 500 ps","lep");
  leg->AddEntry(PercRecognizable[2],"BinWidth 625 ps","lep");
  leg->Draw();
  ************************/
}
