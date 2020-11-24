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
// $Id: GMCG4TrackerHit.cc 69706 2013-05-13 09:12:40Z gcosmo $
//
/// \file GMCG4TrackerHit.cc
/// \brief Implementation of the GMCG4TrackerHit class

#include "GMCG4TrackerHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

//ClassImp(GMCG4TrackerHit)

G4ThreadLocal G4Allocator<GMCG4TrackerHit>* GMCG4TrackerHitAllocator=0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4TrackerHit::GMCG4TrackerHit()
:G4VHit(),
 fTrackID(-1),
 fChamberNb(-1),
 fChannelNb(-1),
 fEdep(0.),
 fNoIEdep(0.),
 fGlobalTime(0.),
 fProperTime(0.),
 fPos(G4ThreeVector()),
 fPosEnding(G4ThreeVector()),
 fMomentum(G4ThreeVector()),
 fStepLength(0.),
 fProcessCode("NoName") {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GMCG4TrackerHit::~GMCG4TrackerHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4TrackerHit::GMCG4TrackerHit(G4int trackID,G4int chamberNb,G4double edep,G4double noIEdep,G4double gtime,G4double ptime,G4ThreeVector pos,G4ThreeVector end,G4ThreeVector mom,G4double len,G4String pcode)
:G4VHit(),
 fTrackID(trackID),
 fChamberNb(chamberNb),
 fChannelNb(-1),
 fEdep(edep),
 fNoIEdep(noIEdep),
 fGlobalTime(gtime),
 fProperTime(ptime),
 fPos(pos),
 fPosEnding(end),
 fMomentum(mom),
 fStepLength(len),
 fProcessCode(pcode) {
}

GMCG4TrackerHit::GMCG4TrackerHit(const GMCG4TrackerHit& right)
  : G4VHit() {
  fTrackID   = right.fTrackID;
  fChamberNb = right.fChamberNb;
  fChannelNb = right.fChannelNb;
  fEdep      = right.fEdep;
  fNoIEdep   = right.fNoIEdep;
  fGlobalTime = right.fGlobalTime;
  fProperTime = right.fProperTime;
  fPos = right.fPos;
  fPosEnding = right.fPosEnding;
  fMomentum = right.fMomentum;
  fStepLength = right.fStepLength;
  fProcessCode = right.fProcessCode;
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const GMCG4TrackerHit& GMCG4TrackerHit::operator=(const GMCG4TrackerHit& right)
{
  fTrackID   = right.fTrackID;
  fChamberNb = right.fChamberNb;
  fChannelNb = right.fChannelNb;
  fEdep      = right.fEdep;
  fNoIEdep   = right.fNoIEdep;
  fGlobalTime = right.fGlobalTime;
  fProperTime = right.fProperTime;
  fPos = right.fPos;
  fPosEnding = right.fPosEnding;
  fMomentum = right.fMomentum;
  fStepLength = right.fStepLength;
  fProcessCode = right.fProcessCode;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int GMCG4TrackerHit::operator==(const GMCG4TrackerHit& right) const
{
  return ( this == &right ) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GMCG4TrackerHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fPos);
    circle.SetScreenSize(6.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(0.,1.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GMCG4TrackerHit::Print()
{
  G4cout
     << "  trackID: " << fTrackID << " chamberNb: " << fChamberNb << " channelNb: " << fChannelNb
     << " Edep: "
     << std::setw(7) << G4BestUnit(fEdep,"Energy")
     << " Position: "
     << std::setw(7) << G4BestUnit( fPos,"Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
