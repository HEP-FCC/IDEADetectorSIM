#ifndef DR_FIBERDIGITIZER
#define DR_FIBERDIGITIZER

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"

#include "DRCalo_FiberType.h"
#include "DR_GeometryHelper.h"

#include "SiPMAnalogSignal.h"
#include "SiPMProperties.h"
#include "SiPMSensor.h"

class FiberDigitizer
{
  // This class is made to create a CalorimeterHitCollection starting from a SimCalorimeterHitCollection.
  // At some point it will be replaced by a real digitization.
  
public: 
  FiberDigitizer();
  ~FiberDigitizer();

  /* Main hooks to calibration and digitization. The digitization is performed by simulated sensors using the sipm library provided by University of Insubria. The calibration (at the moment dummy) is provided by reading calibration constants from teh DR_GeometryHelper */
  
  void Digitize(const edm4hep::SimCalorimeterHitCollection& i_coll, edm4hep::CalorimeterHitCollection& o_coll);

  bool Calibrate(edm4hep::CalorimeterHitCollection& l_coll, DRCalo_FiberType l_type);

  /* Additional printouts if a debug flag is set */
  
  void SetDebug(bool debug = true){m_debug = debug;}

  /* Integration parameters for the sensor's signal */

  void SetIntegrationParameters(unsigned int  intStart, unsigned int intGate, float threshold);
  
  /* The sensor is set externally, so that it can be configured before passing it to the FiberDigitizer (with the corresponding getter */
  
  void SetSiPMSensor(sipm::SiPMSensor * l_sensor) {m_sensor = l_sensor;}
  sipm::SiPMSensor * GetSensor() {return m_sensor;}
  
private:
  bool m_debug;
  DR_GeometryHelper m_geomHelper;
  sipm::SiPMSensor * m_sensor;
  unsigned int m_intStart;
  unsigned int m_intGate;
  float m_threshold;
  
 };

#endif
  
  


  
