//
// Free function wrapper around
//   G4NistManager::FindOrBuildMaterial
// The wrapper does the job of throwing if the pointer comes
// back null.
//
//  Created on: Mar 22, 2017
//      Author: tassiell
//
// Original author Rob Kutschke
//
//

#include "findMaterialOrThrow.hh"

#include "globals.hh"
#include "G4NistManager.hh"

namespace gmc {
  G4Material* findMaterialOrThrow( G4String const& name){

    // Look up the material.
    G4Material* m = G4NistManager::Instance()->
      FindOrBuildMaterial(name,true,true);

    // Throw if necessary.
    if ( !m ){
      G4Exception("GEOM","Fatal error in Argument",
              FatalErrorInArgument,
              G4String("Could not find a material with the name: " + name));
    }
    return m;
  }
}
