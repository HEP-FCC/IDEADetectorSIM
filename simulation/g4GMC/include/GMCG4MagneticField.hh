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
// $Id: GMCG4MagneticField.hh 68019 2013-03-13 13:32:43Z gcosmo $
//
/// \file errorpropagation/include/GMCG4MagneticField.hh
/// \brief Definition of the GMCG4MagneticField class
//

#ifndef GMCG4MagneticField_H
#define GMCG4MagneticField_H

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"

/// Magnetic field class
///

//class GMCG4MagneticField: public G4UniformMagField
class GMCG4MagneticField: public G4MagneticField
{
  public:
  
   GMCG4MagneticField(G4ThreeVector);  //  The value of the field
   GMCG4MagneticField();               //  A zero field
  ~GMCG4MagneticField();  
      
   //Set the field (0,0,fieldValue)
   void SetFieldValue(G4double fieldValue);
   void SetFieldValue(G4ThreeVector fieldVector);
   void SetMaxRho(double maxrho);
 
   void GetFieldValue(const G4double yTrack[4],
                              G4double *MagField) const ;


  
protected:
  double fMaxRho; //maximum rho
  G4UniformMagField *fUniformMagField;
};

#endif
