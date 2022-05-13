//
// DRFPICBuilder builder class for the DRFPI in geant4
//
// Original author G. Tassielli
//

#include "DRFPICBuilder.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
//#include "G4Tubs.hh"
//#include "G4Box.hh"
//#include "G4Trd.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
//#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4GenericTrap.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "GeomService.hh"
#include "GeomHandle.hh"
#include "findMaterialOrThrow.hh"
#include <SensitiveDetectorName.hh>
#include "DRFPICaloSD.hh"
#include "DRFPIcalorimeter.hh"
//#include "Tower.hh"

#include <cmath>

using namespace std;

namespace drc {

bool checkOverlap, detailedCheck, debug;
string drcName("DRFPIMother");

int NbOfBarrel;

G4double phi_unit;
G4double tower_height;

G4double clad_C_rMin;
G4double clad_C_rMax;
G4double clad_C_Dz  ;
G4double clad_C_Sphi;
G4double clad_C_Dphi;

G4double core_C_rMin;
G4double core_C_rMax;
G4double core_C_Dz  ;
G4double core_C_Sphi;
G4double core_C_Dphi;

G4double clad_S_rMin;
G4double clad_S_rMax;
G4double clad_S_Dz  ;
G4double clad_S_Sphi;
G4double clad_S_Dphi;

G4double core_S_rMin;
G4double core_S_rMax;
G4double core_S_Dz  ;
G4double core_S_Sphi;
G4double core_S_Dphi;


G4Material* _clad_C_Material;
G4Material* _core_C_Material;
G4Material* _clad_S_Material;
G4Material* _core_S_Material;
G4Material* _cu;

std::map<int,G4LogicalVolume*> fiberCLog;
std::map<int,G4LogicalVolume*> fiberSLog;


VolumeInfo DRFPICBuilder::construct( G4LogicalVolume* mother/*, double zOff*/ ){

  // Master geometry for the calorimeter.
  GeomHandle<DRFPIcalorimeter> drfpic;
  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  VolumeInfo drcInfo;

  double z0    = CLHEP::mm * drfpic->z0();
  G4ThreeVector caloOffset(0.,0.,z0/*-zOff*/);

  checkOverlap = config.getBool("g4.doSurfaceCheck",false);
  detailedCheck = checkOverlap&&config.getBool("drc.doDetailedSurfCheck",false);
  debug =  config.getBool("drc.debug",false);

  G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::DRFPICalorimeter());

