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
/// \file GMCG4PrimaryGeneratorMessenger.cc
/// \brief Implementation of the GMCG4PrimaryGeneratorMessenger class
//
//

#include "G4UIcommand.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"
#include "GMCG4PrimaryGeneratorMessenger.hh"
#include "GMCG4PrimaryGeneratorAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4PrimaryGeneratorMessenger::GMCG4PrimaryGeneratorMessenger
                                (GMCG4PrimaryGeneratorAction* genaction)
  : fPrimaryAction(genaction)
{
  fMyDetDir = new G4UIdirectory("/mydet/");
  fMyDetDir-> SetGuidance("GMCG4 detector control commands.");

  fDir= new G4UIdirectory("/generator/");
  fDir-> SetGuidance("Control commands for primary generator");

  fSelect= new G4UIcmdWithAString("/generator/select", this);
  fSelect-> SetGuidance("fSelect generator type");
  fSelect-> SetParameterName("generator_type", false, false);
  fSelect-> SetCandidates("particleGun pythia hepmcAscii");
  fSelect-> SetDefaultValue("particleGun");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4PrimaryGeneratorMessenger::~GMCG4PrimaryGeneratorMessenger()
{
  delete fSelect;
  delete fMyDetDir;
  delete fDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                              G4String newValues)
{
  if ( command == fSelect ) {
    fPrimaryAction-> SetGenerator(newValues);
    G4cout << "current generator type: "
            << fPrimaryAction-> GetGeneratorName() << G4endl;
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4String GMCG4PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4String cv, st;
  if ( command == fSelect ) {
    cv = fPrimaryAction-> GetGeneratorName();
  }

 return cv;
}
