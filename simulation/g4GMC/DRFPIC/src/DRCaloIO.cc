#include "GeomService.hh"
#include "GeomHandle.hh"

#include "DRFPIcalorimeter.hh"
#include "DRCaloIO.hh"
//#include "g4root.hh"

#include <fstream>
#include <iostream>
#include <cstddef>

#include <edm4hep/Vector3d.h>
#include <GMCG4PodioManager.hh>

namespace drc {

static DRCaloIO* instance = 0;

  DRCaloIO::DRCaloIO(std::string outFold)
    :  _outFold(outFold),
       //	 cont(0),
       Fibers(0.),                 //vector of Fibers
       FiberIDs(0),                //vector of Fibers IDs
       Fiber_Hits{0.},
    Tracking_Hits{0.},
    Energyem(0.),
    EnergyScin(0.),
    EnergyCher(0.),
    NofCherenkovDetected(0),
    //NofScintillationDetected(0),
    EnergyTot(0.),
    PrimaryParticleName(""),
    PrimaryParticleEnergy(0.),
    neutrinoleakage(0.),
    leakage(0.),
    VectorR_loop(0.),
    VectorL_loop(0.),
    VectorSignalsR(0.),
    VectorSignalsL(0.),
    VectorSignalsCherR(0.),
    VectorSignalsCherL(0.),
    VectorR(0.),
    VectorL(0.),
    fNbOfBarrel(0),
    fNbOfEndcap(0),
    fNbOfZRot(0),
    s_caloHits(NULL),
    c_caloHits(NULL),
    aux_infoHits(NULL),
    s_caloHitContributions(NULL),
    c_caloHitContributions(NULL),
    m_mcParticles(NULL)
    {
    }
  
  DRCaloIO::~DRCaloIO(){
    if (instance !=0 ) {
      delete instance;
      instance=0;
    }
  }
  
  DRCaloIO* DRCaloIO::GetInstance(std::string outFold){
    
    if (instance == 0 ){
      
      instance = new DRCaloIO(outFold);
    }
    
    return instance;
    
  }
  
