#include "AnalysisHistoBase.h"

#include <iostream>

AnalysisHistoBase::AnalysisHistoBase(TFile & l_outfile, Selection & l_sel):
  h_eventInfo(0),
  h_eventInfo_unw(0),
  h_InitialEvents(0),
  h_InitialEvents_unw(0),
  h_xsec(0),                                                                                                                                                                        
  h_sumEventWeights(0),                                                                                                                                                               
  h_mc_channel(0),     
  m_outfile(l_outfile),
  m_sel(l_sel)
{
}

AnalysisHistoBase::~AnalysisHistoBase()
{}

void AnalysisHistoBase::AddTH1F(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
{
  for (auto i : m_sel.GetVector()){
    m_outfile.cd(GetDirName(i));
    TH1F* h;
    h = new TH1F(name, title, nbinsx, xlow, xup);
    h->Sumw2();
  }
}

void AnalysisHistoBase::AddTH2F(const char* name, const char* title,
   Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
{
  for (auto i : m_sel.GetVector()){
    m_outfile.cd(GetDirName(i));
    TH2F* h;
    h = new TH2F(name, title, nbinsx, xlow, xup, nbinsy, ylow, yup);
    h->Sumw2();
  }
}

void AnalysisHistoBase::AddTH3F(const char* name, const char* title,
   Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, Int_t nbinsz, Double_t zlow, Double_t zup)
{
  for (auto i : m_sel.GetVector()){
    TH3F* h;
    h = new TH3F(name, title, nbinsx, xlow, xup, nbinsy, ylow, yup, nbinsz, zlow, zup);
    h->Sumw2();
  }
}

TObject * AnalysisHistoBase::GetObject(Cut l_cut,TString histoname)
{
  return m_outfile.GetDirectory(GetDirName(l_cut))->Get(histoname);
}  


bool AnalysisHistoBase::Init()
{

  std::cout << "AnalysisHistoBase::Init()" << std::endl;   

  if (!m_outfile.IsOpen()) {
    std::cout << "AnalysisHistoBase: ERROR - The output file is not opened. Exiting" << std::endl;
    return false;
  }

  unsigned int ncuts = m_sel.GetVector().size();

  m_outfile.cd();
  TDirectory * m_dirsyst = m_outfile.CurrentDirectory();

  m_dirsyst->mkdir("InfoHistograms");
  m_outfile.cd("InfoHistograms");

  h_eventInfo = new TH1D("h_eventInfo", "", 50, 0, 50);
  h_eventInfo->Sumw2();

  h_eventInfo_unw = new TH1D("h_eventInfo_unw", "", 50, 0, 50);
  h_eventInfo_unw->Sumw2();

  h_InitialEvents     = new TH1D("h_InitialEvents",     "", 1, 0, 1);
  h_InitialEvents->Sumw2();

  h_InitialEvents_unw = new TH1D("h_InitialEvents_unw", "", 1, 0, 1);
  h_InitialEvents_unw->Sumw2();

  h_xsec = new TH1D("h_xsec","",1,0,1);                                              
  h_xsec->Sumw2();                       
  h_sumEventWeights = new TH1D("h_sumEventWeights","",1,0,1);
  h_sumEventWeights->Sumw2();
  h_mc_channel = new TH1D("h_mc_channel","",1,0,1);
  h_mc_channel->Sumw2();

  for (unsigned int i = 0; i < ncuts; ++i){
    m_dirsyst->mkdir(GetDirName(m_sel.GetCut(i)));
  }

  return true;
}

bool AnalysisHistoBase::Finalize()
{
  return true;
}

TString AnalysisHistoBase::GetDirName(Cut l_cut)
{
  TString dirname = "Cut";
  dirname += l_cut.GetIndex();
  dirname += ("_" + l_cut.GetName());
  return dirname;
}
  

      
  
