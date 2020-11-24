#include "Riostream.h"
#include "TCanvas.h"
#include "TVirtualFFT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH1F.h"
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


void DrawWaveFiltro(TString fName="MCHits00001.root",Int_t iev=0,Double_t fTcut=80) {

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

  TCanvas *myC = new TCanvas();
  //  TCanvas *mC2 = new TCanvas();

  Double_t *time = NULL;



  //calcolo rumore medio
  Double_t fftsum[2500];
  Double_t realNoise[2500];
  Double_t imagNoise[2500];
  Int_t nPoint = 0;
  Double_t *truncSign = NULL;
  Double_t *timeShift = NULL;
  Double_t fft_width;
  TH1F *ffttail = new TH1F("H100","FFT tail",100,4.e4,8.e-4);

  TH1F *fftMean = new TH1F("H101","FFT tail",100,4.e4,8.e-4);
  TH1F *fftRMS = new TH1F("H102","FFT tail",100,0.,0.001);

  for (int i=0;i<2500;i++) {
    fftsum[i] = realNoise[i] = imagNoise[i] = 0.;
  }


  //risoluzione osci 100ps tagliamo gli ultimi 80ns
  Int_t nBin = (int)(fTcut/0.1);
  Int_t nInit = 10;
  Int_t nSample = 0;
  //rumore medio
  for (int ie=1;ie<100;ie++) {
    //    if (ie == 17 || ie == 70 || ie == 80) continue;
    tr->GetEntry(ie);
    //    nSample++;
    ffttail->Reset();
    Waveform *wave = (Waveform *)EvData->GetOscWaveformAt(1); 
    nPoint = wave->GetNPoints()-nBin-nInit; //togliamo anche il primo 1ns 
    
    Double_t *sign = wave->GetAmpl();
    time = wave->GetTime();

    //Trasformata di Fourier
    Double_t *Re = new Double_t [nPoint];
    Double_t *Im = new Double_t [nPoint];
    truncSign = new Double_t [nPoint];
    timeShift = new Double_t [nPoint];

    for (int ifft=0;ifft<nPoint;ifft++) {
      truncSign[ifft] = sign[ifft+nInit];
      timeShift[ifft] = time[ifft+nInit];
    }

    FFT(nPoint,truncSign,Re,Im);
    fft_width = 1./(timeShift[100] - timeShift[99])/((float)nPoint);

    for(Int_t ifft=0;ifft<nPoint/2;ifft++){
      fftsum[ifft] = ((ie-1)*fftsum[ifft] + sqrt(Re[ifft]*Re[ifft]+Im[ifft]*Im[ifft]))/((float)ie);
      if (ifft*fft_width*1.e-9 >= 2.) ffttail->Fill(fftsum[ifft]); 

    }

    fftMean->Fill(ffttail->GetMean());
    fftRMS->Fill(ffttail->GetRMS());
    cout<<"pippo pappo "<<ffttail->GetMean()<<"  "<<ffttail->GetRMS()<<endl;

    /************************
    for(Int_t ifft=0;ifft<nPoint/2;ifft++){
      if (ifft*fft_width*1.e-9 < 0.8) fftsum[ifft] += sqrt(Re[ifft]*Re[ifft]+Im[ifft]*Im[ifft]);
    } 
    *************************/

    delete truncSign;
    delete timeShift;
  }

  //  for(Int_t ifft=0;ifft<nPoint/2;ifft++) fftsum[ifft]/((float)nSample);

  /*****************
  TCanvas *hcc = new TCanvas();
  hcc->Divide(2,1);
  hcc->cd(1);
  fftMean->DrawCopy();
  hcc->cd(2);
  fftRMS->DrawCopy();
  ****************/

  //Noise Average
  Double_t *ampl_f = new Double_t [nPoint];
  fft_width = 1./(timeShift[100] - timeShift[99])/((float)nPoint);
  TGraph *noiseFFT = new TGraph(); 
  for(Int_t ifft=0;ifft<nPoint/2;ifft++){

    realNoise[ifft] = fftsum[ifft]/sqrt(2.);
    imagNoise[ifft] = fftsum[ifft]/sqrt(2.);

    noiseFFT->SetPoint(ifft,ifft*fft_width,fftsum[ifft]);
  }
  
 
  InverseFFT(nPoint,realNoise,imagNoise,ampl_f);
  TGraph *noise = new TGraph(nPoint,timeShift,ampl_f);
  /*************
  TCanvas *cc = new TCanvas();
  cc->Divide(1,2);
  cc->cd(1);
  noise->Draw("ac");
  cc->cd(2);
  ((TVirtualPad *)cc->GetPad(2))->SetLogy();
  //  ((TVirtualPad *)cc->GetPad(2))->SetLogx();
  noiseFFT->Draw("ac");
  //  ((TAxis *)noiseFFT->GetXaxis())->SetRangeUser(1.e+6,1.e+9); 
  *****************/
  /*************************************************************/

  //cosa cambia
  for (;iev<nEvents;iev++) {

    myC->Clear();
    myC->Divide(1,4);

    tr->GetEntry(iev);
    //    time = NULL;
    Waveform *wave = (Waveform *)EvData->GetOscWaveformAt(1); 

    
    Double_t *sign = wave->GetAmpl();
    time = wave->GetTime();
    nPoint = wave->GetNPoints()-nBin-nInit; //togliamo anche il primo 1ns

    grL = new TGraph();
    grL->SetTitle(Form("Waveform Event %d, ch %d",iev,1));  
    truncSign = new Double_t [nPoint];
    timeShift = new Double_t [nPoint];
    for(int idot=0;idot<nPoint;idot++) {
      truncSign[idot] = sign[idot+nInit];
      timeShift[idot] = time[idot+nInit];
      grL->SetPoint(idot,timeShift[idot],truncSign[idot]);
    }    

    myC->cd(1);
    grL->Draw("al");
    ((TAxis *)grL->GetXaxis())->SetTitle("Time [s]");
    ((TAxis *)grL->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [V]");
    ((TAxis *)grL->GetYaxis())->SetLabelSize(0.07);

    //Trasformata di Fourier
    Double_t *Re = new Double_t [nPoint];
    Double_t *Im = new Double_t [nPoint];

    FFT(nPoint,truncSign,Re,Im);
    Double_t fftvalue[nPoint/2]; 
    grL_fft = new TGraph();
    grL_fft->SetTitle("Waveform FFT");

    grL_fftnew = new TGraph();
    grL_fftnew->SetTitle("Waveform FFT after substruction");
    fft_width = 1./(nPoint*1.e-10);

    Double_t thrfft = 6.5e-4;
    Double_t thrrms = 1.4e-4;
    //calcolo media ne primi 100MHz
    int ifft = 0;
    Double_t media_fft = 0;
    Double_t coeff = 0;
    while (ifft*fft_width*1.e-9 < 0.1){
      media_fft += sqrt(Re[ifft+1]*Re[ifft+1]+Im[ifft+1]*Im[ifft+1]);
      ifft++;
    }

    //   cout<<"qui arrivo??? "<<media_fft/((float)ifft)<<endl;

    if (media_fft/((float)ifft) < 0.1) continue; 

    for(ifft=0;ifft<nPoint/2;ifft++){

      fftvalue[ifft] = sqrt(Re[ifft]*Re[ifft]+Im[ifft]*Im[ifft]);
      grL_fft->SetPoint(ifft,ifft*fft_width,fftvalue[ifft]);

      /******
      if (ifft >= 2 && ifft < 5) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
	fftvalue[ifft] = 0.;
      }
      **********/
      if (ifft >= 0 && ifft < 11) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
	fftvalue[ifft] = 0.;
      }

      if (ifft >= 11 && ifft < 18) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
	fftvalue[ifft] = 0.;
      }

      //      if (ifft*fft_width*1.e-9 >= 0.004) coeff = 0.;
      /********************
      fftsum[ifft] = 0.;
      if (ifft*fft_width*1.e-9 >= 0.0 && ifft*fft_width*1.e-9 < 0.02) 
	cout<<ifft<<"  "<<ifft*fft_width*1.e-9<<"  "<<fftvalue[ifft]<<endl;

      if (ifft*fft_width*1.e-9 >= 0.0 && ifft*fft_width*1.e-9 < 0.06) {
	coeff = 0.;
	while (fftvalue[ifft]-fftsum[ifft] > thrfft+coeff*thrrms) {
	  fftsum[ifft] += thrrms;
	}
	
	Re[ifft] *=  (1. - fftsum[ifft]/fftvalue[ifft]);
	Im[ifft] *=  (1. - fftsum[ifft]/fftvalue[ifft]);
	fftvalue[ifft] -= fftsum[ifft];
      }

      if (ifft*fft_width*1.e-9 >= 0.06 && ifft*fft_width*1.e-9 < 0.3) {

	while (fftvalue[ifft]-fftsum[ifft] < 0.01) {
	  fftsum[ifft] -= 0.5*(fftvalue[ifft-1]-thrfft);
	}

	Re[ifft] *=  (1. - fftsum[ifft]/fftvalue[ifft]);
	Im[ifft] *=  (1. - fftsum[ifft]/fftvalue[ifft]);
	fftvalue[ifft] -= fftsum[ifft];

      }
      ************************/

      //      else coeff = 300.; 
      /******************************
      //      if (ifft*fft_width*1.e-9 >= 0.02 && ifft*fft_width*1.e-9 < 0.03) coeff = 250.;
      //      if (ifft*fft_width*1.e-9 >= 0.03 && ifft*fft_width*1.e-9 < 0.04) coeff = 400.;
      if (ifft*fft_width*1.e-9 >= 0.04) coeff = 270.;
      if (ifft*fft_width*1.e-9 >= 0.1) coeff = 250.;
      if (ifft*fft_width*1.e-9 >= 0.2) coeff = 180.;
      if (ifft*fft_width*1.e-9 >= 0.3) coeff = 100.;
      if (ifft*fft_width*1.e-9 >= 0.6) coeff = 35.;
      if (ifft*fft_width*1.e-9 >= 0.8) coeff = 20.;
      if (ifft*fft_width*1.e-9 >= 1.8) coeff = 7.;

      ***************************/
      //}
      //      if (fftvalue[ifft] < 0) fftvalue[ifft] = ;


      grL_fftnew->SetPoint(ifft,ifft*fft_width,fftvalue[ifft]);

    }


    Double_t *signNew = new Double_t [nPoint];
    InverseFFT(nPoint,Re,Im,signNew);
    grLnew = new TGraph(nPoint,timeShift,signNew);


    myC->cd(2);
    ((TVirtualPad *)myC->GetPad(2))->SetLogy();
    //    ((TVirtualPad *)myC->GetPad(2))->SetLogx();
    grL_fft->Draw("al");
    ((TAxis *)grL_fft->GetXaxis())->SetTitle("frequence [Hz]");
    ((TAxis *)grL_fft->GetXaxis())->SetLabelSize(0.07);
    //    ((TAxis *)grL_fft->GetXaxis())->SetRangeUser(1000000.,5000000000.);
    ((TAxis *)grL_fft->GetYaxis())->SetTitle("Magnitude");
    ((TAxis *)grL_fft->GetYaxis())->SetLabelSize(0.07);
 
    myC->cd(3);
    ((TVirtualPad *)myC->GetPad(3))->SetLogy();
    //    ((TVirtualPad *)myC->GetPad(3))->SetLogx();
    grL_fftnew->Draw("al");
    ((TAxis *)grL_fftnew->GetXaxis())->SetTitle("frequence [Hz]");
    ((TAxis *)grL_fftnew->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grL_fftnew->GetYaxis())->SetTitle("Magnitude");
    ((TAxis *)grL_fftnew->GetYaxis())->SetLabelSize(0.07);
    ((TAxis *)grL_fftnew->GetYaxis())->SetRangeUser(1.e-04,10.);

    myC->cd(4);
    grLnew->Draw("al");
    ((TAxis *)grLnew->GetXaxis())->SetTitle("Time [s]");
    ((TAxis *)grLnew->GetXaxis())->SetLabelSize(0.07);
    ((TAxis *)grLnew->GetYaxis())->SetTitle("Amplitude [V]");
    ((TAxis *)grLnew->GetYaxis())->SetLabelSize(0.07);

    myC->Update();
    myC->WaitPrimitive();

    delete grL;
    delete grL_fft;
    delete grL_fftnew;
    delete grLnew;
    delete truncSign;
    delete timeShift;
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
