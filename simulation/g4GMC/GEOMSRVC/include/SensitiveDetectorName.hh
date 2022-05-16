/*
 * SensitiveDetectorName.hh
 *
 *  Created on: Apr 3, 2017
 *      Author: tassiell
 */

#ifndef SENSITIVEDETECTORNAME_HH_
#define SENSITIVEDETECTORNAME_HH_

//namespace cdch {

class SensitiveDetectorName {
//static const char* name[4] = { "DCHtrackerSD", "DCHSWiresSD",  "DCHFWiresSD", "DCHWallsSD" };

public:
  enum SDtype { DCHtracker=0, DCHSWires,  DCHFWires, DCHWalls, SVXtracker, PSHWtracker, PHCVtcounter, PHCVradiator, DRFPIcalo, MUSPCtracker };

  static char const * TrackerGas(){
    return name[DCHtracker];
  }

  static char const * TrackerSWires(){
    return name[DCHSWires];
  }

  static char const * TrackerFWires(){
    return name[DCHFWires];
  }

  static char const * TrackerWalls(){
    return name[DCHWalls];
  }

  static char const * SVXTrackerRO(){
    return name[SVXtracker];
  }

  static char const * PSHWTrackerRO(){
    return name[PSHWtracker];
  }

  static char const * PHCVTCounterRO(){
    return name[PHCVtcounter];
  }

  static char const * PHCVRadiator(){
    return name[PHCVradiator];
  }

  static char const * DRFPICalorimeter(){
    return name[DRFPIcalo];
  }

  static char const * MUSPCTrackerRO(){
    return name[MUSPCtracker];
  }

private:
  static const char* name[10];

};

//const char* SensitiveDetectorName::name[4] = { "DCHtrackerSD", "DCHSWiresSD",  "DCHFWiresSD", "DCHWallsSD" };

//} // namespace cdch

#endif /* SENSITIVEDETECTORNAME_HH_ */
