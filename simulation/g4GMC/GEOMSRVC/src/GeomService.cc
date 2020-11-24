/*
 * GeomService.cc
 *
 *  Created on: Mar 27, 2017
 *      Author: tassiell
 */

#include <GeomService.hh>

//#include "manageException.hh"

//#include "CDCHMaker.hh"
//#include "CDCHtracker.hh"

#include <iostream>
#include <string>

GeomService* GeomService::_instance = 0;

GeomService* GeomService::Instance() {
  if (_instance == 0) {
    static GeomService manager;
    _instance = &manager;
  }
  return _instance;
}

GeomService::~GeomService() {
  delete _config;
}

GeomService* GeomService::Instance(std::string inputfile, std::string folder) {
  GeomService* inst = Instance();
  inst->ReadCfgFile(inputfile,folder);
  return inst;

}

GeomService::GeomService() :
    _allowReplacement(true),
    _messageOnReplacement(false),
    _messageOnDefault(false),
    _isRead(false),
    _detectors()
{
  _config = new crd::SimpleConfig();
}

void GeomService::ReadCfgFile(std::string &infilename, std::string &folder) {
  if ( _isRead ) {
    std::cout<<"Warning! GeomService: configuration file already initialized at "<<_config->inputFile()<<std::endl;
  } else {
    _config->ReadFile(infilename,folder,_allowReplacement,_messageOnReplacement,_messageOnDefault);
    _isRead=true;

//    instanceActiveDet();
  }
}

//template <class DET>
//DET* GeomService::getElement()
//{
////    if(_run_count==0)
////      exceptionG4("GEOM",
////          "Cannot get detectors from an unconfigured geometry service.\n",
////          0,
////          "You've attempted to a get an element before the first run\n");
//
//  // to use this generic way requires a map of names (typeid?) to
//  // abstract elements.
//  // find the detector element requested
//  std::string name = typeid(DET).name();
//  DetMap::iterator it(_detectors.find(name));
//  if(it==_detectors.end()) {
//    exc::exceptionG4 e("GEOM","Fatal error in Argument",1);
//    e<<"Failed to retrieve detector element of type " << name << "\n";
//    e.error();
//  }
//
//  // this must succeed or there is something terribly wrong
//  DET* d = dynamic_cast<DET*>(it->second.get());
//
//  if(d==0) {
//    exc::exceptionG4 e("GEOM","Fatal error in Argument",1);
//    e<<"Failed to convert found detector "<< name << " to its correct type.  There is a serious problem.\n";
//    e.error();
//  }
//
//  return d;
//}

void GeomService::setAllowReplacement(bool allowReplacement) {
  if ( _isRead ) {
    std::cout<<"Warning! GeomService: configuration file already initialized at "<<_config->inputFile()<<std::endl;
    std::cout<<"Setting of allowReplacement ignored"<<std::endl;
  } else {
    _allowReplacement = allowReplacement;
  }
}

void GeomService::setMessageOnDefault(bool messageOnDefault) {
  if ( _isRead ) {
    std::cout<<"Warning! GeomService: configuration file already initialized at "<<_config->inputFile()<<std::endl;
    std::cout<<"Setting of messageOnDefault ignored"<<std::endl;
  } else {
    _messageOnDefault = messageOnDefault;
  }
}

void GeomService::setMessageOnReplacement(bool messageOnReplacement) {
  if ( _isRead ) {
    std::cout<<"Warning! GeomService: configuration file already initialized at "<<_config->inputFile()<<std::endl;
    std::cout<<"Setting of messageOnReplacement ignored"<<std::endl;
  } else {
    _messageOnReplacement = messageOnReplacement;
  }
}

//void GeomService::instanceActiveDet() {
//
//  if (_config->getBool("hasCDCH",false)){
//    cdch::CDCHMaker cdchtm( *_config );
//    addDetector( cdchtm.getCDCHtrackerPtr() );
//  }
//
//}

// This template can be defined here because this is a private method which is only
// used by the code below in the same file.
//template <typename DET> void GeomService::addDetector(std::unique_ptr<DET> d)
//{
//  if(_detectors.find(typeid(DET).name())!=_detectors.end()) {
//    exc::exceptionG4 e("GEOM","Fatal Error",0);
//    e<<"Failed to install detector with type name " << typeid(DET).name() << "\n";
//    e.error();
//  }
//
//    DetectorPtr ptr(d.release());
//    _detectors[typeid(DET).name()] = ptr;
//}

//template <typename DETALIAS, typename DET> void GeomService::addDetectorAliasToBaseClass(std::unique_ptr<DET> d)
//{
//
//  std::string OriginalName = typeid(DET).name();
//  DetMap::iterator it(_detectors.find(OriginalName));
//
//  if(it==_detectors.end()) {
//    exc::exceptionG4 e("GEOM","Fatal Error",0);
//    e<<"Can not alias an inexistant detector, detector " << OriginalName << "\n";
//    e.error();
//  }
//
//  std::string detectorName= typeid(DETALIAS).name() ;
//  _detectors[detectorName] = it->second;
//}
