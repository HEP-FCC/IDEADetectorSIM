#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMultiGraph.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TH2I.h"
#include "TMath.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "util/XTRELTIME.h"
#include "util/Geometry.h"
#include "generated/GMCWaveformData.h"
#include "generated/GMCDCHHit.h"
#include "generated/GMCAnalyzer.h"
#include "generated/GMCTGenerateWaveforms_Base.h"
#include "GMCAnalyzerConfigParameters.h"
#include "Riostream.h"

#include "../macros/RecWave.h"


void Ordina(int np, double *dist);
Double_t MPSAlgorithm(Int_t ncls, Double_t *distance, Bool_t flag);

//inizializzazione XT relation
XTREL *fxt  = NULL;

//inizializzazione geometria
Geometry *fGeometry = Geometry::GetInstance();

void ClusterTiming(Int_t nrUser=0) {

  //inizializzazione XT relation
  XTREL *fxt = new XTRELTIME("XTData/X_T_Relation_8515.root"); 
  
  //  TFile f("PeakFound00001.root");  
  //  TTree *tr = (TTree *)f.Get("RecoData");
  
  TChain *tr = new TChain("RecoData");
  for (int iFile=1;iFile<5;iFile++) {
    tr->Add(Form("PeakFound%05i.root",iFile));
  }

  Int_t nEvents = tr->GetEntries(); // numero di eventi
  
  Double_t radius = 0.5*fGeometry->GetTubeSize();

  if (nrUser > 0) nEvents = nrUser;
  cout<<"Number of events Stored --> "<<nEvents<<endl;

  ofstream DebugEv("DebugFile.txt",ios::out);

  TH1F *deltab = new TH1F("h001","b_{rec} - b_{true}",200.,-5.,5.);
  TH1F *deltab1 = new TH1F("h011","b_{1cls} - b_{true}",200.,-5.,5.);

  TH2F *scatter = new TH2F("h002","#Deltab Vs b_{true}",200,0.,5.,200,-5.,5.);

  TH2F *scatter_1 = new TH2F("h003","b_{1cls}-b_{true} Vs b_{true}",200,0.,5.,200,-5.,5.);
  TH2F *scatter_2 = new TH2F("h004","b^{*}-b_{true} Vs b_{true}",200,0.,5.,200,-5.,5.);
  TH2F *scatter_3 = new TH2F("h005","b_{1cls MC}-b_{true} Vs b_{true}",200,0.,5.,200,-5.,5.);

  Int_t IdEvent;
  tr->SetBranchAddress("IdEvent",&IdEvent);
  
  MCWave data_MC;
  tr->SetBranchAddress("NrTube",&data_MC.NrTube);
  tr->SetBranchAddress("IdTrk",&data_MC.IdTrk);
  tr->SetBranchAddress("TotCharge",&data_MC.TotCharge);
  tr->SetBranchAddress("NrCls",&data_MC.NrCls);
  tr->SetBranchAddress("bTrue",&data_MC.bTrue);
  tr->SetBranchAddress("DriftD",&data_MC.DriftD);
  tr->SetBranchAddress("ClsMulti",&data_MC.ClsMulti);
  tr->SetBranchAddress("EleTime",&data_MC.EleTime);
  tr->SetBranchAddress("EleCharge",&data_MC.EleCharge);
  
  RecWave data_REC;
  tr->SetBranchAddress("NrOfPeaks",&data_REC.NrOfPeaks);
  tr->SetBranchAddress("NrClsRec",&data_REC.NrClsRec);
  tr->SetBranchAddress("ClsMultiRec",&data_REC.ClsMultiRec);
  tr->SetBranchAddress("PeakTime",&data_REC.PeakTime);
  tr->SetBranchAddress("PeakAmplitude",&data_REC.PeakAmplitude);
  tr->SetBranchAddress("ClsTimeRec",&data_REC.ClsTimeRec);
  
  bool flag = 0;
  for (int ie=0; ie<nEvents; ie++) {
    
    //selezione l'evento ie
    tr->GetEntry(ie);  
    //    cout<<"****************** evento numero ****************"<<ie<<endl;
    
    //    cout<<"ho trovato quanti cls_rec/cls_MC "<<data_REC.NrClsRec<<"/"<<data_MC.NrCls<<endl;
    if (data_REC.NrClsRec == 0) continue;
 
    if (data_MC.IdTrk != 1) continue;

    Double_t smearing = 99.;
    Double_t MCDrift[data_MC.NrCls+1];
    for (int ih=0;ih<data_MC.NrCls;ih++) {
      MCDrift[ih] = data_MC.DriftD[ih];
      //      cout<<">>>>>>>> "<<MCDrift[ih]<<endl;
      smearing = 99.;
      while(MCDrift[ih]+smearing > radius) smearing = gRandom->Gaus(0,0.160*sqrt(MCDrift[ih]*0.1));
      MCDrift[ih] += smearing;
    }

    MCDrift[data_MC.NrCls] = radius;
    Ordina(data_MC.NrCls+1,MCDrift);

    Double_t parB = MPSAlgorithm(data_MC.NrCls+1,MCDrift,0);
    scatter_2->Fill(data_MC.bTrue,parB-data_MC.bTrue);
    scatter_3->Fill(data_MC.bTrue,MCDrift[0]-data_MC.bTrue);


    Double_t distance[data_REC.NrClsRec+1];
    //____________ciclo sugli hit______________//
    for (int ih=0; ih<data_REC.NrClsRec; ih++) {
      
      //      cout<<"cls nr "<<ih+1<<"  time = "<<data_REC.ClsTimeRec[ih]<<endl;
      
      //convert time2distance
      Double_t time = data_REC.ClsTimeRec[ih];
      distance[ih] = 10.*fxt->timeToSpace((Float_t)time)[0]; //to have mm
      
      if (fabs(distance[ih] - 4.) < 0.00001) 
	distance[ih] = 10.*(0.0345 - 3.438*time*time + 3.094*time);
      
    }

    distance[data_REC.NrClsRec] = radius;

    scatter_1->Fill(data_MC.bTrue,distance[0]-data_MC.bTrue);
    deltab1->Fill(distance[0]-data_MC.bTrue);

    if (IdEvent == 20 && data_MC.NrTube == 5) flag = 1;
    else flag = 0;
    Double_t fImpact = MPSAlgorithm(data_REC.NrClsRec+1,distance,0);


    //    if (fImpact-data_MC.bTrue < -1.) return;
    deltab->Fill(fImpact-data_MC.bTrue);
    scatter->Fill(data_MC.bTrue,fImpact-data_MC.bTrue);

    if (fImpact - data_MC.bTrue > 1 &&  data_MC.bTrue < 10.8) {
      DebugEv<<"******* evento numero *****"<<IdEvent<<" **tube **"<<data_MC.NrTube<<endl;
      DebugEv<<"ho trovato quanti cls_rec/cls_MC "<<data_REC.NrClsRec<<"/"<<data_MC.NrCls<<endl;
      for (int ih=0; ih<data_REC.NrClsRec; ih++) {
	DebugEv<<"cls nr "<<ih+1<<"  time = "<<data_REC.ClsTimeRec[ih]<<"  rec d "<<distance[ih];
	if (ih < data_MC.NrCls) DebugEv<<"  MC d "<<MCDrift[ih]<<endl;
      }
      DebugEv<<IdEvent<<"  "<<data_MC.NrTube<<" controllo b_true = "<<data_MC.bTrue<<" ==> b_rec = "<<fImpact<<" Delat = "<<fImpact-data_MC.bTrue<<"   prima distanza "<<distance[0]<<endl;

    }
    /*********************************
     
     //___________ISTOGRAMMI___________//
     histo3->Fill(d_drift[1]); // prima distanza di drift
     histo4->Fill(b_ottimo); // b MPS
     histo5->Fill(b_vero-d_drift[1]);  // e d[1]
     histo6->Fill(b_vero-b_ottimo);  // e MPS
     **********************************/     
   }


  DebugEv.close();

 //__________CANVAS_______//
 TCanvas *c1 = new TCanvas();
 deltab->DrawCopy(); 
 TCanvas *c1_r = new TCanvas();
 deltab1->DrawCopy(); 

 TCanvas *c2 = new TCanvas();
 scatter->DrawCopy();
 TCanvas *c3 = new TCanvas();
 scatter_1->DrawCopy();


 TGraphErrors *bias = new TGraphErrors();
 TGraphErrors *b_rms = new TGraphErrors();
 TAxis *Assex = scatter->GetXaxis();

 cout<<"quanti bin trovo??? "<<Assex->GetNbins()<<endl;

 for (int ib=0;ib<(Assex->GetNbins()-20)/2.;ib++) {

   TH1D *py = scatter->ProjectionY(Form("_py%i",ib),ib*2,ib*2+1);

   //   py->Fit("gaus");
   //   bias->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetMean());
   bias->SetPoint(ib,ib*0.05+0.025,py->GetMean());
   //   bias->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetMeanError());
   bias->SetPointError(ib,0.025,py->GetMeanError());

   //   b_rms->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetRMS());
   b_rms->SetPoint(ib,ib*0.05+0.025,py->GetRMS());
   //   b_rms->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetRMSError());
   b_rms->SetPointError(ib,0.025,py->GetRMSError());

 }  
 
 TGraphErrors *bias_1 = new TGraphErrors();
 TGraphErrors *b_rms_1 = new TGraphErrors();
 for (int ib=0;ib<(Assex->GetNbins()-20)/2.;ib++) {

   TH1D *py = scatter_1->ProjectionY(Form("_py%i",ib),ib*2,ib*2+1);

   //   py->Fit("gaus");
   //   bias->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetMean());
   bias_1->SetPoint(ib,ib*0.05+0.025,py->GetMean());
   //   bias->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetMeanError());
   bias_1->SetPointError(ib,0.025,py->GetMeanError());

   //   b_rms->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetRMS());
   b_rms_1->SetPoint(ib,ib*0.05+0.025,py->GetRMS());
   //   b_rms->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetRMSError());
   b_rms_1->SetPointError(ib,0.025,py->GetRMSError());

 }  

 TGraphErrors *bias_2 = new TGraphErrors();
 TGraphErrors *b_rms_2 = new TGraphErrors();
 for (int ib=0;ib<(Assex->GetNbins()-20)/2.;ib++) {

   TH1D *py = scatter_2->ProjectionY(Form("_py%i",ib),ib*2,ib*2+1);

   //   py->Fit("gaus");
   //   bias->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetMean());
   bias_2->SetPoint(ib,ib*0.05+0.025,py->GetMean());
   //   bias->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetMeanError());
   bias_2->SetPointError(ib,0.025,py->GetMeanError());

   //   b_rms->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetRMS());
   b_rms_2->SetPoint(ib,ib*0.05+0.025,py->GetRMS());
   //   b_rms->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetRMSError());
   b_rms_2->SetPointError(ib,0.025,py->GetRMSError());

 }  


 TGraphErrors *bias_3 = new TGraphErrors();
 TGraphErrors *b_rms_3 = new TGraphErrors();
 for (int ib=0;ib<(Assex->GetNbins()-20)/2.;ib++) {

   TH1D *py = scatter_3->ProjectionY(Form("_py%i",ib),ib*2,ib*2+1);

   //   py->Fit("gaus");
   //   bias->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetMean());
   bias_3->SetPoint(ib,ib*0.05+0.025,py->GetMean());
   //   bias->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetMeanError());
   bias_3->SetPointError(ib,0.025,py->GetMeanError());

   //   b_rms->SetPoint(ib-1,((TAxis *)scatter->GetXaxis())->GetBinCenter(ib),py->GetRMS());
   b_rms_3->SetPoint(ib,ib*0.05+0.025,py->GetRMS());
   //   b_rms->SetPointError(ib-1,0.5*((TAxis *)scatter->GetXaxis())->GetBinWidth(ib),py->GetRMSError());
   b_rms_3->SetPointError(ib,0.025,py->GetRMSError());

 }  

 TCanvas *c5 = new TCanvas();
 bias->SetMarkerStyle(23);
 bias->Draw("ap");
 bias_1->SetMarkerStyle(23);
 bias_1->SetMarkerColor(kBlue);
 bias_1->Draw("psame");
 bias_2->SetMarkerStyle(23);
 bias_2->SetMarkerColor(kGreen);
 bias_2->Draw("psame");
 bias_3->SetMarkerStyle(23);
 bias_3->SetMarkerColor(kRed);
 bias_3->Draw("psame");

 TCanvas *c4 = new TCanvas();
 b_rms->SetMarkerStyle(24);
 b_rms->Draw("ap");
 b_rms_1->SetMarkerStyle(24);
 b_rms_1->SetMarkerColor(kBlue);
 b_rms_1->Draw("psame");
 b_rms_2->SetMarkerStyle(24);
 b_rms_2->SetMarkerColor(kGreen);
 b_rms_2->Draw("psame");
 b_rms_3->SetMarkerStyle(24);
 b_rms_3->SetMarkerColor(kRed);
 b_rms_3->Draw("psame");

}


