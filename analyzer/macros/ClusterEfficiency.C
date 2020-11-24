#include "Riostream.h"
#include "TList.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TMarker.h"
#include "TObjString.h"
#include "TString.h"
#include "TLegend.h"
#include "TStyle.h"

#include "generated/GMCWaveformData.h"
#include "generated/GMCDCHHit.h"
#include "generated/GMCAnalyzer.h"
#include "generated/GMCTGenerateWaveforms_Base.h"
#include "util/XTRELTIME.h"
#include "util/Geometry.h"



int CluRag(int nGenEle, double *ele_time, double *cut, Bool_t flag);
void Ordina(int np, double *temp);

const int n_tau=30;
const int nfact = 2;
const int n_slice = nfact*18;
const Double_t t_cut=0.2;  // 0.2 ns
const Double_t slice_width=180./n_slice;  

Int_t clsMulti[100];

void ClusterEff(Int_t nrUser=0, Double_t maxTempo=150., Bool_t FlgMtime=false) {

  //inizializzazione XT relation
  XTREL *fxt = new XTRELTIME("XTData/X_T_Relation_8515.root"); 
  
  //______________istogrammi_______________//

  TObjArray *CntXHisto = new TObjArray();
  CntXHisto->Add( new TH1F("1","MC clusters",50,-0.5,49.5));
  CntXHisto->Add( new TH1F("2","MC clusters @ cm",50,-0.5,49.5));
  CntXHisto->Add( new TH1F("3","Nr of e- in each cluster; # nr of e- ",51,-0.5,50.5));
  CntXHisto->Add( new TH1F("4",Form("MC clusters with cut @ %i ns",(int)maxTempo),50,-0.5,49.5));
  CntXHisto->Add( new TH1F("5","Counted clusters",50,-0.5,49.5));
  CntXHisto->Add( new TH1F("6","Cls_{MC}-Cls_{counted} (cut)",21,-10.5,10.5));
  CntXHisto->Add( new TH1F("7","ClsTime_{MC}-ClsTime_{reco e-}; ns",200,-20.,20.));
  CntXHisto->Add( new TH1F("8","ClsTime_{MC}-ClsTime_{first e-}; ns",200,-20.,20.));
  CntXHisto->Add( new TH1F("9","ClsTime_{MC}-ClsTime_{e-}; ns",200,-20.,20.));
  CntXHisto->Add( new TH1F("10",Form("Cls_{MC}-Cls_{counted} @ t_{max = %i}",(int)maxTempo),21,-10.5,10.5));
  CntXHisto->Add( new TH1F("11","Cls_{counted} / Cls_{MC}",100,0.,2.));
  CntXHisto->Add( new TH1F("12","Rejected Cls",20,-0.5,19.5));
  CntXHisto->Add( new TH1F("13","% of rejected Cls",100,0.,1.));
  CntXHisto->Add( new TH1F("14","Cls_{MC}-Cls_{counted} (tau)",31,-10.5,20.5));
  CntXHisto->Add( new TH1F("15","Cls_{MC}-Cls_{counted} (diff)",31,-10.5,20.5));
  CntXHisto->Add( new TH1F("16","Clusters Multiplicity",51,-0.5,50.5));
  CntXHisto->Add( new TH1F("17","(Cls_{MC}-Cls_{counted})/Cls_{MC}",100,-1.,1.));

  Int_t Hthres = CntXHisto->GetEntries();

  for (int i=0;i<n_slice;i++) {
    CntXHisto->Add( new TH1F(Form("cut_%i",i),Form("cls_{MC}-cls_{countd} in slice %i using cut",i),21,-10.5,10.5));
    //    CntXHisto->Add( new TH1F(Form("slice_%i",i),Form("MC cls in slice %i",i),50,-0.5,49.5));
  }

  for (int i=0;i<n_slice;i++) {
    CntXHisto->Add( new TH1F(Form("tau_%i",i),Form("cls_{MC}-cls_{countd} in slice %i usig tau",i),21,-10.5,10.5));
    //    CntXHisto->Add( new TH1F(Form("slcn_%i",i),Form("cls counted in slice %i",i),50,-0.5,49.5));
  }

  for (int i=0;i<n_slice;i++) {
    CntXHisto->Add( new TH1F(Form("diff_%i",i),Form("cls_{MC}-cls_{countd} in slice %i using diffusion",i),21,-10.5,10.5));
  }

  
  TH2F *scatt1 = new TH2F("201","#Delta Cls Vs Cls_{MC}",50,0.5,50.5,51,-25.5,25.5);
  TH2F *scatt2 = new TH2F("202","#Delta Cls Vs b",100,0.,5.,21,-10.5,10.5);
  TH2F *scatt3 = new TH2F("203","Nr Cls_{MC} Vs b",100,0.,5.,50,-0.5,49.5);
  TH2F *scatt4 = new TH2F("204","#Delta t Vs t_{cls}",100,0.,0.2,200,-20.,20.);

  TProfile *pr_1 = new TProfile("pr_1","#Delta Cls Vs b",100,0.,5.);
  TProfile *pr_2 = new TProfile("pr_2","Nr of Cls_{MC} Vs b",100,0.,5.);
  TProfile *pr_3 = new TProfile("pr_3","#Delta Cls Vs Cls_{MC}",50,0.,50.);
  TProfile *pr_4 = new TProfile("pr_4","#Delta t Vs t_{cls}",100,0.,0.2);


  //_____________leggo il file di input contenente i valori dei tagli_____//
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
  
  //  return;
  TChain *tr = new TChain("MCHits");
  TString filename;
  for (int i=0;i<30;i++) {
    filename = TString(Form("../Simdata/MCHits800MHz_g4_SN7_2GSa_%03i.root",i+23));
    tr->Add(filename.Data());
  }
  
  Int_t nEvents = tr->GetEntries(); // numero di eventi
  if (nrUser > 0) nEvents = nrUser;
  
  Double_t cluster_veri_slice[n_slice];
    
  //_____hit selection___________//
  TClonesArray *hits = new TClonesArray("GMCDCHHit");
  tr->SetBranchAddress("DCHits",&hits);
  
  //______waveform selection______//
  TClonesArray *wave = new TClonesArray("GMCWaveformData");
  tr->SetBranchAddress("DCWaveforms",&wave);
  
  cout<<"Number of events Stored --> "<<nEvents<<endl;

  Int_t nFill = 0;
  for (int ie=0;ie<nEvents;ie++) {

    //_________event selection______________//
    tr->GetEntry(ie);  

    Int_t nhits = hits->GetEntries();

    Int_t n_wave = wave->GetEntries();  //numero di waveform
    if (ie%1000 == 0) cout<<"******** evento numero ***********"<<ie<<endl;
    
    if (nhits == 0) continue;
    
    //____________wave selection and cycle________________//
    for (int j=0; j<nhits; j++){
      
      GMCDCHHit *ahit = (GMCDCHHit *)hits->At(j);
      GMCWaveformData *wave_data = (GMCWaveformData *)wave->At(j);  
      
      Int_t nclust = ahit->GetfNrClusters(); // numero di cluster generati

      if (nclust == 0 || nclust == -999 || nclust > 100) continue;

      Double_t bTrue = ahit->GetfImpact();
      Double_t trkLen = ahit->GetfLength();
      
      ((TH1F *)CntXHisto->At(0))->Fill(nclust);
      ((TH1F *)CntXHisto->At(1))->Fill(nclust/(0.1*trkLen));

      scatt3->Fill(bTrue,nclust);
      pr_2->Fill(bTrue,nclust);

      Int_t nGenEle = 0;
      nGenEle = wave_data->GetfNofGenEle();

      //vettore dei tempi di drift delle waveform
      Double_t *time_wave_MC = wave_data->GetfDTimeEle();  
      Double_t tmpDeiTempi[nGenEle];
      for (int kel=0;kel<nGenEle;kel++) tmpDeiTempi[kel] = time_wave_MC[kel];  

      // vettore dei tempi del primo elettrone di ogni cluster
      Double_t t_cluster[nclust], d_drift[nclust]; 
      Int_t nOffset = 0;
      for (int icls=0; icls<nclust; icls++) {
	d_drift[icls] = ahit->GetfClsDistanceAt(icls);

	t_cluster[icls] = fxt->spaceToTime((float)0.1*d_drift[icls])[0];
	if (d_drift[icls] >= 4.) t_cluster[icls] = (3.094-TMath::Sqrt(3.094*3.094 + 4.*3.438*(0.0345-0.1*d_drift[icls])))/(2.*3.438);

	if ((int)ahit->GetfClsChargeAt(icls) == 1) {
	  ((TH1F *)CntXHisto->At(8))->Fill(1000.*(t_cluster[icls]-tmpDeiTempi[nOffset]));
	  //	  cout<<"----> icls "<<nFill<<"  "<<t_cluster[icls]<<"  "<<tmpDeiTempi[nOffset]<<"  "<<1000.*(t_cluster[icls]-tmpDeiTempi[nOffset])<<endl;
	  nFill++;
	}
	
	nOffset += ahit->GetfClsChargeAt(icls);
      }


      Int_t count_clsMC[n_slice];
      Int_t count_cls[n_slice];
      for (int k=0; k<n_slice; k++) count_clsMC[k] = count_cls[k] = 0;
      
      Int_t nMCClustMaxT = 0;

      Int_t n_elett = 0;
      Int_t idxOfCls[nGenEle];
      for (int l=0;l<nclust;l++) {
	
	Int_t num_elect_cls = ahit->GetfClsChargeAt(l); 
	((TH1F *)CntXHisto->At(2))->Fill(num_elect_cls);
	Double_t timeFirstE = time_wave_MC[n_elett];
	for (int ind=0;ind<num_elect_cls;ind++) {
	  idxOfCls[n_elett+ind] = l;
	  if (timeFirstE > time_wave_MC[n_elett+ind]) timeFirstE = time_wave_MC[n_elett+ind];
	}
	//	cout<<"pippo "<<t_cluster[l]<<"   "<<timeFirstE<<"  "<<1000.*(t_cluster[l]-timeFirstE)<<endl;
	((TH1F *)CntXHisto->At(7))->Fill(1000.*(t_cluster[l]-timeFirstE));

	n_elett += num_elect_cls;
      }
      

      //ordino i tempi dei cluster
      Ordina(nclust, t_cluster);

      //per conto i cls per slice
      for (int l=0; l<nclust; l++){ 

	Int_t is = (int)(1000.*t_cluster[l]/slice_width);
	if (is < n_slice) count_clsMC[is]++;
	if (1000.*t_cluster[l] < maxTempo) nMCClustMaxT++; 

      }      
      ((TH1F *)CntXHisto->At(3))->Fill(nMCClustMaxT);
      ((TH1F *)CntXHisto->At(11))->Fill(nclust-nMCClustMaxT);
	
      ((TH1F *)CntXHisto->At(12))->Fill(((float)(nclust-nMCClustMaxT))/nclust);

      //      for (int l=0; l<n_slice;l++) ((TH1F *)CntXHisto->At(Hthres+l))->Fill(count_clsMC[l]);

      //**************************************************************//
      // ordinamento dei tempi di drift degli elettroni
      Ordina(nGenEle, time_wave_MC);
	
      Int_t cluster_contati = CluRag(nGenEle, time_wave_MC, cut, FlgMtime);
      ((TH1F *)CntXHisto->At(4))->Fill(cluster_contati);

      ((TH1F *)CntXHisto->At(10))->Fill(((float)cluster_contati)/nclust);

      scatt1->Fill(nclust,nclust-cluster_contati);
      scatt2->Fill(bTrue,nclust-cluster_contati);

      //Unfolding dei cluster trovati
      //      cout<<"******* cls contati "<<cluster_contati<<"/"<<nclust<<endl;
      for (int kel=0;kel<cluster_contati;kel++) {

	if (clsMulti[kel] == 0) cout<<kel<<"  "<<cluster_contati<<" "<<nclust<<endl;
	((TH1F *)CntXHisto->At(15))->Fill(clsMulti[kel]);

	//dopo la chiamata a CluRag time_wave_MC contiene i tempi dei cls raggr
	Int_t is = (int)(1000.*time_wave_MC[kel]/slice_width);
	if (is < n_slice) count_cls[is]++;

	//facciamo la differenza entro il massimo tempo consentito
	if (1000.*time_wave_MC[kel] < maxTempo) nMCClustMaxT--; 

	//confronto con il tempo di drift del cls a cui e- appartiene
	double dtime = 999.;
	int nxxx=0;
 	for (int lkel=0;lkel<nclust;lkel++) {
	  if (fabs(t_cluster[lkel]-time_wave_MC[kel]) < fabs(dtime)) {
	    dtime = t_cluster[lkel]-time_wave_MC[kel];
	    nxxx = lkel;
	  } 
	}
	((TH1F *)CntXHisto->At(6))->Fill(1000.*dtime);
	scatt4->Fill(t_cluster[nxxx],1000.*dtime);
	pr_4->Fill(t_cluster[nxxx],1000.*dtime);

	/****
	Int_t icls;
	int iii=0;
	Bool_t found = false;
	while(!found) {
	  //	  cout<<">>>>>>>> "<<time_wave_MC[kel]<<"  "<<tmpDeiTempi[iii]<<endl;
	  if (time_wave_MC[kel] == tmpDeiTempi[iii]) found = true;
	  iii++;
	}
	icls = idxOfCls[iii-1];
	//	cout<<"pippo "<<icls<<"  "<<iii-1<<"  "<<t_cluster[icls]<<"   "<<time_wave_MC[kel]<<"  "<<1000.*(t_cluster[icls]-time_wave_MC[kel])<<endl;
	((TH1F *)CntXHisto->At(6))->Fill(1000.*(t_cluster[icls]-time_wave_MC[kel]));
	***************/

      }

      ((TH1F *)CntXHisto->At(5))->Fill(nclust-cluster_contati);
      ((TH1F *)CntXHisto->At(16))->Fill((float)(nclust-cluster_contati)/nclust);
      ((TH1F *)CntXHisto->At(9))->Fill(nMCClustMaxT);
      pr_1->Fill(bTrue,nclust-cluster_contati);
      pr_3->Fill(nclust,nclust-cluster_contati);
      scatt1->Fill(nclust,nclust-cluster_contati);
      scatt2->Fill(bTrue,nclust-cluster_contati);

      for (int l=0; l<n_slice;l++) {
	//	((TH1F *)CntXHisto->At(Hthres+n_slice+l))->Fill(count_cls[l]);
	if (count_cls[l] != 0 || count_clsMC[l] != 0) ((TH1F *)CntXHisto->At(Hthres+l))->Fill(count_clsMC[l]-count_cls[l]);
      }

      //rifaccio il calcolo dei cluster usando il tau
      for (int kel=0;kel<nGenEle;kel++) time_wave_MC[kel] = tmpDeiTempi[kel];  
      for (int k=0; k<n_slice; k++) count_cls[k] = 0;
      Ordina(nGenEle, time_wave_MC);

      cluster_contati = CluRag(nGenEle, time_wave_MC, tau, FlgMtime);
      for (int kel=0;kel<cluster_contati;kel++) {
	//dopo la chiamata a CluRag time_wave_MC contiene i tempi dei cls raggr
	Int_t is = (int)(1000.*time_wave_MC[kel]/slice_width);
	if (is < n_slice) count_cls[is]++;
      }

      ((TH1F *)CntXHisto->At(13))->Fill(nclust-cluster_contati);

      for (int l=0; l<n_slice;l++) {
	//	((TH1F *)CntXHisto->At(Hthres+n_slice+l))->Fill(count_cls[l]);
	if (count_cls[l] != 0 || count_clsMC[l] != 0) ((TH1F *)CntXHisto->At(Hthres+n_slice+l))->Fill(count_clsMC[l]-count_cls[l]);
      }



      //rifaccio il calcolo dei cluster usando la diffusione
      for (int kel=0;kel<nGenEle;kel++) time_wave_MC[kel] = tmpDeiTempi[kel];  
      for (int k=0; k<n_slice; k++) count_cls[k] = 0;
      Ordina(nGenEle, time_wave_MC);

      cluster_contati = CluRag(nGenEle, time_wave_MC, diffusion, FlgMtime);
      for (int kel=0;kel<cluster_contati;kel++) {
	//dopo la chiamata a CluRag time_wave_MC contiene i tempi dei cls raggr
	Int_t is = (int)(1000.*time_wave_MC[kel]/slice_width);
	if (is < n_slice) count_cls[is]++;
      }

      ((TH1F *)CntXHisto->At(14))->Fill(nclust-cluster_contati);

      for (int l=0; l<n_slice;l++) {
	//	((TH1F *)CntXHisto->At(Hthres+n_slice+l))->Fill(count_cls[l]);
	if (count_cls[l] != 0 || count_clsMC[l] != 0) ((TH1F *)CntXHisto->At(Hthres+2.*n_slice+l))->Fill(count_clsMC[l]-count_cls[l]);
      }

    }
  }

  //stampiamo gli histogrammi
  TFile *fOut = new TFile("EfficiencyPlot.root","RECREATE");

  Int_t nHistoSum = 0;

  TCanvas *CntXCanv[200];
  for (int i=0;i<Hthres;i++) {
    CntXCanv[i] = new TCanvas();
    ((TH1F *)CntXHisto->At(i))->DrawCopy();
  }
  nHistoSum += Hthres;

  Int_t nslXcanv = n_slice/nfact;
  Int_t h_col = TMath::Nint(sqrt(nslXcanv));
  Int_t h_line;
  if (nslXcanv < h_col*h_col) h_line = h_col-1;
  else if (nslXcanv > h_col*h_col){
    h_line = h_col;
    h_col++;
  }
  else h_line = h_col;

  gStyle->SetOptFit();

  TGraphErrors *grMeanCut = new TGraphErrors(); 
  TGraphErrors *grRMSCut = new TGraphErrors();
 
  for (int k=0; k<n_slice; k++){

   int idCnv = k%nslXcanv;
    if (idCnv == 0) {
      CntXCanv[nHistoSum] = new TCanvas();
      CntXCanv[nHistoSum]->Divide(h_line,h_col);
      nHistoSum += 1;
  
    }
    CntXCanv[nHistoSum-1]->cd(idCnv+1);

    ((TH1F*)CntXHisto->At(Hthres+k))->DrawCopy();
    grMeanCut->SetPoint(k,(k+0.5)*slice_width,((TH1F*)CntXHisto->At(Hthres+k))->GetMean());
    grMeanCut->SetPointError(k,0.5*slice_width/sqrt(12.),((TH1F*)CntXHisto->At(Hthres+k))->GetMeanError());

    grRMSCut->SetPoint(k,(k+0.5)*slice_width,((TH1F*)CntXHisto->At(Hthres+k))->GetRMS());
    grRMSCut->SetPointError(k,0.5*slice_width/sqrt(12.),((TH1F*)CntXHisto->At(Hthres+k))->GetRMSError());

  }

  TGraphErrors *grMeanTau = new TGraphErrors(); 
  TGraphErrors *grRMSTau = new TGraphErrors();
  for (int k=0; k<n_slice; k++){

   int idCnv = k%nslXcanv;
    if (idCnv == 0) {
      CntXCanv[nHistoSum] = new TCanvas();
      CntXCanv[nHistoSum]->Divide(h_line,h_col);
      nHistoSum += 1;
  
    }
    CntXCanv[nHistoSum-1]->cd(idCnv+1);

    ((TH1F*)CntXHisto->At(Hthres+n_slice+k))->DrawCopy();
    grMeanTau->SetPoint(k,(k+0.5)*slice_width,((TH1F*)CntXHisto->At(Hthres+n_slice+k))->GetMean());
    grMeanTau->SetPointError(k,0.5*slice_width/sqrt(12.),((TH1F*)CntXHisto->At(Hthres+n_slice+k))->GetMeanError());

    grRMSTau->SetPoint(k,(k+0.5)*slice_width,((TH1F*)CntXHisto->At(Hthres+n_slice+k))->GetRMS());
    grRMSTau->SetPointError(k,0.5*slice_width/sqrt(12.),((TH1F*)CntXHisto->At(Hthres+n_slice+k))->GetRMSError());

  }

  //plot della differenza di cls

  TGraphErrors *grMeanDiff = new TGraphErrors(); 
  TGraphErrors *grRMSDiff = new TGraphErrors(); 
  for (int k=0; k<n_slice; k++){

   int idCnv = k%nslXcanv;
    if (idCnv == 0) {
      CntXCanv[nHistoSum] = new TCanvas();
      CntXCanv[nHistoSum]->Divide(h_line,h_col);
      nHistoSum += 1;
  
    }
    CntXCanv[nHistoSum-1]->cd(idCnv+1);

    ((TH1F*)CntXHisto->At(Hthres+2*n_slice+k))->DrawCopy();
    grMeanDiff->SetPoint(k,(k+0.5)*slice_width,((TH1F*)CntXHisto->At(Hthres+2*n_slice+k))->GetMean());
    grMeanDiff->SetPointError(k,0.5*slice_width/sqrt(12.),((TH1F*)CntXHisto->At(Hthres+2*n_slice+k))->GetMeanError());

    grRMSDiff->SetPoint(k,(k+0.5)*slice_width,((TH1F*)CntXHisto->At(Hthres+2*n_slice+k))->GetRMS());
    grRMSDiff->SetPointError(k,0.5*slice_width/sqrt(12.),((TH1F*)CntXHisto->At(Hthres+2*n_slice+k))->GetRMSError());
  }

  CntXCanv[nHistoSum] = new TCanvas();
  grMeanCut->Draw("ap");
  grMeanCut->SetMarkerStyle(24);
  grMeanCut->SetMarkerColor(4);
  ((TAxis *)grMeanCut->GetYaxis())->SetRangeUser(-0.3,0.3);
  grMeanCut->SetTitle("#Delta cls Vs nr slice; time of slice [ns]; cls_{MC} - cls_{reco}");

  grMeanTau->Draw("psame");
  grMeanTau->SetMarkerStyle(23);
  grMeanTau->SetMarkerColor(6);

  grMeanDiff->Draw("psame");
  grMeanDiff->SetMarkerStyle(29);
  grMeanDiff->SetMarkerColor(8);
  nHistoSum++;


  CntXCanv[nHistoSum] = new TCanvas();
  grRMSCut->Draw("ap");
  ((TAxis *)grRMSCut->GetYaxis())->SetRangeUser(0.5,1.2);
  grRMSCut->SetMarkerStyle(22);
  grRMSCut->SetMarkerColor(2);
  grRMSCut->SetTitle("#Delta cls Vs nr slice; time of slice [ns]; cls_{MC} - cls_{reco}");

  grRMSTau->Draw("psame");
  grRMSTau->SetMarkerStyle(23);
  grRMSTau->SetMarkerColor(3);

  grRMSDiff->Draw("psame");
  grRMSDiff->SetMarkerStyle(24);
  grRMSDiff->SetMarkerColor(4);
  nHistoSum++;


  CntXCanv[nHistoSum] = new TCanvas();
  scatt1->DrawCopy("colz");
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  scatt2->DrawCopy();
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  scatt3->DrawCopy();
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  scatt4->DrawCopy("lego2");
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  TGraphErrors *aa = new TGraphErrors();
  TAxis *AsseX = (TAxis*)scatt4->GetXaxis();
  for (int i=1;i<90;i++) {
    TH1D *slice = scatt4->ProjectionY(Form("bin_%i",i),i,i);
    slice->Fit("gaus");
    slice->Fit("gaus");
    slice->Fit("gaus");

    aa->SetPoint(i-1,AsseX->GetBinCenter(i),((TF1 *)slice->GetFunction("gaus"))->GetParameter(1));
    aa->SetPointError(i-1,0.5*AsseX->GetBinWidth(i),((TF1 *)slice->GetFunction("gaus"))->GetParameter(2));
  }
  aa->Draw("ap");
  aa->SetMarkerStyle(22);
  aa->SetMarkerColor(3);
  aa->SetTitle("#Delta t Vs t_{cls}; t_{cls} in #mus; #Delta t in ns");
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  CntXCanv[nHistoSum]->Divide(2,1);
  CntXCanv[nHistoSum]->cd(1);
  pr_1->DrawCopy();
  CntXCanv[nHistoSum]->cd(2);
  pr_2->DrawCopy();
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  pr_3->DrawCopy();
  nHistoSum++;

  CntXCanv[nHistoSum] = new TCanvas();
  pr_4->DrawCopy();
  nHistoSum++;

  for (int k=0;k<nHistoSum;k++) CntXCanv[k]->Write(); 

  fOut->Write();
  fOut->Close();

}//chiusura macro


