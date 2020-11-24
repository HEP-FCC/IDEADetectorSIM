
Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t *pkHgt, Int_t *pkPos);


void PeakAnalysis(Int_t nev=1) {

TFile f("MCHits00001.root");

TTree *tr = (TTree *)f.Get("MCHits");

Int_t nEvents = tr->GetEntries();

cout<<"Number of events Stored --> "<<nEvents<<endl;

TClonesArray *W_array = new TClonesArray("GMCWaveformData");
tr->SetBranchAddress("DCWaveforms",&W_array);

TClonesArray *Hits = new TClonesArray("GMCDCHHit");
tr->SetBranchAddress("DCHits",&Hits);

Int_t tmpPkPos[100];
Float_t tmpPkHgt[100];

Double_t *time = new Double_t [1750];
for (int i=0;i<1750;i++) time[i] = i*0.4;
 
//TGraph *grL;
//TGraph *grR;
//TCanvas *myC = new TCanvas();
TH1F *clsgen = new TH1F("h100","Number of cluster generated; # Cls",50,0.,50.);
TH1F *clsrec = new TH1F("h101","Number of cluster identified; # Cls",50,0.,50.);
TH1F *percen = new TH1F("h102","Percentage of identified peaks; %",120,0.,1.2);

if (nev != 1) nEvents = nev;

ofstream fOut("ClusterZero.txt",ios::out);
Int_t nrPksGen = 0;
Int_t nrise = 8; 
for (int iev=0;iev<nEvents;iev++) {

  tr->GetEntry(iev);
  Int_t nrOfWaves = W_array->GetEntries();

  cout<<"In event "<<iev<<" --> "<<nrOfWaves<<" waveforms"<<endl;
  
   
  for (int iw=0;iw<nrOfWaves;iw++) {

//    myC->Clear();
//    myC->Divide(1,4);
    nrPksGen = ((GMCDCHHit *)Hits->At(iw))->GetfNrClusters();
    if (nrPksGen == -999) nrPksGen = 0;
    clsgen->Fill(nrPksGen);
    

    GMCWaveformData *wave = (GMCWaveformData *)W_array->At(iw); 
    Double_t *signL = wave->GetfSignalL();
    Double_t *signR = wave->GetfSignalR();
    Int_t ntubes = wave->GetfNrOfTube();

    Int_t nFndPks = findPeaks(signL,1750,nrise,tmpPkHgt,tmpPkPos);
 
    Int_t contapicchi = 0;
    for (int ip=0;ip<nFndPks;ip++) {
    
//      cout<<"cosa sto stampando ???? "<<tmpPkHgt[ip]<<endl;

      if (fabs(tmpPkHgt[ip]) > 3.5) {
        contapicchi++;
       } 
    }
//s   contapicchi = nFndPks;
   if (contapicchi == 0 ) {
      cout<<"Event nr "<<iev<<" Waveform "<<iw<<" Numero di picchi generato "<<nrPksGen<<endl;
   }
   
   //cout<<"Nr picchi trovati "<<contapicchi<<"/"<<((GMCDCHHit *)Hits->At(iw))->GetfNrClusters()<<endl;
   clsrec->Fill(contapicchi);
   if (nrPksGen > 0) percen->Fill((float)contapicchi/(nrPksGen));
   
//    grL = new TGraph(2500,time,signL);
//    grL->SetTitle(Form("Waveform Event %d, tube %d, side Left",iev+1,ntubes));  
//    grR = new TGraph(2500,time,signR);  
//    grR->SetTitle(Form("Waveform Event %d, tube %d, side Right",ie+1,ntubes));  

//    cV->cd(4);
//    grL->Draw("al");
//    ((TAxis *)grL->GetXaxis())->SetTitle("Time [ns]");
//    ((TAxis *)grL->GetYaxis())->SetTitle("Amplitude [mV]");

/*************************
    TMarker *marks = new TMarker[nFndPks];
    for (int ip=0; ip<nFndPks; ++ip) {
      marks[ip].SetMarkerStyle(22);
      marks[ip].SetMarkerColor(kRed);
      marks[ip].SetX(time[tmpPkPos[ip]]);
      marks[ip].SetY(signL[tmpPkPos[ip]]);
      marks[ip].Draw();
      cout<<"pk "<<ip<<" at "<<time[tmpPkPos[ip]]<<" ampl "<<signL[tmpPkPos[ip]]<<endl;
    }
*************************/
//    myC->cd(2);
//    grR->Draw("al");
//    ((TAxis *)grR->GetXaxis())->SetTitle("Time [ns]");
//    ((TAxis *)grR->GetYaxis())->SetTitle("Amplitude [mV]");
 
  }
fOut.close();
 }

clsgen->SetFillStyle(3004);
clsgen->SetFillColor(kRed);
clsrec->SetLineWidth(3);
TCanvas *myC = new TCanvas();
myC->Divide(2,1);
myC->cd(1);
clsgen->DrawCopy();
myC->cd(2);
clsrec->DrawCopy();

TCanvas *myC1 = new TCanvas();
clsrec->DrawCopy();
clsgen->DrawCopy("same");
//percen->DrawCopy();

}


//__________________________________________________________
Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t *pkHgt, Int_t *pkPos) {

  Int_t nrise=5;
  Int_t checkUpTo=1;//1; //
  if(checkUpTo>nrise) { checkUpTo=nrise; }
  Float_t sig=0.5e-3;
  Float_t *sigd = new Float_t[nrise];
  Float_t *sigd_2 = new Float_t[nrise];
  
  TH1F tmpWdist("tmpWdist","",1000,-0.5,0.5);
  for (int i=100; i<700; ++i) { tmpWdist.Fill(data[i]); }
  sig = tmpWdist.GetRMS();
  Float_t mean = tmpWdist.GetMean();
  
 // cout<<"Signal Off "<<mean<<" noise "<<sig<<endl;
  
  for (int ir=checkUpTo; ir<=nrise; ++ir) {
    int irId = ir-1;
    sigd[irId]=2.4495*sig/((float)(2*ir+1));
    sigd_2[irId]=1.414*sigd[irId];//%0
    sigd_2[irId]*=5.;
    sigd[irId]*=7.;//% stava a 5
    // better comb 4-3, 2-4
    //cout<<"iRise "<<ir<<" sgm "<<sigd[irId]<<" sgm_2 "<<sigd_2[irId]<<endl;
  }
  sig*=5.;  //stava a 4
  //sig*=1.414;

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
  }
  //Int_t tmpPkPos[maxNPks];
  //Float_t tmpPkHgt[maxNPks];
  Int_t nPks=0;
  Int_t last=-nrise;
  for (int i=nrise+1; i<nPnts-1; ++i) {
    for (int ir=nrise; ir>=checkUpTo; --ir) {
      int irId = ir-1;
      deri[i][irId] = (2.0*wave[i]-wave[i-ir]-wave[i-ir-1])/((float)(2*ir+1));
      riseMeas[i][irId] = (deri[i][irId]-deri[i-ir][irId]);
      
      if ( deri[i][irId]>=sigd[irId] && (deri[i][irId]-deri[i-ir][irId])>sigd_2[irId] && (wave[i]-wave[i-ir])>sig ) {
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


