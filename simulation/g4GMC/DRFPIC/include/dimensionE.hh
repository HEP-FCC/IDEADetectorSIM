// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#ifndef dimensionE_h
#define dimensionE_h 1

#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "globals.hh"
#include <string.h>
#include <vector>


using namespace std;
/// Detector construction

class dimensionE
{
	public:
		dimensionE();
		~dimensionE();
		///////////////////////////
		// initialization of parameter
		void Rbool(G4bool Rbool) { fRbool = Rbool; }
		void SetInnerR(G4double innerR) { finnerR = innerR; }
		void SetTower_height(G4double tower_height) { ftower_height = tower_height; }
		void SetNumZRot(G4int num) { fnumzrot = num; fPhiZRot = 2*M_PI/(G4double)num; }
		void SetDeltaTheta(G4double theta) { fdeltatheta = theta; }
		void SetThetaOfCenter(G4double theta) { fthetaofcenter = theta; }
        void SetDeltaTheta2(G4double theta) { fdeltatheta2 = theta; }
        void SetThetaOfCenter2(G4double theta) { fthetaofcenter2 = theta; }
		void SetPMTT(G4double PMTT) { fPMTT = PMTT; }

		void CalBasic();
		/////////////////////////
		//methods for calculations


		///////////////////////
		// get
		G4double GetInnerR_new();
		G4double GetTrns_Length();
		G4ThreeVector GetTrns_Vector();
		G4ThreeVector GetV1();
		G4ThreeVector GetV2();
		G4ThreeVector GetV3();
		G4ThreeVector GetV4();

		G4ThreeVector GetOrigin(G4int i);
		void Getpt(G4ThreeVector *pt);
		
		G4ThreeVector GetOrigin_PMTG(G4int i);
		void Getpt_PMTG(G4ThreeVector *pt);

		void Getpt_PMTCath(G4ThreeVector *pt);

		G4RotationMatrix* GetRM(G4int i);
	private:
		G4double fPhiZRot;
		G4bool fRbool;
		G4bool fcalbasicbool;
        G4double finnerR;
		G4double ftower_height;
		G4double fnumzrot;
		G4double fdeltatheta;
		G4double fthetaofcenter;
		G4double finnerR_new;
		G4double fTrns_Length;
		G4ThreeVector fTrns_Vector;
		G4ThreeVector fV1;
		G4ThreeVector fV2;
		G4ThreeVector fV3;
		G4ThreeVector fV4;
		G4double fPMTT;
        G4double fthetaofcenter2;
        G4double fdeltatheta2;
        G4double finnerR_new2;
        G4double Ratio;
        G4double Ratio2;
		////////////////////


