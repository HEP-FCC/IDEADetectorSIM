//
// Original author G. Tassielli
//

// GMCG4 includes
#include "DRFPICaloSD.hh"
#include "GeomService.hh"
#include "manageException.hh"

// DRFPI includes
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"
#include <G4OpticalPhoton.hh>
#include <G4OpBoundaryProcess.hh>
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"

#include <string>
#include <random>

#include "DRCaloIO.hh"
#include "Tower.hh"

using namespace std;

namespace drc {

DRFPICaloSD::DRFPICaloSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config) :
		                    SensitiveDetector(name,hitsCollectionName,config)
		                    //                  ,_tower(0)
		                    //                  ,_phiSec(0)
		                    //                  ,_ladder(0)
		                    //                  ,_roSubShell(0)
		                    //                  ,_roIdFstSide(0)
		                    //                  ,_roIdSndSide(0)
		                    {

    if ( !GeomService::Instance()->hasElement<DRFPIcalorimeter>() ) {
        exc::exceptionG4 e("GEOM","Fatal Error",0);
        e<< "Expected DRFPI calorimeter but found neither.\n";
        e.error();
    }
		                    }

DRFPICaloSD::~DRFPICaloSD(){ }

G4bool DRFPICaloSD::ProcessHits(G4Step* step, G4TouchableHistory* ){

//    std::cout<<"DRcaloSD_code in"<<std::endl;
    bool status=false;

    DRCaloIO *ascIO = DRCaloIO::GetInstance();
    // get volume of the current pre-step
//    const G4VTouchable *tmptheTouchable = step->GetPreStepPoint()->GetTouchable();
//    if (!tmptheTouchable) { return false; }
    G4VPhysicalVolume* PreStepVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
//    std::cout<<"DRcaloSD 1 "<<std::endl;

    G4double energydeposited = step->GetTotalEnergyDeposit();
    G4double steplength = step->GetStepLength();

    //define Birk's constant
    double k_B = 0.126;
    G4double saturatedenergydeposited = 0.;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::poisson_distribution<int> cher_distribution(1*0.2723);

    std::poisson_distribution<int> scin_distribution(1.);

    std::exponential_distribution<double> time_distribution(2.8);

    float maxangle = 20.4*M_PI/180.;
    float cosmaxangle = cos(maxangle);

    std::uniform_real_distribution<> angle_distribution(cosmaxangle,1.);

//    std::cout<<"DRcaloSD 2 "<<std::endl;
    //Add energy deposited in towers (copper only)
    if (PreStepVolume->GetName() != "DRFPIMother"){
        ascIO->Addenergy(energydeposited);
        if (PreStepVolume->GetLogicalVolume()->GetMaterial()->GetName() == "Copper"){
            G4double copynumbertower = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
            G4double copynumberslice = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
            if (copynumbertower > 0){          //in barrel right or endcap right
                ascIO->AddVectorR(energydeposited,copynumbertower, copynumberslice);
                status=true;
            }
            if (copynumbertower < 0){          //im in barrel left or endcap left
                ascIO->AddVectorL(energydeposited, copynumbertower, copynumberslice);
                status=true;
            }
        }
    }
//    std::cout<<"DRcaloSD 3 "<<std::endl;

    //compute leak
    //	  if (PreStepVolume->GetName() == "leakageabsorber"){
    if (PreStepVolume->GetName() == "DRFPIMother" && step->IsLastStepInVolume	()){
        //		if (PreStepVolume->GetName() == "DRFPIMother" && step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary){
        auto name = step->GetTrack()->GetDefinition()->GetParticleName();
        if (name=="nu_mu" || name=="nu_e" || name=="anti_nu_e" || name=="anti_nu_mu"){
            ascIO->Addneutrinoleakage(step->GetTrack()->GetKineticEnergy());
            step->GetTrack()->SetTrackStatus(fStopAndKill);
            status=true;
        }
        else{
            ascIO->Addleakage(step->GetTrack()->GetKineticEnergy());
            step->GetTrack()->SetTrackStatus(fStopAndKill);
            status=true;
        }
    };
//    std::cout<<"DRcaloSD 4 "<<std::endl;

    //compute em fraction
    if (PreStepVolume->GetName() != "DRFPIMother" ) {
        if (step->GetTrack()->GetDefinition()->GetParticleName() == "e-" || step->GetTrack()->GetDefinition()->GetParticleName() == "e+"){
            ascIO->Addem(energydeposited);
            status=true;
        }
    }
//    std::cout<<"DRcaloSD 5 "<<std::endl;

    //primary particle energy
    if ( step->GetTrack()->GetTrackID() == 1 && step->GetTrack()->GetCurrentStepNumber() == 1){
        ascIO->SavePrimaryParticle(step->GetTrack()->GetDefinition()->GetParticleName());
        ascIO->SavePrimaryEnergy(step->GetTrack()->GetVertexKineticEnergy());
    }

    //compute and save all informations about scintillating and Cherenkov fibers
    std::string Fiber;
    std::string S_fiber = "fiberCoreScint";
    std::string C_fiber = "fiberCoreChere";
    Fiber = PreStepVolume->GetName(); //name of current step fiber

//    std::cout<<"DRcaloSD 6 "<<std::endl;

    if ( strstr(Fiber.c_str(),S_fiber.c_str())){          //in a scintillating fiber
        //		  std::cout<<"In FIber vol: "<<Fiber<<std::endl;
        //Function to add up energy depoisted in scintillating fibers:
        //- as signal saturated by Birk's law in VectorSignals
        //- as regular energy deposition in all scintillating fibers in EnergyScin
        if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.){
            if (steplength != 0){saturatedenergydeposited = (energydeposited/steplength) / ( 1+k_B*(energydeposited/steplength) ) * steplength;}
        }
        else{saturatedenergydeposited = 0.;}

        ascIO->AddScin(energydeposited);          //energy deposited in scin fibers (not saturated)
        status=true;
        int copynumbertower = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(2);
        int copynumberslice = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(3);
        G4int Sfibercopynumber = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);

        //	    std::cout<<"In FIber: copynumbertower "<<copynumbertower<<" copynumberslice "<<copynumberslice<<" Sfibercopynumber "<<Sfibercopynumber<<std::endl;
        //	    std::cout<<"In FIber: copynumbertower "<<step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(2)<<" copynumberslice "<<step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(3)<<" Sfibercopynumber "<<step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1)<<std::endl;
        //	    std::cout<<"In FIber: copynumbertower "<<step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2)<<" copynumberslice "<<step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(3)<<" Sfibercopynumber "<<step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1)<<std::endl;

        std::string LengthFibr =  step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(1)->GetName();
        int S_fiber_ID = 0;

        scin_distribution = std::poisson_distribution<int> (saturatedenergydeposited*12.5); //smear light emission according to Poissonian distribution
        int s_signal = scin_distribution(generator);                                        //S signal

        if (copynumbertower > 0){                                                       //in barrel right or endcap right
            ascIO->AddVectorScinEnergyR(s_signal,copynumbertower, copynumberslice);    //store signal in vector of towers
            ascIO->AddVectorR(energydeposited, copynumbertower, copynumberslice);      //store energy deposited in vector of towers
            //unique Fiber ID needed: 168750000 is the max of Sfibercopynumber
            S_fiber_ID = Sfibercopynumber+(168750000*copynumberslice);
        }

        if (copynumbertower < 0){                                                           //in barrel left or endcap left
            ascIO->AddVectorScinEnergyL(s_signal, copynumbertower, copynumberslice);
            ascIO->AddVectorL(energydeposited, copynumbertower, copynumberslice);
            //unique Fiber ID needed: 168750000 is the max of Sfibercopynumber
            S_fiber_ID = Sfibercopynumber-(168750000*copynumberslice);
        }

        //Fill vector of Fibers
        if (saturatedenergydeposited>0.){
            //local to global transformation
            G4TouchableHandle theTouchable = step->GetPreStepPoint()->GetTouchableHandle();
            G4ThreeVector origin(0.,0.,0.);
            G4ThreeVector zdir(0.,0.,1.);
            G4ThreeVector vectPos = theTouchable->GetHistory()->
                    GetTopTransform().Inverse().TransformPoint(origin);
            G4ThreeVector direction = theTouchable->GetHistory()->
                    GetTopTransform().Inverse().TransformAxis(zdir);
            G4double lengthfiber = atof(LengthFibr.c_str());
            G4ThreeVector Halffibervect = direction*lengthfiber/2;
            // Fibre tip position
            G4ThreeVector vectPostip = vectPos-Halffibervect;
            // SiPM position	G4ThreeVector SiPMvecPos = vectPos+Halffibervect;
            G4ThreeVector SiPMvecPos = vectPos+Halffibervect;
            if (s_signal>0.0){
                //ascIO->WriteFiber_Info(S_fiber_ID,s_signal,1,vectPostip,copynumberslice,copynumbertower);//1 == S 0 == C
                //create vector with photons time of arrival
                //calculate distance from SiPM (mm)
                double distance = sqrt((SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())*(SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())+(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())*(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())+(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ())*(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ()));
                //calculate time of arrival
                const float speed_s_fiber = 299.792458/1.59;      //mm/ns
                double time_travel;
                vector<double> phtimes;                           //create vector of n identical times of arrival

                for(int i=0;i<s_signal;i++){
                    time_travel = distance/(speed_s_fiber*angle_distribution(generator));
                    phtimes.push_back(step->GetTrack()->GetGlobalTime() + time_distribution(generator) + time_travel);
                }

                ascIO->appendfiber(S_fiber_ID, 1, copynumberslice, copynumbertower, s_signal, vectPostip, phtimes);
                // Extract info for z time
                //std::ofstream TimeFile;
                //TimeFile.open("Time.txt", std::ios_base::app);
                //TimeFile<<"Scin "<< std::fixed << std::setprecision(3) <<S_fiber_ID<<" "<<vectPostip.getX()<<" "<<vectPostip.getY()<<" "<<vectPostip.getZ()<<" "<<s_signal<<" "<<sqrt((SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())*(SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())+(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())*(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())+(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ())*(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ()))<<" "<<step->GetTrack()->GetGlobalTime()<<G4endl;
                //TimeFile.close();
            }
        }
    }