//________funzione che ordina le distanze di drift________//
void Ordina(int np, double *dist){
    double tmp_d[100];
    for(int i=0; i<np; i++){
        tmp_d[i]=TMath::MinElement(np,dist);
        for (int k=0; k<np; k++){
            if(tmp_d[i] == dist[k]) dist[k] = 999.;
        }
    }
    for (int h=0; h<np; h++){
        dist[h]=tmp_d[h];
        //cout<<"vettore ordinato["<<h<<"] = "<<tmp_d[h]<<endl;
    }
}

//_________________________________________________________//
Double_t MPSAlgorithm(Int_t ncls, Double_t *distance, Bool_t fDebug) {

  Double_t step = 0.005;
  Double_t b = 0.02 - step;
  Double_t funH, paramD;

  Double_t compareH = -999999.; 
  Bool_t found = false;
  Bool_t iCycle = false;

  Int_t istep = 0;

  if (ncls == 2) b = distance[0];  
  else {
    while (!found) {
      
      b += step;
      
      if (b >= distance[0]) break;
      
      funH = 0.;
      paramD = TMath::Sqrt(distance[ncls-1]*distance[ncls-1] - b*b); 
      
      funH = TMath::Log(TMath::Sqrt(distance[0]*distance[0] - b*b)/paramD);
      
      for (int j=1; j<ncls; j++) {
	
	Double_t rad1 = TMath::Sqrt(distance[j]*distance[j] - b*b);
	Double_t rad2 = TMath::Sqrt(distance[j-1]*distance[j-1] - b*b);
	
	
	funH += TMath::Log((rad1 - rad2)/paramD);
	
      }
      
      funH /= ncls;
      if (!iCycle && funH < compareH ) {
	found = true;
      	b -= step;
	if (istep == 1) {
	  iCycle = true;
	  b = 0.02 - step;
	  istep = -1;
	  found = false;
	}
      }

      if (iCycle && istep > 0 && (funH - compareH) <= -0.001) {
      	found = true;
	//	cout<<"istep "<<istep<<" b = "<<b<<"  "<<compareH<<"  "<<funH<<endl;    
      	b -= step;
      }
      istep++;
      compareH = funH;
    }
    
  }

  //debug
  if (fDebug) {

    TGraph *aaa = new TGraph();
    found = false;
    Float_t b1 = 0.015;
    compareH = 0.;
    istep = 0;
    while (!found) {
      
      b1 += step;
      
      if (b1 >= distance[0]) break;
      
      funH = 0.;
      paramD = TMath::Sqrt(distance[ncls-1]*distance[ncls-1] - b1*b1); 
      
      funH = TMath::Log(TMath::Sqrt(distance[0]*distance[0] - b1*b1)/paramD);
      
      for (int j=1; j<ncls; j++) {
	
	Double_t rad1 = TMath::Sqrt(distance[j]*distance[j] - b1*b1);
	Double_t rad2 = TMath::Sqrt(distance[j-1]*distance[j-1] - b1*b1);
	
	
	funH += TMath::Log((rad1 - rad2)/paramD);
	
      }
      
      funH /= ncls;
      if (istep > 0 && funH < compareH ) {
	cout<<"dovrei breakkare in questo punto"<<endl;
      }
      cout<<"istep "<<istep<<" b = "<<b1<<"  "<<"  "<<funH<<"  "<<funH-compareH<<endl;    
      compareH = funH;
      aaa->SetPoint(istep,b1,funH);
      istep++;
    }
    TCanvas *myC1 = new TCanvas();
    aaa->Draw("ap*");

  }

  //  if (b > distance[0]) b = distance[0];
  /**********
  ************/
  return b;

}




