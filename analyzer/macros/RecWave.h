struct RecWave {

  Int_t NrOfPeaks;
  Int_t NrClsRec;
  Int_t ClsMultiRec[100];
  Double_t ClsTimeRec[100];
  Double_t PeakAmplitude[200];
  Double_t PeakTime[200];

};

struct MCWave {

  Int_t NrTube;
  Int_t IdTrk;
  Int_t TotCharge;
  Int_t NrCls;
  Int_t ClsMulti[100];
  Double_t bTrue;
  Double_t DriftD[100];
  Double_t EleTime[200];
  Double_t EleCharge[200];

};

struct wClsData {
  Int_t idx;
  Int_t nEl;
  Double_t driftD;
  Double_t timeD; 
  Double_t charge;
  Double_t peakAmpl;
};
