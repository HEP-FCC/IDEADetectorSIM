/*
 * GMCG4ModupixTrkrBuilder.hh
 *
 *  Created on: Mar 28, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_INCLUDE_GMCG4MODUPIXTRKRBUILDER_HH_
#define G4GMC_INCLUDE_GMCG4MODUPIXTRKRBUILDER_HH_


#include "G4LogicalVolume.hh"

#include <cstddef>

class GMCG4ModupixTrkrBuilder {
public:
  static void constructVrtxTracker(G4LogicalVolume* motherLogV=NULL);
};

#endif /* G4GMC_INCLUDE_GMCG4MODUPIXTRKRBUILDER_HH_ */
