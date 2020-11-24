#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#endif
#include <TProfile.h>
#include <fstream>
#include <string>
#include "XTRELTIME.h"

ClassImp(XTRELTIME)
using namespace std;

XTRELTIME::XTRELTIME( Float_t rwire, Float_t rtube, Float_t V, const char *Fname) : XTREL(rwire, rtube, V, Fname) {
  xtfilename=TString(Fname);
//  xtfilename.Replace(xtfilename.First("dat")+1,4,"root");
  xtfilename.ReplaceAll(".dat",".root");
  xtfilename.Insert(0,"XTtr_from");
  Init();
  WriteXT();

  relType = 2;
  xtGraph_X=Xxttime;
  xtGraph_Y=Xxtx;
  xtGraph_ErrX=XDiffL;
  xtGraph_ErrY=0x0;
  delete xtFunc;
  //xtFunc = new TF1("xtFunc","pol3+[4]*sqrt(x)");
  xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
  for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrX[ixb]<=0.0) xtGraph_ErrX[ixb]=1e-6;}
  if (xtGraph!=0x0) { delete xtGraph; }
    xtGraph = new TGraphErrors(xtnbin,xtGraph_X,xtGraph_Y,xtGraph_ErrX,xtGraph_ErrY);
    xtFunc->FixParameter(0,0.0);
    xtGraph->Fit(xtFunc,"w","",0.0,0.7*xtGraph_X[xtnbin-1]);
    //xtGraph->Fit(xtFunc);
  if (dXtFunc==0x0) {
    /*
    dXtFunc = new TF1("dXtFunc","pol1+[2]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,0.5*xtFunc->GetParameter(3));*/
    dXtFunc = new TF1("dXtFunc","pol2+[3]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));
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

XTRELTIME::XTRELTIME(const char *XTFname ) : XTREL(XTFname) {
  relType = 2;
  xtGraph_X=Xxttime;
  xtGraph_Y=Xxtx;
  xtGraph_ErrX=XDiffL;
  xtGraph_ErrY=0x0;
  delete xtFunc;
  xtFunc = new TF1("xtFunc","[0]+[1]*pow(x,1./2.)+[2]*x+[3]*pow(x,3./2.)");
  //xtFunc = new TF1("xtFunc","pol3+[4]*sqrt(x)");
  for (int ixb=0; ixb<xtnbin; ++ixb) {if (xtGraph_ErrX[ixb]<=0.0) xtGraph_ErrX[ixb]=1e-6;}
  if (xtGraph!=0x0) { delete xtGraph; }
    xtGraph = new TGraphErrors(xtnbin,xtGraph_X,xtGraph_Y,xtGraph_ErrX,xtGraph_ErrY);
    xtFunc->FixParameter(0,0.0);
    xtGraph->Fit(xtFunc,"w","",0.0,0.7*xtGraph_X[xtnbin-1]);
    cout<<"xtGraph: "<<xtGraph->GetN()<<endl;
    //xtGraph->Fit(xtFunc);
  if (dXtFunc==0x0) {
    /*dXtFunc = new TF1("dXtFunc","pol1+[2]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,0.5*xtFunc->GetParameter(3));*/
    dXtFunc = new TF1("dXtFunc","pol2+[3]/sqrt(x)");
    dXtFunc->SetParameter(0,xtFunc->GetParameter(1));
    dXtFunc->SetParameter(1,2.0*xtFunc->GetParameter(2));
    dXtFunc->SetParameter(2,3.0*xtFunc->GetParameter(3));
    dXtFunc->SetParameter(3,0.5*xtFunc->GetParameter(4));
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


XTRELTIME::~XTRELTIME(){
}

void XTRELTIME::Init(){
//   Int_t fpos, fpos2;
  fstream grfile(filename,ios::in);
  string buffer;
  getline(grfile, buffer);
  getline(grfile, buffer);
  getline(grfile, buffer);
  
//  Float_t oldE = 0., stepTime = 0., stepDist = 0., stepDiffL = 0., stepDiffT = 0., deltar = 0.;
//  cout<<"Econst "<<Econst<<" Emax "<<Emax<<" Emin "<<Emin<<endl;
  float tempr, tempt, /*tempvd,*/ tempsdl, tempsdt=0., tempdl=0., tempdt=0.;
  std::vector<float > tr, tt, tdl, tdt, tsdl, tsdt;

  while (!grfile.eof()) {
    grfile>>tempr;
//    cout<<xtnbin<<" "<<tempr<<endl;
//    if (xtnbin!=0 && tr[xtnbin]==tempr) break;
    tr.push_back(tempr);
    grfile>>tempt;
//    cout<<xtnbin<<" "<<tempt<<endl;
    tt.push_back(tempt);
    grfile>>tempsdl;
//    cout<<xtnbin<<" "<<tempsdl<<endl;
    tsdl.push_back(tempsdl);
    tsdt.push_back(tempsdt);
    tdl.push_back(tempdl);
    tdt.push_back(tempdt);
    xtnbin++;
  }
  xtnbin--;
  Xxtx = new Float_t [xtnbin];
  Xxttime =  new Float_t [xtnbin];
  XDiffL =  new Float_t [xtnbin];
  XDiffT =  new Float_t [xtnbin];
  XcoeffDiffL =  new Float_t [xtnbin];
  XcoeffDiffT =  new Float_t [xtnbin];
  for(Int_t idarray = 0; idarray<xtnbin; idarray++){
    Xxtx[idarray] = tr[idarray];
    Xxttime[idarray] = tt[idarray];
    XDiffL[idarray] = tsdl[idarray];
    XDiffT[idarray] = tsdt[idarray];
    XcoeffDiffL[idarray] = tdl[idarray];
    XcoeffDiffT[idarray] = tdt[idarray];
  }


/*   while (!grfile.eof()) {
    xtnbin++;
    radius->ResizeTo(xtnbin);
    integrTime->ResizeTo(xtnbin);
    integrDiffL->ResizeTo(xtnbin);
    integrDiffT->ResizeTo(xtnbin);
    coeffDiffL->ResizeTo(xtnbin);
    coeffDiffT->ResizeTo(xtnbin);
    grfile>>tempr;
    cout<<xtnbin<<" "<<idarray<<" "<<tempr<<endl;
    radius[idarray] = (Float_t) tempr;
    grfile>>tempe;
    grfile>>tempt;
    cout<<xtnbin<<" "<<tempt<<endl;
    integrTime[idarray] = (Float_t) tempt;
    grfile>>tempvd;
    grfile>>tempdl;
    cout<<xtnbin<<" "<<tempdl<<endl;
    coeffDiffL[idarray] = (Float_t) tempdl;
    grfile>>tempdt;
    cout<<xtnbin<<" "<<tempdt<<endl;
    coeffDiffT[idarray] =  (Float_t) tempdt;
    grfile>>tempsdl;
    cout<<xtnbin<<" "<<tempsdl<<endl;
    integrDiffL[idarray] = (Float_t) tempsdl;
    grfile>>tempsdt;
    cout<<xtnbin<<" "<<tempsdt<<endl;
    integrDiffT[idarray] = (Float_t) tempsdt;
    idarray = xtnbin;
  }
 */

/*   Xxtx = radius->GetMatrixArray();
  Xxttime = integrTime->GetMatrixArray();
  XDiffL = integrDiffL->GetMatrixArray();
  XDiffT = integrDiffT->GetMatrixArray();
  XcoeffDiffL = coeffDiffL->GetMatrixArray();
  XcoeffDiffT = coeffDiffT->GetMatrixArray();
 */
  grfile.close();
}
