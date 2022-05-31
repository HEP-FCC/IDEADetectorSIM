#include "DR_GeometryHelper.h"

#include <iostream>

DR_GeometryHelper::DR_GeometryHelper():
  m_NbOfBarrel(40),
  m_NbOfEndCap(35),
  m_NZrot(36)                                                         
{
  m_totTower = m_NbOfBarrel + m_NbOfEndCap;
  m_deltaTheta = 45./(m_NbOfBarrel);                              
  m_phiUnit = 360./m_NZrot;

  m_scint_constCalib = {408.21638950554075, 408.3954472740771, 407.1870232421094, 406.63875945884087, 404.8060585388971, 403.97304819147996, 403.3691105878475, 403.49367909804056, 404.55647780600043, 405.58591491094637, 403.9575182245898, 404.4757730162475, 404.72249522199195, 405.272159576985, 404.74332809708255, 404.83205898107536, 405.23195412471205, 404.9766105533868, 404.9085068798063, 404.9314555180952, 404.67532710488985, 404.58364980855805, 405.012793566413, 405.0007315500301, 404.30902206187204, 405.6974274788762, 405.2261341502687, 405.63975175649347, 404.90683641527, 404.37034541526305, 405.67260217215875, 405.5109490861691, 404.2898135363692, 405.07073526391474, 405.58981257625425, 405.3751447994642, 405.36549518339785, 405.3332161707569, 404.88956759976287, 405.37027184803094, 404.8980725551248, 405.34774082392767, 405.2984093045488, 405.14372480308344, 405.19187487160525, 405.03757034167137, 405.16280927227615, 404.7829216539207, 405.03107640207867, 404.7292557576276, 404.8025372723253, 403.9177916263665, 404.7460239584375, 403.96821450150077, 404.1905949169899, 404.1704924951662, 403.16496315846314, 402.2360298379118, 403.3863719919289, 402.9762332238292, 403.15699339382735, 403.4020052256797, 402.3032561236677, 402.8453577277423, 401.11356268338346, 401.3504783424065, 400.94087925309395, 400.29569405733, 400.0328154316862, 399.5130445431503, 398.66148407548866, 399.83880015591535, 398.96289406538807, 398.42261837089694, 391.76612693948175};

  m_cher_constCalib = {103.08779161895677, 102.91302749597065, 102.69865952763615, 102.61869191270468, 102.54928716539662, 102.48068194031679, 102.49984890080964, 102.35556540203991, 102.47969263317724, 102.6281510005559, 102.43322742473204, 102.47810836409134, 102.55371034296142, 102.67118096060427, 102.67297232291142, 102.48284061965019, 102.5649981010228, 102.56155933915096, 102.67809243921879, 102.56067521092992, 102.60224889784466, 102.63726587197354, 102.63191774143888, 102.76496337880408, 102.6929637252195, 102.60491403169074, 102.85913301772406, 102.741217657914, 102.69546934772463, 102.67035622618218, 102.69304228926421, 102.75886941001674, 102.75976221892324, 102.731492956408, 102.7188845221274, 102.77429845330465, 102.78649420797491, 102.75140309520445, 102.70051794706535, 102.68996042906552, 102.78365100098196, 102.8153738834064, 102.71292597825087, 102.73146416207084, 102.6450394621172, 102.61404003462839, 102.66675609739092, 102.60991640602225, 102.750246685674, 102.62575682868824, 102.42720794074478, 102.51305416968992, 102.52098979376447, 102.59751750679058, 102.45780037787654, 102.53083482963227, 102.47068539942974, 102.5721049950492, 102.56599170316093, 102.46469174495641, 102.19238017547394, 102.28148980648412, 102.19817435184497, 102.1330715125064, 102.09230341456059, 102.05765775486448, 101.9644426420847, 101.96014956820567, 101.85273676485993, 101.93311307596035, 101.96637882465569, 101.68716060542853, 101.55050000833062, 101.67603040894112, 99.77195006099979};
  
}

