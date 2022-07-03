/**
 * 
 *  @brief  Header file for the Dumper Helper class.
 * 
 *  $Log: $
 */

#ifndef DUMPER_HELPER_H
#define DUMPER_HELPER_H 1

#include "GaudiKernel/ISvcLocator.h"
#include "edm4hep/CalorimeterHit.h"

#include "Xml/tinyxml.h" //to read xml file 

#include "gear/LayerLayout.h"

#include "Api/PandoraApi.h"
#include <string>
#include "CaloHitCreator.h"
#include <iostream>
#include <fstream>

typedef std::vector<edm4hep::CalorimeterHit *> CalorimeterHitVector;


class CollectionMaps;
/**
 *  @brief  CaloHitCreator class
 */
class DumperHelper
{
public:
    typedef std::vector<std::string> StringVector;
    /**
     *  @brief  Constructor
     * 
     *  @param  settings the creator settings
     *  @param  pPandora address of the relevant pandora instance
     */
    //    CaloHitCreator_SC(const pandora::Pandora *const pPandora);
    DumperHelper(const CaloHitCreator::Settings&, const pandora::Pandora *const pPandora);
    /**
     *  @brief  Destructor
     */
     ~DumperHelper();

    /**
     *  @brief  writes calohits to file
     * 
     */
     pandora::StatusCode WriteToFile(edm4hep::CalorimeterHitCollection      *pCaloHitCollection, std::ofstream &txtFileName, TFile  *out_dumper_histo) const;

     /**
      *  @brief  Read a value from an xml element
      *
      *  @param  xmlHandle the relevant xml handle
      *  @param  xmlElementName the name of the xml element to examine
      *  @param  t to receive the value
      */
     template <typename T>
       static StatusCode ReadValue(const std::string &xmlFileName, const std::string &xmlElementName, T &t);

     pandora::StatusCode my_ReadSettings(const std::string &xmlFileName);


private:

     std::string m_outFileDumperTxT; //provaAdele
     
     const CaloHitCreator::Settings      m_settings;                         ///< The calo hit creator settings
     
     const pandora::Pandora             *m_pPandora;                         ///< Address of the pandora object to create calo hits

};


#endif // #ifndef CALO_HIT_CREATOR_SC_H
