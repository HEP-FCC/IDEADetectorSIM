#include "FiberDigitizer.h"

#include "edm4hep/Vector3f.h"

FiberDigitizer::FiberDigitizer():
  m_debug(false),
  m_sensor(NULL),
  m_intStart(5),
  m_intGate(250),
  m_threshold(0.5)
{}

FiberDigitizer::~FiberDigitizer()
{
}

void FiberDigitizer::Digitize(const edm4hep::SimCalorimeterHitCollection& i_coll, edm4hep::CalorimeterHitCollection& o_coll)
{

  std::vector<double> fiber_times;
  static bool doDummy = false;

  if (m_sensor == NULL){
    doDummy = true;
    std::cout << "WARNING!!!! No sensor provided to FiberDigitizer. The digitization will be dummy!" << std::endl;
  }
  
  // This is indeed a loop over fibres for one event
  
  for (auto sim_hit : i_coll){
    
    if (m_debug) std::cout << "Starting the loop on the hit collections" << std::endl;
    
    fiber_times.clear();
    
    // Create the corresponding hit in the output collection
    
    auto o_coll_elem = o_coll.create();
    
    // Loop over CaloHitContributions and save the hit times
    
    for (auto itr = sim_hit.contributions_begin(); itr != sim_hit.contributions_end(); ++itr){
      fiber_times.push_back(double(itr->getTime()));
    }
    
    // Set basic properties of the CalorimeterHit
    
    o_coll_elem.setCellID(sim_hit.getCellID());
    o_coll_elem.setPosition(sim_hit.getPosition());
    o_coll_elem.setType(0);
    
    if (fiber_times.size() == 0) {
      // No time information available, or no sensor availlable. We need to run a dummy
      if (!doDummy){
	std::cout << "\n\n\n WARNING! No time information available in the input file. The digitization will be dummy\n\n\n" << std::endl;
	doDummy = true;
      }
    }
    
    if (doDummy){
      //run a dummy digitization
	
      o_coll_elem.setEnergy(sim_hit.getEnergy());
      // set the time as the average of the times
      
      float l_time = 0.;
      for (unsigned int i = 0; i < fiber_times.size(); ++i){
	l_time += fiber_times[i];
      }
      if (fiber_times.size() != 0) o_coll_elem.setTime(l_time/float(fiber_times.size()));
      else o_coll_elem.setTime(0);
      
      o_coll_elem.setEnergyError(0);
	
    } else {
	
      // run a proper digitization
	
      m_sensor->resetState();
      m_sensor->addPhotons(fiber_times);
      m_sensor->runEvent();
      
      sipm::SiPMAnalogSignal l_signal = m_sensor->signal();
      o_coll_elem.setEnergy(l_signal.integral(m_intStart,m_intGate,m_threshold));
      o_coll_elem.setTime(l_signal.toa(m_intStart,m_intGate,m_threshold));
	o_coll_elem.setEnergyError(0);
    } // end check whether proper digitization needed
  } // end loop over collection
}

bool FiberDigitizer::Calibrate(edm4hep::CalorimeterHitCollection& l_coll, DRCalo_FiberType l_type)
{
  static float l_cal;
  l_cal = 0;
  static edm4hep::Vector3f t_edm_vec;
  static TVector3 t_vec;

  for (unsigned int i = 0; i < l_coll.size(); ++ i){ //auto& s_hit : l_coll){
    t_edm_vec = l_coll.at(i).getPosition();
    t_vec.SetXYZ(t_edm_vec.x,t_edm_vec.y,t_edm_vec.z);
    if (m_debug){
      std::cout << "Hit position (theta, phi)" << t_vec.Theta() << "\t" << t_vec.Phi() << std::endl;
      std::cout << "Hit energy before calibration" << l_coll.at(i).getEnergy() << std::endl;
    }
    l_cal = m_geomHelper.GetCalibrationConstant(t_vec,l_type);
    if (l_cal != 0) {
      l_coll.at(i).setEnergy(l_coll.at(i).getEnergy()/l_cal);
    } else {
      std::cerr << "ClusterFiberCalibrate: calibration constant 0 for direction (x,y,z) " << t_vec.X() << " " << t_vec.Y() << " " << t_vec.Z() << std::endl;
      l_coll.at(i).setEnergy(0);
    }
    if (m_debug) std::cout << "Hit energy after calibration " << l_coll.at(i).getEnergy() << std::endl;

  }
  return true;
}

void FiberDigitizer::SetIntegrationParameters(unsigned int const intStart, unsigned int const intGate, float threshold)
{
  m_intStart = intStart;
  m_intGate = intGate;
  m_threshold = threshold;
}
  
  


  

