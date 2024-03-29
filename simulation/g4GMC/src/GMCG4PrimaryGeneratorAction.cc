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
// $Id: GMCG4PrimaryGeneratorAction.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file GMCG4PrimaryGeneratorAction.cc
/// \brief Implementation of the GMCG4PrimaryGeneratorAction class

#include "GMCG4PrimaryGeneratorAction.hh"
#include "GMCG4PrimaryGeneratorMessenger.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "HepMCG4AsciiReader.hh"
#include "HepMCG4PythiaInterface.hh"

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GMCG4PrimaryGeneratorAction::GMCG4PrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction()
//    ,fPrimVertex(0)
//    ,fPrimDirection(0)
{
  
//  G4int nofParticles = 1;
//  fCurrentGenerator = fParticleGun = new G4ParticleGun(nofParticles);
//  fCurrentGeneratorName = "fParticleGun";

	fCurrentGenerator= fParticleGun= new G4GeneralParticleSource();
	fCurrentGeneratorName= "gps";

	fHepmcAscii = new HepMCG4AsciiReader();
#ifdef G4LIB_USE_PYTHIA
  fPythiaGen = new HepMCG4PythiaInterface();
#else
  fPythiaGen = 0;
#endif
//  fGentypeMap["particleGun"] = fParticleGun;
  fGentypeMap["gps"] = fParticleGun;
  fGentypeMap["hepmcAscii"] = fHepmcAscii;
  fGentypeMap["pythia"] = fPythiaGen;

  fMessenger= new GMCG4PrimaryGeneratorMessenger(this);

//  // default particle kinematic
//
//  G4ParticleDefinition* particleDefinition
//    = G4ParticleTable::GetParticleTable()->FindParticle("e-");
//
//  fParticleGun->SetParticleDefinition(particleDefinition);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GMCG4PrimaryGeneratorAction::~GMCG4PrimaryGeneratorAction() {
  if (fParticleGun) delete fParticleGun;
  if (fHepmcAscii) delete fHepmcAscii;
  if (fPythiaGen) delete fPythiaGen;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent){
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore.
	if(fCurrentGenerator) {
//		if(fCurrentGeneratorName=="fParticleGun") {
//
//			GenerateVertexPosition();
//
//			GenerateDirection();
//			//  G4double worldZHalfLength = 0;
//			G4LogicalVolume* worldLV
//			= G4LogicalVolumeStore::GetInstance()->GetVolume("World");
//			G4Box* worldBox = NULL;
//			if ( worldLV ) { worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid()); }
//			if ( worldBox ) { /* worldZHalfLength = worldBox->GetZHalfLength(); */ }
//			else  {
//				G4cerr << "World volume of box not found." << G4endl;
//				G4cerr << "Perhaps you have changed geometry." << G4endl;
//				G4cerr << "The gun will be place in the center." << G4endl;
//			}
//
//			fParticleGun->SetParticlePosition(fPrimVertex);
//			fParticleGun->SetParticleMomentumDirection(fPrimDirection);
//			G4double aMomentum = 500.*MeV;
//			fParticleGun->SetParticleMomentum(aMomentum);
//			//  fParticleGun->SetParticleEnergy(500.*MeV);
//
//			fParticleGun->GeneratePrimaryVertex(anEvent);
//		}
//		else {
			fCurrentGenerator->GeneratePrimaryVertex(anEvent);
//		}
	} else {
		G4Exception("ExN04PrimaryGeneratorAction::GeneratePrimaries",
		                "PrimaryGeneratorAction001", FatalException,
		                "generator is not instanciated." );
	}
}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
//void GMCG4PrimaryGeneratorAction::GenerateVertexPosition(){
//
//  G4double size = 5.5*mm;
//  G4double x0 = size * (2.*G4UniformRand() - 1.);
//  G4double y0 = size * (2* G4UniformRand() - 1.);
//  G4double z0 = -500*mm;
//
//  fPrimVertex = G4ThreeVector(x0,y0,z0);
//
//}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//void GMCG4PrimaryGeneratorAction::GenerateDirection(){
//
//  G4double theta = 0.;//0.02*(2.*G4UniformRand() - 1.)*rad;
//  G4double phi = std::acos(-1)*(2.*G4UniformRand() - 1.)*rad;
//
//  G4double px0 = std::sin(theta)*std::cos(phi);
//  G4double py0 = std::sin(theta)*std::sin(phi);
//  G4double pz0 = std::cos(theta);
//
//  fPrimDirection = G4ThreeVector(px0,py0,pz0);
//
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
