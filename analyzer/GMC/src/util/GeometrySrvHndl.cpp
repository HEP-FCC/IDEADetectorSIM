/*
 * GeometrySrvHndl.cpp
 *
 *  Created on: Apr 10, 2017
 *      Author: tassiell
 */

#include <util/GeometrySrvHndl.h>
#include "generated/GMCAnalyzer.h"
#include "generated/GMCGlobalSteering.h"
#include "GeomService.hh"
#include "GeomHandle.hh"
// CDCH
#include "CDCHMaker.hh"
#include "CDCHtracker.hh"
//#include "CellGeometryHandle_v2.hh"
//#include "CellGeometryHandle_v3.hh"
//SVX
#include "SVXMaker.hh"
#include "SVXtracker.hh"
#include "ROGeometryHandle.hh"
//PSHW
#include "PSHWMaker.hh"
#include "PSHWtracker.hh"
#include "PSHWRadiator.hh"
//PHCV
#include "PHCVMaker.hh"
#include "PHCVtcounter.hh"
//FIX insert Geom/RO Handle when will be ready

#include <iostream>

GeometrySrvHndl *GeometrySrvHndl::_instance = 0;
GeomService *GeometrySrvHndl::fGSrvc = 0;

GeometrySrvHndl *GeometrySrvHndl::Instance() {
  if (_instance==0) {
    static GeometrySrvHndl manager;
    _instance = &manager;
  }
  if (fGSrvc == 0) {
    fGSrvc = GeomService::Instance(
        gAnalyzer->GetGSP()->GetGSrvAddFile().Data(),
        gAnalyzer->GetGSP()->GetGSrvAddFold().Data());
  }
  return _instance;
}

GeometrySrvHndl *GeometrySrvHndl::Instance(std::string geomfile) {
  if (_instance==0) {
    static GeometrySrvHndl manager;
    _instance = &manager;
  }
  if (fGSrvc == 0) {
    fGSrvc = GeomService::Instance(geomfile);
  }
  return _instance;
}

GeometrySrvHndl::GeometrySrvHndl() :
    fch(NULL)
   ,fsvx(NULL)
   ,fpshw(NULL)
{
  // TODO Auto-generated constructor stub

}

GeometrySrvHndl::~GeometrySrvHndl() {
  // TODO Auto-generated destructor stub
}

void GeometrySrvHndl::makeDetectors(){
  if (fGSrvc != 0) {
    if (fch==NULL && fGSrvc->getConfig().getBool("hasCDCH",false)) {
      cdch::CDCHMaker cdchtm( fGSrvc->getConfig() );
      fGSrvc->addDetector( cdchtm.getCDCHtrackerPtr() );
      //    fCdchtracker=cdchtm.getCDCHtrackerPtr();
      //if (fch==NULL) {
        GeomHandle<cdch::CDCHtracker> cdchtrkhndl;
        fch = cdchtrkhndl->getCellGeometryHandle();
      //}

      if (fGSrvc->getConfig().getBool("hasPHCV",false)) {
    	  phcv::PHCVMaker phcvtc( fGSrvc->getConfig() );
    	  fGSrvc->addDetector( phcvtc.getPHCVtcounterPtr() );
    	  //FIX insert Geom/RO Handle when will be ready
      }
    }
    if (fsvx==NULL && fGSrvc->getConfig().getBool("hasSVX",false)){
      svx::SVXMaker svxtm( fGSrvc->getConfig() );
      fGSrvc->addDetector( svxtm.getSVXtrackerPtr() );
      //if (fsvx==NULL) {
        GeomHandle<svx::SVXtracker> svxtrkhndl;
        fsvx = svxtrkhndl->getROGeometryHandle();
      //}
    }
    if (fpshw==NULL && fGSrvc->getConfig().getBool("hasPSHW",false)){
      pshw::PSHWMaker pshwtm( fGSrvc->getConfig() );
      fGSrvc->addDetector( pshwtm.getPSHWtrackerPtr() );
      //if (fpshw==NULL) {
        GeomHandle<pshw::PSHWtracker> pshwtrkhndl;
        fpshw = pshwtrkhndl->getROGeometryHandle();
      //}
    }
  } else {
    std::cout<<"Warning: GeomService not yet instantiated"<<std::endl;
  }
}

//void GeometrySrvHndl::GetCDCHtracker() {
//  GeomHandle<cdch::CDCHtracker> cdchtrkhndl;
//}

const cdch::CDCHtracker *GeometrySrvHndl::GetCDCHtracker() {
//  if (cdchtracker==NULL) {
//    GeomHandle<CDCHtracker> cdchtrkhndl;
//    cdchtracker = &(*cdchtrkhndl);
//  }
//  return cdchtracker;
  GeomHandle<cdch::CDCHtracker> cdchtrkhndl;
  return &(*cdchtrkhndl);
//  return fCdchtracker.get();
}

const svx::SVXtracker *GeometrySrvHndl::GetSVXtracker() {
  GeomHandle<svx::SVXtracker> svxtrkhndl;
  return &(*svxtrkhndl);
//  return fCdchtracker.get();
}

const pshw::PSHWtracker *GeometrySrvHndl::GetPSHWtracker() {
  GeomHandle<pshw::PSHWtracker> pshwtrkhndl;
  return &(*pshwtrkhndl);
//  return fCdchtracker.get();
}

const phcv::PHCVtcounter *GeometrySrvHndl::GetPHCVtcounter() {
  GeomHandle<phcv::PHCVtcounter> phcvtckhndl;
  return &(*phcvtckhndl);
//  return fCdchtracker.get();
}
