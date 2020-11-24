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
// $Id: GMCG4TrackerHit.hh 69706 2013-05-13 09:12:40Z gcosmo $
//
/// \file GMCG4TrackerHit.hh
/// \brief Definition of the GMCG4TrackerHit class

#ifndef GMCG4TrackerHit_h
#define GMCG4TrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

/// Tracker hit class
///
/// It defines data members to store the trackID, chamberNb, energy deposit,
/// and position of charged particles in a selected volume:
/// - fTrackID, fChamberNB, fEdep, fPos
// fChannelNB is used as second index for high granularity detector

class GMCG4TrackerHit : public G4VHit
{
  public:
    GMCG4TrackerHit();
  GMCG4TrackerHit(G4int trackID,G4int chamberNb,G4double edep,G4double noIEdep,G4double gtime,G4double ptime,G4ThreeVector pos,G4ThreeVector end,G4ThreeVector mom,G4double len,G4String pcode);

    GMCG4TrackerHit(const GMCG4TrackerHit&);

    virtual ~GMCG4TrackerHit();

    // operators
    const GMCG4TrackerHit& operator=(const GMCG4TrackerHit&);
    G4int operator==(const GMCG4TrackerHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID  (G4int track)      { fTrackID = track; }
    void SetChamberNb(G4int chamb)      { fChamberNb = chamb; }
    void SetChannelNb(G4int chann)      { fChannelNb = chann; }
    void SetEdep     (G4double de)      { fEdep = de; }
    void SetNoIEdep  (G4double nIde)    { fNoIEdep = nIde; }
    void SetGlobalTime(G4double gtime)  { fGlobalTime = gtime; }
    void SetProperTime(G4double ptime)  { fProperTime = ptime; }
    void SetStepLength(G4double len)    { fStepLength = len; }
    void SetMomentum  (G4ThreeVector mom) { fMomentum = mom; }
    void SetPos      (G4ThreeVector xyz) { fPos = xyz; }
    void SetPosEnding      (G4ThreeVector xyz) { fPosEnding = xyz; }
    void SetProcessCode(G4String process) {fProcessCode = process;}

    // Get methods
    G4int GetTrackID() const     { return fTrackID; }
    G4int GetChamberNb() const   { return fChamberNb; }
    G4int GetChannelNb() const   { return fChannelNb; }
    G4double GetEdep() const     { return fEdep; }
    G4double GetNoIEdep() const  { return fNoIEdep; }
    G4double GetGlobalTime() const { return fGlobalTime; }
    G4double GetProperTime() const { return fProperTime; }
    G4double GetStepLength() const { return fStepLength; }
    G4ThreeVector GetPos() const { return fPos; }
    G4ThreeVector GetPosEnding() const { return fPosEnding; }
    G4ThreeVector GetMomentum() const { return fMomentum; }
    G4String GetProcessCode() const {return fProcessCode;}

  private:

      G4int         fTrackID;
      G4int         fChamberNb;
      G4int         fChannelNb;
      G4double      fEdep;
      G4double      fNoIEdep;
      G4double      fGlobalTime;
      G4double      fProperTime;
      G4ThreeVector fPos;
      G4ThreeVector fPosEnding;
      G4ThreeVector fMomentum;
      G4double      fStepLength;
      G4String    fProcessCode;

//   ClassDef(GMCG4TrackerHit,0);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<GMCG4TrackerHit> GMCG4TrackerHitsCollection;

extern G4ThreadLocal G4Allocator<GMCG4TrackerHit>* GMCG4TrackerHitAllocator;

#ifndef __CINT__
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* GMCG4TrackerHit::operator new(size_t)
{
  if(!GMCG4TrackerHitAllocator)
      GMCG4TrackerHitAllocator = new G4Allocator<GMCG4TrackerHit>;
  return (void *) GMCG4TrackerHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void GMCG4TrackerHit::operator delete(void *hit)
{
  GMCG4TrackerHitAllocator->FreeSingle((GMCG4TrackerHit*) hit);
}
#endif
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
