#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#endif
#include <TProfile.h>
#include <TMath.h>
#include <fstream>
#include <string>
#include "XTREL.h"

ClassImp(XTREL)

//XTREL::XTREL() {

//  xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
//  xtFunc->SetParameters(0.,0.4603,0.743,3.127);

//}

//___________________________________________________________________________
XTREL::XTREL( Float_t rwire, Float_t rtube, Float_t V, const char *Fname){
  trxtf=0x0;
  trxt=0x0;
  trdata=0x0;
  radius =  new TVectorF(0);
  integrTime =  new TVectorF(0);
  integrDiffL =  new TVectorF(0);
  integrDiffT =  new TVectorF(0);
  coeffDiffL =  new TVectorF(0);
  coeffDiffT =  new TVectorF(0);
  Xxtx=0x0;
  Xxttime=0x0;
  XDiffL=0x0;
  XDiffT=0x0;
  XcoeffDiffL=0x0;
  XcoeffDiffT=0x0;
  xtData=0x0;
  firsttime = 0.;
  mindrifttime = 0.;
  filename=Fname;
  Volts = V;
  Emax = 0.;
  Emin = 0.;
  E=0x0;
  Vdrift=0x0;
  DiffL=0x0;
  DiffT=0x0;
  xtfilename=TString(Fname);
//  xtfilename.Replace(xtfilename.First("dat")+1,4,"root");
//   xtfilename.ReplaceAll(".dat",".root");
//   xtfilename.Insert(0,"XTtr_from");
  xtnbin = 0;
  curentr = -1;
  drifttime = 0.;
  minpos=0.0;
  pos = rwire;  //r wire
  Rwire = rwire;
  InnerRtube = rtube;
  Econst = Volts/TMath::Log( InnerRtube/Rwire );

  time = 0.0;
  firstdist = 0.0;
  mindriftdist = 0.0;
  curentrSpc = -1;
  driftdist = 0.0;
  mintime = 0.0;
  maxDrfTime = 100.0;

  x = new Float_t [3];
  relType = 0;
  xtGraph=0x0;
  xtGraph_X=0x0;
  xtGraph_ErrX=0x0;
  xtGraph_Y=0x0;
  xtGraph_ErrY=0x0;
  //xtFunc = new TF1("xtFunc","pol2+[3]*sqrt(x)");
  xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
  //xtFunc = new TF1("xtFunc","pol3+[4]*sqrt(x)");
  //xtFunc = new TF1("xtFunc","pol4+[5]*sqrt(x)");
  dXtFunc = 0x0;
//   Init();
//   WriteXT();
}

XTREL::XTREL(const char *XTFname ){
  trxtf=0x0;
  trxt=0x0;
  trdata=0x0;
  radius =  new TVectorF(0);
  integrTime =  new TVectorF(0);
  integrDiffL =  new TVectorF(0);
  integrDiffT =  new TVectorF(0);
  coeffDiffL =  new TVectorF(0);
  coeffDiffT =  new TVectorF(0);
  Xxtx=0x0;
  Xxttime=0x0;
  XDiffL=0x0;
  XDiffT=0x0;
  XcoeffDiffL=0x0;
  XcoeffDiffT=0x0;
  xtData=0x0;
  firsttime = 0.;
  mindrifttime = 0.;
  Volts = 0.;
  Emax = 0.;
  Emin = 0.;
  Econst = 0.;
  E=0x0;
  Vdrift=0x0;
  DiffL=0x0;
  DiffT=0x0;
  filename=0x0;
  if (XTFname==0x0 || strlen(XTFname)==0) xtfilename="XTtr_fromSigmaDiff.root";
  else xtfilename=XTFname;
  xtnbin = 0;
  curentr = -1;
  drifttime = 0.;
  x = new Float_t [3];
  relType = 0;

  curentrSpc = -1;
  driftdist = 0.;

  xtGraph=0x0;
  xtGraph_X=0x0;
  xtGraph_ErrX=0x0;
  xtGraph_Y=0x0;
  xtGraph_ErrY=0x0;
  //xtFunc = new TF1("xtFunc","pol2+[3]*sqrt(x)");
  xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
  //xtFunc = new TF1("xtFunc","pol3+[4]*sqrt(x)");
  //xtFunc = new TF1("xtFunc","pol4+[5]*sqrt(x)");
  dXtFunc = 0x0;
  OpenXT();
}


