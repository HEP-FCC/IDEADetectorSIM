#include "Riostream.h"
#include "TList.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TObjString.h"
#include "TString.h"

void EfficiencyOnNoise(TString NoiseString="") {


  if (NoiseString.IsNull()) {
    printf("Error!!!  \n");
    return;
  }

  TString dPath="../HistRoot";
  //the filename form is
  //PeakAnalysisAt1000MHz_BinW400ps_Noise9.root
  TObjArray *strNoiseArr = NoiseString.Tokenize("-");

  Double_t SignNoiseR[strNoiseArr->GetEntries()];
  Double_t EffWellRec[strNoiseArr->GetEntries()];
  Double_t PksRecognd[strNoiseArr->GetEntries()];
  //  Double_t PksWellRec[strNoiseArr->GetEntries()];
  Double_t EntrNrFake[strNoiseArr->GetEntries()];
  Double_t MeanNrFake[strNoiseArr->GetEntries()];

  Double_t Err_SignNoiseR[strNoiseArr->GetEntries()];
  Double_t Err_EffWellRec[strNoiseArr->GetEntries()];
  Double_t Err_PksRecognd[strNoiseArr->GetEntries()];
  //  Double_t RMS_PksWellRec[strNoiseArr->GetEntries()];
  Double_t Err_EntrNrFake[strNoiseArr->GetEntries()];
  Double_t Err_MeanNrFake[strNoiseArr->GetEntries()];

  Double_t HTotalEntries = 0;
  for (int ii=0;ii<strNoiseArr->GetEntries();ii++) {
 
    SignNoiseR[ii] = (((TObjString*)strNoiseArr->At(ii))->String()).Atof();
    Err_SignNoiseR[ii] = 0.01;

    TString flname = "PeakAnalysisAt620MHz_BinW400ps_Noise";
    flname += (((TObjString*)strNoiseArr->At(ii))->String() + ".root");

    TFile *f = new TFile(Form("%s/%s",dPath.Data(),flname.Data()));
    if (f->IsOpen()) printf("===> the file %s is open!! \n",flname.Data());

    TList *keyLst = f->GetListOfKeys();
    cout<<"entries  "<<keyLst->GetEntries()<<endl;
    for (int idx=0;idx<keyLst->GetEntries();idx++) {
      TString key(keyLst->At(idx)->GetName());
      if (key.CompareTo("c1_n19") == 0)  {

	TH1F *h01 = (TH1F *)((TCanvas*)f->Get(key.Data()))->GetPrimitive("h117");
	EntrNrFake[ii] = h01->GetEntries();
	EntrNrFake[ii] /= HTotalEntries;
	Err_EntrNrFake[ii] = sqrt(EntrNrFake[ii]*(1.-EntrNrFake[ii]));

	MeanNrFake[ii] = h01->GetMean();
	Err_MeanNrFake[ii] = h01->GetRMS();
	Err_MeanNrFake[ii] /= sqrt(h01->GetEntries());

      }

      if (key.CompareTo("c1_n15") == 0)  {
	TH1F *h01 = (TH1F *)((TCanvas*)f->Get(key.Data()))->GetPrimitive("h113");
	HTotalEntries = (Double_t)h01->GetEntries();
	PksRecognd[ii] = h01->GetMean();
	Err_PksRecognd[ii] = h01->GetRMS();
	Err_PksRecognd[ii] /= sqrt(HTotalEntries);

      }

      if (key.CompareTo("c1_n18") == 0)  {
	//efficiency on nr of peaks well recognized 
	TH1F *h01 = (TH1F *)((TCanvas*)f->Get(key.Data()))->GetPrimitive("h116");
	EffWellRec[ii] = h01->GetMean();
	Err_EffWellRec[ii] = h01->GetRMS();
	Err_EffWellRec[ii] /= sqrt(h01->GetEntries());

      }

    }
    f->Close();
  }

  //peak recognized
  TGraphErrors *gr1 = new TGraphErrors(strNoiseArr->GetEntries(),
				       SignNoiseR,PksRecognd,
				       Err_SignNoiseR,Err_PksRecognd);
  gr1->SetTitle("Nr of Peaks Recognized");
  ((TAxis *)gr1->GetXaxis())->SetTitle("Signal/Noise");
  ((TAxis *)gr1->GetYaxis())->SetTitle("Mean of Peaks");
  gr1->SetMarkerStyle(7);
  gr1->SetMarkerColor(2);

  //efficiency well done
  TGraphErrors *gr2 = new TGraphErrors(strNoiseArr->GetEntries(),
				       SignNoiseR,EffWellRec,
				       Err_SignNoiseR,Err_EffWellRec);
  gr2->SetTitle("Efficiency on Peak Well Recognized");
  ((TAxis *)gr2->GetXaxis())->SetTitle("Signal/Noise");
  ((TAxis *)gr2->GetYaxis())->SetTitle("Percentage");
  gr2->SetMarkerStyle(8);
  gr2->SetMarkerColor(3);

  //% of fake
  TGraphErrors *gr3 = new TGraphErrors(strNoiseArr->GetEntries(),
				       SignNoiseR,EntrNrFake,
				       Err_SignNoiseR,Err_EntrNrFake);
  gr3->SetTitle("Percentage of waveform with fake peaks");
  ((TAxis *)gr3->GetXaxis())->SetTitle("Signal/Noise");
  ((TAxis *)gr3->GetYaxis())->SetTitle("Percentage");

  gr3->SetMarkerStyle(9);
  gr3->SetMarkerColor(4);

  //mean of fake
  TGraphErrors *gr4 = new TGraphErrors(strNoiseArr->GetEntries(),
				       SignNoiseR,MeanNrFake,
				       Err_SignNoiseR,Err_MeanNrFake);
  gr4->SetTitle("Mean of fake peaks");
  ((TAxis *)gr4->GetXaxis())->SetTitle("Signal/Noise");
  ((TAxis *)gr4->GetYaxis())->SetTitle("Nr.");
  gr4->SetMarkerStyle(6);
  gr4->SetMarkerColor(5);

  TFile fOut("Efficiency.root","RECREATE");
  TCanvas *myC = new TCanvas();
  myC->Divide(2,2);
  myC->cd(1);
  gr1->Draw("ap");
  gr1->Write();
  myC->cd(2);
  gr2->Draw("ap");
  gr2->Write();
  myC->cd(3);
  gr3->Draw("ap");
  gr3->Write();
  myC->cd(4);
  gr4->Draw("ap");
  gr4->Write();

  myC->Write();
  fOut.Close();

}