  if (drfpic->isExternal()) {
//                throw cet::exception("GEOM") <<"This GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"DRFPI: This GDML file option is temporarily disabled\n";
    e.error();
  } else {

	  cout<<"Building Dual Readout calorimeter 4pi geometry"<<endl;

// TO be Fixed moved below
  //    G4double innerWallOuterRadius=4500.;
  //    G4double oringHeight=2000.;
 //   G4double travkVolRmax=400.;
 // G4double maxEndCapDim=600.;
      
      //drcInfo.solid = new G4Tubs("DRFPIcalo", innerWallOuterRadius-oringHeight-0.001,travkVolRmax,cdchtracker->maxEndCapDim(),0.0,360.0*CLHEP::degree);
 //     drcInfo.solid = new G4Tubs("DRFPIcalo", innerWallOuterRadius-oringHeight-0.001,travkVolRmax,maxEndCapDim,0.0,360.0*CLHEP::degree);
    
      //drcInfo.logical = new G4LogicalVolume(drcInfo.solid , MATmother, drcName,0,0,0);
 //     drcInfo.logical = new G4LogicalVolume(drcInfo.solid , _cu, drcName,0,0,0);
 //     drcInfo.logical->SetVisAttributes(visAtt);


////////////////////////////////////////////////////
      //TO FIX
    //  //absorber to calculate leakage
    //  G4VSolid* leakageabsorber
    //  = new G4Sphere("leakageabsorber",                        // its name
    //              6000., 6500., 0.*deg, 360.*deg, 0.*deg, 180.*deg); // its size
    //
    //  G4LogicalVolume* leakageabsorberLV
    //  = new G4LogicalVolume(
    //                        leakageabsorber,           // its solid
    //                        defaultMaterial,  // its material (Galactic or Air)
    //                        "leakageabsorber");         // its name
    //
    //  leakageabsorberLV->SetVisAttributes(G4VisAttributes::Invisible);
    //  G4VPhysicalVolume* leakageabsorberPV
    //  = new G4PVPlacement(
    //                      0,                // no rotation
    //                      G4ThreeVector(),  // at (0,0,0)
    //                      leakageabsorberLV,          // its logical volume
    //                      "leakageabsorber",          // its name
    //                      worldLV,                // its mother  volume
    //                      false,            // no boolean operation
    //                      0,                // copy number
    //                      fCheckOverlaps);  // checking overlaps


      constructMaterial(); //move in the Material class

// TO be Fixed

      G4VisAttributes* visAtt = new G4VisAttributes(true, G4Colour::White() );
      visAtt->SetForceSolid(true);
      visAtt->SetForceAuxEdgeVisible (false);
      visAtt->SetVisibility(false);
      visAtt->SetDaughtersInvisible(false);

//      G4double outerRadius=4500.;
//      G4double oringHeight=2000.;
//      G4double travkVolRmax=400.;
//      G4double maxEndCapDim=600.;

      double innerR         = drfpic->innerR();
      tower_height          = drfpic->tower_height();
      double endcHlTht      = drfpic->endc_holeAng();
      double endcHlR        = floor( drfpic->zInHalfLength()*tan(endcHlTht) );
      double endcHlR1       = floor( (drfpic->zInHalfLength()+tower_height)*tan(endcHlTht) );

      G4Material* matMother = gmc::findMaterialOrThrow( config.getString("drc.motherVolMat","G4_AIR") );

      G4Tubs *mothOut = new G4Tubs("DRFPIcaloOut",endcHlR-0.001,drfpic->outerR()+0.001,drfpic->zHalfLength()+0.001,0.0,360.0*CLHEP::degree);
      G4Tubs *mothIn = new G4Tubs("DRFPIcaloIn", endcHlR-0.0011,drfpic->innerR()-0.001,drfpic->zInHalfLength()-0.001,0.0,360.0*CLHEP::degree);

      drcInfo.solid = new G4SubtractionSolid("DRFPIcalo",mothOut,mothIn);

      //drcInfo.logical = new G4LogicalVolume(drcInfo.solid , MATmother, drcName,0,0,0);
      drcInfo.logical = new G4LogicalVolume(drcInfo.solid , matMother, drcName,0,0,0);
//      if(sd) drcInfo.logical->SetSensitiveDetector(sd);

      drcInfo.logical->SetVisAttributes(visAtt);



      //  da controllare

      char name[80];

//      G4LogicalVolume* towerLogical[40];

//      double length = tower_height; //(was tower height + 100)length of physical volumes
      NbOfBarrel        = drfpic->NbOfBarrel();
      int NbOfEndcap        = drfpic->NbOfEndcap();
      int NbOfZRot          = drfpic->NbOfZRot();
//      PMTT                  = drfpic->PMTT()*mm;

      //2*pi/number of tower to complete a rotation around the center
      phi_unit = 2*M_PI/(G4double)NbOfZRot;
      //////////////

      //Parameters for fibers
      clad_C_rMin = 0.49*mm; //cladding cherenkov minimum radius
      clad_C_rMax = 0.50*mm; //cladding cherenkov max radius
      clad_C_Dz   = 2.5*m;   //cladding cherenkov lenght
      clad_C_Sphi = 0.;      //cladding cherenkov min rotation
      clad_C_Dphi = 2.*M_PI; //cladding chrenkov max rotation

      core_C_rMin = 0.*mm;
      core_C_rMax = 0.49*mm;
      core_C_Dz   = 2.5*m;
      core_C_Sphi = 0.;
      core_C_Dphi = 2.*M_PI;

      clad_S_rMin = 0.485*mm;
      clad_S_rMax = 0.50*mm;
      clad_S_Dz   = 2.5*m;
      clad_S_Sphi = 0.;
      clad_S_Dphi = 2.*M_PI;

      core_S_rMin = 0.*mm;
      core_S_rMax = 0.485*mm;
      core_S_Dz   = 2.5*m;
      core_S_Sphi = 0.;
      core_S_Dphi = 2.*M_PI;

      //


// non dovrebbe servire
//      //creating fibers solids
//      //G4cout << "r_clad= " << clad_C_rMax << " r_coreC=" << core_C_rMax << " r_coreS=" << core_S_rMax << G4endl;
//      G4Tubs *fiber = new G4Tubs("Fiber",0,clad_C_rMax,tower_height/2.,0*deg,360.*deg);// S is the same
//      G4Tubs *fiberC = new G4Tubs("fiberC",0,core_C_rMax,tower_height/2.,0*deg,360.*deg);
//      G4Tubs *fiberS = new G4Tubs("fiberS",0,core_S_rMax,tower_height/2.,0*deg,360.*deg);
//
//      //vector for logical volumes of fibers
//      //G4LogicalVolume* fiberCLog[2500];
//      //G4LogicalVolume* fiberSLog[2500];
// fine non dovrebbe servire

      // Prepare for logical volume of fiber tower_height=2000
//      map<int,G4LogicalVolume*> fiberCLog;
//      map<int,G4LogicalVolume*> fiberSLog;

      G4VisAttributes *visAttrC = new G4VisAttributes(G4Colour(0.,0.,1.0));
      visAttrC->SetVisibility(true);
      visAttrC->SetDaughtersInvisible(true);
      visAttrC->SetForceWireframe(true);
      visAttrC->SetForceSolid(true);
      G4VisAttributes *visAttrS = new G4VisAttributes(G4Colour(1.,0.,0.));
      visAttrS->SetVisibility(true);
      visAttrS->SetDaughtersInvisible(true);
      visAttrS->SetForceWireframe(true);
      visAttrS->SetForceSolid(true);
      std::cout << "Iacopo, tower height is " << tower_height << std::endl;
      for(int length=1;length<=tower_height;length++){ //from 1 to 20000
          double half=0.5*length; //half to build objects with proper dimensions
//          char name[80];
          sprintf(name,"Fiber%d",length);
          G4Tubs *fiber = new G4Tubs(name,0,clad_C_rMax,half,0*deg,360.*deg); //creating fibers G4Tubs
          sprintf(name,"fiberC%d",length);
          G4Tubs *fiberC = new G4Tubs(name,0,core_C_rMax,half,0*deg,360.*deg);
          sprintf(name,"fiberS%d",length);
          G4Tubs *fiberS = new G4Tubs(name,0,core_S_rMax,half,0*deg,360.*deg);

          fiberCLog[length] = new G4LogicalVolume(fiber,_clad_C_Material,"fiberCladC");
          fiberSLog[length] = new G4LogicalVolume(fiber,_clad_S_Material,"fiberCladS");
          fiberCLog[length]->SetVisAttributes(visAttrC);
          fiberSLog[length]->SetVisAttributes(visAttrS);
          G4LogicalVolume* fiberCoreCLog = new G4LogicalVolume(fiberC,_core_C_Material,"fiberCoreC");
          G4LogicalVolume* fiberCoreSLog = new G4LogicalVolume(fiberS,_core_S_Material,"fiberCoreS");
          new G4PVPlacement(0,G4ThreeVector(0,0,0),fiberCoreCLog,"fiberCoreCherePhys",fiberCLog[length],false,0,detailedCheck);
          new G4PVPlacement(0,G4ThreeVector(0,0,0),fiberCoreSLog,"fiberCoreScintPhys",fiberSLog[length],false,0,detailedCheck);
          if(sd){
        	  fiberCoreCLog->SetSensitiveDetector(sd);
        	  fiberCoreSLog->SetSensitiveDetector(sd);
          }
      }
      //Final logical volumes of fibers
      //fiberCladCLog = fiberCLog[2500];
      //fiberCladSLog = fiberSLog[2500];

      //Counter on number of volumes
      G4int volnum=0;

      //Controllare cosa è necessario, probabilmente solo thetaB e thetaE che andrebbe preso dalle tower
      // vector length has to be the same of NbOfBarrel
//      G4double deltatheta_barrel[40] = {0};
//      for(int i=0;i<NbOfBarrel;i++) deltatheta_barrel [i] = M_PI/4/(NbOfBarrel);
//      G4double deltatheta_endcap[40] = {0};
//      for(int i=0;i<NbOfEndcap+1;i++) deltatheta_endcap [i] = M_PI/4/(NbOfEndcap+1);
//      double thetaB = 0;
//      for(int i=0;i<NbOfBarrel;i++) thetaB += deltatheta_barrel[i];
//      double thetaE = 0;
//      for(int i=0;i<NbOfEndcap+1;i++) thetaE += deltatheta_endcap[i];
      double thetaB=M_PI/4;

      double length = tower_height;
      
//      ht; //(was tower height + 100)length of physical volumes
//      double innerR_Endcap = innerR;

      //BARREL
      G4Trd* phiBarrel = new G4Trd("phiBarrel",(innerR)*tan(0.5*phi_unit),(innerR+length)*tan(0.5*phi_unit),(innerR)*tan(thetaB),(innerR+length)*tan(thetaB),0.5*length);
      G4LogicalVolume* phiBLog = new G4LogicalVolume(phiBarrel,matMother,"phiBLog");
      //ER
      // I use the G4Generictrap class, so I define the 8 points needed
      vector<G4TwoVector> vertices;
      vertices.push_back( G4TwoVector(endcHlR*tan(0.5*phi_unit),endcHlR)); //it was (0,0) changed to have no elements in the endcap hole
      vertices.push_back( G4TwoVector(-endcHlR*tan(0.5*phi_unit),endcHlR)); //it was (0,0) changed to have no elements in the endcap hole
      vertices.push_back( G4TwoVector(-innerR*tan(0.5*phi_unit),innerR));
      vertices.push_back( G4TwoVector(innerR*tan(0.5*phi_unit),innerR));
      vertices.push_back( G4TwoVector(endcHlR1*tan(0.5*phi_unit),endcHlR1)); //it was (0,0) changed to have no elements in the endcap hole
      vertices.push_back( G4TwoVector(-endcHlR1*tan(0.5*phi_unit),endcHlR1)); //it was (0,0) changed to have no elements in the endcap hole
      vertices.push_back( G4TwoVector(-(innerR+tower_height)*tan(0.5*phi_unit),innerR+tower_height));
      vertices.push_back( G4TwoVector((innerR+tower_height)*tan(0.5*phi_unit),innerR+tower_height));

      G4GenericTrap* phiER = new G4GenericTrap( "phiER", tower_height/2., vertices);
      G4LogicalVolume* phiERLog = new G4LogicalVolume(phiER,matMother,"phiERLog");
      //EL
      // I use the G4Generictrap class, so I define the 8 points needed
      vector<G4TwoVector> vertices2;
      vertices2.push_back( G4TwoVector(endcHlR*tan(0.5*phi_unit),endcHlR)); //it was (0,0) changed to have no elements in the endcap hole
      vertices2.push_back( G4TwoVector(-endcHlR*tan(0.5*phi_unit),endcHlR)); //it was (0,0) changed to have no elements in the endcap hole
      vertices2.push_back( G4TwoVector(-innerR*tan(0.5*phi_unit),innerR));
      vertices2.push_back( G4TwoVector(innerR*tan(0.5*phi_unit),innerR));
      vertices2.push_back( G4TwoVector(endcHlR1*tan(0.5*phi_unit),endcHlR1)); //it was (0,0) changed to have no elements in the endcap hole
      vertices2.push_back( G4TwoVector(-endcHlR1*tan(0.5*phi_unit),endcHlR1)); //it was (0,0) changed to have no elements in the endcap hole
      vertices2.push_back( G4TwoVector(-(innerR+tower_height)*tan(0.5*phi_unit),innerR+tower_height));
      vertices2.push_back( G4TwoVector((innerR+tower_height)*tan(0.5*phi_unit),innerR+tower_height));

      G4GenericTrap* phiEL = new G4GenericTrap( "phiEL", tower_height/2., vertices2);
      G4LogicalVolume* phiELLog = new G4LogicalVolume(phiEL,matMother,"phiELLog");

      //Mother volumes and Z ROTATION
      for(int j=0;j<NbOfZRot;j++){ //j<NbOfZRot

          //place physical spacing of BARREL
      	G4RotationMatrix* rmB = new G4RotationMatrix();
      	rmB->rotateZ(M_PI/2.);
      	rmB->rotateZ(-j*phi_unit);
      	rmB->rotateX(M_PI/2.);
      	new G4PVPlacement(rmB,G4ThreeVector((innerR+0.5*length)*cos(j*phi_unit),(innerR+0.5*length)*sin(j*phi_unit),0),phiBLog,"phiDivPhys",drcInfo.logical,false,j,checkOverlap);
          //ER
      	G4RotationMatrix* rmER = new G4RotationMatrix();
      	rmER->rotateZ(-M_PI/2.);
      	rmER->rotateZ(-j*phi_unit);
      	rmER->rotateX(M_PI);
        rmER->rotateY(M_PI);
      	new G4PVPlacement(rmER,G4ThreeVector(0,0,(innerR)*tan(thetaB)+length/2.),phiERLog,"phiERPhys",drcInfo.logical,false,j,checkOverlap);
          //EL
      	G4RotationMatrix* rmEL = new G4RotationMatrix();
      	rmEL->rotateZ(-M_PI/2.);
      	rmEL->rotateZ(-j*phi_unit);
      	rmEL->rotateX(M_PI);
      	new G4PVPlacement(rmEL,G4ThreeVector(0,0,-(innerR)*tan(thetaB)-length/2.),phiELLog,"phiELPhys",drcInfo.logical,false,j,checkOverlap);
      }

      // Vis Attributes
      G4VisAttributes* phiVisAttr = new G4VisAttributes(G4Colour(0.8,0.8,0.8,0.3));
      phiVisAttr->SetVisibility(true);
      phiBLog->SetVisAttributes(phiVisAttr);
      phiERLog->SetVisAttributes(phiVisAttr);
      phiELLog->SetVisAttributes(phiVisAttr);

      G4VisAttributes* towerVisAttr = new G4VisAttributes(G4Colour(0,0,1));
      towerVisAttr->SetVisibility(true);
      towerVisAttr->SetDaughtersInvisible(false);
      towerVisAttr->SetForceWireframe(true);

      G4VisAttributes* towerVisAttr2 = new G4VisAttributes(G4Colour(0.9, 0.4, 0.1));
      towerVisAttr2->SetVisibility(true);
      towerVisAttr2->SetDaughtersInvisible(false);
      towerVisAttr2->SetForceWireframe(true);

      G4VisAttributes* PMTVisAttr = new G4VisAttributes(G4Colour(0.3,0.6,0.0));
      PMTVisAttr->SetVisibility(true);
      PMTVisAttr->SetDaughtersInvisible(false);
      PMTVisAttr->SetForceWireframe(true);

      //BARREL....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//look on towers

      int i=0;
	  for (int iTw = 0; iTw < drfpic->nTower(); iTw++){

		  Tower *tower = drfpic->getTower(iTw);
		  if ( debug ) {
			  cout<<"Tower: "<<tower->Id()/*<<" contains n SLayers "<<tower->nSLayers()*/<<endl;
		  }

		  sprintf(name,"tower%d",iTw+1); //To be CHECK if it is a problem (is diffrent from original code)
	      G4Trap* towerTrap = new G4Trap("TowerBR",tower->getPt());
	      G4LogicalVolume *towerLogical = new G4LogicalVolume(towerTrap,_cu,name);
          if(sd) {
        	  towerLogical->SetSensitiveDetector(sd);
          }

          G4RotationMatrix* rm = new G4RotationMatrix(tower->getRotM());

       
 		  //BARREL
 		  if (tower->getTowerType()==Tower::barrel ){
 		      towerLogical->SetVisAttributes(towerVisAttr2);
 		      
			  if (tower->getTowerSide()==Tower::right){
        		 sprintf(name,"tower%d",iTw+1);
        		 //placing towers in barrel R
        		 i=iTw;
//		  	  	 new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiBLog,false,iTw+1,detailedCheck);
		  	  	 new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiBLog,false,i+1,detailedCheck);

		  	  	 sprintf(name,"fiber%d",volnum);
		         //VERY IMPORTANT TO PLACE FIBERS
//		  	  	 i=iTw;
		         fiberBR(i, tower, towerTrap, towerLogical);

			  }	
			  else if (tower->getTowerSide()==Tower::left){
			     sprintf(name,"tower%d",-iTw-1);
			     i=iTw-NbOfBarrel;
//		  	  	 new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiBLog,false,-iTw-1,detailedCheck);
		  	  	 new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiBLog,false,-i-1,detailedCheck);

		     	sprintf(name,"fiber%d",volnum);
//		     	i=iTw-NbOfBarrel;
		     	fiberBL(i, tower, towerTrap, towerLogical);

			  }
		  }
		  else if (tower->getTowerType()==Tower::endcap ){
		      towerLogical->SetVisAttributes(towerVisAttr);
		   	 
              
              if (tower->getTowerSide()==Tower::right){
        		 sprintf(name,"tower%d",iTw+1);
        		 i=iTw-2*NbOfBarrel;
//        		 if(i<35) {
//        		     new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiERLog,false,iTw+1,detailedCheck);
        			 new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiERLog,false,NbOfBarrel+i+1,detailedCheck);
        			 fiberER(i, tower, towerTrap, towerLogical);
//        		 }
			  }	
			  else if (tower->getTowerSide()==Tower::left){
			     sprintf(name,"tower%d",-iTw-1);
			     i=iTw-2*NbOfBarrel-NbOfEndcap;
//			     if(i<35) {
// 		    	     new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiELLog,false,-iTw-1,detailedCheck);
			    	 new G4PVPlacement(rm,tower->getCenter(),towerLogical,name,phiELLog,false,-NbOfBarrel-i-1,detailedCheck);
			    	 fiberEL(i, tower, towerTrap, towerLogical);
//			     }
			  }

		  }  

