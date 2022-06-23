
#ifndef DRCALOIO_HH
#define DRCALOIO_HH 1

#include <vector>
#include <string>

#include "globals.hh"
#include "G4ThreeVector.hh"

//podio includes 

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/CaloHitContributionCollection.h"

namespace drc {

class DRCaloIO {
public: 

  virtual ~DRCaloIO();
  
  static DRCaloIO* GetInstance(std::string outFold=".");

  void newEvent(G4int evId, bool writeASCII=false);
  void writePodio(G4int evId);
//  void fillAnlysis();

  void Addneutrinoleakage(G4double de); //add energy of neutrinos in the ball containing the calorimeter
  void Addleakage(G4double de); //add energy of all particles that are not neutrinos (or anti_neutrinos) in the ball containing the calorimeter
  void Addem(G4double de);  //Add em component
  void AddScin(G4double de);//Add energy in scintillating fibers
  void AddCher(G4double de);//Add energy in Cherenkov fibers
  void AddCherenkov();//Add cherenkov photoelectron
  //void AddScintillation();
  void Addenergy(G4double de);//Add all energy deposited
  //void AddEnergyfibre(G4double de, G4int number);//Add energy in copy number fiber
  //void AddSignalfibre(G4int number);
  void SavePrimaryParticle(G4String name);
  void SavePrimaryEnergy(G4double primaryparticleenergy);

  //to save variable in ntuple
  G4double& GetEnergyem() { return Energyem; }
  G4double& GetEnergyScin() { return EnergyScin; }
  G4double& GetEnergyCher() { return EnergyCher; }
  G4int   & GetNofCherenkovDetected() { return NofCherenkovDetected; }
  G4double& GetEnergyTot() { return  EnergyTot; }
  G4double& GetPrimaryParticleEnergy() { return  PrimaryParticleEnergy; }
  G4String& GetPrimaryParticleName() { return  PrimaryParticleName; }
  G4double& Getneutrinoleakage() { return  neutrinoleakage; }
  G4double& Getleakage() { return  leakage; }

  //to save vectors in ntuple
  std::vector<double>& GetVectorSignalsR() {return VectorSignalsR;}
  std::vector<double>& GetVectorSignalsL() {return VectorSignalsL;}
  std::vector<double>& GetVectorSignalsCherR() {return VectorSignalsCherR;}
  std::vector<double>& GetVectorSignalsCherL() {return VectorSignalsCherL;}
  std::vector<double>& GetVectorR() {return VectorR;}
  std::vector<double>& GetVectorL() {return VectorL;}
  std::vector<double>& GetVectorR_loop() {return VectorR_loop;}
  std::vector<double>& GetVectorL_loop() {return VectorL_loop;}

  //to fill vectors
  void AddVectorScinEnergyR(G4double de, G4int tower, G4int slice); //fill vector of scintillating fibers with energy deposition
  void AddVectorScinEnergyL(G4double de, G4int tower, G4int slice); //fill vector left side
  void AddVectorCherPER(G4int c_signal, G4int tower, G4int slice);//fill vector of cherenkov fibers with chernekov photoelectrons
  void AddVectorCherPEL(G4int c_signal, G4int tower, G4int slice);
  void AddVectorR(G4double de, G4int tower, G4int slice);
  void AddVectorL(G4double de, G4int tower, G4int slice);
  void AddVectorR_loop(G4double de, G4int tower, G4int slice);
  void AddVectorL_loop(G4double de, G4int tower, G4int slice);

  //define fiber struct: contain info of fiber integrated signal and fiber location, include methods to fill it hit-by-hit
  struct Fiber{
    int ID, Type, Slice, Tower;           //fiber ID, type (S or C), slice, tower
    int E;                                //number of p.e. in fiber
    G4ThreeVector Pos;                    //inner tip position (X,Y,Z) (mm)
    std::vector<double> phtimes;          //vector of float, each float is a single photon time of arrival

    void addfiber(Fiber f1){             //function to add single hit to fiber integrated signal (single hit passed by reference)
	E = E+f1.E;                        //add hit photons
	phtimes.insert(phtimes.end(), f1.phtimes.begin(), f1.phtimes.end()); //append photons time of arriva
    };

    void orderphtimes(){ //function to order photon times from first to last
	sort(phtimes.begin(), phtimes.end());
    };
  };

  void appendfiber(int ID, int Type, int Slice, int Tower, int E, G4ThreeVector Pos, std::vector<double> phtimes);

  typedef struct FiberInfo {
      G4double F_ID, F_E, F_X, F_Y, F_Z; //fiber saturated energy
      G4int F_Type, F_slice, F_tower; //C==0 S ==1;
  } Fiber_Info;

  void WriteFiber_Info(G4double FID, G4double FE, G4int FType, G4ThreeVector Fpos, G4int slice, G4int tower);

  typedef struct TrackingInfo {
      G4double T_ID, T_X, T_Y, T_Z, T_Ek;
      G4String T_Name;
  } Tracking_Info;

  void WriteTracking_Info(G4double T_ID, G4ThreeVector Tpos, G4String Name, G4double Ek);

  void CreateEdm4HepCollections();

protected:
  DRCaloIO(std::string outFold=".");

private:

  std::string _outFold;

//  G4int cont;
  std::vector<Fiber> Fibers;           //vector of Fibers
  std::vector<int> FiberIDs;           //vector of Fibers IDs
  Fiber_Info Fiber_Hits[1000000];
  Tracking_Info Tracking_Hits[200];
  G4double  Energyem; //Energy of em component
  G4double  EnergyScin; //Energy in scintillating fibers
  G4double  EnergyCher; //Energy in Cherenkov fibers
  G4int     NofCherenkovDetected; //Number of Cherenkov photons detected (in cherenkov fibers)
  //G4int     NofScintillationDetected;//Number of Scintillating photons detected (in scintillating fibers)
  G4double  EnergyTot;//Total energy deposited (does not count invisibile energy)
  //G4double  Signalfibre[64];//Signal in 64 single module fibers, to be used with AddEnergyfibre
  G4String PrimaryParticleName; //Name of primary particle
  G4double PrimaryParticleEnergy;//Primary particle energy
  G4double neutrinoleakage; //leakage neutrino
  G4double leakage; //leakage non neutrino

  std::vector<double> VectorR_loop;
  std::vector<double> VectorL_loop;

  std::vector<double> VectorSignalsR;//Vector filled with scintillating fibers energy deposits
  std::vector<double> VectorSignalsL;//vector filled for left side
  std::vector<double> VectorSignalsCherR;//Vector filled with Cherenkov fibers Cherenkov photoelectrons
  std::vector<double> VectorSignalsCherL;//vector filled for left side
  
  std::vector<double> VectorR; //vector with energy deposited in towers right
  std::vector<double> VectorL;

  int fNbOfBarrel;
  int fNbOfEndcap;
  int fNbOfZRot;

  //define edm4hep calo hits + auxiliary info
  edm4hep::SimCalorimeterHitCollection * s_caloHits;
  edm4hep::SimCalorimeterHitCollection * c_caloHits;
  edm4hep::SimCalorimeterHitCollection * aux_infoHits;
  
  // detailed information for timing of the signal
  
  edm4hep::CaloHitContributionCollection * s_caloHitContributions;
  edm4hep::CaloHitContributionCollection * c_caloHitContributions;
 
};

} //namespace drc
#endif // DRCALOIO_HH
