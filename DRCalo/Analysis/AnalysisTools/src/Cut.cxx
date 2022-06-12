#include <Cut.h>

Cut::Cut(unsigned int index, TString name):
  m_index(index), m_name(name),m_passed(false),m_isSel(true)
{}

Cut::~Cut()
{}

void Cut::Reset()
{
  m_passed = false;
}

Selection::Selection()
{}

Selection::~Selection()
{}

void Selection::AddCut(Cut l_cut)
{
  AddCut(l_cut.GetIndex(),l_cut.GetName());
}

void Selection::AddCut(unsigned int index, TString name)
{
  Cut c(index,name);
  m_sel.push_back(c);
}

void Selection::Reset()
{
  for (auto & i : m_sel){
    i.Reset();
  }
}

Cut& Selection::GetCut(unsigned int index)
{
  return m_sel.at(index);
}

Cut& Selection::GetCut(TString name)
{
  for (auto & i : m_sel)                                                                                        
    if (!i.GetName().CompareTo(name)) return i;                                                                     
}     