  void DRCaloIO::newEvent(G4int evId, bool writeASCII) {
    
    if (writeASCII) {
      //Time_distribution event
      std::ofstream TimeFile;
      std::string fname=_outFold+"/"+"Time.txt";
      TimeFile.open(fname.c_str(), std::ios_base::app);
      TimeFile<<"Event "<<evId<<" % % %"<<G4endl;
      TimeFile.close();
    }
    
    // initialisation per event
    Energyem = 0.;
    EnergyScin = 0.;
    EnergyCher = 0.;
    NofCherenkovDetected = 0;
    //NofScintillationDetected = 0;
    EnergyTot = 0.;
    neutrinoleakage = 0.;
    leakage = 0.;
    
    if (fNbOfBarrel==0) {
      GeomHandle<DRFPIcalorimeter> drfpic;
      
      fNbOfBarrel = drfpic->NbOfBarrel();//40;
      fNbOfEndcap = drfpic->NbOfEndcap();//35;
      fNbOfZRot = drfpic->NbOfZRot();//36;
    }
    
    Fibers.clear();
    
    FiberIDs.clear();
    
    for (int i=0;i<1000000;i++)
      Fiber_Hits[i]={0.};
    
    for (int i=0;i<200;i++)
      Tracking_Hits[i]={0.};
    
    for (int i=0;i<VectorR.size();i++){
      VectorR.at(i)=0.;
    }
    for (int i=0;i<VectorL.size();i++){
      VectorL.at(i)=0.;
    }
    
    for (int i=0;i<VectorR_loop.size();i++){
      VectorR_loop.at(i)=0.;
    }
    for (int i=0;i<VectorL_loop.size();i++){
      VectorL_loop.at(i)=0.;
    }
    
    for (int i=0;i<VectorSignalsR.size();i++){
      VectorSignalsR.at(i)=0.;
    }
    for (int i=0;i<VectorSignalsL.size();i++){
      VectorSignalsL.at(i)=0.;
    }
    for (int i=0;i<VectorSignalsCherR.size();i++){
      VectorSignalsCherR.at(i)=0.;
    }
    for (int i=0;i<VectorSignalsCherL.size();i++){
      VectorSignalsCherL.at(i)=0.;
    }
    PrimaryParticleEnergy = 0;
    
    for(int i=0;i<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);i++){
      if(VectorR.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorR.push_back(0.);}}
    for(int i=0;i<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);i++){
      if(VectorL.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorL.push_back(0.);}}
    
    for(int i=0;i<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);i++){
      if(VectorR_loop.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorR_loop.push_back(0.);}}
    for(int i=0;i<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);i++){
      if(VectorL_loop.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorL_loop.push_back(0.);}}
    
    for(int i=0;i<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);i++){
      if(VectorSignalsR.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorSignalsR.push_back(0.);}}
    for(int i=0;i<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);i++){
      if(VectorSignalsL.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorSignalsL.push_back(0.);}}
    //VectorSignals.at(i)=0;}
    for(int k=0;k<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);k++){
      if(VectorSignalsCherR.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorSignalsCherR.push_back(0.);}}
    for(int k=0;k<=fNbOfZRot*(fNbOfBarrel+fNbOfEndcap);k++){
      if(VectorSignalsCherL.size() <= fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)){
	VectorSignalsCherL.push_back(0.);}}
  }
  
  void DRCaloIO::CreateEdm4HepCollections()
  {
    GMCG4PodioManager * l_podioManager = GMCG4PodioManager::Instance();
    podio::EventStore * l_store = l_podioManager->GetEvtStore();
    podio::ROOTWriter * l_writer = l_podioManager->GetWriter();
    
    if (l_store == NULL){
      std::cerr << "Error in DRCaloIO::writePodio, cannot access podio event store" << std::endl;
      return;
    }
    
    if (l_writer == NULL){
      std::cerr << "Error in DRCaloIO::writePodio, cannot access podio writer" << std::endl;
      return;
    }
    
    s_caloHits = new edm4hep::SimCalorimeterHitCollection();
    l_store->registerCollection("S_caloHits",s_caloHits);
    l_writer->registerForWrite("S_caloHits");
    
    c_caloHits = new edm4hep::SimCalorimeterHitCollection();
    l_store->registerCollection("C_caloHits",c_caloHits);
    l_writer->registerForWrite("C_caloHits");
    
    aux_infoHits = new edm4hep::SimCalorimeterHitCollection();
    l_store->registerCollection("Auxiliary_infoHits",aux_infoHits);
    l_writer->registerForWrite("Auxiliary_infoHits");
    
    s_caloHitContributions = new edm4hep::CaloHitContributionCollection();
    l_store->registerCollection("S_caloHitContrib",s_caloHitContributions);
    l_writer->registerForWrite("S_caloHitContrib");
    
    c_caloHitContributions = new edm4hep::CaloHitContributionCollection();
    l_store->registerCollection("C_caloHitContrib",c_caloHitContributions);
    l_writer->registerForWrite("C_caloHitContrib");

    m_mcParticles = new edm4hep::MCParticleCollection();
    l_store->registerCollection("MCPrimaryParticles",m_mcParticles);
    l_writer->registerForWrite("MCPrimaryParticles");
  }
  
  void DRCaloIO::writePodioTruthPrimaryVertex(const G4Event * g_event)
  {
    GMCG4PodioManager * l_podioManager = GMCG4PodioManager::Instance();
    podio::EventStore * l_store = l_podioManager->GetEvtStore();
    podio::ROOTWriter * l_writer = l_podioManager->GetWriter();    

    if (l_store == NULL){
      std::cerr << "Error in DRCaloIO::writePodio, cannot access podio event store" << std::endl;
      return;
    }
    
    if (l_writer == NULL){
      std::cerr << "Error in DRCaloIO::writePodio, cannot access podio writer" << std::endl;
      return;
    }

    G4int n_pv = g_event->GetNumberOfPrimaryVertex();

    for (G4int i_pv = 0; i_pv < n_pv; ++i_pv){
      std::cout << "Iacopo: Now dealing with primary event " << i_pv << std::endl;
      G4PrimaryVertex * g_pv = g_event->GetPrimaryVertex(i_pv);
      edm4hep::Vector3d vtxPos(g_pv->GetX0(), g_pv->GetY0(), g_pv->GetZ0());
      
      G4int n_particles = g_pv->GetNumberOfParticle();
      
      for (G4int i_particle = 0; i_particle < n_particles; ++i_particle){
	G4PrimaryParticle * g_particle = g_pv->GetPrimary(i_particle);
	auto m_particle = m_mcParticles->create();
	m_particle.setPDG(g_particle->GetPDGcode());
	m_particle.setCharge(g_particle->GetCharge());
	m_particle.setTime(g_particle->GetProperTime());
	m_particle.setMass(g_particle->GetMass());
	m_particle.setVertex(vtxPos);
	m_particle.setMomentum({g_particle->GetPx(),g_particle->GetPy(), g_particle->GetPz()});
      
      }
    }
    
    
  }
  
  void DRCaloIO::writePodio(G4int evId){
    
    GMCG4PodioManager * l_podioManager = GMCG4PodioManager::Instance();
    podio::EventStore * l_store = l_podioManager->GetEvtStore();
    podio::ROOTWriter * l_writer = l_podioManager->GetWriter();
    
    if (l_store == NULL){
      std::cerr << "Error in DRCaloIO::writePodio, cannot access podio event store" << std::endl;
      return;
    }
    
    if (l_writer == NULL){
      std::cerr << "Error in DRCaloIO::writePodio, cannot access podio writer" << std::endl;
      return;
    }
    
    for (auto fiber : Fibers){    
      if (fiber.Type == 1){
	auto l_hit = s_caloHits->create();
	l_hit.setCellID((fiber.ID));
	l_hit.setEnergy(fiber.E);
	l_hit.setPosition({fiber.Pos.x(),fiber.Pos.y(),fiber.Pos.z()});
      } 
      else if (fiber.Type == 0){
	auto l_hit = c_caloHits->create();              
	l_hit.setCellID((fiber.ID));
	l_hit.setEnergy(fiber.E);
	l_hit.setPosition({fiber.Pos.x(),fiber.Pos.y(),fiber.Pos.z()});
      }
      
    }
    
    auto l_hit = aux_infoHits->create();
    l_hit.setCellID(0);
    l_hit.setEnergy(Energyem);
    l_hit = aux_infoHits->create();
    l_hit.setCellID(1);
    l_hit.setEnergy(EnergyScin);
    l_hit = aux_infoHits->create();
    l_hit.setCellID(2);
    l_hit.setEnergy(EnergyCher);
    l_hit = aux_infoHits->create();
    l_hit.setCellID(3);
    l_hit.setEnergy(0); //previsouly allocated for NumberofCherenkovDetected (now storing 0, to be removed)
    l_hit = aux_infoHits->create();
    l_hit.setCellID(4);
    l_hit.setEnergy(EnergyTot);
    l_hit = aux_infoHits->create();
    l_hit.setCellID(5);
    l_hit.setEnergy(PrimaryParticleEnergy);
    l_hit = aux_infoHits->create();
    l_hit.setCellID(6);
    l_hit.setEnergy(0); // Not sure what PrimaryParticleEnergy actually is (if needed PDGID can be stored here)
    l_hit = aux_infoHits->create();
    l_hit.setCellID(7);
    l_hit.setEnergy(neutrinoleakage);
    l_hit = aux_infoHits->create();
    l_hit.setCellID(8);
    l_hit.setEnergy(leakage);
    
    if (l_writer != NULL) l_writer->writeEvent();
    if (l_store != NULL) l_store->clearCollections();
    
  }
  
  void DRCaloIO::Addneutrinoleakage(G4double de){
    neutrinoleakage += de;
  }
  
  void DRCaloIO::Addleakage(G4double de){
    leakage += de;
  }
  
  /*inline*/ void DRCaloIO::AddVectorR(G4double de, G4int tower, G4int slice){
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorR.at(id) += de;
    } else {
      std::cerr<<"DRCaloIO::AddVectorR element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //	VectorR.at(tower+(slice*75)) += de;
  }
  
  /*inline*/ void DRCaloIO::AddVectorL(G4double de, G4int tower, G4int slice){
    tower = -1*tower;
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorL.at(id) += de;
    } else {
      std::cerr<<"DRCaloIO::AddVectorL element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //	VectorL.at(tower+(slice*75)) += de;
  }
  
  /*inlineÃ*/ void DRCaloIO::AddVectorR_loop(G4double de, G4int tower, G4int slice){
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorR_loop.at(id) += de;
    } else {
      std::cerr<<"DRCaloIO::AddVectorR_loop element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //    VectorR_loop.at(tower+(slice*75)) = de;
  }
  
  /*inline*/ void DRCaloIO::AddVectorL_loop(G4double de, G4int tower, G4int slice){
    tower = -1*tower;
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorL_loop.at(id) += de;
    } else {
      std::cerr<<"DRCaloIO::AddVectorL_loop element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //    VectorL_loop.at(tower+(slice*75)) = de;
  }
  
  void DRCaloIO::appendfiber(int ID, int Type, int Slice, int Tower, int E, G4ThreeVector Pos, std::vector<double> phtimes){//function to search if fiber already exists
    Fiber f{ID, Type, Slice, Tower, E, Pos, phtimes};
    auto it = find(FiberIDs.begin(), FiberIDs.end(), f.ID); //return iterator to fiber ID if exhists or FiberIDs.end() if fiber ID is not found
    if ( it == FiberIDs.end() ){       //fiber not found
      FiberIDs.push_back(f.ID);       //push_back fiber ID
      Fibers.push_back(f);            //push_back fiber
    }
    else {                             //fiber found
      Fibers[distance(FiberIDs.begin(), it)].addfiber(f); //add fiber contribution with addfiber()
    }
  };
  
  /*inline*/ void DRCaloIO::WriteFiber_Info(G4double FID, G4double FE, G4int FType, G4ThreeVector Fpos, G4int slice, G4int tower){
    int k=0;
    while (Fiber_Hits[k].F_ID!=0 && Fiber_Hits[k].F_ID!=FID){
      k++;}
    Fiber_Hits[k].F_ID = FID;
    Fiber_Hits[k].F_E += FE;
    Fiber_Hits[k].F_Type = FType;
    Fiber_Hits[k].F_X = Fpos[0];
    Fiber_Hits[k].F_Y = Fpos[1];
    Fiber_Hits[k].F_Z = Fpos[2];
    Fiber_Hits[k].F_slice = slice;
    Fiber_Hits[k].F_tower = tower;
  }
  
  /*inline*/ void DRCaloIO::WriteTracking_Info(G4double TID, G4ThreeVector Tpos, G4String Name, G4double Ek){
    int k=0;
    while (Tracking_Hits[k].T_ID!=0 && Tracking_Hits[k].T_ID!=TID){
      k++;}
    Tracking_Hits[k].T_ID = TID;
    Tracking_Hits[k].T_X = Tpos[0];
    Tracking_Hits[k].T_Y = Tpos[1];
    Tracking_Hits[k].T_Z = Tpos[2];
    Tracking_Hits[k].T_Name = Name;
    if (Tracking_Hits[k].T_Ek<=0.) {Tracking_Hits[k].T_Ek = Ek;}
  }
  
  
  /*inline*/ void DRCaloIO::SavePrimaryParticle(G4String name){
    PrimaryParticleName = name;
  }
  
  /*inline*/ void DRCaloIO::SavePrimaryEnergy(G4double primaryparticleenergy){
    PrimaryParticleEnergy = primaryparticleenergy;
  }
  
  /*inline*/ void DRCaloIO::AddVectorScinEnergyR(G4double de, G4int tower, G4int slice) {
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorSignalsR.at(id) += de;
    } else {
      std::cerr<<"DRCaloIO::AddVectorScinEnergyR element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //    VectorSignalsR.at(tower+(slice*75)) += de;
  }
  
  /*inline*/ void DRCaloIO::AddVectorScinEnergyL(G4double de, G4int tower, G4int slice) {
    tower = -1*tower;
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorSignalsL.at(id) += de;
    } else {
      std::cerr<<"DRCaloIO::AddVectorScinEnergyL element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //    VectorSignalsL.at(tower+(slice*75)) += de;
  }
  
  /*inline*/ void DRCaloIO::AddVectorCherPEL(G4int c_signal, G4int tower, G4int slice) {
    tower = -1*tower;
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorSignalsCherL.at(id) = VectorSignalsCherL.at(id)+c_signal;
    } else {
      std::cerr<<"DRCaloIO::AddVectorCherPEL element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //    VectorSignalsCherL.at(tower+(slice*75)) = VectorSignalsCherL.at(tower+(slice*75))+c_signal;
  }
  
  /*inline*/ void DRCaloIO::AddVectorCherPER(G4int c_signal, G4int tower, G4int slice) {
    std::size_t id = tower+(slice*75);
    if (id<fNbOfZRot*(fNbOfBarrel+fNbOfEndcap)) {
      VectorSignalsCherR.at(id) = VectorSignalsCherR.at(id)+c_signal;
      G4cout<<"DRCaloIO step 1 done"<<std::endl;
    } else {
      std::cerr<<"DRCaloIO::AddVectorCherPER element out of range: tw "<<tower<<" slice "<<slice<<std::endl;
    }
    //    VectorSignalsCherR.at(tower+(slice*75)) = VectorSignalsCherR.at(tower+(slice*75))+c_signal;
  }
  
  /*inline*/ void DRCaloIO::Addem(G4double de) {
    Energyem += de;
  }
  
  /*inline*/ void DRCaloIO::AddScin(G4double de){
    EnergyScin += de;
  }
  
  /*inline*/ void DRCaloIO::AddCher(G4double de){
    EnergyCher += de;
  }
  
  /*inline*/ void DRCaloIO::AddCherenkov(){
    NofCherenkovDetected = NofCherenkovDetected + 1;
  }
  
  /*inline*/ void DRCaloIO::Addenergy(G4double de){
    EnergyTot += de;
  }
  
  
  
}
