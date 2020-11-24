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

struct wClsData {
  Int_t idx;
  Int_t nEl;
  Double_t driftD;
  Double_t timeD; 
  Double_t charge;
  Double_t peakAmpl;
};


Int_t findPeaks(Double_t *data, Int_t nPnts, Int_t nrise, Float_t fThrld, Float_t *pkHgt, Int_t *pkPos);
void OrdinaCluster(Int_t nCls,Double_t *driftDist);
Int_t Clusterizzazione(Int_t nGenEle, Double_t *ele_time, Double_t *cut, Bool_t flag);

//void OrdinaCluster(Int_t nCls, std::vector<wClsData > timeArr);
Double_t RecoDriftDistance(Int_t nbin);

//inizializzazione XT relation
XTREL *fxt  = NULL;

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
  XTREL *fxt = new XTRELTIME("XTData/X_T_Relation_8515.root"); 

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
  
  //histograms
  TObjArray *histo = new TObjArray();
  TObjArray *histo2D = new TObjArray();
  
  histo->Add( new TH1F("h100","Number of cluster generated; # count",51,-0.5,50.5));
  histo->Add( new TH1F("h101","Total Charge; # count",100,-0.5,99.5));
  histo->Add( new TH1F("h102","Avalanche; # e- [10^{5}]",200,0.,20.));
  histo->Add( new TH1F("h103","peak amplitude; mV",250,0.,1000.));
  histo->Add( new TH1F("h104","#Delta t  consecutive clusters; [#ns]",200,0.,200.));
  histo->Add( new TH1F("h105","#Delta t between two recognizeable peaks;  time [#mus]",200,0.,0.2));
  histo->Add( new TH1F("h106","Nr of Cls recognizable;",51,-0.5,50.5));
  histo->Add( new TH1F("h107","Number Of electrons recognizable; # count",100,-0.,99.5));
  histo->Add( new TH1F("h108","Number Of skipped electrons ; # count",51,-0.5,50.5));
  histo->Add( new TH1F("h109","Nr of peaks from peak-finder;",100,-0.5,99.5));
  histo->Add( new TH1F("h110","Number of Peaks over threshold; # count",100,-0.5,99.5));
  histo->Add( new TH1F("h111","Nr of peaks after clusterization;",51,-0.5,50.5));
  histo->Add( new TH1F("h112","#Delta t = |t_{e-} - t_{peak}|;  time [ns]",150,0.,3.));
  histo->Add( new TH1F("h113","Number of e^{-} recognized; # Cls",100,-0.5,99.5));
  histo->Add( new TH1F("h114","Number of e^{-} recognized with #deltat < BinWidth; # count",100,-0.,99.5));
  histo->Add( new TH1F("h115","t_{cls} - t_{reco}; ns",200,-50.,50) );
  histo->Add( new TH1F("h116","Nr of peaks associated to MC cluster;",51,-0.5,50.5));
  histo->Add( new TH1F("h117","Nr of peak well recognized #deltat < 5ns",51,-0.5,50.5));
  histo->Add( new TH1F("h118","(cls_{MC}-cls_{reco});",50,-25.5,24.5));
  histo->Add( new TH1F("h119","Efficiency on Nr of peaks well recognized (#deltat < BinWidth); %pecenatage",120,0.,1.2));
  histo->Add( new TH1F("h120","Efficiency on Nr of Cls well recognized ;",120,0.,1.2));
  histo->Add( new TH1F("h121","RMS noise;",100,0.,1.));
  histo->Add( new TH1F("h122","Cluster Multiplicity from MC; # count",31,-0.5,30.5));
  histo->Add( new TH1F("h123","Cluster Multiplicity from clustering; # count",31,-0.5,30.5));
  histo->Add( new TH1F("h124","Missed clusters; %",100,0.,1.));
  histo->Add( new TH1F("h125","Fake clusters; %",100,0.,1.));
  histo->Add( new TH1F("h126","(cls_{MC Recong}-cls_{reco});",50,-25.5,24.5));
  histo->Add( new TH1F("h127","fake peak amplitude; mV",275,-100.,1000.));

  histo2D->Add( new TH2F("h200","#Delta(NrCls) Vs Nr_{recog}; #count; ",51,-0.5,50.5,21,-10.5,10.5));  
  histo2D->Add( new TH2F("h201","#Peaks Efficency Vs Nr e-; #count; %percentage",100,-0.5,99.5,12,0.,1.2));
  histo2D->Add( new TH2F("h202","#Cls Efficiency Vs Nr_{recog}; #count; %percentage",51,-0.5,50.5,12,0,1.2));
  histo2D->Add( new TH2F("h203","#Deltat_{cls} Vs cls time ; us; ns",200,0.,0.2,100.,-50.,50.));
  histo2D->Add( new TH2F("h204","#Deltat_{cls} Vs cls time (Good Cls) ; us; ns",200,0.,0.2,100.,-50.,50.));
  histo2D->Add( new TH2F("h205","Amplitude fake cls Vs cls time ; us; mV",200,0.,0.2,275,-100.,1000.));


  TF1 *funGoodCls = new TF1("funGoodCls","[0]+[1]*sqrt(x)+[2]*x",0.,0.2);
  funGoodCls->SetParameters(2.33094e-01,5.63499e+00,7.21091e-01);

  TClonesArray *W_array = new TClonesArray("GMCWaveformData");
  tr->SetBranchAddress("DCWaveforms",&W_array);
  
  TClonesArray *Hits = new TClonesArray("GMCDCHHit");
  tr->SetBranchAddress("DCHits",&Hits);
  
  Int_t tmpPkPos[200];
  Float_t tmpPkHgt[200];

  Int_t nGoodPeaks = 0;
  Int_t nAsstedPeaks = 0;

  Double_t difft;

  Int_t nrClsGen, nrEleGen; 
  Bool_t fNoRMS = true;  

  for (int iev=0;iev<nEvents;iev++) {
    
    tr->GetEntry(iev);
    Int_t nrOfWaves = W_array->GetEntries();
    
    cout<<"In event "<<iev<<" --> "<<nrOfWaves<<" waveforms"<<endl;

    for (int iw=0;iw<nrOfWaves;iw++) {

      nrClsGen = ((GMCDCHHit *)Hits->At(iw))->GetfNrClusters();
      nrEleGen = (Int_t)((GMCDCHHit *)Hits->At(iw))->GetfTotalCharge();

      if (nrClsGen == -999 || nrClsGen == 0) continue;

      ((TH1F *)histo->At(0))->Fill(nrClsGen);
      Double_t *distance = new Double_t [((GMCDCHHit *)Hits->At(iw))->GetfClsDistanceSize()];

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

      //      if (fNoRMS) {
      TH1F tmpWdist1("tmpWdist1","",1000,-5.,5.);
      for (int i=10; i<500; ++i) { tmpWdist1.Fill(signL[i]); }

      NoiseThrld = fSigma*tmpWdist1.GetRMS();
      ((TH1F *)histo->At(21))->Fill(tmpWdist1.GetRMS());
	//	fNoRMS = false;
	//      }

      Double_t tmpPeaks[wave->GetfNofGenEle()];
      for (int iee=0;iee<wave->GetfNofGenEle();iee++) tmpPeaks[iee] = 0.;
      Int_t ncntpk = 0;
      for (int itime=0;itime<wave->GetfNofGenEle();itime++) {
	Bool_t fnpkg = false;
	Int_t iee = 0;
	while (!fnpkg) {
	  if (fabs(DTime[itime] - DTime[iee]) < BinReso) fnpkg = true;
	  else iee++;
	  //	  cout<<"pippo "<<DTime[itime]<<"  "<<DTime[iee]<<"  "<<fabs(DTime[itime] - DTime[iee])<<"  "<<BinReso<<"  "<<iee<<endl;

	  if (iee == itime) fnpkg = true;
	}

	if (fnpkg) {
	  if (iee < itime) tmpPeaks[itime] = tmpPeaks[iee];
	  else {
	    tmpPeaks[itime] = Amplitude[ncntpk];
	    ncntpk++;
	  }
	}
	//	cout<<"iee "<<iee<<"  "<<DTime[itime]<<"  "<<tmpPeaks[itime]<<endl;
      }


      std::vector<wClsData> clsData; 

      Double_t t_cluster[nrClsGen];
      Double_t a_cluster[nrClsGen];
      Int_t nSumE = 0;
      Int_t icls;
      for (icls=0;icls<nrClsGen;icls++) {
	distance[icls] = ((GMCDCHHit *)Hits->At(iw))->GetfClsDistanceAt(icls);
	a_cluster[icls] = 0.;
        t_cluster[icls] = fxt->spaceToTime((float)0.1*distance[icls])[0];

        if (distance[icls] >= 4.) t_cluster[icls] = (3.094-TMath::Sqrt(3.094*3.094 + 4.*3.438*(0.0345-0.1*distance[icls])))/(2.*3.438);

	Int_t nEle = ((GMCDCHHit *)Hits->At(iw))->GetfClsChargeAt(icls);
	((TH1F *)histo->At(22))->Fill(nEle);
	if (icls > 0) ((TH1F *)histo->At(4))->Fill(1000.*(DTime[nSumE-1]-DTime[nSumE]));

	for (int iee=0;iee<nEle;iee++) {
	  wClsData kCls;    
	  kCls.idx = icls;
	  kCls.nEl = iee+1;
	  kCls.driftD = distance[icls];
	  kCls.timeD = DTime[nSumE+iee];
	  kCls.charge = chrgarr[nSumE+iee]; 
	  kCls.peakAmpl = tmpPeaks[nSumE+iee];
	  //ampiezza del cluster
          ((TH1F *)histo->At(2))->Fill(1.e-5*chrgarr[nSumE+iee]);

	  if (a_cluster[icls] < Amplitude[nSumE+iee]) 
	    a_cluster[icls] = tmpPeaks[nSumE+iee];
 
	  ((TH1F *)histo->At(3))->Fill(tmpPeaks[nSumE+iee]);

	  clsData.push_back(kCls);
	}
	nSumE += nEle;
      }

      ((TH1F *)histo->At(1))->Fill(wave->GetfNofGenEle()); 


      //      OrdinaCluster(nrClsGen,t_cluster);

      std::sort(clsData.begin(),clsData.end(), WayToSort);

      Double_t t_recognizable[nrClsGen];
      Int_t nrClsRecognizable = 0;
      std::vector<Int_t> IndexMeas;
      std::vector<Bool_t> FlagAssPeak;
      std::vector<Int_t> SkippedElectrons;

      Bool_t flagReco[nrClsGen];
      for (icls=0;icls<nrClsGen;icls++) flagReco[nrClsGen] = false; 

      //      Int_t kkcls;
      Int_t skpdEle = 0;
      icls = 0;
      while (icls < clsData.size() && (clsData.at(icls)).peakAmpl < NoiseThrld){
	FlagAssPeak.push_back(false);
	icls++;
	skpdEle++;
      }
      //      cout<<"esco dal ciclo while "<<wave->GetfNofGenEle()<<endl;
      if (clsData.size() == skpdEle) continue; 

      IndexMeas.push_back(icls);
      SkippedElectrons.push_back(skpdEle);
      //      kkcls = (clsData.at(icls)).idx;
      //      t_recognizable[nrClsRecognizable] = t_cluster[kkcls];
      //      flagReco[kkcls] = true;
      //      nrClsRecognizable = 1;
      skpdEle = 0;
      icls++;
      if (icls < (int)wave->GetfNofGenEle()) {
	for (;icls<wave->GetfNofGenEle();icls++) {
	  //	  wClsData cls1 = clsData.at(icls-1);
	  wClsData cls2 = clsData.at(icls);
	  difft = cls2.timeD - (clsData.at(IndexMeas.back())).timeD;

	  if (difft > fTimeCut*BinReso && cls2.peakAmpl > NoiseThrld) {
	    
	    ((TH1F *)histo->At(5))->Fill(difft);
	    IndexMeas.push_back(icls);
	    FlagAssPeak.push_back(false);
	    SkippedElectrons.push_back(skpdEle);
	    //	    if ((clsData.at(icls)).idx != kkcls) {
	    //	      kkcls = cls2.idx;
	    //	      if (!flagReco[kkcls]) {
	    //		t_recognizable[nrClsRecognizable] = t_cluster[kkcls];
	    //		flagReco[kkcls] = true;
	    //		nrClsRecognizable += 1;
	    //	      }
	      //	    }
	    skpdEle = 0;
	  }
	  else skpdEle++;
	  
	}
	SkippedElectrons.push_back(skpdEle);
      }

      Int_t sumSkp = 0;
      for(std::vector<Int_t>::iterator itr=SkippedElectrons.begin();itr!=SkippedElectrons.end();itr++){
        sumSkp += *itr;
      }

      OrdinaCluster(nrClsGen,t_cluster);
      t_recognizable[0] = t_cluster[0];
      nrClsRecognizable++;
      for (int ip=1;ip<nrClsGen;ip++) {
	if (t_cluster[ip] - t_recognizable[nrClsRecognizable-1] > fTimeCut*BinReso) {
	  t_recognizable[nrClsRecognizable] = t_cluster[ip];
	  nrClsRecognizable++;
	}
      }

      ((TH1F *)histo->At(6))->Fill(nrClsRecognizable); 
      ((TH1F *)histo->At(7))->Fill(IndexMeas.size());
      ((TH1F *)histo->At(8))->Fill(sumSkp);

      //      OrdinaCluster(nrClsRecognizable,t_recognizable);

      //individuiamo i picchi
      Int_t nFndPks = findPeaks(signL,nPoints,fnrise,fSigma,tmpPkHgt,tmpPkPos);
      ((TH1F *)histo->At(9))->Fill(nFndPks);
 
      cout<<"mA arrivo qui  "<<nFndPks<<endl;
      Int_t contapicchi = 0;
      Double_t ArrayOfPeaksTime[nFndPks];
      Double_t ArrayOfPeaksAmpl[nFndPks];
      Double_t ArrayDiff[nFndPks][IndexMeas.size()];
      for (int ip=0;ip<nFndPks;ip++) {

	if (fabs(signL[tmpPkPos[ip]]) > NoiseThrld) {

	  ArrayOfPeaksTime[contapicchi] = (tmpPkPos[ip] - fnrise)*BinReso - 0.3;
	  ArrayOfPeaksAmpl[contapicchi] = -tmpPkHgt[ip];
	  contapicchi++;

	  //differenza tra i tempi MC e quelli dei picchi.
	  difft = BinReso*(tmpPkPos[ip]-fnrise) - 0.3; //t in us
	  bool found = false;
	  icls = 0;
	  Float_t diffMim = 1.e6;
	  for (;icls<IndexMeas.size();icls++) {
	    ArrayDiff[contapicchi-1][icls] = fabs(difft - (clsData.at(IndexMeas.at(icls))).timeD);
	  }
	} 
      }

      ((TH1F *)histo->At(10))->Fill(contapicchi);

      nFndPks = contapicchi;

      //clusterizzazione dei picchi
      Int_t ncluster = Clusterizzazione(nFndPks,ArrayOfPeaksTime,cut,0);
      for (int ip=0;ip<ncluster;ip++) ((TH1F *)histo->At(23))->Fill(clsMulti[ip]);
      ((TH1F *)histo->At(11))->Fill(ncluster);
      ((TH1F *)histo->At(18))->Fill((float)(nrClsGen - ncluster));
      ((TH1F *)histo->At(26))->Fill((float)(nrClsRecognizable - ncluster));
      ((TH2F *)histo2D->At(0))->Fill((Float_t)(nrClsRecognizable),(float)(nrClsRecognizable-ncluster));

      //associazione elettroni--picchi
      Bool_t FlagAssRaws[nFndPks];
      Bool_t FlagAssCols[IndexMeas.size()];
      for (int ip=0;ip<nFndPks;ip++) FlagAssRaws[ip] = false;
      for (icls=0;icls<IndexMeas.size();icls++) FlagAssCols[icls] = false;

      nGoodPeaks = 0;
      nAsstedPeaks = 0;
      for (int iloop=0;iloop<nFndPks;iloop++) {
	Double_t rMin = 1.e10; 
	Int_t nRw=-1,nCl=-1;
	Bool_t found = false;
	for (int ip=0;ip<nFndPks;ip++) {
	  if (FlagAssRaws[ip]) continue;
	  
	  for (icls=0;icls<IndexMeas.size();icls++) {
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
	  ((TH1F *)histo->At(12))->Fill(1000.*rMin);
	  nAsstedPeaks++;
	  if (rMin <= BinReso) nGoodPeaks++;
	}
      }

      ((TH1F *)histo->At(13))->Fill(nAsstedPeaks);
      ((TH1F *)histo->At(14))->Fill(nGoodPeaks);
      Float_t Ev_eff = ((float)nGoodPeaks)/IndexMeas.size();
      ((TH1F *)histo->At(19))->Fill(Ev_eff);
      Ev_eff = ((float)nAsstedPeaks)/IndexMeas.size();
      ((TH2F *)histo2D->At(1))->Fill((Float_t)IndexMeas.size(),Ev_eff);

      /*******************************************************/

      /***************************************
      //Associazione cluster-cluster
      Double_t ArrayClsDiff[ncluster];
      Int_t ClsAssocIdx[ncluster];
      Bool_t ClsAssociated[nrClsRecognizable]; 
      for (int iGcls=0;iGcls<nrClsRecognizable;iGcls++) ClsAssociated[iGcls] = 0;
      for (int iRcls=0;iRcls<ncluster;iRcls++) ClsAssocIdx[iRcls] = 0;
 
      for (int iRcls=0;iRcls<ncluster;iRcls++) {
	Double_t diff = 999.;
	for(int iGcls=0;iGcls<nrClsRecognizable;iGcls++) {
	  if (fabs(t_recognizable[iGcls] - clsFound[iRcls]) < diff ) {
	    diff = fabs(t_recognizable[iGcls] - clsFound[iRcls]);
	    ArrayClsDiff[iRcls] = t_recognizable[iGcls] - clsFound[iRcls];
	    ClsAssocIdx[iRcls] = iGcls;
	  }
	}

	if (ClsAssociated[ClsAssocIdx[iRcls]]) {
	  Int_t lIdx = 0;
	  while (ClsAssocIdx[lIdx] != ClsAssocIdx[iRcls]) lIdx++;
	  if (ArrayClsDiff[lIdx] < ArrayClsDiff[iRcls]) ClsAssocIdx[iRcls] = 999;
	  else ClsAssocIdx[lIdx] = 999;
	} 
	else ClsAssociated[ClsAssocIdx[iRcls]] = 1;
      }

      Int_t nrClsWellDone = 0;
      for (int iRcls=0;iRcls<ncluster;iRcls++) {
	if (ClsAssocIdx[iRcls] != 999) {
	  nrClsWellDone++;
	  ((TH1F *)histo->At(22))->Fill(1000.*ArrayClsDiff[iRcls]);
	}
      }
      ((TH1F *)histo->At(21))->Fill(nrClsWellDone);
      ((TH1F *)histo->At(23))->Fill((float)(ncluster-nrClsWellDone));

      Int_t nrClsMCUsed = 0;
      for(int iGcls=0;iGcls<nrClsRecognizable;iGcls++) {
	if (ClsAssociated[iGcls]) nrClsMCUsed++; 
      }
      ((TH1F *)histo->At(24))->Fill((float)(nrClsRecognizable-nrClsMCUsed));
      ((TH1F *)histo->At(25))->Fill(((float)nrClsWellDone)/nrClsRecognizable);
      ((TH1F *)histo->At(20))->Fill((float)(nrClsRecognizable - ncluster));
      ((TH1F *)histo->At(27))->Fill(((float)ncluster)/nrClsRecognizable);

      ((TH2F *)histo2D->At(0))->Fill((Float_t)(nrClsRecognizable),((float)(nrClsRecognizable-ncluster))/nrClsRecognizable);
      ((TH2F *)histo2D->At(1))->Fill((Float_t)(nrClsRecognizable),((float)nrClsWellDone)/nrClsRecognizable);

      ********************************/

      /***************************************************************/

      Bool_t FlagClsRaws[ncluster];
      Int_t IdGoodCluster[ncluster];
      Int_t IdFakeCluster[ncluster];
      Bool_t FlagClsCols[nrClsRecognizable];
      for (int ip=0;ip<ncluster;ip++) {
	FlagClsRaws[ip] = false;
	IdGoodCluster[ip] = IdFakeCluster[ip] = -1;
      }

      for (icls=0;icls<nrClsRecognizable;icls++) FlagClsCols[icls] = false;

      Double_t ArrayClsDiff[ncluster][nrClsRecognizable];
      for (int ip=0;ip<ncluster;ip++) {
	for (icls=0;icls<nrClsRecognizable;icls++) {
	  ArrayClsDiff[ip][icls] = fabs(t_recognizable[icls] - clsFound[ip]);
	}
      }

      nGoodPeaks = 0;
      nAsstedPeaks = 0;
      Int_t nWrongPeaks = 0;
      Int_t nMissedPeaks = 0;
      for (int iloop=0;iloop<ncluster;iloop++) {
	Double_t rMin = 1.e10; 
	Int_t nRw=-1,nCl=-1;
	Bool_t found = false;
	for (int ip=0;ip<ncluster;ip++) {
	  if (FlagClsRaws[ip]) continue;
	  
	  for (icls=0;icls<nrClsRecognizable;icls++) {
	    if (FlagClsCols[icls]) continue;
	    
	    if (ArrayClsDiff[ip][icls] < rMin) {
	      rMin = ArrayClsDiff[ip][icls];
	      nRw = ip;
	      nCl = icls;
	      found = true;
	    }
	  }
	}
	
	if (found) {
	  FlagClsRaws[nRw] = true;
	  FlagClsCols[nCl] = true;
	  ((TH1F *)histo->At(15))->Fill(1000.*(t_recognizable[nCl] - clsFound[nRw]));
	  ((TH2F *)histo2D->At(3))->Fill(t_recognizable[nCl],1000.*(t_recognizable[nCl] - clsFound[nRw]));
	  nAsstedPeaks++;
	  if (1000.*rMin < 3.*funGoodCls->Eval(t_recognizable[nCl])) {
	    IdGoodCluster[nGoodPeaks] = nRw;
	    nGoodPeaks++;      
	    ((TH2F *)histo2D->At(4))->Fill(t_recognizable[nCl],1000.*(t_recognizable[nCl] - clsFound[nRw]));
	  }
	  else {
	    IdFakeCluster[nWrongPeaks] = nRw;
	    ((TH1F *)histo->At(27))->Fill(ArrayOfPeaksAmpl[nRw]);
	    ((TH2F *)histo2D->At(5))->Fill(t_recognizable[nCl],ArrayOfPeaksAmpl[nRw]);
	    nWrongPeaks++;
	  }
	  // 	  if (rMin > 25.*BinReso) nWrongPeaks++;
	  //	  if (rMin <= 7.*BinReso) nGoodPeaks++;
	}
      }

      for (int ip=0;ip<nrClsRecognizable;ip++) 
	if (!FlagClsCols[ip]) nMissedPeaks++;
      
      ((TH1F *)histo->At(24))->Fill((float)nMissedPeaks/nrClsRecognizable);

      for (int ip=0;ip<ncluster;ip++) 
	if (!FlagClsRaws[ip]) nWrongPeaks++;

      ((TH1F *)histo->At(25))->Fill((float)nWrongPeaks/nrClsRecognizable);

      ((TH1F *)histo->At(16))->Fill(nAsstedPeaks);
      ((TH1F *)histo->At(17))->Fill(nGoodPeaks);
      Ev_eff = ((float)nGoodPeaks)/nrClsRecognizable;
      ((TH1F *)histo->At(20))->Fill(Ev_eff);
      Ev_eff = ((float)(nAsstedPeaks-nWrongPeaks))/nrClsRecognizable;
      ((TH2F *)histo2D->At(2))->Fill((Float_t)nrClsRecognizable,Ev_eff);

      /*******************************************************/

      /******************
      Float_t Ev_eff = ((float)nAsstedPeaks)/IndexMeas.size();
      ((TH1F *)histo->At(5))->Fill((Int_t)IndexMeas.size()-contapicchi);
      ((TH2F *)histo2D->At(0))->Fill((Float_t)IndexMeas.size(),(Float_t)IndexMeas.size()-contapicchi);
      if (nrClsGen > 0) {
	((TH1F *)histo->At(2))->Fill(Ev_eff);
	//
	//	((TH2F *)histo2D->At(2))->Fill((Float_t)IndexMeas.size(),Ev_eff);
      }
      *****************************/
      delete [] distance;
    }

  }

  //  ((TH1F *)histo->At(0))->SetFillStyle(3004);
  //  ((TH1F *)histo->At(0))->SetFillColor(kRed);
  ((TH1F *)histo->At(0))->SetLineWidth(3);
  ((TH1F *)histo->At(0))->SetLineColor(kBlack);

  ((TH1F *)histo->At(17))->SetLineColor(kBlue);
  ((TH1F *)histo->At(17))->SetLineWidth(3);
  ((TH1F *)histo->At(17))->SetFillStyle(3004);
  ((TH1F *)histo->At(17))->SetFillColor(kBlue);

  ((TH1F *)histo->At(11))->SetLineColor(kGreen);
  ((TH1F *)histo->At(11))->SetLineWidth(5);
  // ((TH1F *)histo->At(11))->SetFillStyle(3004);
  //  ((TH1F *)histo->At(11))->SetFillColor(kBlue);

  ((TH1F *)histo->At(13))->SetLineColor(kBlue);
  ((TH1F *)histo->At(13))->SetLineWidth(3);
  ((TH1F *)histo->At(13))->SetFillStyle(3004);
  ((TH1F *)histo->At(13))->SetFillColor(kBlue);

  ((TH1F *)histo->At(6))->SetLineColor(kRed);
  ((TH1F *)histo->At(6))->SetLineWidth(3);
  ((TH1F *)histo->At(6))->SetFillStyle(3002);
  ((TH1F *)histo->At(6))->SetFillColor(kRed);
  ((TH1F *)histo->At(7))->SetLineColor(kRed);
  ((TH1F *)histo->At(7))->SetLineWidth(3);
  ((TH1F *)histo->At(7))->SetFillStyle(3002);
  ((TH1F *)histo->At(7))->SetFillColor(kRed);


  //  freqAq *= 1000.;
  TFile fOut(Form("PeakAnalysisAt%iMHz_2GSa.root",BWidth),"RECREATE");
  TCanvas *mCC = new TCanvas();
  ((TH1F *)histo->At(6))->DrawClone();
  ((TH1F *)histo->At(17))->DrawClone("same");
  ((TH1F *)histo->At(0))->DrawClone("same");
  ((TH1F *)histo->At(11))->DrawClone("same");
//  ((TAxis *)mgr->GetYaxis())->SetRangeUser(0.,700.);
  mCC->BuildLegend();
  mCC->SaveAs("../plot19oct/ClusterDistrib.gif");
  mCC->Write();

  TCanvas *mCC1 = new TCanvas();
  ((TH1F *)histo->At(13))->DrawClone();
  ((TH1F *)histo->At(7))->DrawClone("same");
  ((TH1F *)histo->At(1))->DrawClone("same");
//  ((TAxis *)mgr->GetYaxis())->SetRangeUser(0.,700.);
  mCC1->BuildLegend();
  mCC1->SaveAs("../plot19oct/PeaksDistrib.gif");
  mCC1->Write(); 

  
  for(int ig=0;ig<histo->GetEntries();ig++) {
    TCanvas *cc = new TCanvas();
    ((TH1F *)histo->At(ig))->DrawCopy();
    cc->SaveAs(Form("../plot19oct/histo_%s.gif",((TH1F *)histo->At(ig))->GetName()));
    cc->Write();
  }
  
  for(int ig=0;ig<histo2D->GetEntries();ig++) {
    TCanvas *cc1 = new TCanvas();
    ((TH2F *)histo2D->At(ig))->DrawCopy("COLZ2");
    cc1->SaveAs(Form("../plot19oct/histo_%s.gif",((TH1F *)histo2D->At(ig))->GetName()));
    cc1->Write();
  }

  //proiezioi histo2d(203)
  TGraphErrors *aa = new TGraphErrors();
  TGraphErrors *aa_1 = new TGraphErrors();
  Int_t nbinX = ((TH2F *)histo2D->At(3))->GetNbinsX();
  TAxis *AsseX = (TAxis*)((TH2F *)histo2D->At(3))->GetXaxis();
  for (int i=1;i<nbinX-15;i++) {
    TH1D *slice = ((TH2F *)histo2D->At(3))->ProjectionY(Form("bin_%i",i),i,i);
    slice->Fit("gaus","","",-20.,20.);
    slice->Fit("gaus","","",-20.,20.);
    slice->Fit("gaus","","",-20.,20.);

    aa->SetPoint(i-1,AsseX->GetBinCenter(i),((TF1 *)slice->GetFunction("gaus"))->GetParameter(1));
    aa->SetPointError(i-1,0.5*AsseX->GetBinWidth(i),((TF1 *)slice->GetFunction("gaus"))->GetParError(1));

    aa_1->SetPoint(i-1,AsseX->GetBinCenter(i),((TF1 *)slice->GetFunction("gaus"))->GetParameter(2));
    aa_1->SetPointError(i-1,0.5*AsseX->GetBinWidth(i),((TF1 *)slice->GetFunction("gaus"))->GetParError(2));
  }

  TCanvas *cc2 = new TCanvas();
  aa->Draw("ap");
  ((TAxis*)aa->GetYaxis())->SetRangeUser(-1.,5.);
  aa->SetMarkerStyle(22);
  aa->SetMarkerColor(kBlue);
  aa->SetTitle("Mean #DeltaCls; t_{cls} in #mus; #Delta t in ns");
  //  cc2->Write();


  //  TCanvas *cc3 = new TCanvas();
  aa_1->SetMarkerStyle(23);
  aa_1->SetMarkerColor(kRed);
  aa_1->Draw("psame");
  cc2->BuildLegend();
  aa_1->SetTitle("#sigma from fit #DeltaCls");
  cc2->Write();

  fOut.Write();
  fOut.Close();


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

  cout<<"*************  entro in raggruppa ******** "<<endl;

  //azzero il vettore temporaneo dei tempi
  for (int k=0; k<100; k++) {
    clsFound[k] = 0.;
    clsMulti[k] = 0;
  }

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