//		  sprintf(vol,"svolS%03d",tower->Id().getRefId());
//		  std::string cdchSrvShName(vol);
//
//
//		  for (int iLy=0; iLy < tower->nSLayers(); iLy++ ){
//
//			  VolumeInfo SLayerInfo;
//			  boost::shared_ptr<Tower> ily = tower->getSLayer(iLy);
//			  int shellId = ily->Id().getShell();
//			  int slId = ily->Id().getSLayer();
//			  if (ily->getSLayerType() == Tower::scint) {
//				  sprintf(shape,"sctS%dR%d",shellId,slId);
//				  sprintf(vol,"sctvolS%03dR%02d",shellId,slId);
//				  visAtt=visAttSct;
//			  } else {
//				  sprintf(shape,"shlS%dR%d",shellId,slId);
//				  sprintf(vol,"shlvolS%03dR%02d",shellId,slId);
//				  visAtt=visAttUnd;
//			  }
//
//			  if (debugLayer ) {
//				  cout<<"SLayer "<< ily->Id()<<" SLayer type "<<ily->getSLayerType()<<" nFrbWr "<<ily->nFbrWrs()<<endl;
//				  cout<<"Vol name "<<vol<<endl;
//				  cout<<ily->Id()<<" IR "<<ily->getDetail()->centerInnerRadiusRing()<<" OR "<<
//						  ily->getDetail()->centerOuterRadiusRing()<<" SI "<<ily->getDetail()->stereoAngleInnerRing()<<" SO "<<
//						  ily->getDetail()->stereoAngleOuterRing()<<" HL "<<ily->getDetail()->halfLength()<<endl;
//			  }
//
//			  SLayerInfo.solid = new G4Hype(shape,ily->getDetail()->centerInnerRadiusRing(),
//					  ily->getDetail()->centerOuterRadiusRing(),ily->getDetail()->stereoAngleInnerRing(),
//					  ily->getDetail()->stereoAngleOuterRing(),ily->getDetail()->halfLength());
//			  G4Material* mater = gmc::findMaterialOrThrow( ily->getDetail()->materialName() );
//			  SLayerInfo.logical = new G4LogicalVolume(SLayerInfo.solid,mater,vol,0,0,0);
//			  if (ily->voxelizationFactor()==0.0) {
//				  SLayerInfo.logical->SetOptimisation(false);
//			  }
//			  else{
//				  SLayerInfo.logical->SetSmartless(ily->voxelizationFactor());
//			  }
//			  SLayerInfo.logical->SetVisAttributes(visAtt);
//
//			  SLayerInfo.physical = new G4PVPlacement(0,               // no rotation
//					  relPos,                  // at (x,y,z)
//					  SLayerInfo.logical,      // its logical volume
//					  vol,                     // its name
//					  shMothLgc,               // its mother  volume
//					  false,                   // no boolean operations
//					  0,                       // copy number
//					  checkOverlap);
//
//			  if (drfpic->geomType()==0) {
//
//				  if (ily->getSLayerType() == Tower::scint) {
//					  G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::DRFPICalorimeterRO());
//					  if(sd) SLayerInfo.logical->SetSensitiveDetector(sd);
//				  }
//
//			  }
//
//		  }
	  }

