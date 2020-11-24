#ifndef GEOMETRYPROTO_H
#define GEOMETRYPROTO_H

#include <TVector3.h> 
#include <TObject.h> 
#include <TString.h>
#include <TGraph.h> 

class Geometry : public TObject {

 protected:

  Int_t    fNumberOfPixel;
  Double_t fPXModuleLength;
  Double_t fPixelSize;
  Double_t fPixelPitch;
  Double_t fDistanceFromDCH;

  Double_t fTubeSize;  //in mm
  Double_t fAnodeSize; //in mm
  Double_t fMylarWidth;  //in mm

  Double_t wire_posX[16];
  Double_t wire_posY[16];
  Double_t wire_posZ[16];

  Double_t wire_dirX[16];
  Double_t wire_dirY[16];
  Double_t wire_dirZ[16];

  int tdc[17];
  Double_t offset[17];
  Double_t alignment[17];

  //extern file
  TString fCablingFile;
  TString fOffsetFile;
  TString fAlignmentFile;

  //Tube electrical parameters
//  Double_t fLogRadiiRatio;
  Double_t fCapacitance;
  Double_t fZtube;

  Geometry();
  virtual ~Geometry();

 private:

  static Geometry *Instance;

 public:

  static Geometry *GetInstance();
  void     PixelConstruction();
  void     InitElectricalParam();
  void     ReadConfigurationFiles();
  Int_t    GetNumberOfPixel()       { return fNumberOfPixel; }
  Double_t GetPixelModuleLength()   { return fPXModuleLength; }
  Double_t GetPixelSize()           { return fPixelSize; }
  Double_t GetPixelPitch()          { return fPixelPitch; }
  Double_t GetDistanceFromDCH()     { return fDistanceFromDCH; }  

  Double_t GetTubeSize()            { return fTubeSize; }
  Double_t GetAnodeSize()           { return fAnodeSize; }  
  Double_t GetMylarWidth()          { return fMylarWidth; }
  Double_t Getwire_posX(Int_t wire) { return wire_posX[wire]; }
  Double_t Getwire_posY(Int_t wire) { return wire_posY[wire]; }
  Double_t Getwire_posZ(Int_t wire) { return wire_posZ[wire]; }
  TVector3 Getwire_pos(Int_t wire) { return TVector3(wire_posX[wire],wire_posY[wire],wire_posZ[wire]); }
  TVector3 Getwire_dir(Int_t wire) { return TVector3(wire_dirX[wire],wire_dirY[wire],wire_dirZ[wire]); }


  //Electrical parameters
  Double_t GetTubeCapacitance()     { return fCapacitance; }
  Double_t GetTubeImpedance()       { return fZtube; }
  
  Int_t GetTDCChannel(Int_t wire) { return tdc[wire]; };
  Int_t GetWireIndex(Int_t tdc_ch);
  Double_t GetTimeOffset(Int_t wire) { return offset[wire]; }

  void SetCablingFile(TString fName="Cabling.dat") 
  { fCablingFile = fName; };
  void SetOffsetFile(TString fName="Offset.dat") 
  { fOffsetFile = fName; };
  void SetAlignmentFile(TString fName="Alignment.dat") 
  { fAlignmentFile = fName; };

  Double_t T(Double_t t);
  Double_t D(Double_t t);
  
  ClassDef(Geometry,1) //Geometry class
  
};

#endif 