//________FUNZIONE CHE ORDINA I TEMPI DI DRIFT________//
void Ordina(int np, double *temp){
  double tmp_d[np];
  for(int i=0; i<np; i++){
    tmp_d[i]=TMath::MinElement(np,temp);
    for (int k=0; k<np; k++){
      if(tmp_d[i] == temp[k]) temp[k] = 999.;
    }
  }
  for (int h=0; h<np; h++){
    temp[h]=tmp_d[h];
    //    cout<<"vettore ordinato["<<h<<"] = "<<tmp_d[h]<<endl;
  }
};


//___________funzione di raggruppamento cluster_____________//
int CluRag(int nGenEle, double *ele_time, double *cut, Bool_t flag){

  Int_t count_slice[n_slice];
  Double_t time[200];

  //  cout<<"*************  entro in raggruppa ******** "<<endl;

  //azzero il vettore temporaneo dei tempi
  for (int k=0; k<200; k++) time[k] = 0.;

  time[0] = ele_time[0];
  clsMulti[0] = 1;

  Int_t nrg = 1;
  int cls_count=1;
  //  cout<<ele_time[0]<<"   0 "<<" 0  "<<time[0]<<endl;
  for (int n=1; n<nGenEle; n++){

    Double_t dt = 1000.*(ele_time[n] - time[cls_count-1]);

    Int_t is = (int)(time[cls_count-1]*1000./slice_width);
    //    cout<<ele_time[n]<<"  "<<dt<<"  "<< cut[is]<<"  "<<time[cls_count-1]<<endl;

    if (dt > cut[is]) {
      time[cls_count] = ele_time[n]; 
      clsMulti[cls_count-1] = nrg;
      cls_count++;
      nrg = 1;
    }
    else {
      /*      if (nrg == 5) {
	time[cls_count] = ele_time[n]; 
      	clsMulti[cls_count-1] = nrg;
	cls_count++;
	nrg = 0;
	}*/

      if (flag) {
	Double_t media = (nrg*time[cls_count-1] + ele_time[n])/(nrg + 1.);
	time[cls_count-1] = media;
      } 
      //      if (nrg == 0) cout<<"esiste zero "<<n<<"  "<<cls_count<<"  "<<nrg<<endl;
      nrg++;
      //     cout<<"anche dopo "<<nrg<<"  "<<cls_count<<endl;
    }
  }

  clsMulti[cls_count-1] = nrg;
  //  cout<<"ne ho contati "<<cls_count<<endl;

  for (int n=0;n<nGenEle;n++) {
    if (n < cls_count) {
      ele_time[n] = time[n];
      //      cout<<"time cls "<<time[n]<<"  mult "<<clsMulti[n]<<"  "<<nGenEle<<endl;
    }
    else ele_time[n] = 0.;
  }

  return cls_count;

};
