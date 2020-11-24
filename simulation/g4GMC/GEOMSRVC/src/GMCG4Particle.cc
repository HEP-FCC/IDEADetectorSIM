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
// $Id: GMCG4Particle.cc 69706 2013-05-13 09:12:40Z gcosmo $
//
/// \file GMCG4Particle.cc
/// \brief Implementation of the GMCG4Particle class

#include "GMCG4Particle.hh"

//ClassImp(GMCG4Particle)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4Particle::GMCG4Particle()
:fTrackID(-1),
 fParentID(-1),
 fParticleName("NoName"),
 fCharge(0.),
 fPDGCode(-1),
 fPosStart(G4ThreeVector()),
 fPosEnd(G4ThreeVector()),
 fMomentum(G4ThreeVector()){

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4Particle::~GMCG4Particle() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4Particle::GMCG4Particle(G4int trackID, G4int parentID, G4String name, G4double charge, G4int pdgcode, G4ThreeVector posstart, G4ThreeVector posend, G4ThreeVector momentum)
 : fTrackID(trackID),
 fParentID(parentID),
 fParticleName(name),
 fCharge(charge),
 fPDGCode(pdgcode),
 fPosStart(posstart),
 fPosEnd(posend),
 fMomentum(momentum){

}

GMCG4Particle::GMCG4Particle(const GMCG4Particle& right) {

 fTrackID      = right.fTrackID;
 fParentID     = right.fParentID;
 fParticleName = right.fParticleName;
 fCharge       = right.fCharge;
 fPDGCode      = right.fPDGCode;
 fPosStart     = right.fPosStart;
 fPosEnd       = right.fPosEnd;
 fMomentum     = right.fMomentum;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const GMCG4Particle& GMCG4Particle::operator=(const GMCG4Particle& right)
{

 fTrackID      = right.fTrackID;
 fParentID     = right.fParentID;
 fParticleName = right.fParticleName;
 fCharge       = right.fCharge;
 fPDGCode      = right.fPDGCode;

 fPosStart     = right.fPosStart;
 fPosEnd       = right.fPosEnd;      
 fMomentum     = right.fMomentum;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int GMCG4Particle::operator==(const GMCG4Particle& right) const {

  return ( this == &right ) ? 1 : 0;

}


