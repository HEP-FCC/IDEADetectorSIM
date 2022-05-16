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
//
// $Id: PhysicsList.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

#include "OpticalPhysics.hh"

OpticalPhysics::OpticalPhysics(G4bool toggle)
: G4VPhysicsConstructor("Optical")
{
    theWLSProcess                = NULL;
    theScintProcess              = NULL;
    theCerenkovProcess           = NULL;
    theBoundaryProcess           = NULL;
    theAbsorptionProcess         = NULL;
    theRayleighScattering        = NULL;
    theMieHGScatteringProcess    = NULL;
    
    AbsorptionOn                 = toggle;
}

OpticalPhysics::~OpticalPhysics() { }

//#include "G4ParticleDefinition.hh"
//#include "G4ParticleTable.hh"

#include "G4OpticalPhoton.hh"

void OpticalPhysics::ConstructParticle()
{
    G4OpticalPhoton::OpticalPhotonDefinition();
}

#include "G4ProcessManager.hh"

void OpticalPhysics::ConstructProcess()
{
    G4cout << "OpticalPhysics:: Add Optical Physics Processes"
    << G4endl;
    
    //theWLSProcess = new G4OpWLS();
    
    theScintProcess = new G4Scintillation();
    theScintProcess->SetScintillationYieldFactor(1.);
    //theScintProcess->SetTrackSecondariesFirst(true);
    
    
    theCerenkovProcess = new G4Cerenkov();
    theCerenkovProcess->SetMaxNumPhotonsPerStep(1000.);
    //theCerenkovProcess->SetTrackSecondariesFirst(true);
    
    theAbsorptionProcess      = new G4OpAbsorption();
    //theRayleighScattering     = new G4OpRayleigh();
    //theMieHGScatteringProcess = new G4OpMieHG();
    theBoundaryProcess        = new G4OpBoundaryProcess();
    
    G4ProcessManager* pManager =
    G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
    
    if (!pManager) {
        std::ostringstream o;
        o << "Optical Photon without a Process Manager";
        G4Exception("OpticalPhysics::ConstructProcess()","",
                    FatalException,o.str().c_str());
    }
    
    if (AbsorptionOn) pManager->AddDiscreteProcess(theAbsorptionProcess);
    
    //pManager->AddDiscreteProcess(theRayleighScattering);
    //pManager->AddDiscreteProcess(theMieHGScatteringProcess);
    
    pManager->AddDiscreteProcess(theBoundaryProcess);
    
    //theWLSProcess->UseTimeProfile("delta");
    //theWLSProcess->UseTimeProfile("exponential");
    
    //pManager->AddDiscreteProcess(theWLSProcess);
    
    
    //theScintProcess->SetScintillationYieldFactor(1.);
    theScintProcess->SetScintillationExcitationRatio(0.0);
    theScintProcess->SetTrackSecondariesFirst(true);
    
    // Use Birks Correction in the Scintillation process
    
    G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    theScintProcess->AddSaturation(emSaturation);
    

    auto theParticleIterator = GetParticleIterator();
    theParticleIterator->reset();
    while ( (*theParticleIterator)() ){
        
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4String particleName = particle->GetParticleName();
    
        pManager = particle->GetProcessManager();
        if (!pManager) {
            std::ostringstream o;
            o << "Particle " << particleName << "without a Process Manager";
            G4Exception("OpticalPhysics::ConstructProcess()","",
                        FatalException,o.str().c_str());
        }
        // Cherenkov photons production, if you don't want to produce Cherenkov
        // photons comment if body
        if(theCerenkovProcess->IsApplicable(*particle))
        {
            pManager->AddProcess(theCerenkovProcess);
            pManager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
        }
        // Scintillation photons production, if you don't want to produce scintillation
        // photons comment if body
        if(theScintProcess->IsApplicable(*particle))
        {
            //pManager->AddProcess(theScintProcess);
            //pManager->SetProcessOrderingToLast(theScintProcess,idxAtRest);
            //pManager->SetProcessOrderingToLast(theScintProcess,idxPostStep);
        }
        
    }
}

void OpticalPhysics::SetNbOfPhotonsCerenkov(G4int MaxNumber)
{
    theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumber);
}
