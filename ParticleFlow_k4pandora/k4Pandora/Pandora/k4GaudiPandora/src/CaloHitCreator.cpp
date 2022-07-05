/**
 * 
 *  @brief  Implementation of the calo hit creator class.
 * 
 *  $Log: $
 */

#include "GaudiKernel/IService.h"
#include "GearSvc/IGearSvc.h"


#include "PandoraSCAlg.h"
#include "CaloHitCreator.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <iostream>
#include <fstream>
#include <list>




CaloHitCreator::CaloHitCreator(const Settings &settings, const pandora::Pandora *const pPandora, ISvcLocator* svcloc, bool encoder_style) :
  m_outputClusterListName(),
  m_settings(settings),
  m_pPandora(pPandora)
{
  std::cout << "CaloHit Creator constructing " << std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitCreator::~CaloHitCreator()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode CaloHitCreator::CreateCaloHits(const CollectionMaps& collectionMaps)
{
    
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreateHCalCaloHits (collectionMaps));

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode CaloHitCreator::CreateHCalCaloHits(const CollectionMaps& collectionMaps)
{

  for (StringVector::const_iterator iter = m_settings.m_hCalCaloHitCollections.begin(), iterEnd = m_settings.m_hCalCaloHitCollections.end();
       iter != iterEnd; ++iter)
    {
        if(collectionMaps.collectionMap_CaloHit.find(*iter) == collectionMaps.collectionMap_CaloHit.end()) { std::cout<<"not find "<<(*iter)<<std::endl; continue;}
        try
        {

	  std::cout<<"Found HCalo"<<std::endl;
            const std::vector<edm4hep::CalorimeterHit>& pCaloHitCollection = (collectionMaps.collectionMap_CaloHit.find(*iter))->second;
	    const int nElements(pCaloHitCollection.size());

	    std::cout<<"iter = "<<(*iter)<<std::endl;
	    std::cout<<"Found HCalo size = "<<nElements<<std::endl;
            if (0 == nElements)
                continue;

	    //first element of the map, filled with the S/C value
	    std::list<edm4hep::CalorimeterHit> pTemporaryList;
	    //	    pandora::CaloHitList pTemporaryList;

            for (int i = 0; i < nElements; ++i)
            {
                try
                {
                    const edm4hep::CalorimeterHit& pCaloHit0 = pCaloHitCollection.at(i);
                    const edm4hep::CalorimeterHit* pCaloHit = &(pCaloHit0);

                    if (NULL == pCaloHit)
                        throw ("CreateHCalCaloHits Collection type mismatch");
                    
		    edm4hep::CalorimeterHit m_pCaloHit = *pCaloHit;

		    std::cout<<"caloHit print type "<<pCaloHit->getType()<<std::endl;
		    std::cout<<"caloHit print energy "<<pCaloHit->getEnergy()<<std::endl;

		    if((*iter) == "S_CalorimeterHits")
		      m_pCaloHit.setType(0);		
		    else{
		      if((*iter) == "C_CalorimeterHits")
			m_pCaloHit.setType(1);		
		    }

		    std::cout<<"My caloHit print type "<<m_pCaloHit.getType()<<std::endl;
		    std::cout<<"My caloHit print energy "<<m_pCaloHit.getEnergy()<<std::endl;

		    //		    PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::CaloHit::Create(*m_pPandora, caloHitParameters));

		    //		    if (!PandoraContentApi::IsAvailable(*this, m_pCaloHit))
		    //		      continue;
		  
		    pTemporaryList.push_back(m_pCaloHit);

                }
                catch (pandora::StatusCodeException &statusCodeException)
                {
                    std::cout << "Failed to extract hcal calo hit: " << statusCodeException.ToString() << std::endl;
                }
                catch (...)
                {
                    std::cout<<"Failed to extract hcal calo hit" << std::endl;
                }
            }

	    //	    PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTemporaryList));
	    //	    PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*m_pPandora, pTemporaryList));
	    //	    PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<edm4hep::CalorimeterHit>(*this, pTemporaryList, m_outputClusterListName));

	    std::cout<<"pTemporaryList->size() = "<<pTemporaryList.size()<<std::endl;

	    for ( edm4hep::CalorimeterHit x : pTemporaryList) {
	      std::cout <<"type     "<< x.getType() << "     energy    " <<x.getEnergy() << '\n';
	    }

        }
        catch (...)
        {
            std::cout << "Failed to extract hcal calo hit collection: " << *iter << std::endl;
        }
    }

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitCreator::Settings::Settings() :
  m_eCalScToHadGeVEndCap(1.f)
{
}
