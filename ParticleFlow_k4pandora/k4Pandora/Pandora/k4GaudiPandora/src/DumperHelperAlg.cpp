#include "GearSvc/IGearSvc.h"
#include "DumperHelperAlg.h"
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
#include "TLorentzVector.h"

#include <fstream>
#include <iostream>


pandora::Pandora* DumperHelperAlg::m_pPandora=0;

DECLARE_COMPONENT( DumperHelperAlg )

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



DumperHelperAlg::DumperHelperAlg(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc),
    _nEvt(0)
{
    declareProperty("WriteHistFile"              , m_outHistFile,               "Handle of root hist file output name" );
    declareProperty("WriteTxtFile"              , m_outTxtFile ,               "Handle of Txt file output name" );
}


void DumperHelperAlg::FinaliseSteeringParameters(ISvcLocator* svcloc)
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



StatusCode DumperHelperAlg::initialize()
{

  std::cout<<"init DumperHelperAlg"<<std::endl;
  for ( const auto& col : m_readCols ) {
      auto seperater = col.find(':');
      std::string colType = col.substr(0, seperater);
      std::string colName = col.substr(seperater+1);
      std::cout << "colType " << colType << std::endl;
      m_collections[colName] = colType;
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

  try
  {
      ISvcLocator* svcloc = serviceLocator();
      m_pPandora = new pandora::Pandora();
      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RegisterUserComponents());
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

  //Open the output files (.root and .txt)
  myfile.open(m_outTxtFile.c_str());//open the txt file for the dumper

  if(! m_outHistFile.empty()){

    out_dumper_histo = new TFile(m_outHistFile.c_str(),"RECREATE","example");//open the root file for the dumper
    //declare Histos SC;
    h_type        = new TH1F ("h_type", "type", 10, 0.,10.);
    
    h_energy_S    = new TH1F ("h_energy_S", "energy_S", 50, 0.,50.);              
    h_positionX_S = new TH1F ("h_positionX_S", "positionX_S", 250, 2000.,3000.);
    h_positionY_S = new TH1F ("h_positionY_S", "positionY_S", 250, -3000.,3000.); 
    h_positionZ_S = new TH1F ("h_positionZ_S", "positionZ_S", 50, -500.,500.) ;

    //polar coordinates
    h_positionTheta_S = new TH1F ("h_positionTheta_S", "positionTheta_S", 250, -3,3.);
    h_positionPhi_S = new TH1F ("h_positionPhi_S", "positionPhi_S", 250, 0.,6.); 
    		   
    		   
    h_energy_C    = new TH1F ("h_energy_C", "energy_C", 50, 0.,50.)              ;
    h_positionX_C = new TH1F ("h_positionX_C", "positionX_C", 250, 2000.,3000.);
    h_positionY_C = new TH1F ("h_positionY_C", "positionY_C", 250, -3000.,3000.);
    h_positionZ_C = new TH1F ("h_positionZ_C", "positionZ_C", 50, -500.,500.) ;

    //cartesian coordinates
    h_2D_energy_S  = new TH2F("h_2D_energyS","h_2D_energyS",105,0,210,150,2300,2600);//                150,2300,2600,105,0,210);
    h_2D_energy_C  = new TH2F("h_2D_energy_C","h_2D_energy_C",105,0,210,150,2300,2600);
    h_2D_energy_SC = new TH2F("h_2D_energy_SC","h_2D_energy_SC",105,0,210,150,2300,2600);

    //polar coordinates
    h_2Dp_energy_S  = new TH2F("h_2Dp_energyS","h_2Dp_energyS",105,1.4,1.7,150,0,0.2);//                150,2300,2600,105,0,210);
    h_2Dp_energy_C  = new TH2F("h_2Dp_energy_C","h_2Dp_energy_C",105,1.4,1.7,150,0,0.2);
    h_2Dp_energy_SC = new TH2F("h_2Dp_energy_SC","h_2Dp_energy_SC",105,1.4,1.7,150,0,0.2);


    h_3D_energy_SC = new TH3F("h_3D_energy_SC","h_3D_energy_SC",100,-50,200,150,2300,2600,105,0,210);
    //    h_3D_energyTime_SC = new TH3F("h_3D_energyTime_SC","h_3D_energyTime_SC",100,-50,200,150,2300,2600,105,0,210);
  
  }

  else {
    out_dumper_histo = 0;
    h_type           = 0;
    h_energy_S       = 0;
    h_positionX_S    = 0;
    h_positionY_S    = 0;
    h_positionZ_S    = 0;

    h_energy_C       = 0;
    h_positionX_C    = 0;
    h_positionY_C    = 0;
    h_positionZ_C    = 0;
    h_2D_energy_S    = 0;
    h_2D_energy_C    = 0;
    h_2D_energy_SC   = 0;
    
  }

  return GaudiAlgorithm::initialize();
}

StatusCode DumperHelperAlg::execute()
{

  info() << "************************************ event " << _nEvt << " ****************************************** " << endmsg;
  try
    {
      
      for(auto &v : m_dataHandles){
	
	if(m_collections[v.first] != "CalorimeterHit"){
	  error() << "FATAL coll type!" << endmsg;
	  return StatusCode::FAILURE;
	}
	
	auto handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*> (v.second);
	auto po = handle->get();
	if(po != NULL){
	  
	  std::cout<<"*************************************************************DUMPER po->size() = ************************************="<<po->size()<<std::endl;	  
	  
	}

	int status = WriteToFile(po);
	status = FillHistos(po);      

      }

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
  
  info() << "DumperHelperAlg Processed " << _nEvt << " events " << endmsg;
  _nEvt ++ ;

  return StatusCode::SUCCESS;
}






StatusCode DumperHelperAlg::finalize()
{
  info() << "Finalized. Processed " << _nEvt << " events " << endmsg;

  out_dumper_histo->cd();
  writeHistoSC();
  out_dumper_histo->Close();
  //deleteHistoSC();

  delete m_pPandora;
  return GaudiAlgorithm::finalize();
}


pandora::StatusCode DumperHelperAlg::RegisterUserComponents() const
{
    
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, LCContent::RegisterAlgorithms(*m_pPandora));
    
    return pandora::STATUS_CODE_SUCCESS;
}


const pandora::Pandora *DumperHelperAlg::GetPandora() const
{
    if (NULL == m_pPandora)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora;
}
DumperHelperAlg::Settings::Settings()
{
}

void DumperHelperAlg::writeHistoSC()
{
  h_type->Write();

  h_energy_S->Write();   
  h_positionX_S->Write();   
  h_positionY_S->Write();   
  h_positionZ_S->Write();   
  h_positionTheta_S->Write();   
  h_positionPhi_S->Write();   

  h_energy_C->Write();   
  h_positionX_C->Write();   
  h_positionY_C->Write();   
  h_positionZ_C->Write();   

  h_2D_energy_SC->Write();
  h_2D_energy_S->Write();
  h_2D_energy_C->Write();


  h_2Dp_energy_SC->GetXaxis()->SetTitle("#theta (rad)");
  h_2Dp_energy_SC->GetYaxis()->SetTitle("#phi (rad)");
  h_2Dp_energy_S->GetXaxis()->SetTitle("#theta (rad)");
  h_2Dp_energy_S->GetYaxis()->SetTitle("#phi (rad)");
  h_2Dp_energy_C->GetXaxis()->SetTitle("#theta (rad)");
  h_2Dp_energy_C->GetYaxis()->SetTitle("#phi (rad)");

  h_2Dp_energy_SC->Write();
  h_2Dp_energy_S->Write();
  h_2Dp_energy_C->Write();

  h_3D_energy_SC->GetXaxis()->SetTitle("positionZ");
  h_3D_energy_SC->GetYaxis()->SetTitle("positionX");
  h_3D_energy_SC->GetZaxis()->SetTitle("positionY");
  h_3D_energy_SC->Write();

}


void DumperHelperAlg::deleteHistoSC()
{
  delete h_type;
  delete h_energy_S;
  delete h_positionX_S;
  delete h_positionY_S;
  delete h_positionZ_S;
  delete h_positionTheta_S;
  delete h_positionPhi_S;
  delete h_energy_C;
  delete h_positionX_C;
  delete h_positionY_C;
  delete h_positionZ_C;

}



//------------------------------------------------------------------------------------------------------------------------------------------
int DumperHelperAlg::WriteToFile( const edm4hep::CalorimeterHitCollection      *m_pCaloHitCollection) 
{

  Int_t maxHitSize = 1324;  
  Int_t     type        = -999;
  Float_t   energy      = -999.;
  Float_t   energyError = -999.;
  Float_t   time        = -999.;
  Float_t   positionX   = -999.;
  Float_t   positionY   = -999.;
  Float_t   positionZ   = -999.;
  ULong64_t cellID      = -999;

  if (myfile.is_open())
    {

      for(int i = 0; i < m_pCaloHitCollection->size(); ++i){
	//set variables
	type        = m_pCaloHitCollection->at(i).getType()       ;
	energy      = m_pCaloHitCollection->at(i).getEnergy()     ;
	energyError = m_pCaloHitCollection->at(i).getEnergyError();
	time        = m_pCaloHitCollection->at(i).getTime()       ;
	positionX   = m_pCaloHitCollection->at(i).getPosition()[0];
	positionY   = m_pCaloHitCollection->at(i).getPosition()[1];
	positionZ   = m_pCaloHitCollection->at(i).getPosition()[2];
	cellID      = m_pCaloHitCollection->at(i).getCellID()     ;
	
	//write to txt file
	//    myfile <<_nEvt<<" , ";
	myfile <<type<<" , ";
	myfile <<energy<<" , ";
	//    myfile <<energyError<<" , ";
	myfile <<time<<" , ";
	myfile <<positionX<<" , ";
	myfile <<positionY<<" , ";
	myfile <<positionZ<<" , ";
	//    myfile <<cellID<<" \n";
	
      }


      for(int i = m_pCaloHitCollection->size(); i < maxHitSize; ++i){
	//set variables
	type        = 0;
	energy      = 0;
	energyError = 0;
	time        = 0;
	positionX   = 0;
	positionY   = 0;
	positionZ   = 0;
	cellID      = 0;
	
	//write to txt file
	//    myfile <<_nEvt<<" , ";
	myfile <<type<<" , ";
	myfile <<energy<<" , ";
	//    myfile <<energyError<<" , ";
	myfile <<time<<" , ";
	myfile <<positionX<<" , ";
	myfile <<positionY<<" , ";
	myfile <<positionZ<<" , ";
	//    myfile <<cellID<<" \n";
	
      }


      myfile <<" \n";
    }

    return 0;
}



//------------------------------------------------------------------------------------------------------------------------------------------
int DumperHelperAlg::FillHistos( const edm4hep::CalorimeterHitCollection      *m_pCaloHitCollection)
{
  
  Int_t     type        = -999;
  Float_t   energy      = -999.;
  Float_t   energyError = -999.;
  Float_t   time        = -999.;
  Float_t   positionX   = -999.;
  Float_t   positionY   = -999.;
  Float_t   positionZ   = -999.;
  ULong64_t cellID      = -999;

  for(int i = 0; i < m_pCaloHitCollection->size(); ++i){
    //set variables
    type        = m_pCaloHitCollection->at(i).getType()       ;
    energy      = m_pCaloHitCollection->at(i).getEnergy()     ;
    energyError = m_pCaloHitCollection->at(i).getEnergyError();
    time        = m_pCaloHitCollection->at(i).getTime()       ;
    positionX   = m_pCaloHitCollection->at(i).getPosition()[0];
    positionY   = m_pCaloHitCollection->at(i).getPosition()[1];
    positionZ   = m_pCaloHitCollection->at(i).getPosition()[2];
    cellID      = m_pCaloHitCollection->at(i).getCellID()     ;


    TVector3 vec;
    vec.SetX(positionX); vec.SetY(positionY); vec.SetZ(positionZ);
    /*
    if(_nEvt == 0){
      std::cout<<"Coordinate cartesiane: X = "<<positionX<<"      Y = "<<positionY<<"        Z =  "<<positionZ<<std::endl;
      std::cout<<"Coordinate polari: vec.Theta() = "<<vec.Theta()<<"      vec.Phi() = "<<vec.Phi()<<std::endl;
    }
    */

    if(out_dumper_histo){

      if(_nEvt == 1){//fill 2D histos

	h_2D_energy_SC->Fill(positionY,positionX,energy);
	h_2Dp_energy_SC->Fill(vec.Theta(),vec.Phi(),energy);
	h_3D_energy_SC->Fill(positionZ,positionX,positionY,energy);

	if(type == 0){
	  h_2D_energy_S->Fill(positionY,positionX,energy);
	  h_2Dp_energy_S->Fill(vec.Theta(),vec.Phi(),energy);
	}

	else if(type == 1){
	  h_2D_energy_C->Fill(positionY,positionX,energy);
	  h_2Dp_energy_C->Fill(vec.Theta(),vec.Phi(),energy);
	}
      }



      //fill histos
      h_type->Fill(type);
      
      if(type == 0){
	h_energy_S->Fill(energy);
	h_positionX_S->Fill(positionX);
	h_positionY_S->Fill(positionY);
	h_positionZ_S->Fill(positionZ);
	h_positionTheta_S->Fill(vec.Theta());
	h_positionPhi_S->Fill(vec.Phi());
      }
      
      if(type == 1){
	h_energy_C->Fill(energy);
	h_positionX_C->Fill(positionX);
	h_positionY_C->Fill(positionY);
	h_positionZ_C->Fill(positionZ);
      }
      
    }

  }

    return 0;
}












