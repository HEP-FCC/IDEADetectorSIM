#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMultiGraph.h"
#include "TH1F.h"
#include "TF1.h"
#include "TH2I.h"
#include "TMath.h"
#include "TCanvas.h"
#include "util/XTRELTIME.h"
#include "util/Geometry.h"
#include "generated/GMCWaveformData.h"
#include "generated/GMCDCHHit.h"
#include "generated/GMCAnalyzer.h"
#include "generated/GMCTGenerateWaveforms_Base.h"
#include "GMCAnalyzerConfigParameters.h"
#include "ROMEXML.h"
#include "mxml.h"

#include <vector>
#include <iterator>

#include "../macros/RecWave.h"

Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t fThrld, Float_t *pkHgt, Int_t *pkPos);
void OrdinaCluster(Int_t nCls,Double_t *driftDist);
Int_t Clusterizzazione(Int_t nGenEle, Double_t *ele_time, Double_t *cut, Bool_t flag);

//void OrdinaCluster(Int_t nCls, std::vector<wClsData > timeArr);
//Double_t RecoDriftDistance(Int_t nbin);

//inizializzazione XT relation
//XTREL *fxt  = NULL;

//inizializzazione geometria
Geometry *fGeometry = Geometry::GetInstance();
Double_t NoiseThrld = 0.;
Int_t clsMulti[100];
Double_t clsFound[100]; 

bool WayToSort (const wClsData &p1, const wClsData &p2) {
  return (p1.timeD < p2.timeD); 
}

using namespace std;

