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


void Order_d(int np, double *dist);
void Ordina(int np, double *temp);
//int CluRag(int nGenEle, double *ele_time, int n_slice, double *cut);

const int n_tau=30;
const int nfact = 2;
const int n_slice = nfact*18;
const Double_t t_cut=0.2;  // 0.2 ns
Double_t slice_width=180./n_slice;  // largezza slice 160ns/numero di slice

void ClusterRaggr(Int_t nrUser=0) {
    
  Double_t cluster_veri_slice[n_slice];

  //_________________Contenitori degli histogrammi_____________________//
  TObjArray *CntXhisto = new TObjArray();
  TObjArray *CntXhisto2D = new TObjArray();
  TObjArray *Histo_cls = new TObjArray();
  TObjArray *ClsContati = new TObjArray();
  TObjArray *Histo_cls_veri = new TObjArray();
  TObjArray *Distanze_slice = new TObjArray();
  //__________________________________________________________________//

  //______________istogrammi 1D_______________________________________//
  CntXhisto->Add( new TH1F("nclust"," Clusters distribution; # nr of cls ",51,-0.5,50.5) );
  CntXhisto->Add( new TH1F("n_ele"," Nr of e- in each cluster; # nr of e- ",21,-0.5,20.5) );
  CntXhisto->Add( new TH1F("0","time drift of electrons (All e-); time [ns] ",500,0.,200.) );
  CntXhisto->Add( new TH1F("1"," time distance of electrons belongings to the same cluster; #Deltat [ns] ",250,0.,100.) );
  CntXhisto->Add( new TH1F("2"," time distance between different cls; #Deltat [ns]",250,0.,100.) );
  CntXhisto->Add( new TH1F("3"," cls time; time [ns]",500,0.,200.) );
  CntXhisto->Add( new TH1F("4"," nr of cls @ 1cm ",51,-0.5,50.5) );
  CntXhisto->Add( new TH1F("5"," Track Length; [mm]",500,0.,10.) );
  CntXhisto->Add( new TH1F("b_true"," MC Impact parameter [mm] ",250,0.,5.) );

  //************* histo del nr di cls per taglio e slice **********//
  for (int itau=0; itau<n_tau; itau++){
    ClsContati->Add(new TH1F(Form("tau_%i",itau),Form("Counted Cls with cut %i tau",itau),21,-0.5,20.5));

    for (int islice=0; islice<n_slice; islice++){
      Histo_cls->Add(new TH1F(Form("tau%i_slice%i",itau,islice),Form("tau%i_slice%i",itau,islice),21,-0.5,20.5));
    }
  }

  //************ histo delle distanze per slice del nr di cls MC ***//
  for (int islice=0; islice<n_slice; islice++){
    Distanze_slice->Add(new TH1F(Form("SLICE_n%i",islice),
				 Form("temp distance e^- slice %i --> (%i - %i) ns",islice,islice*10,(islice+1)*10),240,0.,60.));
    ((TH1F*) Distanze_slice->At(islice))->SetXTitle(
						    Form("#Delta_{ij} con t_i in (%i - %i) ns",islice*10,(islice+1)*10));
    Histo_cls_veri->Add(new TH1F(Form("slice%i",islice),Form("cluster_veri slice%i ",islice),21,-0.5,20.5));
  }
  //__________________________________________________________________//

  //______________istogrammi 2D_______________________________________//
  CntXhisto2D->Add( new TH2F("h00","#Deltat_{i-(i+1)} of electrons belongings to the same cluster VS t_{i}; t_{i} [ns]; #Deltat_{i-(i+1)} [ns]",400,0,200,100,0,30) );
  
  CntXhisto2D->Add( new TH2F("h21","#Deltat_{i-(i+1)} of electrons VS t_{i}; t_{i} [ns]; #Deltat_{i-(i+1)} [ns] ",200,0.,0.2,200,0.,100.) );
  
  CntXhisto2D->Add( new TH2F("h22","#Deltat_{ij cluster} Vs N_{cluster}; N_{cluster}; #Deltat_{ij cluster} [ns] ",200,0,50,200,0,200) );
  //__________________________________________________________________//
  
  
  //__________________Event File chain________________________________// 
  TChain *tr = new TChain("MCHits");
  TString filename;
  for (int i=0;i<20;i++) {
    filename = TString(Form("../Simdata/MCHits800MHz_g4_SN7_2GSa_%03i.root",i+3));
    tr->Add(filename.Data());
  }
  //__________________________________________________________________//

  //_____hit selection___________//
  TClonesArray *hits = new TClonesArray("GMCDCHHit");
  tr->SetBranchAddress("DCHits",&hits);
  
  //______waveform selection______//
  TClonesArray *wave = new TClonesArray("GMCWaveformData");
  tr->SetBranchAddress("DCWaveforms",&wave);

  Int_t nEvents = tr->GetEntries(); // numero di eventi
  if (nrUser > 0) nEvents = nrUser;
    
  cout<<"Number of events Stored --> "<<nEvents<<endl;
  
  for (int ie=0;ie<nEvents;ie++) {

    //_________event selection______________//
    tr->GetEntry(ie);  

    Int_t nhits = hits->GetEntries();
 
    Int_t n_wave = wave->GetEntries();  //numero di waveform
 
    if (ie%1000 == 0) cout<<"**** evento numero *******"<<ie<<" with nr of hits: "<<nhits<<endl;
 
    if (nhits == 0) continue;

   //____________wave selection and cycle________________//
    for (int j=0; j<nhits; j++){
      
      GMCDCHHit *ahit = (GMCDCHHit *)hits->At(j);
      GMCWaveformData *wave_data = (GMCWaveformData *)wave->At(j); 
       
      Int_t nclust = ahit->GetfNrClusters(); // numero di cluster generati

      if (nclust == 0 || nclust == -999 || nclust > 100) continue;

      Int_t nGenEle = 0;
      nGenEle = wave_data->GetfNofGenEle();

      //cout<<"numero di cluster generati in questa waveform = "<<nclust<<endl;
      //cout<<"numeri di elettroni generati in questa waveform = "<<nGenEle<<endl;
      ((TH1F *)CntXhisto->At(7))->Fill(ahit->GetfLength());
      ((TH1F *)CntXhisto->At(8))->Fill(ahit->GetfImpact());
      ((TH1F *)CntXhisto->At(6))->Fill(nclust/(0.1*ahit->GetfLength()));
      ((TH1F *)CntXhisto->At(0))->Fill(nclust);

      Double_t t_cluster[nclust]; // vettore dei tempi del primo e-
      Double_t *time_wave_MC = wave_data->GetfDTimeEle();  //vettore dei tempi di drift delle waveform

      Int_t count_cls[n_slice];
      for (int k=0; k<n_slice; k++) count_cls[k] = 0;

      Int_t n_elett=0;
      //________ciclo sui cluster_________//
      for (int l=0;l<nclust;l++) {

	t_cluster[l]=0.;// azzero il vettore dei tempi dei cluster	

	Int_t num_elect_cls = ahit->GetfClsChargeAt(l); 
	((TH1F *)CntXhisto->At(1))->Fill(num_elect_cls);

	t_cluster[l] = time_wave_MC[n_elett];
	((TH1F *)CntXhisto->At(2))->Fill(1000.*time_wave_MC[n_elett]);

	for (int ind=1;ind<num_elect_cls;ind++) {

	  ((TH1F *)CntXhisto->At(2))->Fill(1000.*time_wave_MC[n_elett+ind]);

	  //elettroni dello stesso cls
	  Double_t dt = 1000.*fabs(time_wave_MC[n_elett+ind]-time_wave_MC[n_elett+ind-1]);
	  ((TH1F *)CntXhisto->At(3))->Fill(dt);

	  if (t_cluster[l] > time_wave_MC[n_elett+ind]) t_cluster[l] = time_wave_MC[n_elett+ind];

	}

	//differenza in tempo tra cls differenti 
	((TH1F *)CntXhisto->At(4))->Fill(1000.*fabs(time_wave_MC[n_elett+num_elect_cls] - time_wave_MC[n_elett+num_elect_cls-1]));

	((TH1F *)CntXhisto->At(5))->Fill(1000.*t_cluster[l]);
	Int_t is = (int)(1000.*t_cluster[l]/slice_width);
	if (is < n_slice) count_cls[is]++;


	n_elett += num_elect_cls;

      } //chiude il ciclo sui cluster
  	
      
      // ordinamento dei tempi di drift dei cluster
      Ordina(nclust, t_cluster); 
	
      for (int cl=0; cl<nclust; cl++){
	if (cl > 0) {
	  ((TH2F *)CntXhisto2D->At(2))->Fill(nclust,(t_cluster[cl]-t_cluster[cl-1])*1000.);
	  //	  ((TH2F *)CntXhisto2D->At(1))->Fill(t_cluster[cl-1],(t_cluster[cl]-t_cluster[cl-1])*1000.);
	}
      }
      
      for (int k=0; k<n_slice; k++) ((TH1F *)Histo_cls_veri->At(k))->Fill(count_cls[k]);


      //__distanza temporale di tutti gli elettroni (come mi arrivano)____//
      Ordina(nGenEle, time_wave_MC);
      Double_t time[nGenEle]; 

      for(int il=0; il<n_tau; il++){ //ciclo sui tagli

	//azzero il vettore temporaneo dei tempi
	for (int k=0; k<nGenEle; k++) time[k] = 0.;

	time[0] = time_wave_MC[0];
	Int_t cls_count = 1;
	for (int k=1; k<nGenEle; k++){

	  Double_t dt = 1000.*(time_wave_MC[k] - time_wave_MC[k-1]);
	  ((TH2F *)CntXhisto2D->At(1))->Fill(time_wave_MC[k-1],dt);

	  if (il == 0) {

	    Int_t is = (int)(1000.*time_wave_MC[k-1]/slice_width);
	    // distanza temporale elettroni x ogni slice
	    if (is < n_slice) ((TH1F*) Distanze_slice->At(is))->Fill(dt); 
	    
	  }
	  
	  dt = (time_wave_MC[k] - time[cls_count-1])*1000.;
	  if (dt > il*t_cut) {
	    time[cls_count] = time_wave_MC[k];
	    cls_count++;
	  } 
	}

	((TH1F *)ClsContati->At(il))->Fill(cls_count);
	Int_t nCntInSlice[n_slice];
	for (int k=0; k<n_slice; k++) nCntInSlice[k] = 0;

	for (int k=0; k<cls_count;k++) {
	  Int_t is = (int)(1000.*time[k]/slice_width);
	  if (is < n_slice) nCntInSlice[is]++;
	}

	for (int k=0; k<n_slice; k++) ((TH1F *)Histo_cls->At(il*n_slice+k))->Fill(nCntInSlice[k]);

      }  //chiudo il for sul numero di tagli
      
    } //chiudo for sugli hit  
  } //chiudo for sugli eventi
  

  //Apro il file .root 
  TFile *fPlot = new TFile("PlotGenerati.root","RECREATE");

  Double_t tau1[n_slice], tau2[n_slice], x[n_slice], ex[n_slice], et1[n_slice], et2[n_slice], chiSq[n_slice];

  //vettore delle slice temporali
  for(int islice=0; islice<n_slice; islice++){
    ex[islice] = slice_width/2.;
    x[islice] = ex[islice] + islice*slice_width;
  } 	 


  TGraphErrors *mygraph[n_slice];
  TLegend *leg2[n_slice];
  
  Int_t nHistoSum = 0;
  TCanvas *CntXCanvas[100];
  for (int k=0;k<CntXhisto->GetEntries();k++) {
    CntXCanvas[nHistoSum+k] = new TCanvas();
    ((TH1F*)CntXhisto->At(k))->DrawCopy();
  }
  nHistoSum += CntXhisto->GetEntries();

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

  TF1 *my_exp = new TF1("my_exp","[0]*(exp(-x/[1])+[2]*exp(-x/[3]))",0.,100.);
  my_exp->SetParLimits(0,0.,1.e6);
  my_exp->SetParLimits(1,0.,100.);
  my_exp->SetParLimits(2,0.,1000);
  my_exp->SetParLimits(3,0.,100.);

  my_exp->SetParameters(10.,2.,7.,0.5);

  for (int k=0; k<n_slice; k++){

   int idCnv = k%nslXcanv;
    if (idCnv == 0) {
      CntXCanvas[nHistoSum] = new TCanvas();
      CntXCanvas[nHistoSum]->Divide(h_line,h_col);
      nHistoSum += 1;
  
    }
    cout<<"*********************************************"<<endl;
    cout<<"*********  slice  nr "<<k<<"      ***********"<<endl;
    cout<<"*********************************************"<<endl;
    CntXCanvas[nHistoSum-1]->cd(idCnv+1);

    ((TH1F*) Distanze_slice->At(k))->DrawCopy();

    if (k == 25 ) {	 
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,40.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,40.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,40.);
    }   
    else if (k >= 26 && k < 30) {	 
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,30.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,30.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,30.);
    }
    else if (k >= 30 && k < 32) {	 
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,20.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,20.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,20.);
    }
    else if (k >= 32 ) {	 
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,15.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,15.);
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp","","",0.,15.);
    }
    else {
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp");
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp");
      ((TH1F*) Distanze_slice->At(k))->Fit("my_exp");
    }
    chiSq[k] = my_exp->GetChisquare()/my_exp->GetNDF();
    tau1[k] = my_exp->GetParameter(1);
    tau2[k] = my_exp->GetParameter(3);
    et1[k] = my_exp->GetParError(1);
    et2[k] = my_exp->GetParError(3);
    
  }

  TGraph *chisqgr = new TGraph(n_slice,x,chiSq);
  CntXCanvas[nHistoSum] = new TCanvas();
  chisqgr->Draw("ap");
  chisqgr->SetTitle("#Chi^2/ndf from Two--Exp fits; nr of slice; #Chi^2 value");
  chisqgr->SetMarkerStyle(23);
  chisqgr->SetMarkerColor(kRed);
  nHistoSum += 1;

  TGraphErrors *valMediCls = new TGraphErrors();
  for(int k=0; k<n_slice; k++){

    int idCnv = k%nslXcanv;
    if (idCnv == 0) {
      CntXCanvas[nHistoSum] = new TCanvas();
      CntXCanvas[nHistoSum]->Divide(h_line,h_col);
      nHistoSum += 1;
  
    }

    CntXCanvas[nHistoSum-1]->cd(idCnv+1);
    ((TH1F*)Histo_cls_veri->At(k))->DrawCopy();
    valMediCls->SetPoint(k,x[k],((TH1F*)Histo_cls_veri->At(k))->GetMean());
    valMediCls->SetPointError(k,ex[k],((TH1F*)Histo_cls_veri->At(k))->GetMeanError());
  }
  //  nHistoSum += 1;

  CntXCanvas[nHistoSum] = new TCanvas();
  valMediCls->Draw("ap");
  valMediCls->SetTitle("Mean Cluster in slices; time [ns]; <Nr_{cls}>");
  nHistoSum += 1;


  cout<<"*********sono arrivato qui  "<<nHistoSum<<endl;

  for (int k=0; k<n_slice; k++){
    cout<<"**************  slice "<<k<<endl;
    CntXCanvas[nHistoSum+k] = new TCanvas();
    CntXCanvas[nHistoSum+k]->Divide(5,6);

    mygraph[k] = new TGraphErrors();
    for(int n=0; n<n_tau; n++){
      CntXCanvas[nHistoSum+k]->cd(n+1);
      ((TH1F*)Histo_cls->At(n*n_slice+k))->DrawCopy();

      cout<<"punto  "<<n<<"  "<<((TH1F*)Histo_cls->At(n*n_slice+k))->GetMean()<<"  "<<((TH1F*)Histo_cls->At(n*n_slice+k))->GetRMS()<<"  "<<((TH1F*)Histo_cls->At(n*n_slice+k))->GetEntries()<<endl;

      mygraph[k]->SetPoint(n,n*t_cut,((TH1F*)Histo_cls->At(n*n_slice+k))->GetMean());
      mygraph[k]->SetPointError(n,0.1/sqrt(12.),(((TH1F*)Histo_cls->At(n*n_slice+k))->GetRMS())/sqrt(((TH1F*)Histo_cls->At(n*n_slice+k))->GetEntries()));
    }

    //    CntXCanvas[nHistoSum+k]->Write();
  }

  nHistoSum += n_slice;
  
 
  //    TF1 *exp2 = new TF1("exp2","[0]+[1]*exp(-x/[2])",0.,50.);
  //  TF1 *exp2 = new TF1("exp2","[0]+[1]*(exp(-x/[2]) + [3]*exp(-x/[4]))",0.,50.);
    TF1 *exp2 = new TF1("exp2","[0] + [1]/sqrt(1.+ [2]*x)",0.,200.);
  //  TF1 *exp2 = new TF1("exp2","([0] + [1]*x)/(1.+ [2]*x)",0.,200.);

  exp2->SetParLimits(0,0.,10.);
  exp2->SetParLimits(1,0.,100.);
  exp2->SetParLimits(2,0.,10.);
  //  exp2->SetParLimits(3,0.,10.);
  //  exp2->SetParLimits(4,0.,100.);

  exp2->SetParameters(0.07,2.,0.7);
  
  Double_t inters_point[n_slice];
  Double_t err_inters_point[n_slice];

  TLine *lineRef[n_slice];

  Double_t fitParameter[n_slice][3];
  Double_t errfitParameter[n_slice][3];

  for(int n=0; n<n_slice; n++){

    int idCnv = n%nslXcanv;
    if (idCnv == 0) {
      CntXCanvas[nHistoSum] = new TCanvas();
      CntXCanvas[nHistoSum]->Divide(h_line,h_col);
      nHistoSum += 1;
    }
    CntXCanvas[nHistoSum-1]->cd(idCnv+1);
    leg2[n] = new TLegend(0.6,0.7,0.3,0.9);
    mygraph[n]->SetMarkerColor(4);
    mygraph[n]->SetMarkerStyle(22);
    mygraph[n]->SetMarkerSize(0.8);
    //    ((TAxis *)mygraph[n]->GetYaxis())->SetRangeUser(0.,0.5);
    ((TAxis *)mygraph[n]->GetYaxis())->SetLabelSize(0.08);
    ((TAxis *)mygraph[n]->GetYaxis())->SetLabelOffset(0.05);
    ((TAxis *)mygraph[n]->GetYaxis())->SetTitleOffset(1.5);
    ((TAxis *)mygraph[n]->GetYaxis())->SetTitleSize(0.07);
    ((TAxis *)mygraph[n]->GetXaxis())->SetLabelSize(0.08);
    ((TAxis *)mygraph[n]->GetXaxis())->SetLabelOffset(0.05);
    ((TAxis *)mygraph[n]->GetXaxis())->SetTitleOffset(1.5);
    ((TAxis *)mygraph[n]->GetXaxis())->SetTitleSize(0.07);
    mygraph[n]->SetTitle(Form("SLICE %i;#tau;n_clust_ric",n));
    //    exp2->FixParameter(0,0.);

    mygraph[n]->Draw("AP");
    mygraph[n]->Fit("exp2");
    mygraph[n]->Fit("exp2");
    mygraph[n]->Fit("exp2");

    fitParameter[n][0] = exp2->GetParameter(0);
    fitParameter[n][1] = exp2->GetParameter(1);
    fitParameter[n][2] = exp2->GetParameter(2);
    errfitParameter[n][0] = exp2->GetParError(0);
    errfitParameter[n][1] = exp2->GetParError(1);
    errfitParameter[n][2] = exp2->GetParError(2);

    inters_point[n] = exp2->GetX(((TH1F*)Histo_cls_veri->At(n))->GetMean());
    Double_t xmin = exp2->GetX(((TH1F*)Histo_cls_veri->At(n))->GetMean() -
			       3.*((TH1F*)Histo_cls_veri->At(n))->GetRMS());

    Double_t xmax = exp2->GetX(((TH1F*)Histo_cls_veri->At(n))->GetMean() +
			       3.*((TH1F*)Histo_cls_veri->At(n))->GetRMS());

    err_inters_point[n] = (xmax - xmin)/3.;

    lineRef[n] = new TLine(0.,((TH1F*)Histo_cls_veri->At(n))->GetMean(),6.,((TH1F*)Histo_cls_veri->At(n))->GetMean());
    lineRef[n]->SetLineColor(2);
    lineRef[n]->SetLineWidth(2);
    lineRef[n]->Draw("same");
 
    TMarker *m = new TMarker(inters_point[n],((TH1F*)Histo_cls_veri->At(n))->GetMean(),24);
    m->SetMarkerColor(kRed);
    m->SetMarkerSize(3);
    m->Draw();

    //    clust_medi[n]->Draw("Same");
    //    clust_medi[n]->SetLineColor(2);
    //    clust_medi[n]->SetLineWidth(2);
    leg2[n]->AddEntry(mygraph[n],"cluster riconosciuti","P");
    leg2[n]->AddEntry(lineRef[n],"cluster veri","L");	
    leg2[n]->Draw();
  }
  //  nHistoSum += 1;

  /*  Double_t cut[n_slice]; 
  for(int n=0; n<n_slice; n++){
    //cout<<"int point = "<<inters_point[n]<<endl;
    cut[n]=inters_point[n];	
    //    cout<<"taglio["<<n<<"] = "<<cut[n]<<"  "<<inters_point[n]<<endl;
  }
  ************/

  TGraphErrors *grParFit[3];
  for (int igr=0;igr<3;igr++) {
    CntXCanvas[nHistoSum] = new TCanvas();
    grParFit[igr] = new TGraphErrors(n_slice);
    grParFit[igr]->SetTitle(Form("Parameter %i from fit; time slice [ns]; par value",igr));
    grParFit[igr]->SetMarkerStyle(22+igr);
    grParFit[igr]->SetMarkerColor(2+igr);
    for (int k=0;k<n_slice;k++) {
      grParFit[igr]->SetPoint(k,x[k],fitParameter[k][0]);
      grParFit[igr]->SetPointError(k,ex[k],errfitParameter[k][0]);
    }
    grParFit[igr]->Draw("ap");
    ((TAxis *)grParFit[igr]->GetYaxis())->SetRangeUser(0.,0.5);
    nHistoSum += 1;
  }


  TF1 *diffFun = new TF1("diffFun","[0]+[1]*sqrt(x)+[2]*x+[3]*pow(x,1.5)",0.,200.);
  //utilizzo i parametri ottenuti dal fit di Tax per ottenere la diffusione
  //teorica
  diffFun->SetParameters(-0.1489,0.3493,-0.01862,0.001238);

  CntXCanvas[nHistoSum] = new TCanvas();
  TGraphErrors *gr_tau1 = new TGraphErrors(n_slice,x,tau1,ex,et1);
  gr_tau1 -> SetMarkerSize(1);
  gr_tau1 -> SetMarkerStyle(22);
  gr_tau1 -> SetMarkerColor(2);
  gr_tau1 -> Draw("AP");
  gr_tau1 -> GetXaxis()->SetTitle("t_{drift} [ns]");
  gr_tau1 -> GetYaxis()->SetTitle("#tau [ns]");
  gr_tau1 -> GetYaxis()->SetRangeUser(0.,25.);

  TGraphErrors *gr_tau2 = new TGraphErrors(n_slice,x,tau2,ex,et2);
  gr_tau2 -> SetMarkerSize(1);
  gr_tau2 -> SetMarkerStyle(23);
  gr_tau2 -> SetMarkerColor(6);
  gr_tau2 -> Draw("PSame");
  
  TGraphErrors *gr_cut = new TGraphErrors(n_slice,x,inters_point,ex,err_inters_point);
  gr_cut -> SetMarkerSize(1);
  gr_cut -> SetMarkerStyle(24);
  gr_cut -> SetMarkerColor(4);
  gr_cut -> Draw("PSame");
  
  diffFun->Draw("same");
  diffFun->SetLineColor(8);
  diffFun->SetLineWidth(1.5);

  TLegend *leg = new TLegend(0.1,0.7,0.3,0.9);
  leg->AddEntry(gr_tau1,"#tau_{1}","lep");
  leg->AddEntry(gr_tau2,"#tau_{2}","lep");
  leg->AddEntry(gr_cut,"cut","lep");
  leg->AddEntry(diffFun,"Diffusion value","lp");
  leg->Draw();
  nHistoSum += 1;


  for (int k=0;k<CntXhisto2D->GetEntries()-1;k++) { 
    CntXCanvas[nHistoSum] = new TCanvas();
    ((TH2F *)CntXhisto2D->At(k+1))->DrawCopy();
    nHistoSum += 1;
  }

  for (int k=0;k<nHistoSum;k++) CntXCanvas[k]->Write(); 
  
  fPlot->Write();
  fPlot->Close();

  /******************
  ofstream fOut;
  fOut.open("data_file.txt",ios::out);
  fOut<<"Slice  "<<"tempo_slice  "<<"taglio "<<" tau2 "<<" Diffusione "<<endl;
  for (int i=0;i<n_slice;i++) {
    fOut<<i<<"   "<<(i + 0.5)*slice_width<<"  "<<inters_point[i]<<
      "  "<<tau2[i]<<"  "<<diffFun->Eval((i + 0.5)*slice_width)<<endl;
  }
  
  fOut.close();
  *****************************/

};



