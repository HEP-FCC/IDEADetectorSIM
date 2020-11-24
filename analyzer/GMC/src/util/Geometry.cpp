#include <util/Geometry.h>
#include "TVector3.h"
#include "TMath.h"
#include "TString.h"
#include <iostream>
#include "generated/GMCAnalyzer.h"
#include "generated/GMCGlobalSteering.h"

//change if you want to rotate the detector by ROTANGLE
#define ROTATE 0
#define ROTANGLE 0.21
//change if you want that the central tube has a different staggering
#define stagCentral 0
#define dstagCentral 2.5

// H/m vacuum permeability
#define mu0 4.e-7*TMath::Pi()   
// pF/m vacuum dielectric constant
#define epsilon0 1.e12/(mu0*TMath::C()*TMath::C())   


using namespace std;

ClassImp(Geometry)

Geometry* Geometry::Instance = 0;

//_________________________________________________________________________
Geometry::Geometry()
{

  TVector3 c(0.,0.,0.);
  TVector3 zed(0.,0.,1.);

  Int_t numberOfTubes = 8;
  fTubeSize = 9.75;  //in mm
  fAnodeSize = 0.020; //in mm
  fMylarWidth = 0.036;  //in mm
  Double_t Angle = 0.;

  wire_posX[0] = 9999999;
  wire_posY[0] = 9999999;
  wire_posZ[0] = 9999999;
  wire_dirX[0] = 9999999;
  wire_dirY[0] = 9999999;
  wire_dirZ[0] = 9999999;

  Double_t staggering = 0.5*fTubeSize + fMylarWidth; 

  for(int irows=0; irows<2; irows++){

    for(int itb=0; itb<numberOfTubes; itb++){

      c.SetX((1. - numberOfTubes + itb*2.)*staggering);
     
      if(itb%2 == 0) c.SetY((1 - irows)*2.*staggering);
      else c.SetY((1 - 2.*irows)*staggering);
    
      c.SetZ(0.);

      //rotation matrix 
      //     | cos th  0 -sin th  |
      // M = |   0     1    0     |   ==> newV = M * c
      //     | sin th  0  cos th  |

      //wire position at plane z=0
      if (Angle > 0.) {
	wire_posX[numberOfTubes*irows+itb] = 
	  c.X()*TMath::Cos(Angle + 0.5*TMath::Pi()) - c.Z()*TMath::Sin(Angle + 0.5*TMath::Pi());
	wire_posY[numberOfTubes*irows+itb] = c.Y();
	wire_posZ[numberOfTubes*irows+itb] = 
	  c.X()*TMath::Sin(Angle + 0.5*TMath::Pi()) + c.Z()*TMath::Cos(Angle + 0.5*TMath::Pi());

	//wire vector
	wire_dirX[numberOfTubes*irows+itb] = -TMath::Sin(Angle + 0.5*TMath::Pi());
	wire_dirY[numberOfTubes*irows+itb] = 0.;
	wire_dirZ[numberOfTubes*irows+itb] = TMath::Cos(Angle + 0.5*TMath::Pi());
      }
      else {
	wire_posX[numberOfTubes*irows+itb] = - c.Z();
	wire_posY[numberOfTubes*irows+itb] = c.Y();
	wire_posZ[numberOfTubes*irows+itb] = c.X();

	//wire vector
	wire_dirX[numberOfTubes*irows+itb] = -1.;
	wire_dirY[numberOfTubes*irows+itb] = 0.;
	wire_dirZ[numberOfTubes*irows+itb] = 0.;
      }
    }
    
  }

  PixelConstruction();

  //Calculate the elctrical parameters
  InitElectricalParam();

  fCablingFile = gAnalyzer->GetGSP()->GetCablingFile();

  fOffsetFile = gAnalyzer->GetGSP()->GetTimeOffsetsFile();

  fAlignmentFile = gAnalyzer->GetGSP()->GetAlignmentFile();

  //  ReadConfigurationFiles();

}

//_________________________________________________________________
Geometry::~Geometry()
{

}

//__________________________________________________________________
Geometry *Geometry::GetInstance()
{

  if(Instance) return Instance;

  Instance = new Geometry();
  return Instance; 

}

//__________________________________________________________________
void Geometry::PixelConstruction(){

  //set the active area and pitch of pixels
  fPXModuleLength = 14.25;  //in mm The active region is square
  fPixelSize = 55.;  //in micron
  fPixelPitch = 0.667; //in micron
  fNumberOfPixel = 256; 
  fDistanceFromDCH = 50.; //in mm

}

//__________________________________________________________________
void Geometry::InitElectricalParam(){

  //Tube capacitance  C = 2*pi*eps0/log(Rtube/Rwire) 
  fCapacitance = 2.*TMath::Pi()*epsilon0/TMath::Log(fTubeSize/fAnodeSize); //in pF/m

  //Tube impedance sqrt(L/C)
  fZtube = TMath::Sqrt(mu0/(1.e-12*epsilon0))*TMath::Log(GetTubeSize()/GetAnodeSize())/(2.*TMath::Pi());
  
  
}

//__________________________________________________________________
void Geometry::ReadConfigurationFiles(){

  //read the cabling file

  cout<<"leggo il file "<<fCablingFile.Data()<<endl;
  ifstream cabling(fCablingFile.Data());

  int wire;

  cout << "CABLING" << endl;
  for(Int_t i=0;i<17;i++){
    tdc[wire] = -1;
  }

  for(Int_t i=0;i<17;i++){

    cabling >> wire ;
    cabling >> tdc[wire];

    if(wire != 0)
      cout << "Wire " << wire << " ---- TDC channel " << tdc[wire] << endl;
    else
      cout << "Trigger ---- TDC channel " << tdc[0] << endl;

  }

  cabling.close();

  //read the offset parameters

  ifstream offsets(fOffsetFile.Data());

  for(Int_t i=0;i<17;i++){

    offsets >> wire ;
    offsets >> offset[wire];

    if(wire != 0)
      cout << "Wire " << wire << " ---- Offset " << offset[wire] << endl;

  }

  offsets.close();

  //read the alignment file

  ifstream align(fAlignmentFile.Data());

  for(Int_t i=0;i<17;i++){

    align >> wire ;
    align >> alignment[wire];

  }

  align.close();

}

//__________________________________________________________________
Int_t Geometry::GetWireIndex(Int_t tdc_ch) {

  for(Int_t i=0;i<17;i++)
    if(tdc_ch == tdc[i]) return i;

  return -1;

}

//__________________________________________________________________
Double_t Geometry::T(Double_t D){

  if(D < 0) return 0.;
  
  return((29.95*D) + 3.087*D*D);

}

//__________________________________________________________________
Double_t Geometry::D(Double_t t){

  if(t < 0) return 0.;

  return (-29.95 + sqrt(29.95*29.95 + 4.*3.087*(t)))/(2.*3.087);

}
