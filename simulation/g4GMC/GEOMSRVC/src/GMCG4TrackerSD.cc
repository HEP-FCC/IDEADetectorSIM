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
// $Id: GMCG4TrackerSD.cc 87359 2014-12-01 16:04:27Z gcosmo $
//
/// \file GMCG4TrackerSD.cc
/// \brief Implementation of the GMCG4TrackerSD class

#include "GMCG4TrackerSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

#include "RootIO.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GMCG4TrackerSD::GMCG4TrackerSD(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL),
   fVersion(0) {

  collectionName.insert(hitsCollectionName);
  fCollPos = collectionName.size()-1;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4TrackerSD::~GMCG4TrackerSD() {


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4TrackerSD::Initialize(G4HCofThisEvent* hce) {

  // Create hits collection

  fHitsCollection 
    = new GMCG4TrackerHitsCollection(SensitiveDetectorName, collectionName[fCollPos]);

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[fCollPos]);

  hce->AddHitsCollection( hcID, fHitsCollection ); 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool GMCG4TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {  

  G4int IdCell; 

  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();

  if (edep==0.) return false;
  G4int nLay = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();

  IdCell = nLay;

  if (fVersion == 1) {

    G4ThreeVector worldPosition = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector localPosition = aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(worldPosition);

    G4int ncell = (G4int)(localPosition.x() + 60.)/10 + 1;

    if (ncell > 12) return false;

    ncell += (nLay - 1)*12;

    IdCell = ncell;

  }


  GMCG4TrackerHit* newHit = new GMCG4TrackerHit(aStep->GetTrack()->GetTrackID(),
					  IdCell,
					  edep,
					  aStep->GetNonIonizingEnergyDeposit(),
					  aStep->GetPreStepPoint()->GetGlobalTime(),
					  aStep->GetPreStepPoint()->GetProperTime(),
					  aStep->GetPreStepPoint()->GetPosition(),
					  aStep->GetPostStepPoint()->GetPosition(),
					  aStep->GetPreStepPoint()->GetMomentum(),
					  aStep->GetStepLength(),
					  aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());

  fHitsCollection->insert( newHit );

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4TrackerSD::EndOfEvent(G4HCofThisEvent*) {

  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }

  // storing the hits in ROOT file
  G4int NbHits = fHitsCollection->entries();
  std::vector<GMCG4TrackerHit*> hitsVector;


  for (G4int i=0;i<NbHits;i++)  
    hitsVector.push_back((*fHitsCollection)[i]);

//  if (SensitiveDetectorName == "TrackerChamberSD") RootIO::GetInstance()->PutMCStepHitCh(hitsVector);
//  if (SensitiveDetectorName == "PixelTrackerSD") RootIO::GetInstance()->PutMCStepHitPx(hitsVector);
  if (SensitiveDetectorName == "TrackerChamberSD") RootIO::GetInstance()->PutMCStepHit(SensitiveDetectorName.c_str(),hitsVector);
  if (SensitiveDetectorName == "PixelTrackerSD") RootIO::GetInstance()->PutMCStepHit(SensitiveDetectorName.c_str(),hitsVector);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