XTREL::~XTREL(){
  delete x;
  if (xtGraph_X!=0) { delete [] xtGraph_X;}
  if (xtGraph_ErrX!=0) {delete [] xtGraph_ErrX;}
  if (xtGraph_Y!=0) {delete [] xtGraph_Y;}
  if (xtGraph_ErrY!=0) {delete [] xtGraph_ErrY;}
  if (xtFunc!=0) {delete xtFunc;}
  if (dXtFunc!=0) {delete dXtFunc;}
  if (xtGraph!=0) {delete xtGraph;}
  if (XcoeffDiffL!=0) {delete XcoeffDiffL;}
  if (XcoeffDiffT!=0) {delete XcoeffDiffT;}
  if(trxtf!=0) { if (trxtf->IsOpen()) {trxtf->Close();} }
}

void XTREL::Init(Int_t nbin, Float_t *tr, Float_t *tt, Float_t *tdl, Float_t *tdt, Float_t *tsdl, Float_t *tsdt){
//   Int_t fpos, fpos2;

  if (nbin<=0) return;
  xtnbin=nbin;
  if (Xxtx!=0) {delete [] Xxtx; Xxtx=0x0; }
  if (Xxttime!=0) {delete [] Xxttime; Xxttime=0x0; }
  if (XDiffL!=0) {delete [] XDiffL;  XDiffL=0x0;}
  if (XDiffT !=0) {delete [] XDiffT; XDiffT=0x0; }
  if (XcoeffDiffL!=0) {delete [] XcoeffDiffL; XcoeffDiffL=0x0; }
  if (XcoeffDiffT!=0) {delete [] XcoeffDiffT; XcoeffDiffT=0x0; }
  if (xtGraph!=0x0) {delete xtGraph; xtGraph=0x0; }
  //if (xtGraph!=0x0) {xtGraph->Delete(); xtGraph=0x0;}
  if (dXtFunc!=0x0) {delete dXtFunc; dXtFunc=0x0;}
  Xxtx = new Float_t [xtnbin];
  Xxttime =  new Float_t [xtnbin];
  XDiffL =  new Float_t [xtnbin];
  XDiffT =  new Float_t [xtnbin];
  XcoeffDiffL =  new Float_t [xtnbin];
  XcoeffDiffT =  new Float_t [xtnbin];
  for(Int_t idarray = 0; idarray<xtnbin; idarray++){
    Xxtx[idarray] = tr[idarray];
    Xxttime[idarray] = tt[idarray];
    XDiffL[idarray] = (tdl==0x0) ? 0.0 : tdl[idarray];
    XDiffT[idarray] = (tdt==0x0) ? 0.0 : tdt[idarray];
    XcoeffDiffL[idarray] = (tsdl==0x0) ? 0.0 : tsdl[idarray];
    XcoeffDiffT[idarray] = (tsdt==0x0) ? 0.0 : tsdt[idarray];
  }
  firsttime = Xxttime[0];
  mindrifttime = 0.;
  curentr = -1;
  drifttime = 0.;
  pos = Rwire;

  firstdist = Xxtx[0];
  mindriftdist = Xxtx[0];
  curentrSpc = -1;
  driftdist = Xxtx[0];
  time = Xxttime[0];

  if (xtGraph==0x0) {
    xtGraph_X=Xxttime;
    xtGraph_Y=Xxtx;
    if ( relType == 2 ) {
      xtGraph_ErrY=0x0;
      xtGraph_ErrX=XDiffL;
      for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrX[ixb]<=0.0) xtGraph_ErrX[ixb]=1e-6;}
    } else {
      xtGraph_ErrX=0x0;
      xtGraph_ErrY=XDiffL;
      for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrY[ixb]<=0.0) xtGraph_ErrY[ixb]=1e-6;}
    }
    xtGraph = new TGraphErrors(xtnbin,xtGraph_X,xtGraph_Y,xtGraph_ErrX,xtGraph_ErrY);
    xtFunc->FixParameter(0,0.0);
    xtGraph->Fit(xtFunc,"","",0.0,0.7*xtGraph_X[xtnbin-1]);
    //xtGraph->Fit(xtFunc);
  }
  if ( relType == 2 && dXtFunc==0x0 ) {
    dXtFunc = new TF1("dXtFunc","pol1+[2]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,0.5*xtFunc->GetParameter(3));
    /*dXtFunc = new TF1("dXtFunc","pol2+[3]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));*/
    /*dXtFunc = new TF1("dXtFunc","pol3+[4]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,4.0*xtFunc->GetParameter(4));
    dXtFunc->SetParameter(4,0.5*xtFunc->GetParameter(5));*/
    /*xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));*/
  }
}

