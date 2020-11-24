/*
 * GMCG4BeamPipeBuilder.hh
 *
 *  Created on: Mar 22, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_GMCG4BEAMPIPEBUILDER_HH_
#define G4GMC_GMCG4BEAMPIPEBUILDER_HH_

#include "G4LogicalVolume.hh"

#include <cstddef>

class GMCG4BeamPipeBuilder {
public:
  static void constructPipe(G4LogicalVolume* motherLogV=NULL);
};

#endif /* G4GMC_GMCG4BEAMPIPEBUILDER_HH_ */
