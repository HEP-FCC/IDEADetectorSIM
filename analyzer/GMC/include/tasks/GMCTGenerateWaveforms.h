// Author: AuroraPepino

#ifndef GMCTGenerateWaveforms_H
#define GMCTGenerateWaveforms_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// GMCTGenerateWaveforms                                                      //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "TF1.h"

#include "util/XTREL.h"
#include "generated/GMCTGenerateWaveforms_Base.h"
#include "generated/GMCDCHHit.h"
#include "util/Geometry.h"


//#include "generated/GMCTGenerateWaveforms_Base.h"

class GMCTGenerateWaveforms : public GMCTGenerateWaveforms_Base
{
 
 private:

  Double_t fTimeres;       //time resolution ADC
  Double_t fTimeWindow;    //Acquisition Time Window
  Double_t fConst1;        //time costant used to simulate signal rampDown
  Double_t fConst2;        //time costant used to simulate signal rampUp
  Double_t fVo;            //signal peak value (in mV)
  Double_t fVo_1;            //signal peak value (in mV)
  Double_t fGasGain;       //gas gain
  Int_t    fNrOfBins;      //number of waveform bins
  Double_t fSignalShapePar[6]; //par for the Tax's waveforms shape
  Double_t fNoiseLevel;     //level of noise
  Double_t fDigitStep;
  Int_t    fADCbin;

  Double_t *signL;         //signal on the left side
  Double_t *signR;         //signal on the right side
  
  
  XTREL     *fxt; //variab globale (f davanti) by Chiri
  Geometry  *fGeometry;
  TF1       *fPolya;     //Polya function to calculate the avalanche charge
  
  int rumpUp;



private:
  GMCTGenerateWaveforms(const GMCTGenerateWaveforms &c); // not implemented
  GMCTGenerateWaveforms &operator=(const GMCTGenerateWaveforms &c); // not implemented
  
public:
 GMCTGenerateWaveforms(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)
   :GMCTGenerateWaveforms_Base(name,title,level,taskSuffix,histoFolder) {}
  virtual ~GMCTGenerateWaveforms() {}
  

 protected:
   // Event Methods
  void Init();
  void BeginOfRun();
  void Event();
  void EndOfRun();
  void Terminate();
  
  void       SetSignalParameters();
  void       InitPolyaFunction();
  void       CreateSignal(Double_t time, Double_t pValue);   
  void       CreateSignal2(Double_t time, Double_t pValue);
  Int_t      CreateSignal3(Double_t time, Double_t pValue);
  void       AddNoise();
  void       FEGain();
  void       WaveDigitization();   
  Double_t*  DefinePeakValues(GMCDCHHit* ht);
  Double_t*  DefineTimeValues(GMCDCHHit* ht);
  Double_t   DefineNoiseLevel(); 

  ClassDef(GMCTGenerateWaveforms,0) // Generate waveforms in DCH
};

#endif   // GMCTGenerateWaveforms_H
