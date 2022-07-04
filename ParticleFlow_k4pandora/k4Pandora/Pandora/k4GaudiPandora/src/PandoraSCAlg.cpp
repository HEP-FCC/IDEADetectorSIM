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
    _nEvt(0)
{
 m_CollectionMaps = new CollectionMaps();
 declareProperty("WriteCaloHitCollection"              , m_SC_CaloHitCollection_w,               "Handle of the SC_CaloHitCollection               output collection" );
 declareProperty("WriteCaloHitCollection_S"              , m_S_CaloHitCollection_w,               "Handle of the S_CaloHitCollection               output collection" );
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
      info() << "colType " << colType << endmsg;

      if ( colType == "CalorimeterHit" ) {
          m_dataHandles[colName] =
              new DataHandle<edm4hep::CalorimeterHitCollection>(colName, Gaudi::DataHandle::Reader, this);
	  info() << "object dataHandles created with address " <<  m_dataHandles[colName] << endmsg;
      }
      else {
            error() << "invalid collection type: " << colType << endmsg;
            return StatusCode::FAILURE;
      }
  }

  // XML file
  m_settings.m_pandoraSettingsXmlFile =  m_PandoraSettingsXmlFile ; 
  //caloHit settings to implement
  //  m_caloHitCreatorSettings.m_hCalCaloHitCollections = m_HCalCaloHitCollections;

  try
  {
    ISvcLocator* svcloc = serviceLocator();
    info() << "svcloc " << svcloc << endmsg;

      m_pPandora = new pandora::Pandora();
      info() << " new pandora alg created " <<  m_pPandora <<endmsg;

      m_pCaloHitCreator = new CaloHitCreator(m_caloHitCreatorSettings, m_pPandora, svcloc, 0);
      info()<< " new CaloHitCreator " <<  m_pCaloHitCreator << endmsg;

      m_pDumperHelper = new DumperHelper(m_caloHitCreatorSettings, m_pPandora);
      info()<< " new DumperHelper " <<  m_pDumperHelper << endmsg;

      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RegisterUserComponents());
      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::ReadSettings(*m_pPandora, m_settings.m_pandoraSettingsXmlFile));

  }
  catch (pandora::StatusCodeException &statusCodeException)
  {
    error() << "Failed to initialize gaudi pandora: " << statusCodeException.ToString() << endmsg;
      throw statusCodeException;
  }
  catch (...)
  {
    error() << "Failed to initialize gaudi pandora: unrecognized exception" << endmsg;
      throw;
  }

  myfile.open("exampleOutputTxT.txt");

  out_dumper_histo = new TFile("./exampleOutputFile.root","RECREATE","example");

  //declareHistosSC();
  h_type = new TH1F ("h_type", "type", 10, 0.,10.);

  h_energy_S    = new TH1F ("h_energy_S", "energy_S", 16, 0.,16.);              
  h_positionX_S = new TH1F ("h_positionX_S", "positionX_S", 1000, -3000.,3000.);
  h_positionY_S = new TH1F ("h_positionY_S", "positionY_S", 1000, -3000.,3000.);  //open the txt file for the dumper
  h_positionZ_S = new TH1F ("h_positionZ_S", "positionZ_S", 10, -3000.,-1900.) ;


  h_energy_C    = new TH1F ("h_energy_C", "energy_C", 16, 0.,16.)              ;
  h_positionX_C = new TH1F ("h_positionX_C", "positionX_C", 1000, -3000.,3000.);
  h_positionY_C = new TH1F ("h_positionY_C", "positionY_C", 1000, -3000.,3000.);
  h_positionZ_C = new TH1F ("h_positionZ_C", "positionZ_C", 10, -3000.,-1900.) ;
  

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
	
	auto handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*> (v.second);
	auto po = handle->get();
	if(po != NULL){
	  
	  //	  std::cout<<"*************************************************************SCsaved col name************************************="<<v.first<<std::endl;	  

	  //	  std::cout<<"*************************************************************po->size() = ************************************="<<po->size()<<std::endl;	  
	  
	  for(unsigned int i=0 ; i< po->size(); i++) {
	    
	    edm4hep::CalorimeterHit m_pCaloHit = pCaloHitCollection->create();
	    
	    //	    std::cout<<"hit energy Before of setType " <<m_pCaloHit.getEnergy()<<std::endl;	    
	    //	    std::cout<<"hit type   Before of setType " <<m_pCaloHit.getType()<<std::endl;	    
	    
	    copyHit(m_pCaloHit,po->at(i));

	    if((v.first) == "S_CalorimeterHits"){
	      m_pCaloHit.setType(0);		
	    }
	    else{
	      if((v.first) == "C_CalorimeterHits")
		m_pCaloHit.setType(1);		
	    }
	    
	    //	    std::cout<<"hit energy After setType " <<m_pCaloHit.getEnergy()<<std::endl;	    
	    //	    std::cout<<"hit type   After setType " <<m_pCaloHit.getType()<<std::endl;	    	    
	  }
	}
      }

      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, m_pDumperHelper->WriteToFile(pCaloHitCollection, myfile));
      //fill histos S/0 type.....to be improved
      //fill histos C/1 type.....to be improved
      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, m_pDumperHelper->FillHistos(pCaloHitCollection, h_type, h_energy_S,h_positionX_S,h_positionY_S,h_positionZ_S, h_energy_C,h_positionX_C,h_positionY_C,h_positionZ_C));

      //      std::cout << "nElements in pCaloHitCollection= "  << pCaloHitCollection->size()<< std::endl;

      this->Reset();
    }
    catch (pandora::StatusCodeException &statusCodeException)
    {
      error() << "Gaudi pandora failed to process event: " << statusCodeException.ToString() <<endmsg;
        throw statusCodeException;
    }
    catch (...)
    {
      error() << "Gaudi pandora failed to process event: unrecognized exception" << endmsg;
        throw;
    }
  
  debug() << "PandoraSCAlg Processed " << _nEvt << " events " << endmsg;
  _nEvt ++ ;

  return StatusCode::SUCCESS;
}






StatusCode PandoraSCAlg::finalize()
{
  info() << "Finalized. Processed " << _nEvt << " events " << endmsg;

  out_dumper_histo->cd();
  writeHistoSC();
  out_dumper_histo->Close();
  //deleteHistoSC();

  delete m_pPandora;
  delete m_pCaloHitCreator;
  return GaudiAlgorithm::finalize();
}

//Adele, copyHit function.....TODO: ask to implement a copy function centrally
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
    m_pCaloHitCreator->Reset();
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


void PandoraSCAlg::writeHistoSC()
{
  h_type->Write();

  h_energy_S->Write();   
  h_positionX_S->Write();   
  h_positionY_S->Write();   
  h_positionZ_S->Write();   

  h_energy_C->Write();   
  h_positionX_C->Write();   
  h_positionY_C->Write();   
  h_positionZ_C->Write();   

}


void PandoraSCAlg::deleteHistoSC()
{
  delete h_type;
  delete h_energy_S;
  delete h_positionX_S;
  delete h_positionY_S;
  delete h_positionZ_S;
  delete h_energy_C;
  delete h_positionX_C;
  delete h_positionY_C;
  delete h_positionZ_C;

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
	  info()<<"don't find col name="<<v.first<<endmsg;
	}
      }
    }//try
    catch(...){
      error()<<"don't find "<<v.first<<"in event"<<endmsg;
      error()<<"don't find  col name="<<v.first<<",with type="<<m_collections[v.first]<<" in this event"<<endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
 
