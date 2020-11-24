#include "Riostream.h"
#include "TCanvas.h"
#include "TVirtualFFT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMath.h"
#include "TAxis.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TFile.h"
#include "TVectorF.h"

#include "generated/GMCEvent.h"
#include "util/Waveform.h"

void FFT(Int_t N, Double_t *ampl, Double_t *ReFFT, Double_t *ImFFT);
void InverseFFT(Int_t N, Double_t *ReFFT, Double_t *ImFFT, Double_t *ampl);


void DrawWaveFiltro(TString fName="MCHits00001.root", Int_t iev=0) {

  TFile f(fName.Data());
  
  TTree *tr = (TTree *)f.Get("RawData");
  
  Int_t nEvents = tr->GetEntries();
  
  cout<<"Number of events Stored --> "<<nEvents<<endl;
  
  GMCEvent *EvData = NULL;

  tr->SetBranchAddress("EventData",&EvData);
  
  
  TGraph *grL;
  TGraph *grL_fft;
  TGraph *grL_fftnew;
  TGraph *grLnew;

  //  TCanvas *mC2 = new TCanvas();

  Double_t *time = NULL;

  //calcolo rumore medio
  Double_t fftsum[2500];
  Double_t realNoise[2500];
  Double_t imagNoise[2500];
  Int_t nPoint = 0;
  Double_t fft_width;
  for (int i=0;i<2500;i++) {
    fftsum[i] = realNoise[i] = imagNoise[i] = 0.;
  }

  for (int ie=1;ie<17;ie++) {
    tr->GetEntry(ie);
    
    Waveform *wave = (Waveform *)EvData->GetOscWaveformAt(1); 
    nPoint = wave->GetNPoints();
    
    Double_t *sign = wave->GetAmpl();
    time = wave->GetTime();

    //Trasformata di Fourier
    Double_t *Re = new Double_t [wave->GetNPoints()];
    Double_t *Im = new Double_t [wave->GetNPoints()];

    FFT(wave->GetNPoints(),sign,Re,Im);
    //   cout<<"init value "<<fftsum[0]<<endl;
    for(Int_t ifft=0;ifft<nPoint/2;ifft++){
      fftsum[ifft] = ((ie-1)*fftsum[ifft] + sqrt(Re[ifft]*Re[ifft]+Im[ifft]*Im[ifft]))/((float)ie);

      realNoise[ifft] = ((ie-1)*realNoise[ifft] + Re[ifft])/((float)ie);
      imagNoise[ifft] = ((ie-1)*imagNoise[ifft] + Im[ifft])/((float)ie);
    }
  }
  
  //Noise Average
  Double_t *ampl_f = new Double_t [nPoint];
  fft_width = 1./(time[100] - time[99])/((float)nPoint);
  TGraph *noiseFFT = new TGraph(); 
  for(Int_t ifft=0;ifft<nPoint/2;ifft++){
    //    noiseFFT->SetPoint(ifft,ifft*fft_width,
    //		       TMath::Sqrt(TMath::Power(realNoise[ifft],2) +
    //				   TMath::Power(imagNoise[ifft],2)) );

    noiseFFT->SetPoint(ifft,ifft*fft_width,fftsum[ifft]);

    //   if(ifft < 100) cout<<ifft<<"  "<<ifft*fft_width<<"  "<<fftsum[ifft]<<endl;
  }
  
 
  InverseFFT(nPoint,realNoise,imagNoise,ampl_f);
  TGraph *noise = new TGraph(nPoint,time,ampl_f);

  /*******************  
  TCanvas *cc = new TCanvas();
  cc->Divide(1,2);
  cc->cd(1);
  noise->Draw("al");
  cc->cd(2);
  ((TVirtualPad *)cc->GetPad(2))->SetLogy();
  ((TVirtualPad *)cc->GetPad(2))->SetLogx();
  noiseFFT->Draw("ac");
  //  ((TAxis *)noiseFFT->GetXaxis())->SetRangeUser(1.e+6,1.e+9); 
  ************************/
  /*************************************************************/

  TCanvas *myC = new TCanvas();
  TCanvas *myC1 = new TCanvas();
  //cosa cambia
  for (;iev<nEvents;iev++) {

    myC->Clear();
    myC->Divide(1,2);
    myC1->Clear();
    myC1->Divide(1,3);

    tr->GetEntry(iev);
    //    time = NULL;
    Waveform *wave = (Waveform *)EvData->GetOscWaveformAt(1); 

    
    Double_t *sign = wave->GetAmpl();
    time = wave->GetTime();
    
    grL = new TGraph(wave->GetNPoints(),time,sign);
    grL->SetTitle(Form("Waveform Event %d, ch %d",iev,1));  
    
    myC->cd(1);
    grL->Draw("al");
    ((TAxis *)grL->GetXaxis())->SetTitle("Time [s]");
    ((TAxis *)grL->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [V]");
    ((TAxis *)grL->GetYaxis())->SetLabelSize(0.07);
    ((TAxis *)grL->GetYaxis())->SetRangeUser(0.05,0.35);

    //Trasformata di Fourier
    Double_t *Re = new Double_t [wave->GetNPoints()];
    Double_t *Im = new Double_t [wave->GetNPoints()];

    FFT(wave->GetNPoints(),sign,Re,Im);
    Double_t fftvalue[wave->GetNPoints()/2]; 
    grL_fft = new TGraph();
    grL_fft->SetTitle("Waveform FFT");

    grL_fftnew = new TGraph();
    grL_fftnew->SetTitle("Waveform FFT after substruction");

    for(Int_t ifft=0;ifft<wave->GetNPoints()/2;ifft++){

      fftvalue[ifft] = sqrt(Re[ifft]*Re[ifft]+Im[ifft]*Im[ifft]);
      grL_fft->SetPoint(ifft+1,ifft*fft_width,fftvalue[ifft]);

      //      Re[ifft] -= realNoise[ifft];
      //      Im[ifft] -= imagNoise[ifft];
      if (ifft > 400 && ifft < 1000) cout<<"FFT sign "<<ifft<<" "<<ifft*fft_width<<"  "<<fftvalue[ifft]<<endl;
      //      fftvalue[ifft] = sqrt(Re[ifft]*Re[ifft]+Im[ifft]*Im[ifft]);
      
      if (ifft*fft_width > 0.8*1.e9) {
	Double_t freq = ifft*fft_width*1.e-9;
	//	cout<<freq<<endl;
	Double_t thr = 1.5*1.e-3 - (1.8*1.e-4)*freq; 
	if (fftvalue[ifft] > thr) 
	  fftvalue[ifft] -= (fftvalue[ifft] - thr);	
      }
      
      Re[ifft] = fftvalue[ifft]/sqrt(2.);
      Im[ifft] = fftvalue[ifft]/sqrt(2.);
      
      grL_fftnew->SetPoint(ifft+1,ifft*fft_width,fftvalue[ifft]);

    }

    cout<<"qui arrivo??? "<<endl;
    Double_t *signNew = new Double_t [nPoint];
    InverseFFT(nPoint,Re,Im,signNew);
    grLnew = new TGraph(nPoint,time,signNew);

    myC1->cd(1);
    ((TVirtualPad *)myC1->GetPad(1))->SetLogy();
    //    ((TVirtualPad *)myC->GetPad(2))->SetLogx();
    grL_fft->Draw("al");
    ((TAxis *)grL_fft->GetXaxis())->SetTitle("frequence [Hz]");
    ((TAxis *)grL_fft->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grL_fft->GetYaxis())->SetTitle("Magnitude");
    ((TAxis *)grL_fft->GetYaxis())->SetLabelSize(0.07);
    ((TAxis *)grL_fft->GetYaxis())->SetRangeUser(1.e-5,5.);

    myC1->cd(2);
    ((TVirtualPad *)myC1->GetPad(2))->SetLogy();
    //    ((TVirtualPad *)myC->GetPad(2))->SetLogx();
    noiseFFT->Draw("al");
    ((TAxis *)noiseFFT->GetXaxis())->SetTitle("frequence [Hz]");
    ((TAxis *)noiseFFT->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)noiseFFT->GetYaxis())->SetTitle("Magnitude");
    ((TAxis *)noiseFFT->GetYaxis())->SetLabelSize(0.07);
    ((TAxis *)noiseFFT->GetYaxis())->SetRangeUser(1.e-5,5.);

    myC1->cd(3);
    ((TVirtualPad *)myC1->GetPad(3))->SetLogy();
    //    ((TVirtualPad *)myC->GetPad(3))->SetLogx();
    grL_fftnew->Draw("al");
    ((TAxis *)grL_fftnew->GetXaxis())->SetTitle("frequence [Hz]");
    ((TAxis *)grL_fftnew->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grL_fftnew->GetYaxis())->SetTitle("Magnitude");
    ((TAxis *)grL_fftnew->GetYaxis())->SetLabelSize(0.07);
    ((TAxis *)grL_fftnew->GetYaxis())->SetRangeUser(1.e-5,5.);

    myC->cd(2);
    grLnew->Draw("al");
    ((TAxis *)grLnew->GetXaxis())->SetTitle("Time [s]");
    ((TAxis *)grLnew->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grLnew->GetYaxis())->SetTitle("Amplitude [V]");
    ((TAxis *)grLnew->GetYaxis())->SetLabelSize(0.07);
    ((TAxis *)grLnew->GetYaxis())->SetRangeUser(-0.15,0.25);
    myC->Update();
    myC->WaitPrimitive();

    delete grL;
    delete grL_fft;
    delete grL_fftnew;
    delete grLnew;

  }
  
}

//________________________________________________________________________
void FFT(Int_t N, Double_t *ampl, Double_t *ReFFT, Double_t *ImFFT) {

  TVirtualFFT *fft = TVirtualFFT::FFT(1,&N,"R2C");
  fft->SetPoints(ampl);
  fft->Transform();
  for(Int_t i=0;i<N/2;i++){
    fft->GetPointComplex(i,ReFFT[i],ImFFT[i]);
    ReFFT[i] /= sqrt(N);
    ImFFT[i] /= sqrt(N);
  }

}

//__________________________________________________________________________
void InverseFFT(Int_t N, Double_t *ReFFT, Double_t *ImFFT, Double_t *ampl) {

  TVirtualFFT *at = TVirtualFFT::FFT(1,&N,"C2R");
  at->SetPointsComplex(ReFFT,ImFFT);
  at->Transform();

  at->GetPoints(ampl);

  for(Int_t i=0;i<N;i++){
    ampl[i] /= sqrt(N);
  }

}
