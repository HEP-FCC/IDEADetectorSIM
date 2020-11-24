#include "../macros/RecWave.h"

#include "util/XTRELTIME.h"
#include "util/Geometry.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TRandom.h"

void Prova(Int_t Ntry=100) {


  XTREL *fxt = new XTRELTIME("XTData/X_T_Relation_8515.root"); 
  
  TH2F *gr = new TH2F("0002","Time2space",500,0.,0.2,500,0.,5.);

  float xcoor;
  float *ycoor;
  for (int i=0;i<Ntry;i++) {
    xcoor = 0.2*gRandom->Uniform();

    ycoor = fxt->timeToSpace(xcoor,0);
    cout<<"(x,y) "<<xcoor<<"  "<<ycoor[0]<<endl;
    if (fabs(ycoor[0]-0.4) < 0.00001) {
      ycoor[0] = 0.0345 - 3.438*xcoor*xcoor + 3.094*xcoor;
      cout<<"qui nn entro mai "<<ycoor[0]<<endl;
    }
    //    if (distance[icls] >= 4.) t_cluster[icls] = (3.094-TMath::Sqrt(3.094*3.094 +// 4.*3.438*(0.0345-0.1*distance[icls])))/(2.*3.438);

    gr->Fill(xcoor,10.*ycoor[0]);
  }

  TCanvas *myC = new TCanvas();
  gr->DrawCopy();

}
