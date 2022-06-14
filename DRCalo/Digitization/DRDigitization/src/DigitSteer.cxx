#include "DigitSteer.h"

#include "DRCalo_FiberType.h"

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"

#include "podio/ROOTWriter.h"

#include <iostream>

bool DigitSteer::Process()
{

  if (m_digitizer == NULL) {
    std::cerr << "ERROR in DigitSteer::Process : the pointer to the FiberDigitizer is NULL.  Cannot process the input file for digitization. Please set it to something valid using the SigitSteer::SetFiberDigitizer methood" << std::endl;
    return false;
  }
  
  //  podio::EventStore l_store;                                                                                 
  //podio::ROOTWriter l_writer->m_outputFileName,&l_store);
  
  unsigned int nevents = m_reader.getEntries();
  
  edm4hep::CalorimeterHitCollection & s_recoCaloHits = l_store->create<edm4hep::CalorimeterHitCollection>(m_outputScintHitsName );             
  l_writer->registerForWrite(m_outputScintHitsName);

   edm4hep::CalorimeterHitCollection & c_recoCaloHits = l_store->create<edm4hep::CalorimeterHitCollection>(m_outputCherHitsName );             
  l_writer->registerForWrite(m_outputCherHitsName );

  edm4hep::CalorimeterHitCollection & auxCaloHits = l_store->create<edm4hep::CalorimeterHitCollection>(m_outputAuxHitsName );             

  // check that the digitiser has a sensor set.

  if (m_digitizer->GetSensor() == NULL){
    std::cout << "WARNING!!!!!!! No sensor has been set for the digitizer. The digitisation step will be dummy" << std::endl;
  }
  
  // main loop over events

  for (unsigned int i_evt = 0; i_evt < nevents; ++i_evt){

    if (i_evt%100 == 0) std::cout << "Processed " << i_evt << " events" << std::endl;

    // retrieve the simulation hits
    
    auto & s_hitColl = m_read_store.get<edm4hep::SimCalorimeterHitCollection>(m_inputScintHitsName);

    if (!s_hitColl.isValid()){
      std::cerr << "Cannot read S simulation hits with name " << m_inputScintHitsName << std::endl;
      return false;
    }

    m_debug=true;

    if (m_debug) std::cout << "Number of s fibers fired " << s_hitColl.size() << std::endl;

    m_digitizer->Digitize(s_hitColl,s_recoCaloHits);    

    auto & c_hitColl = m_read_store.get<edm4hep::SimCalorimeterHitCollection>(m_inputCherHitsName);

    if (!c_hitColl.isValid()){
      std::cerr << "Cannot read C simulation hits with name " << m_inputCherHitsName << std::endl;
      return false;
    }

    if (m_debug) std::cout << "Number of c fibers fired " << c_hitColl.size() << std::endl;

    m_digitizer->Digitize(c_hitColl,c_recoCaloHits);

    // now calibrate the collections

    if (m_doCalibration){

      if (!m_digitizer->Calibrate(s_recoCaloHits,DRCalo_FiberType::S)){
	std::cerr << "Cannot calibrate scint collection " << std::endl;
	return false;
      }
      if (!m_digitizer->Calibrate(c_recoCaloHits,DRCalo_FiberType::C)){
	std::cerr << "Cannot calibrate cher collection " << std::endl;
	return false;
      }
    }

    // Final cleanup

    l_writer->writeEvent();
    m_read_store.clear();
    m_reader.endOfEvent();
    l_store->clearCollections();
  }
  l_writer->finish();
  return true;
}

DigitSteer::DigitSteer():
  m_inputCherHitsName("C_caloHits"),
  m_inputScintHitsName("S_caloHits"),
  m_inputAuxHitsName("Auxiliary_infoHits"),
  m_outputCherHitsName("C_CalorimeterHits"),
  m_outputScintHitsName("S_CalorimeterHits"),
  m_outputAuxHitsName("Aux_CalorimeterInfoHits"),
  m_outputFileName("digi_output.podio.root"),
  m_debug(false),
  m_doCalibration(false),
  m_digitizer(NULL)
{
}

DigitSteer::~DigitSteer()
{
}

bool DigitSteer::LoadFile(std::string input_file_name)
{
  m_reader.openFile(input_file_name);

  m_read_store.setReader(&m_reader);

  unsigned int nevents = m_reader.getEntries();
  std::cout << "Loaded file " << input_file_name << " with " << nevents << " entries " << std::endl;
  
  return true;
}

bool DigitSteer::CloseFile()
{
  m_reader.closeFile();
  return true;
}

