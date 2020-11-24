
void FFT(Int_t N, Double_t *ampl, Double_t *ReFFT, Double_t *ImFFT);

void SignalShape() {

  Double_t mu0 = 4.e-7*TMath::Pi();
  Double_t eps0 = 1./(mu0*TMath::C()*TMath::C());

  Double_t rTube = 4.975;
  Double_t rWire = 0.010;
  Double_t DeltaV = 1500.;
  Double_t mob = 10.4*100;   //  mm^2/Vs

  Double_t Ztube = TMath::Sqrt(mu0/eps0)*TMath::Log(rTube/rWire)/(2.*TMath::Pi());

  Double_t t0 = 1e9*TMath::Power(rWire,2)*TMath::Log(rTube/rWire)/(2.*DeltaV*mob);

  Double_t reflect = (370. - Ztube)/(370. + Ztube);

  Double_t parK = -1e9*TMath::Qe()/(2.*TMath::Log(rTube/rWire))*370.*(1.-reflect);

  TF1 *fPolya = new TF1("Polya","((1.+[2])/([1]*TMath::Gamma([2]+1)))*TMath::Power(x*(1.+ [2])/[1],[2])*TMath::Exp(-x*(1.+ [2])/[1])",1.e3,1.e7);


  fPolya->SetParameter(0,1.);          //Normalization
  fPolya->SetParameter(1,1.e5);    //GasGain
  fPolya->SetParameter(2,0.5);         //Geometrical parameter for Drift Tube 
                                       //(see G.D. Alkhazov NIM A89 pag 155)

  Double_t charge = fPolya->GetRandom();

  cout<<"Ztube  "<<Ztube<<endl;
  cout<<"t0     "<<t0<<endl;
  cout<<"parK   "<<parK<<endl;
  cout<<"charge "<<charge<<endl;

  TF1 *shape = new TF1("Shape","[0]*(1./([1]+x))",0.,700.);

  parK *= charge;
 
  shape->SetParameter(0,parK);
  shape->SetParameter(1,t0);
  Double_t ampl[2000];

  TGraph *gr = new TGraph();
  for (int i=0;i<2000;i++) {
    if (i < 100) ampl[i] = 0.;
    else ampl[i] = 1000.*shape->Eval(i*0.4);

    gr->SetPoint(i,i*0.4,ampl[i]);
  }

  Double_t Re[2000];
  Double_t Im[2000];
  Double_t fft_width = 1./(2000.*0.4*1.e-9);

  TGraph *grFFT = new TGraph();
  FFT(2000,ampl,Re,Im);
  for (int ifft=0;ifft<1000;ifft++) {  
    grFFT->SetPoint(ifft,ifft*fft_width,TMath::Sqrt(Re[ifft]*Re[ifft] + Im[ifft]*Im[ifft]));
  }


  TCanvas *myC = new TCanvas();
  myC->Divide(1,2);
  myC->cd(1);
  gr->Draw("al");
  myC->cd(2);
  grFFT->Draw("al");


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