void XTREL::Init(Int_t nbin, Double_t *tr, Double_t *tt, Double_t *tdl, Double_t *tdt, Double_t *tsdl, Double_t *tsdt){
//   Int_t fpos, fpos2;
  if (nbin<=0) return;
  xtnbin=nbin;
  if (Xxtx!=0) {delete [] Xxtx; Xxtx=0x0; }
  if (Xxttime!=0) {delete [] Xxttime; Xxttime=0x0; }
  if (XDiffL!=0) {delete [] XDiffL;  XDiffL=0x0;}
  if (XDiffT !=0) {delete [] XDiffT; XDiffT=0x0; }
  if (XcoeffDiffL!=0) {delete [] XcoeffDiffL; XcoeffDiffL=0x0; }
  if (XcoeffDiffT!=0) {delete [] XcoeffDiffT; XcoeffDiffT=0x0; }
  if (xtGraph!=0x0) {delete xtGraph; xtGraph=0x0; }
  //if (xtGraph!=0x0) {xtGraph->Delete(); xtGraph=0x0;}
  if (dXtFunc!=0x0) {delete dXtFunc; dXtFunc=0x0;}
  Xxtx = new Float_t [xtnbin];
  Xxttime =  new Float_t [xtnbin];
  XDiffL =  new Float_t [xtnbin];
  XDiffT =  new Float_t [xtnbin];
  XcoeffDiffL =  new Float_t [xtnbin];
  XcoeffDiffT =  new Float_t [xtnbin];
  for(Int_t idarray = 0; idarray<xtnbin; idarray++){
    Xxtx[idarray] = (Float_t) tr[idarray];
    Xxttime[idarray] = (Float_t) tt[idarray];
    XDiffL[idarray] = (tdl==0x0) ? 0.0 : ((Float_t) tdl[idarray]);
    XDiffT[idarray] = (tdt==0x0) ? 0.0 : ((Float_t) tdt[idarray]);
    XcoeffDiffL[idarray] = (tsdl==0x0) ? 0.0 : ((Float_t) tsdl[idarray]);
    XcoeffDiffT[idarray] = (tsdt==0x0) ? 0.0 : ((Float_t) tsdt[idarray]);
  }
  firsttime = Xxttime[0];
  mindrifttime = 0.;
  curentr = -1;
  drifttime = 0.;
  pos = Rwire;

  firstdist = Xxtx[0];
  mindriftdist = Xxtx[0];
  curentrSpc = -1;
  driftdist = Xxtx[0];
  time = Xxttime[0];

  if (xtGraph==0x0) {
    xtGraph_X=Xxttime;
    xtGraph_Y=Xxtx;
    if ( relType == 2 ) {
      xtGraph_ErrY=0x0;
      xtGraph_ErrX=XDiffL;
      for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrX[ixb]<=0.0) xtGraph_ErrX[ixb]=1e-6;}
    } else {
      xtGraph_ErrX=0x0;
      xtGraph_ErrY=XDiffL;
      for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrY[ixb]<=0.0) xtGraph_ErrY[ixb]=1e-6;}
    }
    xtGraph = new TGraphErrors(xtnbin,xtGraph_X,xtGraph_Y,xtGraph_ErrX,xtGraph_ErrY);
    xtFunc->FixParameter(0,0.0);
    xtGraph->Fit(xtFunc,"","",0.0,0.7*xtGraph_X[xtnbin-1]);
    //xtGraph->Fit(xtFunc);
  }
  if ( relType == 2 && dXtFunc==0x0 ) {
    dXtFunc = new TF1("dXtFunc","pol1+[2]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,0.5*xtFunc->GetParameter(3));
    /*dXtFunc = new TF1("dXtFunc","pol2+[3]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));*/
    /*dXtFunc = new TF1("dXtFunc","pol3+[4]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,4.0*xtFunc->GetParameter(4));
    dXtFunc->SetParameter(4,0.5*xtFunc->GetParameter(5));*/
    /*xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));*/
  }
}

