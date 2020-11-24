#ifndef G4GMC_MATER_findMaterialOrThrow_hh
#define G4GMC_MATER_findMaterialOrThrow_hh
//
// Free function wrapper around
//   G4NistManager::FindOrBuildMaterial
// The wrapper does the job of throwing if the pointer comes
// back null.
//
//  Created on: Mar 22, 2017
//      Author: tassiell
//
//

class G4Material;
class G4String;

namespace gmc {

  G4Material* findMaterialOrThrow( G4String const& name);

} // end namespace gmc
#endif /* G4GMC_MATER_findMaterialOrThrow_hh */
