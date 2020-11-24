/*
 * GMCG4RomaProtoBuilder.hh
 *
 *  Created on: Mar 22, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_GMCG4ROMAPROTOBUILDER_HH_
#define G4GMC_GMCG4ROMAPROTOBUILDER_HH_

#include "G4LogicalVolume.hh"

#include <cstddef>

class GMCG4RomaProtoBuilder {
public:
  static void constructTracker(G4LogicalVolume* motherLogV=NULL);
};

#endif /* G4GMC_GMCG4ROMAPROTOBUILDER_HH_ */