//	  G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::DRFPICalorimeter());
//	  if(sd) drcInfo.logical->SetSensitiveDetector(sd);


	  drcInfo.physical =  new G4PVPlacement( 0,
			  caloOffset,
			  drcInfo.logical,
			  drcName,
			  mother,
			  0,
			  0,
			  checkOverlap);

	  if ( checkOverlap ) { cout<<"DRFPI Overlap Checking "<<drcInfo.physical->CheckOverlaps(100000,0.0001,true)<<endl; }

	  cout<<"Dual Readout calorimeter 4pi geometry built"<<endl;

  }

  return drcInfo;

}

void DRFPICBuilder::constructMaterial() {
	  //
	    G4NistManager* nistManager = G4NistManager::Instance();

	  G4String symbol;             //a=mass of a mole;
	    G4double a, z, density;      //z=mean number of protons;
//	    G4int iz, n;                 //iz=number of protons  in an isotope;
	    // n=number of nucleons in an isotope;

	    G4int ncomponents, natoms;
//	    G4double abundance, fractionmass;
	    _cu  =new G4Material("Copper"   , z=29., a=63.546*g/mole, density=8.96*g/cm3);
	    G4Element* H  = nistManager->FindOrBuildElement(1);
	    G4Element* C  = nistManager->FindOrBuildElement(6);
//	    G4Element* N  = nistManager->FindOrBuildElement(7);
	    G4Element* O  = nistManager->FindOrBuildElement(8);
	    G4Element* F  = nistManager->FindOrBuildElement(9);
//	    G4Element* Si = nistManager->FindOrBuildElement(14);

//		//---for Solenoid COIL
//		G4Material* Fe = new G4Material("Iron",z=26., a=55.845*g/mole, density=7.874*g/cm3);
//
//		//---for PSD
//		G4Material* Pb = new G4Material("Lead",z=82., a=207.2*g/mole, density=11.35*g/cm3);
//
//	    //--- for PMT Cathod ---
//	    G4Material* Al = new G4Material("Aluminium", z=13., a=26.98*g/mole, density=2.700*g/cm3);

	    //--- for PMT Glass ---
	    G4Material* Glass = new G4Material("Glass", density=1.032*g/cm3,2);
	    Glass->AddElement(C,91.533*perCent);
	    Glass->AddElement(H,8.467*perCent);

	    ///--- for scintillation fiber core ---
	    G4Material* polystyrene =
	    new G4Material("Polystyrene",density= 1.05*g/cm3, ncomponents=2);
	    polystyrene->AddElement(C, natoms=8);
	    polystyrene->AddElement(H, natoms=8);

	    ///--- for cladding (scintillation fibers) ---
	    G4Material* pmma_clad =
	    new G4Material("PMMA_Clad",density= 1.19*g/cm3, ncomponents=3);
	    pmma_clad->AddElement(C, natoms=5);
	    pmma_clad->AddElement(H, natoms=8);
	    pmma_clad->AddElement(O, natoms=2);

	    ///--- for Cerenkov fiber core ---
	    G4Material* pmma =
	    new G4Material("PMMA",density= 1.19*g/cm3, ncomponents=3);
	    pmma->AddElement(C, natoms=5);
	    pmma->AddElement(H, natoms=8);
	    pmma->AddElement(O, natoms=2);

	    ///--- for cladding (Cerenkov fibers) ---
	    G4Material* fluorinatedPolymer =
	    new G4Material("Fluorinated_Polymer", density= 1.43*g/cm3, ncomponents=2);
	    fluorinatedPolymer->AddElement(C,2);
	    fluorinatedPolymer->AddElement(F,2);

//	    G4Material* Air = nistManager->FindOrBuildMaterial("G4_AIR",false);

	    ///--- Material property tables for fiber materials ---
//	    G4MaterialPropertiesTable* mpAir;
//	    G4MaterialPropertiesTable* mpPS;
	    G4MaterialPropertiesTable* mpPMMA;
	    G4MaterialPropertiesTable* mpFS;
	    G4MaterialPropertiesTable* mpGlass;
//	    G4MaterialPropertiesTable* mpPMTPC;

	    //--- Generate and add material properties table ---
	    G4double PhotonEnergy[] = {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
	    	2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
	    	2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
	    	2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
	    	2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
	    	2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
	    	2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
	    	3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
	    	3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
	    	3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};

	    	const G4int nEntries = sizeof(PhotonEnergy) / sizeof(G4double);
	    //--- PMMA ---
	    	G4double RefractiveIndex_PMMA[nEntries] =
	    	{
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49
	    	};
	    	mpPMMA = new G4MaterialPropertiesTable();
	    	mpPMMA->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex_PMMA,nEntries);
	    	pmma->SetMaterialPropertiesTable(mpPMMA);

	    //--- Fluorinated Polymer (FS) ---
	    	G4double RefractiveIndex_FluorinatedPolymer[nEntries] =
	    	{
	    		1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	    		1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	    		1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	    		1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	    		1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42
	    	};
	    	mpFS = new G4MaterialPropertiesTable();
	    	mpFS->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex_FluorinatedPolymer,nEntries);
	    	fluorinatedPolymer->SetMaterialPropertiesTable(mpFS);

	    //Glass
	    	G4double RefractiveIndex_Glass[nEntries] =
	    	{  1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	    		1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49
	    	};
	    	mpGlass = new G4MaterialPropertiesTable();
	    	mpGlass->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex_Glass,nEntries);
	    //mpGlass->AddProperty("ABSLENGTH",PhotonEnergy,Glass_AbsLength,nEntries);
	    //Glass->SetMaterialPropertiesTable(mpGlass);
	    /*if you want air refractive index
	    	G4double RefractiveIndex_Air[nEntries] =
	    	{
	    		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	    		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	    		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	    		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	    		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00
	    	};
	    	mpAir = new G4MaterialPropertiesTable();
	    	mpAir->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex_Air, nEntries);
	    	Air->SetMaterialPropertiesTable(mpAir);*/
	    //default materials of the World
	    //---Materials for Cerenkov fiber---
	    	_clad_C_Material = fluorinatedPolymer;
	    	_core_C_Material = pmma;
	    //---Materials for Scintillation fiber---
	    	_clad_S_Material = pmma_clad;
	    	_core_S_Material = polystyrene;
