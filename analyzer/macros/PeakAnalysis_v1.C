#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMultiGraph.h"
#include "TH1F.h"
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

struct wClsData {
  Int_t idx;
  Int_t nEl;
  Double_t driftD;
  Double_t timeD; 
  Double_t charge;
};


Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t *pkHgt, Int_t *pkPos);
void OrdinaCluster(Int_t nCls,Double_t *driftDist);
//void OrdinaCluster(Int_t nCls, std::vector<wClsData > timeArr);
Double_t RecoDriftDistance(Int_t nbin);

//inizializzazione XT relation
XTREL *fxt  = NULL;

//inizializzazione geometria
Geometry *fGeometry = Geometry::GetInstance();
Double_t NoiseThrld = 0.;
bool operator< (const wClsData &p1, const wClsData &p2) {
  return (p1.timeD < p2.timeD); 
}

using namespace std;

void PeakAnalysis_v1(TString fName="MCHits00001.root",Int_t nev=0,Int_t fnrise=0,Float_t fAmplCut=0.1,Float_t fTimeCut=1.1) {
  
  Double_t BandWidth = 1000.;
  Double_t maxPeakValue = 297.;
  Double_t shiftPeakTime = 0.30; //ns
  TString Bw = "1000MHz";

  Bool_t freqFromFile = false;
  Bool_t fExtnRise = false;
  Float_t freqAq; 
  if (fnrise != 0) fExtnRise = true;

  if (fName.Contains("MHz")) {
    //estraggo la BW
    freqFromFile = true;
    Int_t iniP = (Int_t)fName.First("s");
    Int_t stoP = (Int_t)fName.First("z");
    Bw = TString(fName(iniP+1,stoP-iniP));
    //    Int_t posMHz = (Int_t)Bw.First("M");
    //    if (posMHz == 4) BandWidth = TString(Bw(0,4)).Atoi();
    /*else*/ BandWidth = TString(Bw(0,Bw.Length()-3)).Atof();
  }

  if (BandWidth == 300.) {
    maxPeakValue = 176.;
    shiftPeakTime = 0.70; //ns
  }

  if (BandWidth == 620.) {
    maxPeakValue = 247.;
    shiftPeakTime = 0.41; //ns
  }

  cout<<Bw.Data()<<"   "<<BandWidth<<endl;

  //lettura file.xml
  Double_t resistRR;
  Double_t timeRUp;
  TString ConfFile = "geant4MC.xml";
  if (freqFromFile) ConfFile ="geant4MC_"+Bw+".xml";

  ROMEXML *xml = new ROMEXML();
  xml->OpenFileForRead(ConfFile.Data());
  if (xml->ExistPath("/Configuration/MainConfiguration/Analyzer/Tasks")) {
    PMXML_NODE TasksNode = xml->GetPathNode("/Configuration/MainConfiguration/Analyzer/Tasks");
    PMXML_NODE TaskGenWave = xml->GetSubNode(TasksNode,2);
    PMXML_NODE SteerPar = xml->GetSubNode(TaskGenWave,2);
    TString NodeValue(xml->GetNodeValue(xml->GetSubNode(SteerPar,1))); 
    if (NodeValue.CompareTo("ResistRR") == 0)  
      resistRR = TString(xml->GetNodeValue(xml->GetSubNode(SteerPar,2))).Atof();
    if (freqFromFile) {
      SteerPar = xml->GetSubNode(TaskGenWave,8);
      TString BinWidth(xml->GetNodeValue(xml->GetSubNode(SteerPar,2)));
      cout<<"Stampo ultimo par "<<BinWidth.Data()<<endl;   
      freqAq = BinWidth.Atof();
      SteerPar = xml->GetSubNode(TaskGenWave,4);
      TString NodeValue(xml->GetNodeValue(xml->GetSubNode(SteerPar,1))); 
   if (!fExtnRise && NodeValue.CompareTo("tauRumpUp") == 0)  
	fnrise = TMath::Nint(3.*TString(xml->GetNodeValue(xml->GetSubNode(SteerPar,2))).Atof()/freqAq);
    }
  }
   
  Double_t Ztube=fGeometry->GetTubeImpedance();
  Double_t ReflectFact = (resistRR - Ztube)/(resistRR + Ztube);

  Double_t Convfactor = -0.5*TMath::Qe()*resistRR*1.e9*10.*maxPeakValue*(1.-ReflectFact);

  TFile f(fName.Data());
  
  fxt = new XTRELTIME("XTData/X_T_Relation_8515.root");  
  TTree *tr = (TTree *)f.Get("MCHits");
  
  Int_t nEvents = tr->GetEntries();

  cout<<"Number of events Stored --> "<<nEvents<<endl;
  cout<<"Sto utilizzando nRise = "<<fnrise<<endl;
  Int_t nFake = 0;
  TClonesArray *W_array = new TClonesArray("GMCWaveformData");
  tr->SetBranchAddress("DCWaveforms",&W_array);
  
  TClonesArray *Hits = new TClonesArray("GMCDCHHit");
  tr->SetBranchAddress("DCHits",&Hits);
  
  Int_t tmpPkPos[200];
  Float_t tmpPkHgt[200];
  
  //  Double_t *time = new Double_t [1750];
  //  for (int i=0;i<1750;i++) time[i] = i*0.4;
  
  //TGraph *grL;
  //TGraph *grR;
  //TCanvas *myC = new TCanvas();
  TObjArray *histo = new TObjArray();
  TObjArray *histo2D = new TObjArray();
  
  histo->Add( new TH1F("h100","Number of cluster generated; # count",50,0.,50.));
  histo->Add( new TH1F("h101","Number of Peaks identified; # count",50,0.,50.));
  histo->Add( new TH1F("h102","Percentage of identified peaks; %",120,0.,1.2));
  histo->Add( new TH1F("h103","#Delta d = d_{gen} - d_{rec}; mm",100,-0.5,0.5));
  histo->Add( new TH1F("h104","#Delta t  consecutive clusters; [#ns]",200,0.,200.));
  histo->Add( new TH1F("h105","(NrEle_{recog}-NrPeaks) wave by wave; # count",21,-10.5,10.5));
  histo->Add( new TH1F("h106","peak amplitude; mV",200,0.,100.));
  histo->Add( new TH1F("h107","Impact Parameter from MC; mm",100,0.,5.));
  histo->Add( new TH1F("h108","Number Of electrons generated; # count",50,0.,50.));
  histo->Add( new TH1F("h109","Avalanche; # e- [10^{5}]",100,0.,10.));
  histo->Add( new TH1F("h110","#Delta t between two recognizeable peaks;  time [#mus]",200,0.,0.2));
  histo->Add( new TH1F("h111","Number Of electrons recognizable; # count",50,0.,50.));
  histo->Add( new TH1F("h112","Number Of skipped electrons ; # count",25,0.,25.));
  histo->Add( new TH1F("h113","Number Of peaks recognized; # Cls",50,0.,50.));
  histo->Add( new TH1F("h114","#Delta t = |t_{e-} - t_{peak}|;  time [ns]",150,0.,3.));
  histo->Add( new TH1F("h115","Number Of peaks Well recognized (#deltat < BinWidth); # count",50,0.,50.));
  histo->Add( new TH1F("h116","Effciency on Nr Of peaks Well recognized (#deltat < BinWidth); %pecenatage",120,0.,1.2));
  histo->Add( new TH1F("h117","Nr of fake peaks;",20,0.5,21.5));


  histo2D->Add( new TH2F("h200","#Delta(NrEle) Vs Nr_{recog}; #count; #e-",30,0.5,30.5,20,-4.5,15.5));
  histo2D->Add( new TH2F("h201","#Efficency Vs Nr_{recog}; #count; %percentage",30,0.5,30.5,120,0,1.5));
  histo2D->Add( new TH2F("h202","#Efficency (well rec) Vs Nr_{recog}; #count; %percentage",30,0.5,30.5,120,0,1.5));


  
//  TMultiGraph *mgr = new TMultiGraph("mult",Form("Peak distribution @ %s",Bw.Data()));
  Int_t nrPksGen; 
  Int_t nGoodPeaks = 0;
  Int_t nAsstedPeaks = 0;
  Double_t difft;
  //  nEvents = 1;
  for (int iev=0;iev<nEvents;iev++) {
    
    tr->GetEntry(iev);
    Int_t nrOfWaves = W_array->GetEntries();
    
    // cout<<"In event "<<iev<<" --> "<<nrOfWaves<<" waveforms"<<endl;

    for (int iw=0;iw<nrOfWaves;iw++) {
      nFake = 0;
      nrPksGen = ((GMCDCHHit *)Hits->At(iw))->GetfNrClusters();
      GMCWaveformData *wave = (GMCWaveformData *)W_array->At(iw); 

      //    cout<<"waveform nr "<<iw<<" nr cluster  "<<nrPksGen<<endl;
      std::vector<wClsData> clsData; 
      
      Double_t *signL = wave->GetfSignalL();
      Double_t *signR = wave->GetfSignalR();
      Double_t *DTime = wave->GetfDTimeEle();
      Double_t *chrgarr = wave->GetfNrEleAvalanche();
      Double_t BinReso = wave->GetfBinWidth();
      Int_t nPoints = wave->GetfNpoints();

      if (nrPksGen == -999) nrPksGen = 0;
      if (nrPksGen == 0) continue;

      ((TH1F *)histo->At(0))->Fill(nrPksGen);
      
      double bPar = ((GMCDCHHit *)Hits->At(iw))->GetfImpact();
      if (nrPksGen > 0) ((TH1F *)histo->At(7))->Fill(bPar);
      Double_t distance[200];
      if (nrPksGen >= 200) nrPksGen = 200;    
      Int_t nSumE = 0;
      for (int icls=0;icls<nrPksGen;icls++) {
	distance[icls] = ((GMCDCHHit *)Hits->At(iw))->GetfClsDistanceAt(icls);
	Int_t nEle = ((GMCDCHHit *)Hits->At(iw))->GetfClsChargeAt(icls);
	((TH1F *)histo->At(4))->Fill(1000.*(DTime[nSumE-1]-DTime[nSumE]));
	for (int iee=0;iee<nEle;iee++) {
	  wClsData kCls;    
	  kCls.idx = icls;
	  kCls.nEl = iee+1;
	  kCls.driftD = distance[icls];
	  kCls.timeD = DTime[nSumE+iee];
	  kCls.charge = chrgarr[nSumE+iee]; 
	  clsData.push_back(kCls);
	  ((TH1F *)histo->At(9))->Fill(1.e-5*chrgarr[nSumE+iee]);
	}
	nSumE += nEle;
      }
      
      OrdinaCluster(nrPksGen,distance);
           
      Int_t ntubes = wave->GetfNrOfTube();
      Int_t nPeaksCut = 0;
      ((TH1F *)histo->At(8))->Fill(wave->GetfNofGenEle());

      /*****      if (wave->GetfNofGenEle() > 1) { 
	for (int ip=0;ip<wave->GetfNofGenEle()-1;ip++) {
	  difft = 1000.*(DTime[ip+1]-DTime[ip]);
	  ((TH1F *)histo->At(10))->Fill(difft);
	  if (fabs(difft) <= 3.2) nPeaksCut++;
	}
	}*****************/

      std::sort(clsData.begin(),clsData.end());
      std::vector<Int_t> IndexMeas;
      std::vector<Bool_t> FlagAssPeak;
      std::vector<Int_t> SkippedElectrons;
      IndexMeas.push_back(0);
      FlagAssPeak.push_back(false);
      Int_t skpdEle = 0;
      for (int icls=1;icls<wave->GetfNofGenEle();icls++) {
	wClsData cls1 = clsData.at(icls-1);
	wClsData cls2 = clsData.at(icls);
	if (cls2.timeD-cls1.timeD > fTimeCut*BinReso && cls2.charge*Convfactor < -fAmplCut) {
	  difft = (clsData.at(icls)).timeD - (clsData.at(IndexMeas.back())).timeD;
	  ((TH1F *)histo->At(10))->Fill(difft);
	  IndexMeas.push_back(icls);
	  FlagAssPeak.push_back(false);
	  SkippedElectrons.push_back(skpdEle);
	  skpdEle = 0;
	}
	else skpdEle++;
      }
      SkippedElectrons.push_back(skpdEle);
      Int_t sumSkp = 0;
      for(std::vector<Int_t>::iterator itr=SkippedElectrons.begin();itr!=SkippedElectrons.end();itr++){
	sumSkp += *itr;
      }

      ((TH1F *)histo->At(12))->Fill(sumSkp);

      ((TH1F *)histo->At(11))->Fill(IndexMeas.size());
      Int_t nFndPks = findPeaks(signL,nPoints,fnrise,tmpPkHgt,tmpPkPos);

      nGoodPeaks = 0;
      nAsstedPeaks = 0;
      Int_t contapicchi = 0;
      Double_t ArrayDiff[nFndPks][IndexMeas.size()];
      for (int ip=0;ip<nFndPks;ip++) {
	Double_t rcdist = RecoDriftDistance(tmpPkPos[ip]);

	if (fabs(signL[tmpPkPos[ip]]) > NoiseThrld) {

	  ((TH1F *)histo->At(6))->Fill(fabs(signL[tmpPkPos[ip]]));
	  
	  contapicchi++;
	  if (contapicchi == 1) ((TH1F *)histo->At(3))->Fill(bPar - rcdist);
	  //if (contapicchi > 1) ((TH1F *)histo->At(4))->Fill(0.4*(tmpPkPos[ip]-tmpPkPos[ip-1]));

	  //differenza tra i tempi MC e quelli dei picchi.
	  difft = BinReso*tmpPkPos[ip] - shiftPeakTime*0.001 - 0.3; //t in us
	  //difft = BinReso*tmpPkPos[ip] - 0.3; //t in us
	  bool found = false;
	  int icls = 0;
	  Float_t diffMim = 1.e6;
	  for (;icls<IndexMeas.size();icls++) {
	    ArrayDiff[contapicchi-1][icls] = fabs(difft - (clsData.at(IndexMeas.at(icls))).timeD);
	  }
	  /*********
	  while (!found && icls < IndexMeas.size()) {
	    //	    cout<<"prova "<<difft<<" Vs "<<(clsData.at(IndexMeas.at(icls))).timeD<<endl;
		if (!FlagAssPeak.at(icls)) {
	      if (fabs(difft - (clsData.at(IndexMeas.at(icls))).timeD) < diffMim)
		diffMim = fabs(difft - (clsData.at(IndexMeas.at(icls))).timeD);
	      else {
		found = true;
		FlagAssPeak.at(icls-1) = true;
		difft -= (clsData.at(IndexMeas.at(icls-1))).timeD;
	      }
	      }
	    icls++;
	  }

	  if (found) {
	    ((TH1F *)histo->At(10))->Fill(difft);
	    nGoodPeaks++;
	  }
	  ***********/
	}
	else nFake++; 
      }

      if (nFake > 0) ((TH1F *)histo->At(17))->Fill(nFake);
      //associazione picchi
      nFndPks = contapicchi;
      Bool_t FlagAssRaws[nFndPks];
      Bool_t FlagAssCols[IndexMeas.size()];
      for (int ip=0;ip<nFndPks;ip++) FlagAssRaws[ip] = false;
      for (int icls=0;icls<IndexMeas.size();icls++) FlagAssCols[icls] = false;

      nGoodPeaks = 0;
      nAsstedPeaks = 0;
      for (int iloop=0;iloop<nFndPks;iloop++) {
	Double_t rMin = 1.e10; 
	Int_t nRw=-1,nCl=-1;
	Bool_t found = false;
	for (int ip=0;ip<nFndPks;ip++) {
	  if (FlagAssRaws[ip]) continue;
	  
	  for (int icls=0;icls<IndexMeas.size();icls++) {
	    if (FlagAssCols[icls]) continue;
	    
	    if (ArrayDiff[ip][icls] < rMin) {
	      rMin = ArrayDiff[ip][icls];
	      nRw = ip;
	      nCl = icls;
	      found = true;
	    }
	  }
	}
	
	if (found) {
	  FlagAssRaws[nRw] = true;
	  FlagAssCols[nCl] = true;
	  ((TH1F *)histo->At(14))->Fill(1000.*rMin);
	  nAsstedPeaks++;
	  if (rMin <= BinReso) nGoodPeaks++;
	}
      }

      ((TH1F *)histo->At(13))->Fill(nAsstedPeaks);
      ((TH1F *)histo->At(15))->Fill(nGoodPeaks);
      
      Float_t Ev_eff = ((float)nAsstedPeaks)/IndexMeas.size();
      ((TH1F *)histo->At(1))->Fill(contapicchi);
      ((TH1F *)histo->At(5))->Fill((Int_t)IndexMeas.size()-contapicchi);
      ((TH2F *)histo2D->At(0))->Fill((Float_t)IndexMeas.size(),(Float_t)IndexMeas.size()-contapicchi);
      if (nrPksGen > 0) {
	((TH1F *)histo->At(2))->Fill(Ev_eff);
	((TH2F *)histo2D->At(1))->Fill((Float_t)IndexMeas.size(),Ev_eff);
	Ev_eff = ((float)nGoodPeaks)/IndexMeas.size();
	((TH1F *)histo->At(16))->Fill(Ev_eff);
	((TH2F *)histo2D->At(2))->Fill((Float_t)IndexMeas.size(),Ev_eff);
      }

    }
  }
  
  ((TH1F *)histo->At(0))->SetFillStyle(3004);
  ((TH1F *)histo->At(0))->SetFillColor(kRed);
  ((TH1F *)histo->At(1))->SetLineWidth(3);

  ((TH1F *)histo->At(8))->SetLineColor(kBlue);
  ((TH1F *)histo->At(8))->SetLineWidth(3);
  ((TH1F *)histo->At(11))->SetFillStyle(3004);
  ((TH1F *)histo->At(11))->SetFillColor(kRed);
  ((TH1F *)histo->At(13))->SetFillStyle(3002);
  ((TH1F *)histo->At(13))->SetFillColor(kBlack);


  freqAq *= 1000.;
  TFile fOut(Form("PeakAnalysisAt%s_BinW%dps.root",Bw.Data(),(int)freqAq),"RECREATE");
  TCanvas *mCC = new TCanvas();
  ((TH1F *)histo->At(13))->DrawClone();
  ((TH1F *)histo->At(11))->DrawClone("same");
  ((TH1F *)histo->At(8))->DrawClone("same");
//  ((TAxis *)mgr->GetYaxis())->SetRangeUser(0.,700.);
  mCC->BuildLegend();
//mCC->SaveAs("PeaksDistrib.png");
  mCC->Write();
  
  for(int ig=0;ig<histo->GetEntries();ig++) {
    TCanvas *cc = new TCanvas();
    ((TH1F *)histo->At(ig))->DrawCopy();
    cc->Write();
  }

  for(int ig=0;ig<histo2D->GetEntries();ig++) {
    TCanvas *cc1 = new TCanvas();
    ((TH2F *)histo2D->At(ig))->DrawCopy("COLZ2");
    cc1->Write();
  }

  fOut.Write();
  fOut.Close();
}
  

  //__________________________________________________________
Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t *pkHgt, Int_t *pkPos) {

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
    sigd_2[irId]*=4.5;
    sigd[irId]*=4.5;//% stava a 5
    // better comb 4-3, 2-4
    //cout<<"iRise "<<ir<<" sgm "<<sigd[irId]<<" sgm_2 "<<sigd_2[irId]<<endl;
  }
  sig*=4.5;  //stava a 4
  //sig*=1.414;
  NoiseThrld = sig;
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
//______________________________________________________________________________________
Double_t RecoDriftDistance(Int_t nBin) {

  Double_t distance = 9999.;
  Double_t binRes = 0.0004;
  
  Float_t tmpTime = (nBin-5)*binRes - 0.3;
  
  if (tmpTime <= 0.) return distance; 
 
  distance = 10.*((double )fxt->timeToSpace(tmpTime)[0]);
//  distance[1] = 10.*((double )fxt->timeToSpace(tmpTime)[1]);
  
  return distance;

}
