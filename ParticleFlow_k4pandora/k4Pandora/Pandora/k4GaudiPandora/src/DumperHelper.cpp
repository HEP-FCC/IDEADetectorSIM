/**
 * 
 *  @brief  Implementation of Dumper Helper class. 
 *          Reads the output map of the CalohitCreator_SC class and dumps the relevant info in a txt (or svc) file and stores TH1 objects.
 *  A. D'Onofrio, 10/06/2022
 * 
 *  $Log: $
 */


#include "gear/GearParameters.h"
#include "gear/CalorimeterParameters.h"
#include "gear/GearDistanceProperties.h"
#include "gear/GearPointProperties.h"
#include "gear/GEAR.h"
#include "gear/TPCParameters.h"
#include "gear/PadRowLayout2D.h"
#include "gear/LayerLayout.h"

#include "cellIDDecoder.h"
#include "GaudiKernel/IService.h"
#include "GearSvc/IGearSvc.h"

#include "PandoraSCAlg.h"
#include "CaloHitCreator.h"
#include "DumperHelper.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <iostream>
#include <fstream>


DumperHelper::DumperHelper(const CaloHitCreator::Settings& settings, const pandora::Pandora *const pPandora) :
    m_settings(settings),
    m_pPandora(pPandora)
{
  //  my_collectionMap_CaloHit = new std::map<int, const edm4hep::CalorimeterHit* >;
  std::cout << "Dumper Helper SC constructor " << std::endl;
  //  declareProperty("outFileDumperTxTNelXML"               , m_outFileDumperTxT,                "Handle of the VertexCollection                output collection" );  
}

//------------------------------------------------------------------------------------------------------------------------------------------

DumperHelper::~DumperHelper()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------



pandora::StatusCode DumperHelper::my_ReadSettings(const std::string &xmlFileName)
{
  //  PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, DumperHelper::ReadValue(xmlFileName, "MyMandatoryString", m_myMandatoryString));
}




//------------------------------------------------------------------------------------------------------------------------------------------
pandora::StatusCode DumperHelper::WriteToFile( edm4hep::CalorimeterHitCollection      *pCaloHitCollection, std::ofstream &txtFileName, TFile  *out_dumper_histo) const
{
  
  if (txtFileName.is_open())
    {
      txtFileName << "nElements in pCaloHitCollection= "  << pCaloHitCollection->size()<< " ; \n";
      txtFileName << "CaloHitType        Energy        EnergyError        time        position X        position Y        position Z        type       cellID\n";
    }

  for(int i = 0; i < pCaloHitCollection->size(); ++i){
    txtFileName <<pCaloHitCollection->at(i).getType()<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getEnergy()<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getEnergyError()<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getTime()<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getPosition()[0]<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getPosition()[1]<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getPosition()[2]<<" ; ";
    txtFileName <<pCaloHitCollection->at(i).getCellID()<<" ; \n";
  }

    return pandora::STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------