//	    //--Material for PMT glass---
//	    	Glass_Material = Glass;
//	    //--- Material for PMT Photocathod ---
//	    	PMTPC_Material = Al;
	    //--- Photocathod property ---
	    /* uncomment if you want photocayhod properties
	    	G4double p_mppc[2] = {2.00*eV, 3.47*eV};
	    	G4double refl_mppc[2] = {0.0, 0.0};
	    G4double effi_mppc[2] = {0.11, 0.11}; // mimic Quantum Efficiency
	    G4double photocath_ReR[] = {1.92, 1.92};
	    G4double photocath_ImR[] = {1.69, 1.69};

	    mpPMTPC = new G4MaterialPropertiesTable();
	    mpPMTPC->AddProperty("REFLECTIVITY",p_mppc,refl_mppc,2);
	    mpPMTPC->AddProperty("EFFICIENCY",p_mppc,effi_mppc,2);

	    G4OpticalSurface* photocath_opsurf = new G4OpticalSurface("photocath_opsurf",glisur,polished,dielectric_metal);
	    photocath_opsurf->SetMaterialPropertiesTable(mpPMTPC);*/

}

void DRFPICBuilder::instantiateSensitiveDetectors( const std::string hitsCollectionName){

  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  G4SDManager* SDman      = G4SDManager::GetSDMpointer();

  // G4 takes ownership and will delete the detectors at the job end

  GeomHandle<DRFPIcalorimeter> drfpic;
  G4ThreeVector drcPos(0.,0.,CLHEP::mm * drfpic->z0()/*-zOff*/);

  DRFPICaloSD* drfpicSD=0x0;
  drfpicSD = new DRFPICaloSD(SensitiveDetectorName::DRFPICalorimeter(), hitsCollectionName,  config);
  drfpicSD->setDRFPICenterInDetSys(drcPos);
  SDman->AddNewDetector(drfpicSD);

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DRFPICBuilder::fiberBR(G4int itw,Tower *tower_, G4Trap* tower, G4LogicalVolume *towerLogical){

	if (debug) { cout<<"Placing BR fibers in tower "<<itw<<endl; }

	char name[80];

	vector<G4double> temp_x;
	vector<G4double> temp_y;// vector for grid calculation

	temp_x.clear();
	temp_y.clear();
	temp_x.push_back(0.*mm);
	temp_y.push_back(0.*mm);

	int T_index=itw+1;

	int fiber_N = 1500;

	for(int j = 0 ; j<fiber_N;j++){
		temp_x.push_back(0.*mm);
		temp_y.push_back(1.5*(j+1)*mm);
		temp_x.push_back(0.*mm);
		temp_y.push_back(-1.5*(j+1)*mm);
	}
	for(int i=0; i<fiber_N;i++){
		temp_x.push_back(1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
		temp_x.push_back(-1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
	}
	for(int j = 0;j<fiber_N;j++){
		for(int i = 0;i<fiber_N;i++){
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
		}
	}

	G4double center_x;
	G4double center_y;

	int hi1;
	int hi2;
	int hi1reminder; // 0 -> even 1-> odd
	int hi2reminder;

	int numx;
	int numy;
	int reminderx;
	int remindery;

	G4ThreeVector v1 = tower_->getV()[0];
	G4ThreeVector v2 = tower_->getV()[1];
	G4ThreeVector v3 = tower_->getV()[2];
	G4ThreeVector v4 = tower_->getV()[3];

	G4double innerSide_half = tower_->getInnerR_new()*tan(tower_->getDeltatheta()/2.);
	G4double outerSide_half = (tower_->getInnerR_new()+tower_height)*tan(tower_->getDeltatheta()/2.);

	G4double theta_bc=0;


	//1. 4 types of grid coordinate(x,y) =  (o,o),(e,o),(o,e),(e,e) { e= 0, o=1 }
	int type_x_BR;
	int type_y_BR;

	numx = (int)(((v4.getX()*tan(phi_unit/2.)*2)-1.*mm)/1.5*mm);
	numy = (int)((outerSide_half*2-1.*mm)/(1.5*mm));
	reminderx = numx%2;
	remindery = numy%2;
	if(reminderx == 1) type_x_BR=0;
	if(reminderx == 0) type_x_BR=1;
	if(remindery == 1) type_y_BR=0;
	if(remindery == 0) type_y_BR=1;

	////2. aplying the boundary conditions, reject the fibre which near the boundary of tower surface, get coord of cetres of fibre
	//prepare the grid vector : v2 ~ 114 ( #76 ) , outerSide ~ 132 (#88)

	////////////////////////////////////////////
	vector<G4double> center_x_BR;
	vector<G4double> center_y_BR;
	vector<G4int> bool_cfiber_BR;// 0 -> c fiber 1 s fiber

	// select the type of grid coord
	theta_bc=atan(2*outerSide_half/((v2.getX()-v4.getX())*tan(phi_unit/2.)));

	if(type_x_BR==1&&type_y_BR==1){//(o,o)
		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num);
			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;


			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BR.push_back(center_x);
				center_y_BR.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BR.push_back(0);
				else bool_cfiber_BR.push_back(1);
			}

		}
	}

	if(type_x_BR==0&&type_y_BR==0){//(e,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BR.push_back(center_x);
				center_y_BR.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BR.push_back(0);
				else bool_cfiber_BR.push_back(1);
			}
		}
	}


	if(type_x_BR==0&&type_y_BR==1){//(e,o)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num);

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BR.push_back(center_x);
				center_y_BR.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BR.push_back(0);
				else bool_cfiber_BR.push_back(1);
			}

		}
	}

	if(type_x_BR==1&&type_y_BR==0){//(o,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BR.push_back(center_x);
				center_y_BR.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BR.push_back(0);
				else bool_cfiber_BR.push_back(1);
			}

		}
	}

	//int counter=0;
	for(unsigned int j = 0; j<center_x_BR.size();j++){
		double z = tower->GetZHalfLength();
		bool outside = false;
		for(int ip = 0;ip<4;ip++){
			TrapSidePlane plane = tower->GetSidePlane(ip);
			double zpoint = (-plane.a*center_x_BR.at(j)-plane.b*center_y_BR.at(j)-plane.d)/plane.c;
			outside = (tower->Inside(G4ThreeVector(center_x_BR.at(j),center_y_BR.at(j),zpoint))==kOutside);
			//G4cout << ip << ": " << plane.a << " " << plane.b << " " << plane.c << " " << plane.d << " " << outside << G4endl;
			if(!outside){
				G4ThreeVector normal = tower->SurfaceNormal(G4ThreeVector(center_x_BR.at(j),center_y_BR.at(j),zpoint));
				double angle = normal.angle(G4ThreeVector(0,0,-1));
				double shift = fabs(clad_C_rMax/tan(0.5*M_PI-angle));
				int length = z - zpoint - shift;
				//G4cout<<" "<< length <<" ";
				sprintf(name,"%d",length);
				if(length>=1&&length<=2*z){
					int f=(j+1)+fiber_N*fiber_N*(T_index-1);
					new G4PVPlacement(0,G4ThreeVector(center_x_BR.at(j),center_y_BR.at(j),z-0.5*length),
							(bool_cfiber_BR.at(j)==0)?fiberCLog[length]:fiberSLog[length],name,towerLogical,false,f,detailedCheck);
					//counter++;

				}
				break;
			}
		}
		if(outside) {
			sprintf(name,"%d",2000);
			//cout<<" 2000 ";
			int f=(j+1)+fiber_N*fiber_N*(T_index-1);
			new G4PVPlacement(0,G4ThreeVector(center_x_BR.at(j),center_y_BR.at(j),0),
					(bool_cfiber_BR.at(j)==0)?fiberCLog[2*z]:fiberSLog[2*z],name,towerLogical,false,f,detailedCheck);
			//counter++;

		}
	}//G4cout<<counter<<std::endl;

	if (debug) { cout<<"BR fibers in tower "<<itw<<" placed"<<endl; }

}

