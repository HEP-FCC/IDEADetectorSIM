/*
 * GeomService.h
 *
 *  Created on: Mar 27, 2017
 *      Author: tassiell
 */

#ifndef G4GMC_INCLUDE_GEOMSERVICE_HH_
#define G4GMC_INCLUDE_GEOMSERVICE_HH_

// CRD include files
#include "SimpleConfig.hh"

// G4GMC include files
#include "Detector.hh"
#include "manageException.hh"

// C++ include files
#include <string>
#include <memory>
#include <map>

// Boost include files
#include "boost/shared_ptr.hpp"

class GeomService {

public:
  static GeomService *Instance();
  static GeomService *Instance(std::string inputfile, std::string folder="config");
  ~GeomService();

  const crd::SimpleConfig& getConfig() const {
    return *_config;
  }

  void setAllowReplacement(bool allowReplacement);
  void setMessageOnDefault(bool messageOnDefault);
  void setMessageOnReplacement(bool messageOnReplacement);

//  std::string getInputfile() const {
//    return _inputfile;
//  }

  template <class DET>
  bool hasElement()
  {
//    if(_run_count==0)
//      exceptionG4("GEOM",
//          "Cannot get detectors from an unconfigured geometry service.\n",
//          0,
//          "You've attempted to a get an element before the first run\n");

    // to use this generic way requires a map of names (typeid?) to
    // abstract elements.
    // find the detector element requested
    std::string name = typeid(DET).name();
    DetMap::iterator it(_detectors.find(name));

    return !(it==_detectors.end());

  }

  template <typename DET> void addDetector(std::unique_ptr<DET> d){
    if(_detectors.find(typeid(DET).name())!=_detectors.end()) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<<"Failed to install detector with type name " << typeid(DET).name() << "\n";
      e.error();
    }

      DetectorPtr ptr(d.release());
      _detectors[typeid(DET).name()] = ptr;
  }


protected:
  GeomService();

  void ReadCfgFile(std::string &infilename, std::string &folder);
  static GeomService* _instance;

  crd::SimpleConfig *_config;

  // Control the behaviour of messages from the SimpleConfig object holding
  // the geometry parameters.
  bool _allowReplacement;
  bool _messageOnReplacement;
  bool _messageOnDefault;

  bool _isRead;

private:
  typedef boost::shared_ptr<Detector> DetectorPtr;
  typedef std::map<std::string,DetectorPtr> DetMap;

  template <typename DET> friend class GeomHandle;

  template <class DET>
  DET* getElement(){
    //    if(_run_count==0)
    //      exceptionG4("GEOM",
    //          "Cannot get detectors from an unconfigured geometry service.\n",
    //          0,
    //          "You've attempted to a get an element before the first run\n");

      // to use this generic way requires a map of names (typeid?) to
      // abstract elements.
      // find the detector element requested
      std::string name = typeid(DET).name();
      DetMap::iterator it(_detectors.find(name));
      if(it==_detectors.end()) {
        exc::exceptionG4 e("GEOM","Fatal error in Argument",1);
        e<<"Failed to retrieve detector element of type " << name << "\n";
        e.error();
      }

      // this must succeed or there is something terribly wrong
      DET* d = dynamic_cast<DET*>(it->second.get());

      if(d==0) {
        exc::exceptionG4 e("GEOM","Fatal error in Argument",1);
        e<<"Failed to convert found detector "<< name << " to its correct type.  There is a serious problem.\n";
        e.error();
      }

      return d;
    }


//  void instanceActiveDet();

  // All of the detectors that we know about.
  DetMap _detectors;

  // Don't need to expose definition of private template in header
//  template <typename DET> void addDetector(std::unique_ptr<DET> d);
  template <typename DETALIAS, typename DET> void addDetectorAliasToBaseClass(std::unique_ptr<DET> d){

    std::string OriginalName = typeid(DET).name();
    DetMap::iterator it(_detectors.find(OriginalName));

    if(it==_detectors.end()) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<<"Can not alias an inexistant detector, detector " << OriginalName << "\n";
      e.error();
    }

    std::string detectorName= typeid(DETALIAS).name() ;
    _detectors[detectorName] = it->second;
  }


};

#endif /* G4GMC_INCLUDE_GEOMSERVICE_HH_ */