// void XTREL::Close(){
// //  delete trdata;
//   xtf->Close();
// }

void XTREL::WriteXT(){
  trxtf = new TFile(xtfilename.Data(),"recreate");
  trxt = new TTree("XTrelation","XTrelation");
  xtData= new XTDataVlGas();
  trxt->Branch("XTData","XTDataVlGas",&xtData);
  xtData->SetNpoints(xtnbin);
//  cout<<"lllll "<<xtData->GetNpoints()<<endl;
  xtData->SetRwire (Rwire);
  xtData->SetInnerRtube (InnerRtube);
  xtData->SetVolts (Volts);
  xtData->SetDist (Xxtx);
  xtData->SetTime (Xxttime);
  xtData->SetDiffL (XDiffL);
  xtData->SetDiffT (XDiffT);
  xtData->SetCoeffDiffL (XcoeffDiffL);
  xtData->SetCoeffDiffT (XcoeffDiffT);
  trxt->Fill();
  trxtf->cd();
  trxt->Write();
  //xtGraph->Write();
//  trxtf->Close();
//  Close();
}

void XTREL::OpenXT(){
  trxtf = new TFile(xtfilename.Data(),"read");
  trxtf->Open(xtfilename.Data());
  trxt = (TTree *) trxtf->Get("XTrelation");
  xtData= new XTDataVlGas();
  trxt->SetBranchAddress("XTData",&xtData);
  trxt->GetEntry(0);
  Rwire = xtData->GetRwire();
  InnerRtube = xtData->GetInnerRtube();
  Volts = xtData->GetVolts();
  Econst = Volts/TMath::Log( InnerRtube/Rwire );
  Emax = Econst/Rwire;
  Emin = Econst/InnerRtube;
  pos = Rwire;  //r wire
  if (Xxtx!=0) {delete [] Xxtx; Xxtx=0x0; }
  if (Xxttime!=0) {delete [] Xxttime; Xxttime=0x0; }
  if (XDiffL!=0) {delete [] XDiffL;  XDiffL=0x0;}
  if (XDiffT !=0) {delete [] XDiffT; XDiffT=0x0; }
  if (XcoeffDiffL!=0) {delete [] XcoeffDiffL; XcoeffDiffL=0x0; }
  if (XcoeffDiffT!=0) {delete [] XcoeffDiffT; XcoeffDiffT=0x0; }
  if (xtGraph!=0x0) {delete xtGraph; xtGraph=0x0; }
  //if (xtGraph!=0x0) {xtGraph->Delete(); xtGraph=0x0;}
  if (dXtFunc!=0x0) {delete dXtFunc; dXtFunc=0x0;}
  Xxtx = xtData->GetDist();
  Xxttime = xtData->GetTime();
  XDiffL = xtData->GetDiffL();
  XDiffT = xtData->GetDiffT();
  firsttime = Xxttime[0];
  mindrifttime = 0.;
  xtnbin = xtData->GetNpoints();

  firstdist = Xxtx[0];
  mindriftdist = Xxtx[0];
  curentrSpc = -1;
  driftdist = Xxtx[0];
  time = Xxttime[0];
  maxDrfTime = Xxttime[xtnbin-1]/Xxtx[xtnbin-1]*InnerRtube;
  
  if (xtGraph==0x0) {
    xtGraph_X=Xxttime;
    xtGraph_Y=Xxtx;
    if ( relType == 2 ) {
      xtGraph_ErrY=0x0;
      xtGraph_ErrX=XDiffL;
      for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrX[ixb]<=0.0) xtGraph_ErrX[ixb]=1e-6;}
    } else {
      xtGraph_ErrX=0x0;
      xtGraph_ErrY=XDiffL;
      for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrY[ixb]<=0.0) xtGraph_ErrY[ixb]=1e-6;}
    }
    xtGraph = new TGraphErrors(xtnbin,xtGraph_X,xtGraph_Y,xtGraph_ErrX,xtGraph_ErrY);
    xtFunc->FixParameter(0,0.0);
    xtGraph->Fit(xtFunc,"","",0.0,0.7*xtGraph_X[xtnbin-1]);
    //xtGraph->Fit(xtFunc);
  }
  if ( relType == 2 && dXtFunc==0x0 ) {
    dXtFunc = new TF1("dXtFunc","pol1+[2]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,0.5*xtFunc->GetParameter(3));
    /*dXtFunc = new TF1("dXtFunc","pol2+[3]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));*/
    /*dXtFunc = new TF1("dXtFunc","pol3+[4]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,4.0*xtFunc->GetParameter(4));
    dXtFunc->SetParameter(4,0.5*xtFunc->GetParameter(5));*/
    /*xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));*/
  }
  
}

