#ifndef ANALYSISHISTOBASE_H
#define ANALYSISHISTOBASE_H

#include <TH1D.h>
#include <TH2F.h>
#include <TH3F.h>
#include <THnSparse.h>
#include <TProfile.h>
#include <TString.h>
#include <TFile.h>
#include <TObject.h>

#include <vector>

// Framework includes

#include <Cut.h>


// This is the class that manages the histograms. To see an example of the use, see UpgradeAnalysis

class AnalysisHistoBase {

public:

  AnalysisHistoBase(TFile & l_file, Selection & l_sel);
  virtual ~AnalysisHistoBase();

  virtual bool Init();
  bool Finalize();

  //void SetInitialEventsInfo(TH1D* initialEvents, TH1D* initialEvents_unw, TH1D* initialEvents_MCw, TH1D* crossSection);
  void SetInitialEventsInfo(TH1D* initialEvents, TH1D* initialEvents_unw, TH1D* initialEvents_MCw);

  void AddTH1F(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup);
  void AddTH2F(const char* name, const char* title,
    Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
  void AddTH3F(const char* name, const char* title,
    Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, Int_t nbinsz, Double_t zlow, Double_t zup);

  TString GetDirName(Cut l_cut);

  TObject * GetObject(Cut l_cut, TString h_name);

  // event info histo
  TH1D * h_eventInfo;
  TH1D * h_eventInfo_unw;

  TH1D* h_InitialEvents;
  TH1D* h_InitialEvents_unw;

  TH1D* h_xsec;
  TH1D* h_sumEventWeights;
  TH1D* h_mc_channel;

protected:

  TFile & m_outfile;

  Selection & m_sel;

};

#endif
