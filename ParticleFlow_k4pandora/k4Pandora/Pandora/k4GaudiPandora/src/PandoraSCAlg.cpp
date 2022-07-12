#include "GearSvc/IGearSvc.h"
#include "PandoraSCAlg.h"
#include "edm4hep/Vector3f.h"
#include "edm4hep/Vector3d.h"
#include "edm4hep/SimCalorimeterHit.h"
#include "edm4hep/CaloHitContribution.h"
#include "edm4hep/ClusterConst.h"
#include <cmath>
#include <algorithm>
#include "gear/BField.h"
#include <gear/GEAR.h>
#include "LCContent.h"

#include <fstream>
#include <iostream>


pandora::Pandora* PandoraSCAlg::m_pPandora=0;

DECLARE_COMPONENT( PandoraSCAlg )

template<typename T ,typename T1>
StatusCode getCol(T & t, T1 & t1)
{
  try {
   t1 = t.get();
     }
 catch ( GaudiException &e ) {
        std::cout << "Collection " << t.fullKey() << " is unavailable in event "  << std::endl;
    }
    return StatusCode::SUCCESS;
}



PandoraSCAlg::PandoraSCAlg(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc),
    _nEvt(0),
    maxHitSize(0)
{
 m_CollectionMaps = new CollectionMaps();
 declareProperty("WriteCaloHitCollection"              , m_SC_CaloHitCollection_w,               "Handle of the SC_CaloHitCollection               output collection" );
}


void PandoraSCAlg::FinaliseSteeringParameters(ISvcLocator* svcloc)
{
    // ATTN: This function seems to be necessary for operations that cannot easily be performed at construction of the processor,
    // when the steering file is parsed e.g. the call to GEAR to get the inner bfield
    std::cout << "FinaliseSteeringParameters " << std::endl; 
    
    IGearSvc*  iSvc = 0;
    StatusCode sc = svcloc->service("GearSvc", iSvc, false);
    if ( !sc ) 
    {
        throw "Failed to find GearSvc ...";
    }
    gear::GearMgr* _GEAR = iSvc->getGearMgr();

}



StatusCode PandoraSCAlg::initialize()
{

  std::cout<<"init PandoraSCAlg"<<std::endl;
  for ( const auto& col : m_readCols ) {
      auto seperater = col.find(':');
      std::string colType = col.substr(0, seperater);
      std::string colName = col.substr(seperater+1);
      m_collections[colName] = colType;
      std::cout << "colType " << colType << std::endl;

      if ( colType == "CalorimeterHit" ) {
          m_dataHandles[colName] =
              new DataHandle<edm4hep::CalorimeterHitCollection>(colName, Gaudi::DataHandle::Reader, this);
	  std::cout << "object dataHandles created with address " <<  m_dataHandles[colName] << std::endl;
      }
      else {
            error() << "invalid collection type: " << colType << endmsg;
            return StatusCode::FAILURE;
      }
  }

  // XML file
  m_settings.m_pandoraSettingsXmlFile =  m_PandoraSettingsXmlFile ; 

  try
  {
      std::cout << "in try " << std::endl;

      ISvcLocator* svcloc = serviceLocator();

      m_pPandora = new pandora::Pandora();

      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RegisterUserComponents());
      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::ReadSettings(*m_pPandora, m_settings.m_pandoraSettingsXmlFile));

  }
  catch (pandora::StatusCodeException &statusCodeException)
  {
      std::cout << "Failed to initialize gaudi pandora: " << statusCodeException.ToString() << std::endl;
      throw statusCodeException;
  }
  catch (...)
  {
      std::cout << "Failed to initialize gaudi pandora: unrecognized exception" << std::endl;
      throw;
  }

  return GaudiAlgorithm::initialize();
}

