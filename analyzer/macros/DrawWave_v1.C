#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TVirtualFFT.h"
#include "TVirtualPad.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TAxis.h"
#include "Riostream.h"
#include "generated/GMCWaveformData.h"

void FFT(Int_t N, Double_t *ampl, Double_t *ReFFT, Double_t *ImFFT);

void DrawWave(TString fName="MCHits00001.root") {

  TFile f(fName.Data());

TTree *tr = (TTree *)f.Get("MCHits");

Int_t nEvents = tr->GetEntries();

cout<<"Number of events Stored --> "<<nEvents<<endl;

TClonesArray *W_array = new TClonesArray("GMCWaveformData");
tr->SetBranchAddress("DCWaveforms",&W_array);

tr->GetEntry(0);
Int_t nPoints = ((GMCWaveformData *)W_array->At(0))->GetfNpoints();
Double_t binWidth = ((GMCWaveformData *)W_array->At(0))->GetfBinWidth();

Double_t fft_width = 1./(nPoints*binWidth);

Double_t time[nPoints];
for (int i=0;i<nPoints;i++) time[i] = i*binWidth; 

cout<<binWidth<<"  "<<fft_width<<endl;
//return;

TGraph *grL;
TGraph *grR;
TGraph *grFFT;
TCanvas *myC = new TCanvas();
//TFile fOut("Waveform.root","RECREATE");
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
/*********
    if (ie == 0 && ntubes == 3) {
      ofstream fOut("Waveform_1_tube3_1GHz.txt",ios::out); 
      for (int idot=0;idot<nPoints;idot++) fOut<<time[idot]<<"  "<<signL[idot]<<endl;  
      fOut.close();
    }  *****************/
    grL = new TGraph(nPoints,time,signL);
    grL->SetTitle(Form("Waveform Event %d, tube %d, side Left",ie+1,ntubes));  
//    grR = new TGraph(nPoints,time,signR);  
//    grR->SetTitle(Form("Waveform Event %d, tube %d, side Right",ie+1,ntubes));  

    myC->cd(1);
    grL->Draw("al");
    ((TAxis *)grL->GetXaxis())->SetTitle("Time [ns]");
    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [mV]");

    grFFT = new TGraph();
//  invochiamo la trasformata di fourier
    Double_t Re[nPoints];
    Double_t Im[nPoints];
    FFT(nPoints,signL,Re,Im);

    for (int ifft=0;ifft<nPoints/2;ifft++) {  
      grFFT->SetPoint(ifft,ifft*fft_width,TMath::Sqrt(Re[ifft]*Re[ifft] + Im[ifft]*Im[ifft]));
    }
    
    myC->cd(2);
    ((TVirtualPad*)myC->GetPad(2))->SetLogx();
    grFFT->Draw("al");
    ((TAxis *)grFFT->GetXaxis())->SetTitle("frequence [MHz]");
    ((TAxis *)grFFT->GetYaxis())->SetTitle("Magnitude");
 
    myC->Update();
    myC->WaitPrimitive();
    //myC->Write();

    delete grFFT;
    delete grL;
  }

}
// fOut.Close();
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