Float_t* XTREL::xtadjust(Float_t time, Int_t version){
  
  if ( time <= firsttime ) {
    x[0] = 0.0;//Rwire;
    x[1] = XDiffL[0];
    x[2] = XDiffT[0];
    return x;
  }

  if (version==0) {

    if ( time <= mindrifttime && curentr != -1){
      curentr = -1;
      drifttime = 0.;
    }
    while (time>drifttime && curentr != (xtnbin-1)){
      curentr++;
      minpos = pos;
      mindrifttime = drifttime;
      drifttime = Xxttime[curentr];
      pos = Xxtx[curentr];
      x[1] = XDiffL[curentr];
      x[2] = XDiffT[curentr];
    }
    if (curentr == xtnbin-1){
      if( time>drifttime ) {
        if (drifttime==mindrifttime) x[0] = pos;
        else x[0] = (pos-minpos)/(drifttime-mindrifttime)*(time-drifttime) + pos; //linear extrapolation afer last point
      }
      else x[0] = (pos-minpos)/(drifttime-mindrifttime)*(time-mindrifttime) + minpos; //linear interpolation
  //    else x=(pos+minpos)/2;

      x[0] = (x[0] > InnerRtube) ? InnerRtube : x[0];
    }
    else{
      x[0] = (pos-minpos)/(drifttime-mindrifttime)*(time-mindrifttime) + minpos; //linear interpolation
  //    x = (pos+minpos)/2;
    }
  //cout<<curentr<<" "<<xtnbin<<" "<<"time "<<time<<" t1 "<<mindrifttime<<" t2 "<<drifttime<<" x "<<x<<" x1 "<<minpos<<" x2 "<<pos<<endl;
  //  x[0] = (x[0] < Rwire) ? Rwire : x[0];
    x[0] = (x[0] < 0.0) ? 0.0 : x[0];
    return x;
    
  } else if (version==1) {
    x[0] = xtFunc->Eval(time);
    int ibin=0;
    for ( ; ibin<xtnbin; ++ibin) {
      if (Xxttime[ibin]>time) break;
    }
    if (ibin==0) {x[1] = XDiffL[0];}
    else {x[1] = 0.5*(XDiffL[ibin-1]+XDiffL[ibin]);}
    if (relType==2) {
      x[1] *= dXtFunc->Eval(time);
    }
    x[2] = 0.0;
    return x;
  } else if (version==2) {
    x[0] = xtFunc->Eval(time);
    int ibin=0;
    for ( ; ibin<xtnbin; ++ibin) {
      if (Xxttime[ibin]>time) break;
    }
    if (ibin==0) {x[1] = XDiffL[0];}
    else {x[1] = 0.5*(XDiffL[ibin-1]+XDiffL[ibin]);}
    x[1] *= dXtFunc->Eval(time);
    x[2] = 0.0;
    return x;
  } else {
    x[0] = 0.0;
    x[1] = 0.0;
    x[2] = 0.0;
    return x;
  }

}