//________FUNZIONE CHE ORDINA I TEMPI DI DRIFT________//
void Ordina(int np, double *temp){
  double tmp_d[1000];
  for(int i=0; i<np; i++){
    tmp_d[i]=TMath::MinElement(np,temp);
    for (int k=0; k<np; k++){
      if(tmp_d[i] == temp[k]) temp[k] = 999.;
    }
  }
  for (int h=0; h<np; h++){
    temp[h]=tmp_d[h];
    //cout<<"vettore ordinato["<<h<<"] = "<<tmp_d[h]<<endl;
  }
};

//_____FINE FUNZIONE CHE ORDINA I TEMPI DI DRIFT_____//

//________Organizes cluster distance in an ascending order________//
void Order_d(int np, double *dist){
  double tmp_d[100];
  for(int i=0; i<np; i++){
    tmp_d[i]=TMath::MinElement(np,dist);
    for (int k=0; k<np; k++){
      if(tmp_d[i] == dist[k]) dist[k] = 999.;
    }
  }
  for (int h=0; h<np; h++){
    dist[h]=tmp_d[h];
  }
};

//_________________________________________________________//


/*******************
//___________funzione di raggruppamento cluster_____________//

int CluRag(int nGenEle, double *ele_time, int n_slice, double *cut){
  Double_t count[n_slice];
  for (int n=0; n<n_slice; n++) count[n]=0;
  int clust_count=1;
  for (int n=0; n<nGenEle; nGenEle++){
    int is=ele_time[n]*1000./slice_width;  // stabilisco a che slice appartiene l'i-esimo tempo d drift
    if (ele_time[n+1]-ele_time[n]>cut[is]) count[is]++;
  }
  for (int ncl=0; ncl<n_slice; ncl++){
    clust_count += count[ncl];
  }

  return clust_count;

}

**********************************/
