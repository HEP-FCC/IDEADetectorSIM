#include "util/Waveform.h"
#include "TAxis.h"
#include <iostream>

using namespace std;

ClassImp(Waveform)

Waveform::Waveform()
{

  fNPoints = 0;
  fTime = 0;
  fAmpl = 0;
  fGraph = 0;

}

Waveform::Waveform(Int_t npoints)
{

  fNPoints = npoints;
  fTime = new Double_t[npoints];
  fAmpl = new Double_t[npoints];

}

Waveform::~Waveform()
{

  if(fTime) delete [] fTime;
  fTime = 0;

  if(fAmpl) delete [] fAmpl;
  fAmpl = 0;

}

void Waveform::SetNPoints(Int_t npoints)
{

  fNPoints = npoints;

  delete [] fTime;
  delete [] fAmpl;

  fTime = new Double_t[npoints];
  fAmpl = new Double_t[npoints];

}

void Waveform::Set(Int_t npoints, Double_t *time, Double_t *ampl)
{

  fNPoints = npoints;

  delete [] fTime;
  delete [] fAmpl;

  fTime = new Double_t[npoints];
  fAmpl = new Double_t[npoints];

  for(Int_t i=0;i<fNPoints;i++){

    fTime[i] = time[i];
    fAmpl[i] = ampl[i];

  }

}

void Waveform::Add(Double_t *ampl)
{

  for(Int_t i=0;i<fNPoints;i++) fAmpl[i] += ampl[i];

}

TGraph *Waveform::GetGraph()
{ 

  if(!fTime || !fAmpl) { cout << "Waveform not set -- cannot build the graph" << endl; return 0; }

  if(!fGraph) fGraph = new TGraph(fNPoints);

  for(Int_t i=0;i<fNPoints;i++){

    fGraph->SetPoint(i,fTime[i],fAmpl[i]);

  }

  fGraph->GetXaxis()->SetRangeUser(fTime[0],fTime[fNPoints-1]);

  return fGraph;

}