//    std::cout<<"DRcaloSD_code_step1 done"<<std::endl;
    if ( strstr(Fiber.c_str(),C_fiber.c_str())){          //in a Cherenkov fiber
        ascIO->AddCher(step->GetTotalEnergyDeposit());
//        std::cout<<"DRcaloSD_code_step2 done"<<std::endl;
        G4double copynumbertower = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(2);
        G4double copynumberslice = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(3);
//        std::cout<<"DRcaloSD_code_step3 done"<<std::endl;
        if (copynumbertower>0) {ascIO->AddVectorR(energydeposited, copynumbertower, copynumberslice);}
        if (copynumbertower<0) {ascIO->AddVectorL(energydeposited, copynumbertower, copynumberslice);}
        status=true;
    }
//    std::cout<<"DRcaloSD_code_step4 done"<<std::endl;
    //part for cherenkov photons
    G4OpBoundaryProcessStatus theStatus = Undefined;

    G4ProcessManager* OpManager =
            G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    if (OpManager) {
        G4int MAXofPostStepLoops =
                OpManager->GetPostStepProcessVector()->entries();
        G4ProcessVector* fPostStepDoItVector =
                OpManager->GetPostStepProcessVector(typeDoIt);

        for ( G4int i=0; i<MAXofPostStepLoops; i++) {
            G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
            G4OpBoundaryProcess* fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
            if (fOpProcess) { theStatus = fOpProcess->GetStatus(); break;}
        }
    }
