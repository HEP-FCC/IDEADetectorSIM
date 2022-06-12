#ifndef CUT_H
#define CUT_H

#include <vector>

#include <TString.h>

class Cut
{
 public:
  Cut(unsigned int index, TString name);
  ~Cut();
  void Reset();
  unsigned int GetIndex() {return m_index;}
  TString GetName() {return m_name;}
  bool IsPassed() {return m_passed;}
  void SetPassed(bool passed) {m_passed = passed;}
  bool StopSelIfFailed() {return m_isSel;}
  void SetSelIfFailed(bool isSel = true) {m_isSel = isSel;}
 private:
  unsigned int m_index;
  TString m_name;
  bool m_passed;
  bool m_isSel;
};

class Selection
{
 public:
  Selection();
  ~Selection();
  void AddCut(Cut l_cut);
  void AddCut(unsigned int index, TString name);
  void Reset();
  Cut& GetCut(unsigned int index);
  Cut& GetCut(TString name);
  std::vector<Cut> & GetVector() {return m_sel;}
 private:
  std::vector<Cut> m_sel;
};
  
  
#endif
