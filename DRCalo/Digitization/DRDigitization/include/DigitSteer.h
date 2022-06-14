#ifndef DR_DIGITSTEER
#define DR_DIGITSTEER

//std includes

#include <string>

// podio includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

#include "FiberDigitizer.h"

class DigitSteer
{

  /** Class to steer the digitization of dual readout SimCaloriemeterHits collections. 
      The class makes heavy use of a FiberDigitizer object, which needs to be configured and passed to this class for its operations. 
      The configuration of the SiPM sensor is external to this class. 
      DigitSteer can run a calibration step as well. 
  **/
  
 public:
  
  DigitSteer();
  ~DigitSteer();
  bool Process(); // Run digitization (and calibration, if DoCalibration == true)

  // Functions to deal with file opening and closing
  bool LoadFile(std::string input_file_name);
  bool CloseFile();

  // Setters for the collection names
  void SetScintInputHitsName(std::string input_hits_name) {m_inputScintHitsName=input_hits_name;}
  void SetCherInputHitsName(std::string input_hits_name) {m_inputCherHitsName=input_hits_name;}
  void SetAuxInputHitsName(std::string input_hits_name) {m_inputScintHitsName=input_hits_name;}
  void SetScintOutputHitsName(std::string hits_name) {m_outputScintHitsName=hits_name;}
  void SetCherOutputHitsName(std::string hits_name) {m_outputCherHitsName=hits_name;}
  void SetAuxOutputHitsName(std::string hits_name) {m_outputCherHitsName=hits_name;}

  void SetOutputEventStore(podio::EventStore * store) {l_store = store;}
  void SetOutputROOTWriter(podio::ROOTWriter * writer) {l_writer = writer;} 

  void SetFiberDigitizer(FiberDigitizer * l_digitizer){m_digitizer = l_digitizer;}
  const FiberDigitizer * GetFiberDigitizer() {return m_digitizer;}

  // A few flags
  void SetDebug(bool debug = true) {m_debug = debug;}
  void DoCalibration(bool doCal = true) {m_doCalibration = doCal;}
  
 private:
  std::string m_inputCherHitsName;
  std::string m_inputScintHitsName;
  std::string m_inputAuxHitsName;
  std::string m_outputCherHitsName;
  std::string m_outputScintHitsName;
  std::string m_outputAuxHitsName;
  std::string m_outputFileName;
  bool m_debug;
  bool m_doCalibration;
  podio::ROOTReader m_reader;
  podio::EventStore m_read_store;
  podio::ROOTWriter * l_writer;
  podio::EventStore * l_store;
  FiberDigitizer * m_digitizer;
};

#endif
