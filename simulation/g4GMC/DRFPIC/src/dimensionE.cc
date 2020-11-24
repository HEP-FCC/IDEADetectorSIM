//
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

#include "dimensionE.hh"

#include "G4ThreeVector.hh"


#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "geomdefs.hh"

#include <cmath>
#include <stdio.h>
#include <float.h>

using namespace std;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

dimensionE::dimensionE()
{	fRbool = 0;
	fcalbasicbool = 0;
	finnerR = 0;
	ftower_height = 0;
	fnumzrot = 0;
	fdeltatheta = 0;
    fdeltatheta2 = 0;
	fthetaofcenter = 0;
    fthetaofcenter2 = 0;
	finnerR_new = 0;
    finnerR_new2 = 0;	
	fPhiZRot = 0;
	fV1 = G4ThreeVector();
	fV2 = G4ThreeVector();
	fV3 = G4ThreeVector();
	fV4 = G4ThreeVector();
	fPMTT = 0;
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

dimensionE::~dimensionE()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void dimensionE::CalBasic()
{
	
	fcalbasicbool = 1;
    
    finnerR_new = finnerR/(cos(fthetaofcenter)-sin(fthetaofcenter)*tan(fdeltatheta/2.));
    finnerR_new2 = finnerR/(cos(fthetaofcenter2)-sin(fthetaofcenter2)*tan(fdeltatheta2/2.));
    
	G4double innerSide_half = finnerR_new*tan(fdeltatheta/2.);
	G4double outerSide_half = (finnerR_new+ftower_height)*tan(fdeltatheta/2.);	
    
    G4double innerSide_half2 = finnerR_new2*tan(fdeltatheta2/2.);

	fTrns_Length = ftower_height/2.+finnerR_new;
	
	fTrns_Vector = G4ThreeVector(cos(fthetaofcenter)*fTrns_Length,0,sin(fthetaofcenter)*fTrns_Length);
    
    G4double dx1=finnerR;
    G4double dxi=sin(fthetaofcenter)*finnerR_new+innerSide_half*cos(fthetaofcenter);
    G4double dxi2=sin(fthetaofcenter2)*finnerR_new2+innerSide_half2*cos(fthetaofcenter2);
    Ratio=dxi/dx1;
    Ratio2=dxi2/dx1;
	
	fV1 = G4ThreeVector(
			(Ratio2)*(cos(fthetaofcenter)*finnerR_new+sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)),
			0,
			sin(fthetaofcenter)*finnerR_new-sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)
			);
	
	fV2 = G4ThreeVector(
			(Ratio2)*(cos(fthetaofcenter)*(finnerR_new+ftower_height)+sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)),
			0,
			sin(fthetaofcenter)*(finnerR_new+ftower_height)-sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)
			);
	
	fV3 = G4ThreeVector(
			(Ratio)*(cos(fthetaofcenter)*finnerR_new-sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)),
			0,
			sin(fthetaofcenter)*finnerR_new+sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)
			);
	
	fV4 = G4ThreeVector(
			(Ratio)*(cos(fthetaofcenter)*(finnerR_new+ftower_height)-sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)),
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
		pt[0]=G4ThreeVector(-(fV1.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
		pt[1]=G4ThreeVector((fV1.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
		pt[2]=G4ThreeVector(-(fV3.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
		pt[3]=G4ThreeVector((fV3.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
		pt[4]=G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
		pt[5]=G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
		pt[6]=G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
		pt[7]=G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
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
	
	//cout<<"ENDCAP Y_int = "<<innerSide_half*2<<" Y_out = "<<outerSide_half*2<<" X_inn = "<<pt[3](0)-pt[2](0)<<" X_out = "<<pt[7](0)-pt[6](0)<<std::endl;
//cout<<std::endl;
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

