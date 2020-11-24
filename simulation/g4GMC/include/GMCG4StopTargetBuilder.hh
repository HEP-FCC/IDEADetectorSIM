/*
 * GMCG4StopTargetBuilder.hh
 *
 *  Created on: Mar 22, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_GMCG4STOPTARGETBUILDER_HH_
#define G4GMC_GMCG4STOPTARGETBUILDER_HH_

#include "G4LogicalVolume.hh"

#include <cstddef>

class GMCG4StopTargetBuilder {
public:
  static void construct(G4LogicalVolume* motherLogV=NULL);
};

#endif /* G4GMC_GMCG4STOPTARGETBUILDER_HH_ */
