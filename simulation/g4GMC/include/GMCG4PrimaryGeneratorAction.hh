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
// $Id: GMCG4PrimaryGeneratorAction.hh 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file GMCG4PrimaryGeneratorAction.hh
/// \brief Definition of the GMCG4PrimaryGeneratorAction class

#ifndef GMCG4PrimaryGeneratorAction_h
#define GMCG4PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>

class G4ParticleGun;
class G4Event;
class G4VPrimaryGenerator;
class GMCG4PrimaryGeneratorMessenger;

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the Tracker 
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class 
/// (see the macros provided with this example).

class GMCG4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    GMCG4PrimaryGeneratorAction();    
    virtual ~GMCG4PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* );

//  G4ParticleGun* GetParticleGun() {return fParticleGun; }
////  G4ThreeVector GetPrimaryVertex() {return fPrimVertex; }
////  G4ThreeVector GetPrimaryDirection() {return fPrimDirection; }
//  void  GenerateVertexPosition();
//  void  GenerateDirection();

    // Set methods
    void SetRandomFlag(G4bool );

    void SetGenerator(G4VPrimaryGenerator* gen);
    void SetGenerator(G4String genname);

    G4VPrimaryGenerator* GetGenerator() const;
    G4String GetGeneratorName() const;

  private:
//  G4ParticleGun*          fParticleGun; // G4 particle gun
//  G4ThreeVector           fPrimVertex;
//  G4ThreeVector           fPrimDirection;

  G4VPrimaryGenerator* fParticleGun;
  G4VPrimaryGenerator* fHepmcAscii;
  G4VPrimaryGenerator* fPythiaGen;

  G4VPrimaryGenerator* fCurrentGenerator;
  G4String fCurrentGeneratorName;
  std::map<G4String, G4VPrimaryGenerator*> fGentypeMap;

  GMCG4PrimaryGeneratorMessenger* fMessenger;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
inline void GMCG4PrimaryGeneratorAction::SetGenerator(G4VPrimaryGenerator* gen)
{
  fCurrentGenerator = gen;
}

inline void GMCG4PrimaryGeneratorAction::SetGenerator(G4String genname)
{
  std::map<G4String, G4VPrimaryGenerator*>::iterator pos =
                                            fGentypeMap.find(genname);
  if ( pos != fGentypeMap.end() ) {
    fCurrentGenerator = pos->second;
    fCurrentGeneratorName = genname;
  }
}

inline G4VPrimaryGenerator* GMCG4PrimaryGeneratorAction::GetGenerator() const
{
  return fCurrentGenerator;
}

inline G4String GMCG4PrimaryGeneratorAction::GetGeneratorName() const
{
  return fCurrentGeneratorName;
}

#endif
