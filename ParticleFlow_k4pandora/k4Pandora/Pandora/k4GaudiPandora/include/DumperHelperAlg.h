#ifndef DumperHelperAlg_H
#define DumperHelperAlg_H

#include "k4FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "edm4hep/ClusterCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/EventHeaderCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/VertexCollection.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/MCParticle.h" 
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/MCRecoCaloAssociation.h"
#include "edm4hep/MCRecoTrackerAssociation.h"
#include "edm4hep/MCRecoTrackerAssociationCollection.h"
#include "edm4hep/MCRecoCaloAssociationCollection.h"
#include "edm4hep/MCRecoParticleAssociation.h"
#include "edm4hep/MCRecoParticleAssociationCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

#include "Api/PandoraApi.h"

#ifdef MONITORING
#include "TApplication.h"
#endif

#include <iostream>
#include <random>
#include <string>
#include <unistd.h>


#include "CaloHitCreator.h"
#include "GeometryCreator.h"
#include "MCParticleCreator.h"
#include "PfoCreator.h"
#include "TrackCreator.h"
#include <fstream>

#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"



/* DumperHelperAlg ========== <br>
 * 
 */
namespace pandora {class Pandora;}



class DumperHelperAlg : public GaudiAlgorithm
{
 
public:
 
  DumperHelperAlg(const std::string& name, ISvcLocator* svcLoc);
 
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual StatusCode initialize() ;
 
  /** Called for every event - the working horse.
   */
  virtual StatusCode execute() ; 
 
  /** Called after data processing for clean up.
   */
  virtual StatusCode finalize() ;
 
  void FinaliseSteeringParameters(ISvcLocator* svcloc);
  pandora::StatusCode RegisterUserComponents() const;

  void writeHistoSC();
  void deleteHistoSC();

  typedef std::vector<float> FloatVector;
  typedef std::vector<std::string> StringVector;


  class Settings
  {
  public:
      /**
       *  @brief  Default constructor
       */
      Settings();

  };


    /**
     *  @brief  Get address of the pandora instance
     * 
     *  @return address of the pandora instance
     */
    const pandora::Pandora *GetPandora() const;
    StatusCode updateMap();

 protected:
 
    
    typedef std::vector<float> FloatVec;
    
    int _nEvt ;
    
    Gaudi::Property<int>                        m_NEventsToSkip                   { this, "NEventsToSkip", 0 };
    
    //    Gaudi::Property< std::vector<std::string> > m_HCalCaloHitCollections{ this, "HCalCaloHitCollections", {"HCALBarrel","HCALEndcap","HCALOther"} };
    
    static pandora::Pandora        *m_pPandora;
    
    Settings                        m_settings;                     ///< The settings for the pandora pfa new algo
    
    std::string                     m_detectorName;                 ///< The detector name
    unsigned int                    m_nRun;                         ///< The run number
    unsigned int                    m_nEvent;                       ///< The event number
    
    //######################
    std::string              m_outHistFile;                 ///< The hist file name
    std::string              m_outTxtFile ;                 ///< The txt file name
    std::ofstream            myfile       ;
    
    TFile *out_dumper_histo;
    
    TH1F *h_type;
    //declare histos S/0 type.....to be improved
    TH1F *  h_energy_S   ;
    TH1F *  h_positionX_S;
    TH1F *  h_positionY_S;
    TH1F *  h_positionZ_S;
    
    TH1F *  h_positionTheta_S;
    TH1F *  h_positionPhi_S;
    //declare histos C/1 type.....to be improved
    TH1F *  h_energy_C   ;
    TH1F *  h_positionX_C;
    TH1F *  h_positionY_C;
    TH1F *  h_positionZ_C;
    
    TH2F *h_2D_energy_S;
    TH2F *h_2D_energy_C;
    TH2F *h_2D_energy_SC;

    TH2F *h_2Dp_energy_S;
    TH2F *h_2Dp_energy_C;
    TH2F *h_2Dp_energy_SC;

    TH3F *h_3D_energy_SC;
    
    
    std::map< std::string, std::string > m_collections;
    Gaudi::Property<std::vector<std::string>> m_readCols{this, "collections", {}, "Places of collections to read"};
    //the map of collection name to its corresponding DataHandle
    std::map<std::string, DataObjectHandleBase*> m_dataHandles;
    

    int FillHistos( const edm4hep::CalorimeterHitCollection      *pCaloHitCollection);
    int WriteToFile( const edm4hep::CalorimeterHitCollection      *pCaloHitCollection);
    
};

#endif