//    std::cout<<"DRcaloSD_code_step5 done"<<std::endl;
    std::string SiPMC = "SiPMC";
    std::string SiPMS = "SiPMS";
    std::string SiPMdetection;

    //If the particle is an optical photon...
//    std::cout<<"Track ptr "<<step->GetTrack()<<std::endl;
//    std::cout<<"Definition ptr "<<step->GetTrack()->GetDefinition()<<std::endl;
//    std::cout<<"Particle name "<<step->GetTrack()->GetDefinition()->GetParticleName()<<std::endl;
    if(/*step->GetTrack()!=nullptr &&*/ step->GetTrack()->GetDefinition()->GetParticleName() == "opticalphoton"){

        switch (theStatus){

        case TotalInternalReflection:
            Fiber = PreStepVolume->GetName();

            if(strstr(Fiber.c_str(), C_fiber.c_str())){          //in a Cherenkov fibre
                int copynumbertower = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(2);
                int copynumberslice = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(3);
                G4int Cfibercopynumber = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);

                std::string LengthFibr =  step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(1)->GetName();

                int C_fiber_ID = 0;
                int c_signal = cher_distribution(generator);
                if (copynumbertower>0){          //in barrel right or endcap right
                    ascIO->AddVectorCherPER(c_signal, copynumbertower, copynumberslice);
                    //unique Fiber ID needed: 168750000 is the max of Cfibercopynumber
                    C_fiber_ID = Cfibercopynumber+(168750000*copynumberslice);
                }
                if (copynumbertower<0){          //in barrel left or endcap left
                    ascIO->AddVectorCherPEL(c_signal, copynumbertower, copynumberslice);
                    //I want unique Fiber ID: 168750000 is the max of Cfibercopynumber
                    C_fiber_ID = Cfibercopynumber-(168750000*copynumberslice);
                }
                //ascIO->AddCherenkov(); // add one photoelectron from Cherenkov process in Cherenkov fibers
//                std::cout<<"DRcaloSD_code_step6 done"<<std::endl;
                // Fibers routine: fill the C fibres info
                G4TouchableHandle theTouchable = step->GetPreStepPoint()->GetTouchableHandle();
                G4ThreeVector origin(0.,0.,0.);
                G4ThreeVector zdir(0.,0.,1.);
                G4ThreeVector vectPos = theTouchable->GetHistory()->
                        GetTopTransform().Inverse().TransformPoint(origin);
                G4ThreeVector direction = theTouchable->GetHistory()->
                        GetTopTransform().Inverse().TransformAxis(zdir);
                G4double lengthfiber = atof(LengthFibr.c_str());
                G4ThreeVector Halffibervect = direction*lengthfiber/2;
                // Fibre tip position
                G4ThreeVector vectPostip = vectPos-Halffibervect;
                // SiPM position
                G4ThreeVector SiPMvecPos = vectPos+Halffibervect;
                if (c_signal>0){
                    //ascIO->WriteFiber_Info(C_fiber_ID,c_signal,0,vectPostip,copynumberslice,copynumbertower);// 1 == S 0 == C
                    //calculate distance from SiPM (mm)
                    double distance = sqrt((SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())*(SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())+(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())*(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())+(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ())*(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ()));
                    //calculate time of arrival
                    const float speed_s_fiber = 299.792458/1.59;     //mm/ns
                    double time_travel;
                    vector<double> phtimes;
                    for (int i = 0; i<c_signal; i++){
                        time_travel = distance/(speed_s_fiber*angle_distribution(generator));
                        phtimes.push_back(step->GetTrack()->GetGlobalTime() + time_travel);     //fill vector of photon times
                    }
                    ascIO->appendfiber(C_fiber_ID, 0, copynumberslice, copynumbertower, c_signal, vectPostip, phtimes);
                    //std::ofstream TimeFile;
                    //TimeFile.open("Time.txt", std::ios_base::app);
                    //TimeFile<<"Cher "<<std::fixed << std::setprecision(3) <<C_fiber_ID<<" "<<vectPostip.getX()<<" "<<vectPostip.getY()<<" "<<vectPostip.getZ()<<" "<<c_signal<<" "<<sqrt((SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())*(SiPMvecPos[0]-step->GetTrack()->GetPosition().getX())+(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())*(SiPMvecPos[1]-step->GetTrack()->GetPosition().getY())+(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ())*(SiPMvecPos[2]-step->GetTrack()->GetPosition().getZ()))<<" "<<step->GetTrack()->GetGlobalTime()<<G4endl;
                    //TimeFile.close();
                }
//                std::cout<<"DRcaloSD opticalphoton done"<<std::endl;
                step->GetTrack()->SetTrackStatus(fStopAndKill); //kill photon
            }
            status=true;
            break;

            //case Detection: //in case optical surface is used to detect photons

        default:
            step->GetTrack()->SetTrackStatus(fStopAndKill);
            break;
        }
    }

    return status;
//    std::cout<<"DRcaloSD_code all done"<<std::endl;
}

} //namespace drc
