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
//  A class for control of the Magnetic Field of the detector.
//  The field is assumed to be uniform.
//
// Should this be a:
//    i) messenger
//   ii) user class that creates the field       ?
//  iii) simply a derived class of Uniform field ?  <== I have chosen this now.
//   iv) a field manager that creates/updates field    (Prefered?)
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef GMCG4FieldSetup_h
#define GMCG4FieldSetup_h 1

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"

class G4FieldManager;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;
class GMCG4FieldMessenger;
class GMCG4MagneticField;

class GMCG4FieldSetup
{
public:
  GMCG4FieldSetup(G4ThreeVector);  //  The value of the field
  GMCG4FieldSetup();               //  A zero field

  virtual ~GMCG4FieldSetup();

  void SetStepperType( G4int i ) { fStepperType = i; }

  void SetStepper();

  void SetMinStep(G4double s) { fMinStep = s; }

  void InitialiseAll();    //  Set parameters and call method below
  void CreateStepperAndChordFinder();

  void SetFieldValue(G4ThreeVector fieldVector);
  void SetFieldValue(G4double      fieldValue);
  void SetMaxRhoValue(G4double     maxrhoValue);
  G4ThreeVector GetConstantFieldValue();

protected:

  // Find the global Field Manager

  G4FieldManager*         GetGlobalFieldManager();

  G4FieldManager*         fFieldManager;
  G4ChordFinder*          fChordFinder;
  G4Mag_UsualEqRhs*       fEquation;
  GMCG4MagneticField*     fMagneticField;

  G4MagIntegratorStepper* fStepper;
  G4int                   fStepperType;

  G4double                fMinStep;
 
  GMCG4FieldMessenger*      fFieldMessenger;

};

#endif
