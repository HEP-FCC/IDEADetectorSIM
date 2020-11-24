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
//
// $Id: GMCG4MagneticField.cc 68019 2013-03-13 13:32:43Z gcosmo $
//
/// \file GMCG4MagneticField.cc
/// \brief Implementation of the GMCG4MagneticField class
//

#include "GMCG4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4MagneticField::GMCG4MagneticField()
//  : G4UniformMagField(G4ThreeVector()),fMaxRho(1.e10)
    : fMaxRho(1.e10), fUniformMagField(new G4UniformMagField(G4ThreeVector()))
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4MagneticField::GMCG4MagneticField(G4ThreeVector fieldVector)
//  : G4UniformMagField(fieldVector),fMaxRho(1.e10)
    : fMaxRho(1.e10), fUniformMagField( new G4UniformMagField(fieldVector))
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4MagneticField::SetFieldValue(G4double fieldValue)
{
//  G4UniformMagField::SetFieldValue(G4ThreeVector(0,0,fieldValue));
	fUniformMagField->SetFieldValue(G4ThreeVector(0,0,fieldValue));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4MagneticField::SetFieldValue(G4ThreeVector fieldVector)
{
//  G4UniformMagField::SetFieldValue(fieldVector);
  fUniformMagField->SetFieldValue(fieldVector);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4MagneticField::SetMaxRho(double maxrho)
{
  fMaxRho=maxrho;
  G4cout<<"Maximum field rho set "<<maxrho/mm<<" mm "<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4MagneticField::~GMCG4MagneticField()
{
	delete fUniformMagField;
}


void GMCG4MagneticField::GetFieldValue (const G4double xyztrack[4],
                                        G4double *B  ) const 
{

  if(xyztrack[0]*xyztrack[0]+xyztrack[1]*xyztrack[1]<fMaxRho*fMaxRho){
//    G4UniformMagField::GetFieldValue (xyztrack,B);
    fUniformMagField->GetFieldValue (xyztrack,B);
    return;
  }
  
  B[0]= 0. ;
  B[1]= 0. ;
  B[2]= 0. ;
}