StatusCode PandoraSCAlg::execute()
{

  info() << "************************************ event " << _nEvt << " ****************************************** " << endmsg;
    
  pCaloHitCollection  = m_SC_CaloHitCollection_w.createAndPut();

  try
    {
      updateMap();
      
      for(auto &v : m_dataHandles){
	
	if(m_collections[v.first] != "CalorimeterHit"){
	  error() << "FATAL coll type!" << endmsg;
	  return StatusCode::FAILURE;
	}
	
	if(m_collections[v.first] == "CalorimeterHit"){
	  auto handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*> (v.second);
	  auto po = handle->get();
	  if(po != NULL){
	    
	    std::cout<<"*************************************************************SCsaved col name************************************="<<v.first<<std::endl;	  
	    
	    std::cout<<"*************************************************************po->size() = ************************************="<<po->size()<<std::endl;	  
	    
	    for(unsigned int i=0 ; i< po->size(); i++) {
	      
	      edm4hep::CalorimeterHit m_pCaloHit = pCaloHitCollection->create();
	      
	      copyHit(m_pCaloHit,po->at(i));
	      
	      if((v.first) == "S_CalorimeterHits"){
		m_pCaloHit.setType(0);		
	      }
	      else{
		if((v.first) == "C_CalorimeterHits")
		  m_pCaloHit.setType(1);		
	      }

	    }
	    if(po->size() > maxHitSize) 
	      maxHitSize = po->size();

	  }//calohits

	}
      }

      std::cout << "nElements in pCaloHitCollection= "  << pCaloHitCollection->size()<< std::endl;

      this->Reset();
    }
    catch (pandora::StatusCodeException &statusCodeException)
    {
        std::cout << "Gaudi pandora failed to process event: " << statusCodeException.ToString() << std::endl;
        throw statusCodeException;
    }
    catch (...)
    {
        std::cout << "Gaudi pandora failed to process event: unrecognized exception" << std::endl;
        throw;
    }
  
  info() << "PandoraSCAlg Processed " << _nEvt << " events " << endmsg;
  _nEvt ++ ;

  return StatusCode::SUCCESS;
}






StatusCode PandoraSCAlg::finalize()
{
  info() << "Finalized. Processed " << _nEvt << " events " << endmsg;

  info() <<"maxHitSize = " << maxHitSize << endmsg;

  delete m_pPandora;

  return GaudiAlgorithm::finalize();
}

//copyHit function.....TODO: ask to implement a copy function centrally
void PandoraSCAlg::copyHit(edm4hep::CalorimeterHit targetHit, edm4hep::CalorimeterHit poHit)
{
  targetHit.setEnergy(poHit.getEnergy());
  targetHit.setType(poHit.getType());
  targetHit.setEnergyError(poHit.getEnergyError());
  targetHit.setCellID(poHit.getCellID());
  targetHit.setTime(poHit.getTime());
  targetHit.setPosition(poHit.getPosition());
}


pandora::StatusCode PandoraSCAlg::RegisterUserComponents() const
{
    
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, LCContent::RegisterAlgorithms(*m_pPandora));
    
    return pandora::STATUS_CODE_SUCCESS;
}


void PandoraSCAlg::Reset()
{
    m_CollectionMaps->clear();
}

const pandora::Pandora *PandoraSCAlg::GetPandora() const
{
    if (NULL == m_pPandora)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora;
}
PandoraSCAlg::Settings::Settings()
{
}
CollectionMaps::CollectionMaps()
{
}
void CollectionMaps::clear()
{
collectionMap_MC.clear();
collectionMap_CaloHit.clear();
collectionMap_Vertex.clear();
collectionMap_Track.clear();
collectionMap_CaloRel.clear();
collectionMap_TrkRel.clear();
}


StatusCode PandoraSCAlg::updateMap()
{


  for(auto &v : m_dataHandles){
    try{
      if(m_collections[v.first]=="CalorimeterHit"){
	auto handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*> (v.second);
	auto po = handle->get();
	if(po != NULL){
	  std::vector<edm4hep::CalorimeterHit> v_cal;
	  m_CollectionMaps->collectionMap_CaloHit[v.first] = v_cal ;

	  for(unsigned int i=0 ; i< po->size(); i++) {
	    m_CollectionMaps->collectionMap_CaloHit [v.first].push_back(po->at(i));
	  }
	}
	else{
	  std::cout<<"don't find col name="<<v.first<<std::endl;
	}
      }//calohit
    }//try
    catch(...){
      std::cout<<"don't find "<<v.first<<"in event"<<std::endl;
      std::cout<<"don't find  col name="<<v.first<<",with type="<<m_collections[v.first]<<" in this event"<<std::endl;
    }
  }

  return StatusCode::SUCCESS;
}
 
