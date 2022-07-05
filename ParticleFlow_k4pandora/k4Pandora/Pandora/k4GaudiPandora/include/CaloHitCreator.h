/**
 * 
 *  @brief  Header file for the calo hit creator class.
 * 
 *  $Log: $
 */

#ifndef CALO_HIT_CREATOR_H
#define CALO_HIT_CREATOR_H 1

#include "GaudiKernel/ISvcLocator.h"
#include "edm4hep/CalorimeterHit.h"
#include "Pandora/Algorithm.h"
#include "Api/PandoraApi.h"
#include "Api/PandoraContentApi.h"

#include <string>

typedef std::vector<edm4hep::CalorimeterHit *> CalorimeterHitVector;

namespace gear { class GearMgr; }

class CollectionMaps;
/**
 *  @brief  CaloHitCreator class
 */
class CaloHitCreator
{
public:
    typedef std::vector<std::string> StringVector;

    /**
     *  @brief  Settings class
     */
    class Settings
    {
    public:
        /**
         *  @brief  Default constructor
         */
        Settings();
        StringVector    m_hCalCaloHitCollections;               ///< The hcal calorimeter hit collections
        float           m_eCalScToHadGeVEndCap;                 ///< The calibration from deposited Sc-layer energy on the endcaps to hadronic energy
    };

    /**
     *  @brief  Constructor
     * 
     *  @param  settings the creator settings
     *  @param  pPandora address of the relevant pandora instance
     */
     CaloHitCreator(const Settings &settings, const pandora::Pandora *const pPandora, ISvcLocator* svcloc, bool encoder_style);

    /**
     *  @brief  Destructor
     */
     ~CaloHitCreator();

    /**
     *  @brief  Create calo hits
     * 
     */    
    pandora::StatusCode CreateCaloHits(const CollectionMaps& collectionMaps);

    /**
     *  @brief  Get the calorimeter hit vector
     * 
     *  @return The calorimeter hit vector
     */
    const CalorimeterHitVector &GetCalorimeterHitVector() const;
    /**
     *  @brief  Reset the calo hit creator
     */
    void Reset();



private:
    /**
     *  @brief  Create hcal calo hits
     * 
     */
    pandora::StatusCode CreateHCalCaloHits(const CollectionMaps& collectionMaps);

    const Settings                      m_settings;                         ///< The calo hit creator settings

    const pandora::Pandora             *m_pPandora;                         ///< Address of the pandora object to create calo hits

    CalorimeterHitVector                m_calorimeterHitVector;             ///< The calorimeter hit vector
    std::string                         m_outputClusterListName;


};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CalorimeterHitVector &CaloHitCreator::GetCalorimeterHitVector() const
{
    return m_calorimeterHitVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void CaloHitCreator::Reset()
{
    m_calorimeterHitVector.clear();
}



#endif // #ifndef CALO_HIT_CREATOR_H
