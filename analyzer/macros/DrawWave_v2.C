#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TVirtualFFT.h"
#include "TVirtualPad.h"
#include "TMultiGraph.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TAxis.h"
#include "Riostream.h"
#include "generated/GMCWaveformData.h"

void FFT(Int_t N, Double_t *ampl, Double_t *ReFFT, Double_t *ImFFT);
void InverseFFT(Int_t N, Double_t *ReFFT, Double_t *ImFFT, Double_t *ampl);

void DrawWave(Double_t fCut=100., Double_t fCut2=50.) {

TFile f1("MCHits300MHz_g5_01.root");
TFile f2("MCHits620MHz_g5_01.root");
TFile f3("MCHits1000MHz_g5_01.root");

TTree *tr1 = (TTree *)f1.Get("MCHits");
TTree *tr2 = (TTree *)f2.Get("MCHits");
TTree *tr3 = (TTree *)f3.Get("MCHits");

Int_t nEvents = tr1->GetEntries();

cout<<"Number of events Stored --> "<<nEvents<<endl;

TClonesArray *W_array_1 = new TClonesArray("GMCWaveformData");
TClonesArray *W_array_2 = new TClonesArray("GMCWaveformData");
TClonesArray *W_array_3 = new TClonesArray("GMCWaveformData");

tr1->SetBranchAddress("DCWaveforms",&W_array_1);
tr2->SetBranchAddress("DCWaveforms",&W_array_2);
tr3->SetBranchAddress("DCWaveforms",&W_array_3);

tr1->GetEntry(0);
Int_t nPoints = ((GMCWaveformData *)W_array_1->At(0))->GetfNpoints();
Double_t binWidth = ((GMCWaveformData *)W_array_1->At(0))->GetfBinWidth();

Double_t fft_width = 1./(nPoints*binWidth);

Double_t time[nPoints];
for (int i=0;i<nPoints;i++) time[i] = i*binWidth; 

cout<<binWidth<<"  "<<fft_width<<endl;
//return;

TGraph *grL_1;
//TGraph *grR_1;
TGraph *grFFT_1;
TGraph *grCut_1;
TGraph *grL_2;
//TGraph *grR_2;
TGraph *grFFT_2;
TGraph *grCut_2;
TGraph *grL_3;
//TGraph *grR_3;
TGraph *grFFT_3;
TGraph *grCut_3;
TGraph *grCut_4;
TCanvas *myC = new TCanvas();
 
for (int ie=0;ie<nEvents;ie++) {

  tr1->GetEntry(ie);
  tr2->GetEntry(ie);
  tr3->GetEntry(ie);

  Int_t nrOfWaves = W_array_1->GetEntries();

  cout<<"In event "<<ie<<" --> "<<nrOfWaves<<" waveforms"<<endl;
  
  for (int iw=0;iw<nrOfWaves;iw++) {

    myC->Clear();
    myC->Divide(1,2);
    
    GMCWaveformData *wave_1 = (GMCWaveformData *)W_array_1->At(iw);
    GMCWaveformData *wave_2 = (GMCWaveformData *)W_array_2->At(iw);
    GMCWaveformData *wave_3 = (GMCWaveformData *)W_array_3->At(iw);


    Double_t *signL_1 = wave_1->GetfSignalL();
    Double_t *signL_2 = wave_2->GetfSignalL();
    Double_t *signL_3 = wave_3->GetfSignalL();

    //    Double_t *signR = wave->GetfSignalR();
    Int_t ntubes = wave_1->GetfNrOfTube();

    grL_1 = new TGraph(nPoints,time,signL_1);
    grL_2 = new TGraph(nPoints,time,signL_2);
    grL_3 = new TGraph(nPoints,time,signL_3);

    grL_1->SetMarkerColor(2);
    grL_1->SetLineColor(2);
    grL_2->SetMarkerColor(3);
    grL_2->SetLineColor(3);
    grL_3->SetMarkerColor(4);
    grL_3->SetLineColor(4);

    //    grL->SetTitle(Form("Waveform Event %d, tube %d, side Left",ie+1,ntubes));  
//    grR = new TGraph(nPoints,time,signR);  
//    grR->SetTitle(Form("Waveform Event %d, tube %d, side Right",ie+1,ntubes));  

    myC->cd(1);
    TMultiGraph *mgr_1 = new TMultiGraph();
    mgr_1->Add(grL_1);
    mgr_1->Add(grL_2);
    mgr_1->Add(grL_3);
    mgr_1->Draw("al");
    ((TAxis *)mgr_1->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)mgr_1->GetYaxis())->SetTitle("Amplitude [mV]");

//  invochiamo la trasformata di fourier
    grFFT_1 = new TGraph();
    Double_t Re[nPoints];
    Double_t Im[nPoints];
    Double_t ampl_f[nPoints];
    FFT(nPoints,signL_1,Re,Im);

    for (int ifft=0;ifft<nPoints/2;ifft++) {  
      grFFT_1->SetPoint(ifft,ifft*fft_width,TMath::Sqrt(Re[ifft]*Re[ifft] + Im[ifft]*Im[ifft]));

      /*      if (ifft*fft_width > fCut) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
	}*/
    }

    //    InverseFFT(nPoints,Re,Im,ampl_f);
    //    grCut_1 = new TGraph(nPoints,time,ampl_f);

    grFFT_2 = new TGraph();
    FFT(nPoints,signL_2,Re,Im);

    for (int ifft=0;ifft<nPoints/2;ifft++) {  
      grFFT_2->SetPoint(ifft,ifft*fft_width,TMath::Sqrt(Re[ifft]*Re[ifft] + Im[ifft]*Im[ifft]));

      /*      if (ifft*fft_width > fCut) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
	}*/

    }

    //    InverseFFT(nPoints,Re,Im,ampl_f);
    //    grCut_2 = new TGraph(nPoints,time,ampl_f);

    grFFT_3 = new TGraph();
    FFT(nPoints,signL_3,Re,Im);

    for (int ifft=0;ifft<nPoints/2;ifft++) {  
      grFFT_3->SetPoint(ifft,ifft*fft_width,TMath::Sqrt(Re[ifft]*Re[ifft] + Im[ifft]*Im[ifft]));

      /*      if (ifft*fft_width > fCut) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
	}*/

    }

    //    InverseFFT(nPoints,Re,Im,ampl_f);
    //    grCut_3 = new TGraph(nPoints,time,ampl_f);

    //different cut
    /*    for (int ifft=0;ifft<nPoints/2;ifft++) {  

      if (ifft*fft_width > fCut2) {
	Re[ifft] = 0.;
	Im[ifft] = 0.;
      }

    }

    InverseFFT(nPoints,Re,Im,ampl_f);
    grCut_4 = new TGraph(nPoints,time,ampl_f);
    */
    

    grFFT_1->SetMarkerColor(2);
    grFFT_1->SetLineColor(2);
    grFFT_2->SetMarkerColor(3);
    grFFT_2->SetLineColor(3);
    grFFT_3->SetMarkerColor(4);
    grFFT_3->SetLineColor(4);

    myC->cd(2);
    ((TVirtualPad*)myC->GetPad(2))->SetLogx();
    ((TVirtualPad*)myC->GetPad(2))->SetLogy();

    TMultiGraph *mgr_2 = new TMultiGraph();
    mgr_2->Add(grFFT_1);
    mgr_2->Add(grFFT_2);
    mgr_2->Add(grFFT_3);
    //    mgr_2->Add(grCut_4);
    mgr_2->Draw("al");

    mgr_2->Draw("al");
    ((TAxis *)mgr_2->GetXaxis())->SetTitle("frequence [MHz]");
    ((TAxis *)mgr_2->GetYaxis())->SetTitle("Magnitude");
 

    //    myC->cd(3);
    /*    grCut_1->SetMarkerColor(2);
    grCut_1->SetLineColor(2);
    grCut_2->SetMarkerColor(3);
    grCut_2->SetLineColor(3);
    grCut_3->SetMarkerColor(4);
    grCut_3->SetLineColor(4);

    TMultiGraph *mgr_3 = new TMultiGraph();
    //    mgr_3->Add(grCut_1);
    //   mgr_3->Add(grCut_2);
    mgr_3->Add(grCut_3);
    mgr_3->Draw("al");

    mgr_3->Draw("al");
    ((TAxis *)mgr_3->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)mgr_3->GetYaxis())->SetTitle("Amplitude [mV]");
    */

    myC->Update();
    myC->WaitPrimitive();

    delete grFFT_1;
    delete grFFT_2;
    delete grFFT_3;
    delete grL_1;
    delete grL_2;
    delete grL_3;
    //    delete grCut_1;
    //    delete grCut_2;
    //    delete grCut_3;
    delete mgr_1;
    delete mgr_2;
    //    delete mgr_3;
  }

}
 
}

//_______________________________________________________________________
void FFT(Int_t N, Double_t *ampl, Double_t *ReFFT, Double_t *ImFFT) {

    TVirtualFFT *fft = TVirtualFFT::FFT(1,&N,"R2C");
    for (int k=0;k<N;k++) fft->SetPoint(k,ampl[k]);
    fft->Transform();
    for(Int_t i=0;i<N/2;i++){
        fft->GetPointComplex(i,ReFFT[i],ImFFT[i]);
        ReFFT[i] /= sqrt(N);
        ImFFT[i] /= sqrt(N);
    }

    delete fft;
}

//__________________________________________________________________-
void InverseFFT(Int_t N, Double_t *ReFFT, Double_t *ImFFT, Double_t *ampl)
{

    TVirtualFFT *at = TVirtualFFT::FFT(1,&N,"C2R");
    at->SetPointsComplex(ReFFT,ImFFT);
    at->Transform();

    at->GetPoints(ampl);

    for(Int_t i=0;i<N;i++){
        ampl[i] /= sqrt(N);
    }

}