Float_t* XTREL::timeToSpace(Float_t time, Int_t version) {
  return xtadjust(time, version);
}

Float_t* XTREL::spaceToTime(Float_t dist, Int_t version){
  
  if ( dist <= firstdist ) {
    x[0] = 0.0;//Rwire;
    x[1] = XDiffL[0];
    x[2] = XDiffT[0];
    return x;
  }

  if (version==0) {

    if ( dist <= mindriftdist && curentrSpc != -1){
      curentrSpc = -1;
      driftdist = 0.;
    }
    while (dist>driftdist && curentrSpc != (xtnbin-1)){
      curentrSpc++;
      mintime = time;
      mindriftdist = driftdist;
      driftdist = Xxtx[curentrSpc];
      time = Xxttime[curentrSpc];
      x[1] = XDiffL[curentrSpc];
      x[2] = XDiffT[curentrSpc];
    }
    if (curentrSpc == xtnbin-1){
      if( dist>driftdist ) {
        if (driftdist==mindriftdist) x[0] = time;
        else x[0] = (time-mintime)/(driftdist-mindriftdist)*(dist-driftdist) + time; //linear extrapolation afer last point
      }
      else x[0] = (time-mintime)/(driftdist-mindriftdist)*(dist-mindriftdist) + mintime; //linear interpolation
  //    else x=(time+mintime)/2;

      x[0] = (x[0] > maxDrfTime) ? maxDrfTime : x[0];
    }
    else{
      x[0] = (time-mintime)/(driftdist-mindriftdist)*(dist-mindriftdist) + mintime; //linear interpolation
  //    x = (time+mintime)/2;
    }
  //cout<<curentrSpc<<" "<<xtnbin<<" "<<"dist "<<dist<<" t1 "<<mindriftdist<<" t2 "<<driftdist<<" x "<<x<<" x1 "<<mintime<<" x2 "<<time<<endl;
  //  x[0] = (x[0] < Rwire) ? Rwire : x[0];
    x[0] = (x[0] < 0.0) ? 0.0 : x[0];
    return x;
    
  } else if (version==1) {
    x[0] = xtFunc->Eval(dist);
    int ibin=0;
    for ( ; ibin<xtnbin; ++ibin) {
      if (Xxtx[ibin]>dist) break;
    }
    if (ibin==0) {x[1] = XDiffL[0];}
    else {x[1] = 0.5*(XDiffL[ibin-1]+XDiffL[ibin]);}
    if (relType==2) {
      x[1] *= dXtFunc->Eval(dist);
    }
    x[2] = 0.0;
    return x;
  } else if (version==2) {
    x[0] = xtFunc->Eval(dist);
    int ibin=0;
    for ( ; ibin<xtnbin; ++ibin) {
      if (Xxtx[ibin]>dist) break;
    }
    if (ibin==0) {x[1] = XDiffL[0];}
    else {x[1] = 0.5*(XDiffL[ibin-1]+XDiffL[ibin]);}
    x[1] *= dXtFunc->Eval(dist);
    x[2] = 0.0;
    return x;
  } else {
    x[0] = 0.0;
    x[1] = 0.0;
    x[2] = 0.0;
    return x;
  }

}


