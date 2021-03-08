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
// $Id: exampleGMCG4.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleGMCG4.cc
/// \brief Main program of the GMCG4 example

//#include "GMCG4DetectorConstruction.hh"
//#include "GMCG4ActionInitialization.hh"
#include "G4GlobalConfig.hh"

#if defined(G4MULTITHREADED) && !defined(MT_OFF)
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "GMCG4DetectorConstruction.hh"
#include "GMCG4ActionInitialization.hh"

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "QGSP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4GDMLParser.hh"
//#include "G4VPhysicalVolume.hh"
#include "Randomize.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <iostream>
#include <cstdlib>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#if defined(G4MULTITHREADED) && !defined(MT_OFF)
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  //
  if ( argc==3 ) { runManager->SetUserInitialization(new GMCG4DetectorConstruction( argv[2]) ); }
  else if ( argc==4 ) { runManager->SetUserInitialization(new GMCG4DetectorConstruction( argv[2], atoi(argv[3])) ); }
  else if ( argc==5 ) { runManager->SetUserInitialization(new GMCG4DetectorConstruction( argv[2], atoi(argv[3]), argv[4]) ); }
  else { runManager->SetUserInitialization(new GMCG4DetectorConstruction()); }

  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  physicsList->RegisterPhysics(new G4EmStandardPhysics_option3());
  physicsList->RegisterPhysics(new G4EmExtraPhysics());
  physicsList->RegisterPhysics(new G4DecayPhysics());
  runManager->SetUserInitialization(physicsList);
    
  // Set user action classes
  runManager->SetUserInitialization(new GMCG4ActionInitialization());
  
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !
  //
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
