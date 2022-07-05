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
pandora::StatusCode DumperHelper::WriteToFile( edm4hep::CalorimeterHitCollection      *pCaloHitCollection, std::ofstream &txtFileName) const
{
  
  Int_t     type        = -999;
  Float_t   energy      = -999.;
  Float_t   energyError = -999.;
  Float_t   time        = -999.;
  Float_t   positionX   = -999.;
  Float_t   positionY   = -999.;
  Float_t   positionZ   = -999.;
  ULong64_t cellID      = -999;


  if (txtFileName.is_open())
    {
      txtFileName << "nElements in pCaloHitCollection= "  << pCaloHitCollection->size()<< " ; \n";
      txtFileName << "CaloHitType        Energy        EnergyError        time        position X        position Y        position Z        type       cellID\n";
    }

  for(int i = 0; i < pCaloHitCollection->size(); ++i){
    //set variables
    type        = pCaloHitCollection->at(i).getType()       ;
    energy      = pCaloHitCollection->at(i).getEnergy()     ;
    energyError = pCaloHitCollection->at(i).getEnergyError();
    time        = pCaloHitCollection->at(i).getTime()       ;
    positionX   = pCaloHitCollection->at(i).getPosition()[0];
    positionY   = pCaloHitCollection->at(i).getPosition()[1];
    positionZ   = pCaloHitCollection->at(i).getPosition()[2];
    cellID      = pCaloHitCollection->at(i).getCellID()     ;

    //write to txt file
    txtFileName <<type<<" ; ";
    txtFileName <<energy<<" ; ";
    txtFileName <<energyError<<" ; ";
    txtFileName <<time<<" ; ";
    txtFileName <<positionX<<" ; ";
    txtFileName <<positionY<<" ; ";
    txtFileName <<positionZ<<" ; ";
    txtFileName <<cellID<<" ; \n";

  }

    return pandora::STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------
pandora::StatusCode DumperHelper::FillHistos( edm4hep::CalorimeterHitCollection      *pCaloHitCollection, TH1F *h_type, TH1F *h_energy_S, TH1F *h_positionX_S, TH1F *h_positionY_S, TH1F *h_positionZ_S, TH1F *h_energy_C, TH1F *h_positionX_C, TH1F *h_positionY_C, TH1F *h_positionZ_C) const
{
  
  Int_t     type        = -999;
  Float_t   energy      = -999.;
  Float_t   energyError = -999.;
  Float_t   time        = -999.;
  Float_t   positionX   = -999.;
  Float_t   positionY   = -999.;
  Float_t   positionZ   = -999.;
  ULong64_t cellID      = -999;

  for(int i = 0; i < pCaloHitCollection->size(); ++i){
    //set variables
    type        = pCaloHitCollection->at(i).getType()       ;
    energy      = pCaloHitCollection->at(i).getEnergy()     ;
    energyError = pCaloHitCollection->at(i).getEnergyError();
    time        = pCaloHitCollection->at(i).getTime()       ;
    positionX   = pCaloHitCollection->at(i).getPosition()[0];
    positionY   = pCaloHitCollection->at(i).getPosition()[1];
    positionZ   = pCaloHitCollection->at(i).getPosition()[2];
    cellID      = pCaloHitCollection->at(i).getCellID()     ;

    //fill histos
    h_type->Fill(type);

    if(type == 0){
      h_energy_S->Fill(energy);
      h_positionX_S->Fill(positionX);
      h_positionY_S->Fill(positionY);
      h_positionZ_S->Fill(positionZ);
    }

    if(type == 1){
      h_energy_C->Fill(energy);
      h_positionX_C->Fill(positionX);
      h_positionY_C->Fill(positionY);
      h_positionZ_C->Fill(positionZ);
    }

  }

    return pandora::STATUS_CODE_SUCCESS;
}