float DR_GeometryHelper::GetCalibrationConstant(TVector3& direction, DRCalo_FiberType fiber)
{
  // First define the angle with respect to 90 degrees, then divide by m_deltaTheta
  int index = TMath::Abs(direction.Theta()*TMath::RadToDeg() - 90)/m_deltaTheta;
  float retval = 0;
  if (fiber == DRCalo_FiberType::S) retval = m_scint_constCalib[index];
  else if (fiber == DRCalo_FiberType::C) retval = m_cher_constCalib[index];
  else {
    std::cerr << "WTF??????" << std::endl;
  }
  return retval;
}
  
TVector3 DR_GeometryHelper::GetTowerVec(unsigned int index, char side)
{  
  // Create an empty TLorentzVector

  TVector3 tower;
  if (side != 'l' && side != 'r') return tower;
  if (index == 0) return tower;

  --index;

  unsigned int sliceindex = index/m_totTower;
  unsigned int towerindex = index-(sliceindex*m_totTower);
  double theta = towerindex*m_deltaTheta+m_deltaTheta/2.;
  double phi = ((double)sliceindex) * m_phiUnit;

  if (side == 'r') theta = theta + 90.;
  else if (side == 'l') theta = 90. - theta;
  else std::cout << "What the hell??????" << std::endl;
  
  tower.SetMagThetaPhi(1,TMath::DegToRad()*(theta),TMath::DegToRad()*phi);

  return tower;
}

void DR_GeometryHelper::PrintGeometry()
{
  std::cout << "\n\n\nGeometryHelper\n\n\n" << std::endl;
  std::cout << "Nb Of Barrel towers = " << m_NbOfBarrel << std::endl;      
  std::cout << "Nb Of EndCap towers = " << m_NbOfEndCap << std::endl;
  std::cout << "Nb Of Phi sliced = " << m_NZrot << std::endl;                  
  std::cout << "Total number of towers = " << m_totTower << std::endl;
  std::cout << "Tower separation in theta = " << m_deltaTheta << std::endl;
  std::cout << "Slice separation in phi = " << m_phiUnit << std::endl;
}

std::vector<double> DR_GeometryHelper::calibscin(std::vector<double> vectorscin){

  std::vector<double> s_cont;

  int loop1 = m_scint_constCalib.size();
  int loop2 = 36;

  for(int b=0; b<loop2; b++){
    for(int i=0; i<loop1; i++){
      s_cont.push_back(m_scint_constCalib[i]);
    }
  }

  double c = 0.1;
  s_cont.insert(s_cont.begin(),c);

  if(s_cont.size() != vectorscin.size()){std::cout<<"ERROR in calibration!"<<std::endl;}

  std::vector<double> Calib_vectorScin;

  for(uint i=0; i<vectorscin.size(); i++){
    Calib_vectorScin.push_back(vectorscin[i]*(1.0/s_cont[i]));
  }

  return Calib_vectorScin;
}

std::vector<double> DR_GeometryHelper::calibcher(std::vector<double> vectorcher){
  std::vector<double> c_cont;


  int loop1 = m_cher_constCalib.size();
  int loop2 = 36;

  for(int b=0; b<loop2; b++){
    for(int i=0; i<loop1; i++){
      c_cont.push_back(m_cher_constCalib[i]);
    }
  }

  double c = 0.1;
  c_cont.insert(c_cont.begin(),c);

  if(c_cont.size() != vectorcher.size()){std::cout<<"ERROR in calibration!"<<std::endl;}

  std::vector<double> Calib_vectorCher;

  for(uint i=0; i<vectorcher.size(); i++){
    Calib_vectorCher.push_back(vectorcher[i]*(1.0/c_cont[i]));
  }

  return Calib_vectorCher;
}

void DR_GeometryHelper::PrintCalibration(){
  std::cout << "Calibration constants established for " << m_scint_constCalib.size() << " towers" << std::endl;
  std::cout << "\n\nConstants for scintillation:\n\n";
  for (unsigned int i = 0; i < m_scint_constCalib.size(); ++i){
    std::cout << "Tower[" << i << "] = " << m_scint_constCalib[i] << std::endl;
  }
  std::cout << "\n\nConstants for cherenkov:\n\n";
  for (unsigned int i = 0; i < m_cher_constCalib.size(); ++i){
    std::cout << "Tower[" << i << "] = " << m_cher_constCalib[i] << std::endl;
  }
}
