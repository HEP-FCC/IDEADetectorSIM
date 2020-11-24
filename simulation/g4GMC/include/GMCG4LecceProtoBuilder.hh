/*
 * GMCG4LecceProtoBuilder.hh
 *
 *  Created on: Mar 27, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_INCLUDE_GMCG4LECCEPROTOBUILDER_HH_
#define G4GMC_INCLUDE_GMCG4LECCEPROTOBUILDER_HH_


#include "G4LogicalVolume.hh"

#include <cstddef>

class GMCG4LecceProtoBuilder {
public:
  static void constructTracker(G4LogicalVolume* motherLogV=NULL);
};

#endif /* G4GMC_INCLUDE_GMCG4LECCEPROTOBUILDER_HH_ */
