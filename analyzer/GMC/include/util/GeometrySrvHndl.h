/*
 * GeometrySrvHndl.h
 *
 *  Created on: Apr 10, 2017
 *      Author: tassiell
 */

#ifndef GEOMETRYSRVHNDL_H
#define GEOMETRYSRVHNDL_H
#include <string>

class GeomService;
namespace cdch {
class CDCHtracker;
class CellGeometryHandle;
}
namespace svx {
class SVXtracker;
class ROGeometryHandle;
}
namespace pshw {
class PSHWtracker;
}
namespace phcv {
class PHCVtcounter;
}

class GeometrySrvHndl {
public:
  static GeometrySrvHndl* Instance();
  static GeometrySrvHndl* Instance(std::string geomfile);
  virtual ~GeometrySrvHndl();
  void makeDetectors();

  GeomService *GetGeomService() {return fGSrvc;}
  const cdch::CDCHtracker *GetCDCHtracker();
  cdch::CellGeometryHandle *GetCellHandle() {return fch;}
//  void GetCDCHtracker();
  const svx::SVXtracker *GetSVXtracker();
  svx::ROGeometryHandle *GetROChanHandle() {return fsvx;}
  const pshw::PSHWtracker *GetPSHWtracker();
  svx::ROGeometryHandle *GetPSHWROChanHandle() {return fpshw;}
  const phcv::PHCVtcounter *GetPHCVtcounter();

private:
  GeometrySrvHndl();

  static GeometrySrvHndl *_instance;
  static GeomService *fGSrvc;
  //CDCHtracker *cdchtracker;
//  std::unique_ptr<CDCHtracker> fCdchtracker;

  cdch::CellGeometryHandle *fch;
  svx::ROGeometryHandle *fsvx;
  svx::ROGeometryHandle *fpshw;
};

#endif /* GEOMETRYSRVHNDL_H */