void DRFPICBuilder::fiberBL(G4int itw, Tower *tower_ , G4Trap* tower, G4LogicalVolume *towerLogical){

	if (debug) { cout<<"Placing BL fibers in tower "<<itw<<endl; }

	char name[80];

	vector<G4double> temp_x;
	vector<G4double> temp_y;// vector for grid calculation

	temp_x.clear();
	temp_y.clear();
	temp_x.push_back(0.*mm);
	temp_y.push_back(0.*mm);

	int T_index=itw+1;

	int fiber_N=1500;

	for(int j = 0 ; j<fiber_N;j++){
		temp_x.push_back(0.*mm);
		temp_y.push_back(1.5*(j+1)*mm);
		temp_x.push_back(0.*mm);
		temp_y.push_back(-1.5*(j+1)*mm);
	}
	for(int i=0; i<fiber_N;i++){
		temp_x.push_back(1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
		temp_x.push_back(-1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
	}
	for(int j = 0;j<fiber_N;j++){
		for(int i = 0;i<fiber_N;i++){
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
		}
	}

	G4double center_x;
	G4double center_y;
	int hi1;
	int hi2;
	int hi1reminder; // 0 -> even 1-> odd
	int hi2reminder;

	int numx;
	int numy;
	int reminderx;
	int remindery;

	G4ThreeVector v1 = tower_->getV()[0];
	G4ThreeVector v2 = tower_->getV()[1];
	G4ThreeVector v3 = tower_->getV()[2];
	G4ThreeVector v4 = tower_->getV()[3];

	G4double innerSide_half = tower_->getInnerR_new()*tan(tower_->getDeltatheta()/2.);
	G4double outerSide_half = (tower_->getInnerR_new()+tower_height)*tan(tower_->getDeltatheta()/2.);

	G4double theta_bc=0;
	/////////////////////
	//1. 4 types of grid coordinate(x,y) =  (o,o),(e,o),(o,e),(e,e) { e= 0, o=1 }

	int type_x_BL;
	int type_y_BL;

	numx = (int)(((v4.getX()*tan(phi_unit/2.)*2)-1.*mm)/1.5*mm);
	numy = (int)((outerSide_half*2-1.*mm)/(1.5*mm));
	reminderx = numx%2;
	remindery = numy%2;

	if(reminderx == 1) type_x_BL=0;
	if(reminderx == 0) type_x_BL=1;
	if(remindery == 1) type_y_BL=0;
	if(remindery == 0) type_y_BL=1;


	////2. aplying the boundary conditions, reject the fibre which near the boundary of tower surface, get coord of cetres of fibre
	vector<G4double> center_x_BL;
	vector<G4double> center_y_BL;
	vector<G4int> bool_cfiber_BL;// 0 -> c fiber 1 s fiber


	// select the type of grid coord
	theta_bc=atan(2*outerSide_half/((v2.getX()-v4.getX())*tan(phi_unit/2.)));
	if(type_x_BL==1&&type_y_BL==1){//(o,o)
		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num);
			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BL.push_back(center_x);
				center_y_BL.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BL.push_back(0);
				else bool_cfiber_BL.push_back(1);
			}

		}
	}

	if(type_x_BL==0&&type_y_BL==0){//(e,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;
			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BL.push_back(center_x);
				center_y_BL.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BL.push_back(0);
				else bool_cfiber_BL.push_back(1);
			}
		}
	}


	if(type_x_BL==0&&type_y_BL==1){//(e,o)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num);

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BL.push_back(center_x);
				center_y_BL.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BL.push_back(0);
				else bool_cfiber_BL.push_back(1);
			}

		}
	}
	if(type_x_BL==1&&type_y_BL==0){//(o,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v4.getX()*tan(phi_unit/2.)-2*v2.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4.getX()*tan(phi_unit/2.))+outerSide_half))
					&&
					((center_y+clad_C_rMax)<(outerSide_half))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half/(2*v2.getX()*tan(phi_unit/2.)-2*v4.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4.getX()*tan(phi_unit/2.))+outerSide_half
					)))
			{
				center_x_BL.push_back(center_x);
				center_y_BL.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_BL.push_back(0);
				else bool_cfiber_BL.push_back(1);
			}

		}
	}

	for(unsigned int j = 0; j<center_x_BL.size();j++){
		// determine z value for center of fibre
		double z = tower->GetZHalfLength();
		bool outside = false;
		for(int ip = 0;ip<4;ip++){
			TrapSidePlane plane = tower->GetSidePlane(ip);
			double zpoint = (-plane.a*center_x_BL.at(j)-plane.b*center_y_BL.at(j)-plane.d)/plane.c;
			outside = (tower->Inside(G4ThreeVector(center_x_BL.at(j),center_y_BL.at(j),zpoint))==kOutside);
			if(!outside){
				G4ThreeVector normal = tower->SurfaceNormal(G4ThreeVector(center_x_BL.at(j),center_y_BL.at(j),zpoint));
				double angle = normal.angle(G4ThreeVector(0,0,-1));
				double shift = fabs(clad_C_rMax/tan(0.5*M_PI-angle));
				int length = z - zpoint - shift;
				sprintf(name,"%d",length);
				if(length>=1&&length<=2*z){
					int f=-((j+1)+fiber_N*fiber_N*(T_index-1));
					new G4PVPlacement(0,G4ThreeVector(center_x_BL.at(j),center_y_BL.at(j),z-0.5*length),
							(bool_cfiber_BL.at(j)==0)?fiberCLog[length]:fiberSLog[length],name,towerLogical,false,f,detailedCheck);
				}
				break;
			}
		}
		if(outside){
			sprintf(name,"%d",2000);
			int f=-((j+1)+fiber_N*fiber_N*(T_index-1));
			new G4PVPlacement(0,G4ThreeVector(center_x_BL.at(j),center_y_BL.at(j),0),
					(bool_cfiber_BL.at(j)==0)?fiberCLog[2*z]:fiberSLog[2*z],name,towerLogical,false,f,detailedCheck);
		}
	}

	if (debug) { cout<<"BL fibers in tower "<<itw<<" placed"<<endl; }

}

