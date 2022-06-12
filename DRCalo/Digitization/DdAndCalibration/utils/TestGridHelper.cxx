#include "DR_CaloGridHelper.h"

#include "edm4hep/CalorimeterHit.h"

#include <iostream>

int main()
{
  DR_CaloGridHelper mygrid(2.5);
  mygrid.CreateGrid();
  std::cout << "m_delta = " << mygrid.GetDelta() << std::endl;
  std::cout << "GetId (1.6,0) " << mygrid.GetID(1.6,0) << std::endl;
  std::cout << "GetPhi(1) " << mygrid.GetPhi(1) << std::endl;
  std::cout << "GetPhi(5000) " << mygrid.GetPhi(5000) << std::endl;
  std::cout << "GetCosTheta(5000) " << mygrid.GetCosTheta(5000) << std::endl;
  std::cout << "GetCosTheta(5000000) " << mygrid.GetCosTheta(5000000) << std::endl;
  std::cout << "GetTheta(5000000) " << mygrid.GetTheta(5000000) << std::endl;
  std::cout << "GetID(3.00836,0)" << mygrid.GetID(3.00836,0) << std::endl;
  //mygrid.Add(1.6,0,10000);
  std::cout << "GetEnergy(0.1,0.1) " << mygrid.GetEnergy(0.1,0.1) << std::endl;
  std::cout << "GetEnergy(1.6,0)" << mygrid.GetEnergy(1.6,0) << std::endl;
  //mygrid.Add(1.6,0,10000);
  std::cout << "GetEnergy(1.6,0)" << mygrid.GetEnergy(1.6,0) << std::endl;
  mygrid.Print();
  std::cout << "cazzo crasho" << std::endl;
  mygrid.Reset();
  std::cout << "cazzo crasho 1" << std::endl;
  mygrid.Print();
  std::cout << "cazzo crasho 2" << std::endl;
  //Now testing adding a few fibers in random positions
  edm4hep::CalorimeterHit mycalo;
  mycalo.setPosition({1000.,1000.,1000.});
  mycalo.setEnergy(5000);
  std::cout << "cazzo crasho 2.5" << std::endl;
  edm4hep::ConstCalorimeterHit const_mycalo = mycalo;
  mygrid.Add(&const_mycalo);
  std::cout << "cazzo crasho 3" << std::endl;
  edm4hep::CalorimeterHit mycalo1;
  mycalo1.setPosition({-1000.,1000.,300.});
  edm4hep::ConstCalorimeterHit const_mycalo1 = mycalo1;
  mygrid.Add(&const_mycalo1);
  std::cout << "cazzo crasho 4" << std::endl;
  edm4hep::CalorimeterHit mycalo2;
  mycalo2.setPosition({1000.,1000.,1000.});
  edm4hep::ConstCalorimeterHit const_mycalo2 = mycalo2;
  mygrid.Add(&const_mycalo2);
  std::cout << "cazzo crasho 5" << std::endl;
  edm4hep::CalorimeterHit mycalo3;
  mycalo3.setPosition({200.,100.,-200.});
  edm4hep::ConstCalorimeterHit const_mycalo3 = mycalo3;
  mygrid.Add(&const_mycalo3);
  mygrid.Print();
  return 0;
  
}
  
  