	protected:

};
/*
dimensionE::dimensionE()
{	fRbool = 0;
	fcalbasicbool = 0;
	ftower_height = 0;
	fnumzrot = 0;
	fdeltatheta = 0;
	fthetaofcenter = 0;
	finnerR_new = 0;	
	fPhiZRot = 0;
	fV1 = G4ThreeVector();
	fV2 = G4ThreeVector();
	fV3 = G4ThreeVector();
	fV4 = G4ThreeVector();
	
}

dimensionE::~dimensionE()
{

}

void dimensionE::CalBasic()
{
	
	fcalbasicbool = 1;


	fTrns_Length = ftower_height/2.+finnerR_new;
	
	fTrns_Vector =  G4ThreeVector(cos(fthetaofcenter)*fTrns_Length,0,sin(fthetaofcenter)*fTrns_Length);
	
	fV1 = G4ThreeVector(
			cos(fthetaofcenter)*finnerR_new+sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.),
			0,
			sin(fthetaofcenter)*finnerR_new-sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)
			);
	
	fV2 = G4ThreeVector(
			cos(fthetaofcenter)*(finnerR_new+ftower_height)+sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.),
			0,
			sin(fthetaofcenter)*(finnerR_new+ftower_height)-sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)
			);
	
	fV3 = G4ThreeVector(
			cos(fthetaofcenter)*finnerR_new-sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.),
			0,
			sin(fthetaofcenter)*finnerR_new+sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)
			);
	
	fV4 = G4ThreeVector(
			cos(fthetaofcenter)*(finnerR_new+ftower_height)-sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.),
			0,
			sin(fthetaofcenter)*(finnerR_new+ftower_height)+sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)
			);

}

G4double dimensionE::GetInnerR_new() {

	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return 0;
	} 
	else return finnerR_new; 
}
	
G4double dimensionE::GetTrns_Length() { 
	
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return 0;
	} 
	else return fTrns_Length; 
}

G4ThreeVector dimensionE::GetTrns_Vector() { 
	
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	} 
	else return fTrns_Vector;
}

G4ThreeVector dimensionE::GetV1() {
	
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	} 
	else return fV1;
}

G4ThreeVector dimensionE::GetV2() {
	
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	} 
	else return fV2;
}

G4ThreeVector dimensionE::GetV3() {
	
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	}
	else return fV3;
}

G4ThreeVector dimensionE::GetV4() {
	
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	}
	else return fV4;
}

G4ThreeVector dimensionE::GetOrigin(G4int i) {

	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	}
	else{ 
		if(fRbool==1){
			G4double x,y,z;
			x=cos(i*fPhiZRot)*fTrns_Vector.getX();
			y=sin(i*fPhiZRot)*fTrns_Vector.getX();
			z=fTrns_Vector.getZ();

			return G4ThreeVector(x,y,z);
		}

		else
		{
			G4double x,y,z;
			x=cos(i*fPhiZRot)*fTrns_Vector.getX();
			y=sin(i*fPhiZRot)*fTrns_Vector.getX();
			z=-fTrns_Vector.getZ();

			return G4ThreeVector(x,y,z);
		}
	}
}

G4ThreeVector dimensionE::GetOrigin_PMTG(G4int i) {
	if(fcalbasicbool==0) 
	{cout<<"fcalbasicbool = 0"<<endl; 
		return G4ThreeVector();
	}
	else{ 
		if(fRbool==1){
			G4double x,y,z;
			x=cos(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT*1.5/2.)/fTrns_Vector.mag();
			y=sin(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT*1.5/2.)/fTrns_Vector.mag();
			z=fTrns_Vector.getZ()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT*1.5/2.)/fTrns_Vector.mag();

			return G4ThreeVector(x,y,z);
		}

		else
		{
			G4double x,y,z;
			x=cos(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT*1.5/2.)/fTrns_Vector.mag();
			y=sin(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT*1.5/2.)/fTrns_Vector.mag();
			z=-fTrns_Vector.getZ()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT*1.5/2.)/fTrns_Vector.mag();

			return G4ThreeVector(x,y,z);
		}
	}
}

void dimensionE::Getpt(G4ThreeVector *pt) {
	G4double innerSide_half = finnerR_new*tan(fdeltatheta/2.);
	G4double outerSide_half= (finnerR_new+ftower_height)*tan(fdeltatheta/2.);

	if(fRbool == 1){
		pt[0]=G4ThreeVector(-(fV3.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
		pt[1]=G4ThreeVector((fV3.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
		pt[2]=G4ThreeVector(-(fV1.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
		pt[3]=G4ThreeVector((fV1.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
		pt[4]=G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
		pt[5]=G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
		pt[6]=G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
		pt[7]=G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
	}

	else{
		pt[0]=G4ThreeVector(-(fV1.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
		pt[1]=G4ThreeVector((fV1.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
		pt[2]=G4ThreeVector(-(fV3.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
		pt[3]=G4ThreeVector((fV3.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
		pt[4]=G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
		pt[5]=G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
		pt[6]=G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
		pt[7]=G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
	}
}

void dimensionE::Getpt_PMTG(G4ThreeVector *pt) {
	G4double innerSide_half = finnerR_new*tan(fdeltatheta/2.);
	G4double outerSide_half= (finnerR_new+ftower_height)*tan(fdeltatheta/2.);

	if(fRbool == 1){
		pt[0] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT*1.5/2.);
		pt[1] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT*1.5/2.);
		pt[2] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT*1.5/2.);          
		pt[3] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT*1.5/2.);
		pt[4] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT*1.5/2.);
		pt[5] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT*1.5/2.);
		pt[6] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT*1.5/2.);
		pt[7] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT*1.5/2.);
	}
	
	else{
		pt[0] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT*1.5/2.);
		pt[1] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT*1.5/2.);
		pt[2] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT*1.5/2.);          
		pt[3] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT*1.5/2.);
		pt[4] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT*1.5/2.);
		pt[5] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT*1.5/2.);
		pt[6] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT*1.5/2.);
		pt[7] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT*1.5/2.);
	}
}

void dimensionE::Getpt_PMTCath(G4ThreeVector *pt) {
	G4double innerSide_half = finnerR_new*tan(fdeltatheta/2.);
	G4double outerSide_half= (finnerR_new+ftower_height)*tan(fdeltatheta/2.);

	if(fRbool == 1){
		pt[0] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/4.);
		pt[1] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/4.);
		pt[2] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/4.);          
		pt[3] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/4.);
		pt[4] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/4.);
		pt[5] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/4.);
		pt[6] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/4.);
		pt[7] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/4.);
	}

	else{
		pt[0] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/4.);
		pt[1] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/4.);
		pt[2] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/4.);          
		pt[3] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/4.);
		pt[4] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/4.);
		pt[5] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/4.);
		pt[6] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/4.);
		pt[7] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/4.);
	}
}

G4RotationMatrix* dimensionE::GetRM(G4int i){
	if(fRbool==1){
		G4RotationMatrix* RotMatrix = new G4RotationMatrix();

		//RotMatrix->rotateZ(90*deg);
		RotMatrix->rotateZ(M_PI/2.);
		RotMatrix->rotateZ(-i*fPhiZRot);
		//RotMatrix->rotateX(90*deg);
		RotMatrix->rotateX(M_PI/2.);
		RotMatrix->rotateX(-fthetaofcenter);

		return RotMatrix;
	}
	else{
		G4RotationMatrix* RotMatrix = new G4RotationMatrix();

		//RotMatrix->rotateZ(90*deg);
		RotMatrix->rotateZ(M_PI/2.);
		RotMatrix->rotateZ(-i*fPhiZRot);
		//RotMatrix->rotateX(90*deg);
		RotMatrix->rotateX(M_PI/2.);
		RotMatrix->rotateX(fthetaofcenter);

		return RotMatrix;
	}
}
*/










//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