void DRFPICBuilder::fiberER(G4int itw, Tower *tower_, G4Trap* tower, G4LogicalVolume *towerLogical){

	if (debug) { cout<<"Placing RR fibers in tower "<<itw<<endl; }

	char name[80];

	vector<G4double> temp_x;
	vector<G4double> temp_y;// vector for grid calculation

	temp_x.clear();
	temp_y.clear();
	temp_x.push_back(0.*mm);
	temp_y.push_back(0.*mm);

	int fiber_N=1500;
	int T_index=NbOfBarrel+itw+1;

	for(int j = 0 ; j<fiber_N;j++){
		temp_x.push_back(0.*mm);
		temp_y.push_back(1.5*(j+1)*mm);
		temp_x.push_back(0.*mm);
		temp_y.push_back(-1.5*(j+1)*mm);
	}
	for(int i=0; i<fiber_N;i++){
		temp_x.push_back(1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
		temp_x.push_back(-1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
	}
	for(int j = 0;j<fiber_N;j++){
		for(int i = 0;i<fiber_N;i++){
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
		}
	}

	G4double center_x;
	G4double center_y;
	int hi1;
	int hi2;
	int hi1reminder; // 0 -> even 1-> odd
	int hi2reminder;

	int numx;
	int numy;
	int reminderx;
	int remindery;

	G4ThreeVector v1_ = tower_->getV()[2];//dimE->GetV3();
	G4ThreeVector v2_ = tower_->getV()[3];//dimE->GetV4();
	G4ThreeVector v3_ = tower_->getV()[0];//dimE->GetV1();
	G4ThreeVector v4_ = tower_->getV()[1];//dimE->GetV2();

	G4double innerSide_half_ = tower_->getInnerR_new()*tan(tower_->getDeltatheta()/2.);
	G4double outerSide_half_ = (tower_->getInnerR_new()+tower_height)*tan(tower_->getDeltatheta()/2.);

	G4double theta_bc=0;
	/////////////////////
	//1. 4 types of grid coordinate(x,y) =  (o,o),(e,o),(o,e),(e,e) { e= 0, o=1 }


	int type_x_ER;
	int type_y_ER;

	numx = (int)(((v4_.getX()*tan(phi_unit/2.)*2)-1.*mm)/1.5*mm);
	numy = (int)((outerSide_half_*2-1.*mm)/(1.5*mm));
	reminderx = numx%2;
	remindery = numy%2;

	if(reminderx == 1) type_x_ER=0;
	if(reminderx == 0) type_x_ER=1;
	if(remindery == 1) type_y_ER=0;
	if(remindery == 0) type_y_ER=1;

	////2. aplying the boundary conditions, reject the fibre which near the boundary of tower surface, get coord of cetres of fibre

	vector<G4double> center_x_ER;
	vector<G4double> center_y_ER;
	vector<G4int> bool_cfiber_ER;// 0 -> c fiber 1 s fiber

	theta_bc=atan(2*outerSide_half_/((v2_.getX()-v4_.getX())*tan(phi_unit/2.)));

	if(type_x_ER==1&&type_y_ER==1){//(o,o)
		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num);
			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_ER.push_back(center_x);
				center_y_ER.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_ER.push_back(0);
				else bool_cfiber_ER.push_back(1);
			}

		}
	}

	if(type_x_ER==0&&type_y_ER==0){//(e,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_ER.push_back(center_x);
				center_y_ER.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_ER.push_back(0);
				else bool_cfiber_ER.push_back(1);
			}
		}
	}


	if(type_x_ER==0&&type_y_ER==1){//(e,o)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num);

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2_.getX()*tan(phi_unit/2.))+outerSide_half_))

					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_ER.push_back(center_x);
				center_y_ER.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_ER.push_back(0);
				else bool_cfiber_ER.push_back(1);
			}

		}
	}
	if(type_x_ER==1&&type_y_ER==0){//(o,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v2_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y-cos(theta_bc)*clad_C_rMax)>(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v2_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_ER.push_back(center_x);
				center_y_ER.push_back(center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_ER.push_back(0);
				else bool_cfiber_ER.push_back(1);
			}

		}
	}

	//int counter=0;
	for(unsigned int j=0;j<center_x_ER.size();j++){
		double z = tower->GetZHalfLength();
		bool outside = false;
		for(int ip = 0;ip<4;ip++){
			TrapSidePlane plane = tower->GetSidePlane(ip);
			double zpoint = (-plane.a*center_x_ER.at(j)-plane.b*center_y_ER.at(j)-plane.d)/plane.c;
			outside = (tower->Inside(G4ThreeVector(center_x_ER.at(j),center_y_ER.at(j),zpoint))==kOutside);
			if(!outside){

				G4ThreeVector normal = tower->SurfaceNormal(G4ThreeVector(center_x_ER.at(j),center_y_ER.at(j),zpoint));
				double angle = normal.angle(G4ThreeVector(0,0,-1));
				double shift = fabs(clad_C_rMax/tan(0.5*M_PI-angle));
				int length = z - zpoint - shift;
				//cout<<length<<std::endl;
				sprintf(name,"%d",length);
				if(length>=1&&length<=2*z){
					//counter++; //sprintf(name,"%.1f_%.1f",center_x_ER.at(j),center_y_ER.at(j));
					int f=(j+1)+fiber_N*fiber_N*(T_index-1);
					new G4PVPlacement(0,G4ThreeVector(center_x_ER.at(j),center_y_ER.at(j),z-0.5*length),
							(bool_cfiber_ER.at(j)==0)?fiberCLog[length]:fiberSLog[length],name,towerLogical,false,f,detailedCheck);
				}
				break;
			}
		}
		if(outside){
			//cout<<" 2000 ";
			sprintf(name,"%d",2000);
			int f=(j+1)+fiber_N*fiber_N*(T_index-1);

			new G4PVPlacement(0,G4ThreeVector(center_x_ER.at(j),center_y_ER.at(j),0),
					(bool_cfiber_ER.at(j)==0)?fiberCLog[2*z]:fiberSLog[2*z],name,towerLogical,false,f,detailedCheck);
			//counter++;
		}
	}
	//G4cout<<counter<<std::endl;
	if (debug) { cout<<"ER fibers in tower "<<itw<<" placed"<<endl; }

}