void PeakAnalysis(Int_t BWidth=800,Int_t nev=0,Int_t fnrise=0,Float_t fSigma=3.,Float_t fTimeCut=1.1) {
  

  TString dPath="../Simdata";
  TString filename = TString(Form("MCHits%iMHz_g4_SN7_2GSa",BWidth));
  TString fullName;

  TChain *tr = new TChain("MCHits");

  for (int i=0;i<30;i++) {
    fullName = TString(Form("%s/%s_%03i.root",dPath.Data(),filename.Data(),i+23));
    //fullName = "MCHits00023.root";
    tr->Add(fullName.Data());
  }

  Double_t SNRatio = 7.; 
  Int_t nEvents = tr->GetEntries(); // numero di eventi
  cout<<"ho trovato nr "<<nEvents<<" eventi"<<endl;

  if (nev != 0) nEvents = nev;


  //inizializzazione XT relation
  //  XTREL *fxt = new XTRELTIME("XTData/X_T_Relation_8515.root"); 

  //leggiamo i tagli da usare per la clusterizzazione
  //_____________leggo il file di input contenente i valori dei tagli_____//
  Int_t n_slice = 36;
  ifstream fIn;
  fIn.open("data_file.txt",ios::in);
  Double_t cut[n_slice], tau[n_slice], diffusion[n_slice];
  char buff[200];
  Int_t islice;
  Double_t time,val,val1,val2;
  Int_t iline = 0;
  fIn.getline(buff,200);
  while (iline < n_slice) {
    fIn>>islice>>time>>val>>val1>>val2;
    cut[iline] = val;
    tau[iline] = val1;
    diffusion[iline] = val2;
    iline++;
    //    cout<<islice<<"  "<<time<<"  "<<val<<"  "<<val1<<"  "<<val2<<endl;
  }
  fIn.close();
  
  TF1 *funGoodCls = new TF1("funGoodCls","[0]+[1]*sqrt(x)+[2]*x",0.,0.2);
  funGoodCls->SetParameters(2.33094e-01,5.63499e+00,7.21091e-01);

  TClonesArray *W_array = new TClonesArray("GMCWaveformData");
  tr->SetBranchAddress("DCWaveforms",&W_array);
  
  TClonesArray *Hits = new TClonesArray("GMCDCHHit");
  tr->SetBranchAddress("DCHits",&Hits);
  
  Int_t tmpPkPos[200];
  Float_t tmpPkHgt[200];

  Double_t difft;

  Int_t nrClsGen, nrEleGen; 
  Bool_t fNoRMS = true;  
  TFile *fOutput = 0x0;
  TTree *trOut = 0x0;
  Int_t nrFileWrite = 0;
  Int_t nrEvtsXfile = 1000;
  Int_t IdEvent;
  MCWave data_MC;
  RecWave data_REC;
  Int_t iev = 0;
  for (;iev<nEvents;iev++) {
    
    if (iev%nrEvtsXfile == 0) {
      nrFileWrite++;

      //creo il file output 
      fOutput = new TFile(Form("PeakFound%05d.root",nrFileWrite),"RECREATE");
      
      trOut = new TTree("RecoData","RecoData");
      
      //iEvent
      trOut->Branch("IdEvent",&IdEvent,"IdEvent/I");
      
      //MC data
      trOut->Branch("NrTube",&data_MC.NrTube,"NrTube/I");
      trOut->Branch("IdTrk",&data_MC.IdTrk,"IdTrk/I");
      trOut->Branch("TotCharge",&data_MC.TotCharge,"TotCharge/I");
      trOut->Branch("NrCls",&data_MC.NrCls,"NrCls/I");
      trOut->Branch("bTrue",&data_MC.bTrue,"bTrue/D");
      trOut->Branch("DriftD",&data_MC.DriftD,"DriftD[NrCls]/D");
      trOut->Branch("ClsMulti",&data_MC.ClsMulti,"ClsMulti[NrCls]/I");
      trOut->Branch("EleTime",&data_MC.EleTime,"EleTime[TotCharge]/D");
      trOut->Branch("EleCharge",&data_MC.EleCharge,"EleCharge[TotCharge]/D");
      
      //RecData
      trOut->Branch("NrOfPeaks",&data_REC.NrOfPeaks,"NrOfPeaks/I");
      trOut->Branch("NrClsRec",&data_REC.NrClsRec,"NrClsRec/I");
      trOut->Branch("ClsMultiRec",&data_REC.ClsMultiRec,"ClsMultiRec[NrClsRec]/I");
      trOut->Branch("PeakTime",&data_REC.PeakTime,"PeakTime[NrOfPeaks]/D");
      trOut->Branch("PeakAmplitude",&data_REC.PeakAmplitude,"PeakAmplitude[NrOfPeaks]/D");
      trOut->Branch("ClsTimeRec",&data_REC.ClsTimeRec,"ClsTimeRec[NrClsRec]/D");
    }

    tr->GetEntry(iev);
    Int_t nrOfWaves = W_array->GetEntries();
    
    cout<<"In event "<<iev<<" --> "<<nrOfWaves<<" waveforms"<<endl;
    IdEvent = iev%nrEvtsXfile;
    for (int iw=0;iw<nrOfWaves;iw++) {

      nrClsGen = ((GMCDCHHit *)Hits->At(iw))->GetfNrClusters();
      nrEleGen = (Int_t)((GMCDCHHit *)Hits->At(iw))->GetfTotalCharge();

      if (nrClsGen == -999 || nrClsGen == 0) continue;

      //      Double_t *distance = new Double_t [((GMCDCHHit *)Hits->At(iw))->GetfClsDistanceSize()];
      data_MC.NrTube = ((GMCDCHHit *)Hits->At(iw))->GetfNrOfTube();
      data_MC.IdTrk = ((GMCDCHHit *)Hits->At(iw))->GetfTrkIDofHit();
      data_MC.NrCls = nrClsGen;
      data_MC.TotCharge = nrEleGen;
      data_MC.bTrue =  ((GMCDCHHit *)Hits->At(iw))->GetfImpact();

      GMCWaveformData *wave = (GMCWaveformData *)W_array->At(iw); 
      
      cout<<"waveform nr "<<iw<<" nr cluster  "<<nrClsGen<<" total charge "<<nrEleGen<<endl;
      
      Double_t *signL = wave->GetfSignalL();
      Double_t *signR = wave->GetfSignalR();
      Double_t *DTime = wave->GetfDTimeEle();
      Double_t *Amplitude = wave->GetfPeakAmplitude();
      Double_t *chrgarr = wave->GetfNrEleAvalanche();
      Int_t nGenPks = wave->GetfPeakSize();
      Double_t BinReso = wave->GetfBinWidth(); //in us
      Int_t nPoints = wave->GetfNpoints();

      Double_t t_cluster[nrClsGen];
      Double_t a_cluster[nrClsGen];
      Int_t nSumE = 0;
      Int_t icls;
      for (icls=0;icls<nrClsGen;icls++) {

	Int_t nEle = ((GMCDCHHit *)Hits->At(iw))->GetfClsChargeAt(icls);
	data_MC.ClsMulti[icls] = nEle; 

	data_MC.DriftD[icls] = ((GMCDCHHit *)Hits->At(iw))->GetfClsDistanceAt(icls);

	for (int iee=0;iee<nEle;iee++) {

	  data_MC.EleTime[nSumE+iee] = DTime[nSumE+iee];
	  data_MC.EleCharge[nSumE+iee] = chrgarr[nSumE+iee]; 

	}

	nSumE += nEle;
      }

      Float_t fThrld = fSigma;
      //individuiamo i picchi
      Int_t nFndPks = findPeaks(signL,nPoints,fnrise,fThrld,tmpPkHgt,tmpPkPos);
      //      ((TH1F *)histo->At(9))->Fill(nFndPks);

      Double_t ArrayOfPeaksTime[nFndPks];
      Int_t nGood = 0;
      for (int ip=0;ip<nFndPks;ip++) {
	if (((tmpPkPos[ip] - fnrise)*BinReso - 0.3) > 0. && fabs(tmpPkHgt[ip]) > 5.*fThrld/fSigma ) { 
	  ArrayOfPeaksTime[nGood] = (tmpPkPos[ip] - fnrise)*BinReso - 0.3;
	  data_REC.PeakTime[nGood] = ArrayOfPeaksTime[nGood]; 
	  data_REC.PeakAmplitude[nGood] = -tmpPkHgt[ip];
	  nGood++;
	}
      }

      data_REC.NrOfPeaks = nGood;
      //clusterizzazione dei picchi
      Int_t ncluster = Clusterizzazione(nGood,ArrayOfPeaksTime,cut,0);
      data_REC.NrClsRec = ncluster;
      for (int ip=0;ip<ncluster;ip++) {
	data_REC.ClsMultiRec[ip] = clsMulti[ip];
	data_REC.ClsTimeRec[ip] = clsFound[ip];
	cout<<"cls "<<ip+1<<"  time "<<clsFound[ip]<<endl;
      }

      trOut->Fill();
    }

    if (iev%nrEvtsXfile == nrEvtsXfile-1) {
      trOut->Write();
      fOutput->Close();
    }
  }

  /*********
  if (iev%nrEvtsXfile < nrEvtsXfile-1) {
    trOut->Write();
    fOutput->Close();
  }
  *****************/

}
  
