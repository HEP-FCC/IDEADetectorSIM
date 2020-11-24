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
/// \file persistency/P02/include/ExP02Classes.hh
/// \brief Declaration of the classes for generating dictionaries
//
// $Id: ExP02Classes.hh 71569 2013-06-18 10:13:24Z gcosmo $
//
//
//#include "GMCG4DetectorConstruction.hh"
//
#include "CLHEP/Vector/EulerAngles.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Transform3D.h"
//
#include <vector>

#include "GMCG4Particle.hh"
#include "GMCG4TrackerHit.hh"
//
//template class std::vector<CLHEP::Hep3Vector>;
//template class std::vector<GMCG4TrackerHit*>;
//template class std::vector<GMCG4Particle*>;
//
//#undef __G4String