void DRFPICBuilder::fiberEL(G4int itw, Tower *tower_, G4Trap* tower, G4LogicalVolume *towerLogical){

	if (debug) { cout<<"Placing EL fibers in tower "<<itw<<endl; }

	char name[80];

	vector<G4double> temp_x;
	vector<G4double> temp_y;// vector for grid calculation

	temp_x.clear();
	temp_y.clear();
	temp_x.push_back(0.*mm);
	temp_y.push_back(0.*mm);

	int T_index=NbOfBarrel+itw+1;
	int fiber_N = 1500;

	for(int j = 0 ; j<fiber_N;j++){
		temp_x.push_back(0.*mm);
		temp_y.push_back(1.5*(j+1)*mm);
		temp_x.push_back(0.*mm);
		temp_y.push_back(-1.5*(j+1)*mm);
	}
	for(int i=0; i<fiber_N;i++){
		temp_x.push_back(1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
		temp_x.push_back(-1.5*(i+1)*mm);
		temp_y.push_back(0.*mm);
	}
	for(int j = 0;j<fiber_N;j++){
		for(int i = 0;i<fiber_N;i++){
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
			temp_x.push_back(-1.5*(i+1)*mm);
			temp_y.push_back(1.5*(j+1)*mm);
			temp_x.push_back(1.5*(i+1)*mm);
			temp_y.push_back(-1.5*(j+1)*mm);
		}
	}

	G4double center_x;
	G4double center_y;
	int hi1;
	int hi2;
	int hi1reminder; // 0 -> even 1-> odd
	int hi2reminder;

	int numx;
	int numy;
	int reminderx;
	int remindery;

	G4ThreeVector v1_ = tower_->getV()[2];//dimE->GetV3();
	G4ThreeVector v2_ = tower_->getV()[3];//dimE->GetV4();
	G4ThreeVector v3_ = tower_->getV()[0];//dimE->GetV1();
	G4ThreeVector v4_ = tower_->getV()[1];//dimE->GetV2();
	G4double innerSide_half_ = tower_->getInnerR_new()*tan(tower_->getDeltatheta()/2.);
	G4double outerSide_half_ = (tower_->getInnerR_new()+tower_height)*tan(tower_->getDeltatheta()/2.);

	G4double theta_bc=0;
	/////////////////////
	//1. 4 types of grid coordinate(x,y) =  (o,o),(e,o),(o,e),(e,e) { e= 0, o=1 }

	int type_x_EL;
	int type_y_EL;

	numx = (int)(((v4_.getX()*tan(phi_unit/2.)*2)-1.*mm)/1.5*mm);
	numy = (int)((outerSide_half_*2-1.*mm)/(1.5*mm));
	reminderx = numx%2;
	remindery = numy%2;

	if(reminderx == 1) type_x_EL=0;
	if(reminderx == 0) type_x_EL=1;
	if(remindery == 1) type_y_EL=0;
	if(remindery == 0) type_y_EL=1;

	////2. aplying the boundary conditions, reject the fibre which near the boundary of tower surface, get coord of cetres of fibre


	vector<G4double> center_x_EL;
	vector<G4double> center_y_EL;
	vector<G4int> bool_cfiber_EL;// 0 -> c fiber 1 s fiber

	theta_bc=atan(2*outerSide_half_/((v2_.getX()-v4_.getX())*tan(phi_unit/2.)));

	if(type_x_EL==1&&type_y_EL==1){//(o,o)
		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num);
			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_EL.push_back(-center_x);
				center_y_EL.push_back(-center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_EL.push_back(0);
				else bool_cfiber_EL.push_back(1);
			}

		}
	}
	if(type_x_EL==0&&type_y_EL==0){//(e,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_EL.push_back(-center_x);
				center_y_EL.push_back(-center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_EL.push_back(0);
				else bool_cfiber_EL.push_back(1);
			}
		}
	}
	if(type_x_EL==0&&type_y_EL==1){//(e,o)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num)+0.75*mm;
			center_y = temp_y.at(num);

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_EL.push_back(-center_x);
				center_y_EL.push_back(-center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_EL.push_back(0);
				else bool_cfiber_EL.push_back(1);
			}

		}
	}
	if(type_x_EL==1&&type_y_EL==0){//(o,e)

		for(unsigned int num = 0;num<temp_x.size();num++){
			center_x = temp_x.at(num);
			center_y = temp_y.at(num)-0.75*mm;

			hi1 = fabs(temp_x.at(num))/(1.5*mm);
			hi2 = fabs(temp_y.at(num))/(1.5*mm);
			hi1reminder = hi1%2; // 0 -> even 1-> odd
			hi2reminder = hi2%2;

			if(((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v4_.getX()*tan(phi_unit/2.)-2*v2_.getX()*tan(phi_unit/2.))*(center_x+sin(theta_bc)*clad_C_rMax-v4_.getX()*tan(phi_unit/2.))+outerSide_half_))
					&&
					((center_y+clad_C_rMax)<(outerSide_half_))
					&&
					((center_y-clad_C_rMax)>(-outerSide_half_))
					&&
					((center_y+cos(theta_bc)*clad_C_rMax)<(2*2*outerSide_half_/(2*v2_.getX()*tan(phi_unit/2.)-2*v4_.getX()*tan(phi_unit/2.))*(center_x-sin(theta_bc)*clad_C_rMax+v4_.getX()*tan(phi_unit/2.))+outerSide_half_
					)))
			{
				center_x_EL.push_back(-center_x);
				center_y_EL.push_back(-center_y);
				if(((hi1reminder==0)&&(hi2reminder==0))||((hi1reminder==1)&&(hi2reminder==1)))
					bool_cfiber_EL.push_back(0);
				else bool_cfiber_EL.push_back(1);
			}

		}
	}

	for(unsigned int j = 0; j<center_x_EL.size();j++){
		double z = tower->GetZHalfLength();
		bool outside = false;
		for(int ip = 0;ip<4;ip++){
			TrapSidePlane plane = tower->GetSidePlane(ip);
			double zpoint = (-plane.a*center_x_EL.at(j)-plane.b*center_y_EL.at(j)-plane.d)/plane.c;
			outside = (tower->Inside(G4ThreeVector(center_x_EL.at(j),center_y_EL.at(j),zpoint))==kOutside);
			if(!outside){
				G4ThreeVector normal = tower->SurfaceNormal(G4ThreeVector(center_x_EL.at(j),center_y_EL.at(j),zpoint));
				double angle = normal.angle(G4ThreeVector(0,0,-1));
				double shift = fabs(clad_C_rMax/tan(0.5*M_PI-angle));
				int length = z - zpoint - shift;
				sprintf(name,"%d",length);
				if(length>=1&&length<=2*z){
					int f=-((j+1)+fiber_N*fiber_N*(T_index-1));
					new G4PVPlacement(0,G4ThreeVector(center_x_EL.at(j),center_y_EL.at(j),z-0.5*length),
							(bool_cfiber_EL.at(j)==0)?fiberCLog[length]:fiberSLog[length],name,towerLogical,false,f,detailedCheck);
				}
				break;
			}
		}
		if(outside){
			sprintf(name,"%d",2000);
			int f=-((j+1)+fiber_N*fiber_N*(T_index-1));
			new G4PVPlacement(0,G4ThreeVector(center_x_EL.at(j),center_y_EL.at(j),0),
					(bool_cfiber_EL.at(j)==0)?fiberCLog[2*z]:fiberSLog[2*z],name,towerLogical,false,f,detailedCheck);
		}
	}

	if (debug) { cout<<"EL fibers in tower "<<itw<<" placed"<<endl; }

}


} // end namespace drc