//__________________________________________________________
Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t fThrld,Float_t *pkHgt, Int_t *pkPos) {

  //  Int_t nrise=3;
  Int_t checkUpTo=1;//1; //
  if(checkUpTo>nrise) { checkUpTo=nrise; }
  Float_t sig=0.5e-3;
  Float_t *sigd = new Float_t[nrise];
  Float_t *sigd_2 = new Float_t[nrise];
  
  TH1F tmpWdist("tmpWdist","",1000,-5.,5.);
  for (int i=10; i<500; ++i) { tmpWdist.Fill(data[i]); }
  sig = tmpWdist.GetRMS();
  Float_t mean = tmpWdist.GetMean();
  
 // cout<<"Signal Off "<<mean<<" noise "<<sig<<endl;
  
  for (int ir=checkUpTo; ir<=nrise; ++ir) {
    int irId = ir-1;
    sigd[irId]=2.4495*sig/((float)(2*ir+1));
    sigd_2[irId]=1.414*sigd[irId];//%0
    sigd_2[irId]*=3.;
    sigd[irId]*=3.5;//% stava a 5
    // better comb 4-3, 2-4
    //cout<<"iRise "<<ir<<" sgm "<<sigd[irId]<<" sgm_2 "<<sigd_2[irId]<<endl;
  }
  sig *= fThrld;  //stava a 4
  fThrld = sig;
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
      
      if ( deri[i][irId]>=sigd[irId] && (deri[i][irId]-deri[i-ir][irId])>sigd_2[irId] && (wave[i]-wave[i-ir])>2.*sig ) {
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

//______________________________________________________________________________________
void OrdinaCluster(Int_t nCls, Double_t *driftDist) {


  Double_t tmpDistance[200];
  for (int icls=0;icls<nCls;icls++) {
  
    tmpDistance[icls] = TMath::MinElement(nCls,driftDist);
    Bool_t found = false;
    int idx = 0;
    while (!found) {
     if (tmpDistance[icls] == driftDist[idx]) {
       driftDist[idx] = 999.;
       found = true;
     }
     idx++;  
    }
  }

  for (int icls=0;icls<nCls;icls++) driftDist[icls] = tmpDistance[icls];

}

//__________________________________________________________
Int_t Clusterizzazione(Int_t nGenEle, Double_t *ele_time, Double_t *cut, Bool_t flag){


  //  Int_t count_slice[n_slice];
  //  Double_t time[100];

  if (nGenEle == 0) return 0;

  cout<<"*************  entro in raggruppa ******** "<<endl;

  //azzero il vettore temporaneo dei tempi
  for (int k=0; k<100; k++) {
    clsFound[k] = 0.;
    clsMulti[k] = 0;
  }

  cout<<" tempo  "<<ele_time[0]<<endl;
  clsFound[0] = ele_time[0];
  clsMulti[0] = 1;

  Int_t nrg = 1;
  int cls_count=1;

  for (int n=1; n<nGenEle; n++){
    cout<<" tempo  "<<ele_time[n]<<endl;
    Double_t dt = 1000.*(ele_time[n] - clsFound[cls_count-1]);

    Int_t is = (int)(clsFound[cls_count-1]*1000./5.);

    if (dt > cut[is]) {
      clsFound[cls_count] = ele_time[n]; 
      clsMulti[cls_count-1] = nrg;
      cls_count++;
      nrg = 1;
    }
    else {

      if (flag) {
        Double_t media = (nrg*clsFound[cls_count-1] + ele_time[n])/(nrg + 1.);
        clsFound[cls_count-1] = media;
      } 
      nrg++;
    }
  }

  clsMulti[cls_count-1] = nrg;

  cout<<"riesco dalla fuction con "<<cls_count<<endl;
  return cls_count;

};


/*************************************
//______________________________________________________________________________________
void OrdinaCluster(Int_t nCls, std::vector<wClsData > timeArr) {

  Double_t tmpDistance[200];



  for (int icls=0;icls<nCls;icls++) {
    wClsData cls1 = timeArr.at(icls);
    cout<<"idx "<<cls1.idx<<" nEle "<<cls1.nEl<<" ddrift "<<cls1.driftD<<" time "<<cls1.timeD<<endl;
  }



}
****************************************************************/
//_________________________________________________________________
/******
Double_t RecoDriftDistance(Int_t nBin) {

  Double_t distance = 9999.;
  Double_t binRes = 0.0004;
  
  Float_t tmpTime = (nBin-5)*binRes - 0.3;
  
  if (tmpTime <= 0.) return distance; 
 
  distance = 10.*((double )fxt->timeToSpace(tmpTime)[0]);
//  distance[1] = 10.*((double )fxt->timeToSpace(tmpTime)[1]);
  
  return distance;

}
******************/
