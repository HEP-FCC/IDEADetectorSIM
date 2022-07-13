#ifndef PandoraSCAlg_H
#define PandoraSCAlg_H

#include "k4FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "edm4hep/ClusterCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/EventHeaderCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
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

/* PandoraSCAlg ========== <br>
 * 
 */
namespace pandora {class Pandora;}


class CollectionMaps
{
public:
    CollectionMaps();
    void clear();
    std::map<std::string, std::vector<edm4hep::MCParticle> >     collectionMap_MC;
    std::map<std::string, std::vector<edm4hep::CalorimeterHit> > collectionMap_CaloHit;
    std::map<std::string, std::vector<edm4hep::Vertex> >         collectionMap_Vertex;
    std::map<std::string, std::vector<edm4hep::Track> >          collectionMap_Track;
    std::map<std::string, std::vector<edm4hep::MCRecoCaloAssociation> > collectionMap_CaloRel;
    std::map<std::string, std::vector<edm4hep::MCRecoTrackerAssociation> > collectionMap_TrkRel;
};



class PandoraSCAlg : public GaudiAlgorithm
{
 
public:
 
  PandoraSCAlg(const std::string& name, ISvcLocator* svcLoc);
 
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
  void Reset();

  void copyHit(edm4hep::CalorimeterHit targetHit, edm4hep::CalorimeterHit poHit);

  typedef std::vector<float> FloatVector;
  typedef std::vector<std::string> StringVector;


  class Settings
  {
  public:
      /**
       *  @brief  Default constructor
       */
      Settings();

      std::string     m_pandoraSettingsXmlFile;           ///< The pandora settings xml file
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

  int maxHitSize ;


  Gaudi::Property< std::string >              m_PandoraSettingsXmlFile { this, "PandoraSettingsDefault_xml", "PandoraSettingsDefault.xml" };
  Gaudi::Property<int>                        m_NEventsToSkip                   { this, "NEventsToSkip", 0 };

  Gaudi::Property< std::vector<std::string> > m_HCalCaloHitCollections{ this, "HCalCaloHitCollections", {"HCALBarrel","HCALEndcap","HCALOther"} };

  static pandora::Pandora        *m_pPandora;

  Settings                        m_settings;                     ///< The settings for the pandora pfa new algo
  CollectionMaps                  *m_CollectionMaps;               ///< The settings for the pandora pfa new algo

  //  CaloHitCreator::Settings        m_caloHitCreatorSettings;       ///< The calo hit creator settings

  std::string                     m_detectorName;                 ///< The detector name
  unsigned int                    m_nRun;                         ///< The run number
  unsigned int                    m_nEvent;                       ///< The event number

  //######################

  edm4hep::CalorimeterHitCollection      *pCaloHitCollection;

  
  std::map< std::string, std::string > m_collections;
  Gaudi::Property<std::vector<std::string>> m_readCols{this, "collections", {}, "Places of collections to read"};
 //the map of collection name to its corresponding DataHandle
  std::map<std::string, DataObjectHandleBase*> m_dataHandles;

  DataHandle<edm4hep::CalorimeterHitCollection>  m_SC_CaloHitCollection_w {"SC_CalorimeterHitCollection",Gaudi::DataHandle::Writer, this};

};

#endif
