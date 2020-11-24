//
// PHCVtrackerBuilder builder class for the PHCV in geant4
//
// Original author G. Tassielli
//

#include "PHCVtcounter.hh"
#include "Shell.hh"
#include "SLayer.hh"
#include "PHCVBuilder.hh"
#include "PHCVscntSD.hh"
#include "PHCVradSD.hh"

#include "G4Hype.hh"
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

#include "GeomService.hh"
#include "GeomHandle.hh"
#include "findMaterialOrThrow.hh"
#include <SensitiveDetectorName.hh>

using namespace std;

namespace phcv {

bool checkOverlap, detailedCheck;
//string phcvName("PHCVMother");

VolumeInfo PHCVBuilder::construct( G4LogicalVolume* mother/*, double zOff*/ ){

  // Master geometry for the tracker.
  GeomHandle<PHCVtcounter> phcvtc;
  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  VolumeInfo phcvInfo;

  double z0    = CLHEP::mm * phcvtc->z0();
  G4ThreeVector trackerOffset(0.,0.,z0/*-zOff*/);

  checkOverlap = config.getBool("g4.doSurfaceCheck",false);
  detailedCheck = checkOverlap&&config.getBool("phcv.doDetailedSurfCheck",false);
  bool debugLayer =  config.getBool("phcv.debugLayer",false);

  if (phcvtc->isExternal()) {
//                throw cet::exception("GEOM") <<"This GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"PHCV: This GDML file option is temporarily disabled\n";
    e.error();
  } else {

	  G4VisAttributes* visAtt=NULL;

	  G4VisAttributes* visAttUnd = new G4VisAttributes(true, G4Colour::Brown());
	  visAttUnd->SetForceSolid(true);
	  visAttUnd->SetForceAuxEdgeVisible (false);
	  visAttUnd->SetVisibility(false);
	  visAttUnd->SetDaughtersInvisible(false);

	  G4VisAttributes* visAttRad = new G4VisAttributes(true, G4Colour::Brown());
	  visAttRad->SetForceSolid(true);
	  visAttRad->SetForceAuxEdgeVisible (false);
	  visAttRad->SetVisibility(false);
	  visAttRad->SetDaughtersInvisible(false);

	  G4VisAttributes* visAttSct = new G4VisAttributes(true, G4Colour::Yellow() );
	  visAttSct->SetForceSolid(true);
	  visAttSct->SetForceAuxEdgeVisible (false);
	  visAttRad->SetVisibility(false);
	  visAttSct->SetDaughtersInvisible(false);

	  //    G4Material* matMother = gmc::findMaterialOrThrow( config.getString("phcv.motherVolMat","G4_AIR") );

	  char vol[30], shape[30];
	  G4ThreeVector relPos = G4ThreeVector(0,0,0);

	  for (int iSl = 0; iSl < phcvtc->nShell(); iSl++){

		  Shell *shell = phcvtc->getShell(iSl);
		  if ( debugLayer ) {
			  cout<<"Shell: "<<shell->Id()<<" contains n SLayers "<<shell->nSLayers()<<endl;
		  }

		  sprintf(vol,"svolS%03d",shell->Id().getRefId());
		  std::string cdchSrvShName(vol);

		  G4LogicalVolume* shMothLgc=NULL;
		  bool foundMoth=false;
//		  if ( debugLayer ) {
//			  std::cout<<"N of mother vol daugthers: "<<mother->GetNoDaughters()<<std::endl;
//			  std::cout<<"Searching for vol name: "<<cdchSrvShName<<std::endl;
//		  }
		  for (int ivl=0; ivl<mother->GetNoDaughters(); ++ivl) {
			  G4VPhysicalVolume *tmpVol = mother->GetDaughter(ivl);
			  G4String tmpVolName=tmpVol->GetName();
//			  if ( debugLayer ) {
//				  std::cout<<"Sub vol: "<<ivl<<" vol name: "<<tmpVolName<<std::endl;
//			  }
			  if (tmpVolName.contains(cdchSrvShName)) {
				  if ( debugLayer ) {
					  std::cout<<"Service vol found: "<<ivl<<" vol name: "<<tmpVolName<<std::endl;
				  }
				  shMothLgc=tmpVol->GetLogicalVolume();
				  foundMoth=true;
				  break;
			  }
		  }
		  if (foundMoth) {

			  for (int iLy=0; iLy < shell->nSLayers(); iLy++ ){

				  VolumeInfo SLayerInfo;
				  boost::shared_ptr<SLayer> ily = shell->getSLayer(iLy);
				  int shellId = ily->Id().getShell();
				  int slId = ily->Id().getSLayer();
				  if (ily->getSLayerType() == SLayer::radiat) {
					  sprintf(shape,"rdS%dR%d",shellId,slId);
					  sprintf(vol,"rdvolS%03dR%02d",shellId,slId);
					  visAtt=visAttRad;
				  } else if (ily->getSLayerType() == SLayer::scint) {
					  sprintf(shape,"sctS%dR%d",shellId,slId);
					  sprintf(vol,"sctvolS%03dR%02d",shellId,slId);
					  visAtt=visAttSct;
				  } else {
					  sprintf(shape,"shlS%dR%d",shellId,slId);
					  sprintf(vol,"shlvolS%03dR%02d",shellId,slId);
					  visAtt=visAttUnd;
				  }

				  if (debugLayer ) {
					  cout<<"SLayer "<< ily->Id()<<" SLayer type "<<ily->getSLayerType()<<" nFrbWr "<<ily->nFbrWrs()<<endl;
					  cout<<"Vol name "<<vol<<endl;
					  cout<<ily->Id()<<" IR "<<ily->getDetail()->centerInnerRadiusRing()<<" OR "<<
							  ily->getDetail()->centerOuterRadiusRing()<<" SI "<<ily->getDetail()->stereoAngleInnerRing()<<" SO "<<
							  ily->getDetail()->stereoAngleOuterRing()<<" HL "<<ily->getDetail()->halfLength()<<endl;
				  }

				  SLayerInfo.solid = new G4Hype(shape,ily->getDetail()->centerInnerRadiusRing(),
						  ily->getDetail()->centerOuterRadiusRing(),ily->getDetail()->stereoAngleInnerRing(),
						  ily->getDetail()->stereoAngleOuterRing(),ily->getDetail()->halfLength());
				  G4Material* mater = gmc::findMaterialOrThrow( ily->getDetail()->materialName() );
				  SLayerInfo.logical = new G4LogicalVolume(SLayerInfo.solid,mater,vol,0,0,0);
				  if (ily->voxelizationFactor()==0.0) {
					  SLayerInfo.logical->SetOptimisation(false);
				  }
				  else{
					  SLayerInfo.logical->SetSmartless(ily->voxelizationFactor());
				  }
				  SLayerInfo.logical->SetVisAttributes(visAtt);

				  SLayerInfo.physical = new G4PVPlacement(0,               // no rotation
						  relPos,                  // at (x,y,z)
						  SLayerInfo.logical,      // its logical volume
						  vol,                     // its name
						  shMothLgc,               // its mother  volume
						  false,                   // no boolean operations
						  0,                       // copy number
						  checkOverlap);

				  if (phcvtc->geomType()==0) {

					  if (ily->getSLayerType() == SLayer::scint) {
						  G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::PHCVTCounterRO());
						  if(sd) SLayerInfo.logical->SetSensitiveDetector(sd);
					  } else if (ily->getSLayerType() == SLayer::radiat) {
						  G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::PHCVRadiator());
						  if(sd) SLayerInfo.logical->SetSensitiveDetector(sd);
					  }

				  }
//				  else if (...) {
//				  VolumeInfo tmpFieldWireInfo;
//				  for ( int iFw=0; iFw < ily->nFieldWires(); iFw++){
//					  VolumeInfo FieldWireInfo;
//					  boost::shared_ptr<Wire> iwire = ily->getFWire(iFw);
//					  boost::shared_ptr<WireDetail> wdet = iwire->getDetail();
//					  if (iFw==0) {
//						  sprintf(vol_name_FD,"tubeFD_%d_%d",superlayer,iring);
//						  tmpFieldWireInfo = buildWire(wdet->outerRadius(),wdet->halfLength(),shape_name_FD,vol_name_FD,wdet->materialNames(),wdet->shellsThicknesses(),activeWireSD);
//						  tmpFieldWireInfo.logical->SetVisAttributes(visAttFw);
//					  }
//					  FieldWireInfo.solid = tmpFieldWireInfo.solid;
//					  FieldWireInfo.logical = tmpFieldWireInfo.logical;
//					  sprintf(wire_name,"%s_%i",vol_name_FD,iwire->Id().getUId());
//					  FieldWireInfo.name = wire_name;
//					  FieldWireInfo.physical = new G4PVPlacement(iwire->get3DTransfrom(),
//							  FieldWireInfo.logical,         // its logical volume
//							  wire_name,                     // its name
//							  LayerInfo.logical,             // its mother  volume
//							  false,                         // no boolean operations
//							  iwire->Id().getUId(),          // copy number
//							  detailedCheck);
//				  }
//
//				  VolumeInfo tmpSenseWireInfo;
//				  for ( int iSw=0; iSw < ily->nCells(); iSw++){
//					  VolumeInfo SenseWireInfo;
//					  boost::shared_ptr<Wire> iwire = ily->getCell(iSw)->getWire();
//					  boost::shared_ptr<WireDetail> wdet = iwire->getDetail();
//					  if (iSw==0) {
//						  sprintf(vol_name_SD,"tubeSD_%d_%d",superlayer,iring);
//						  tmpSenseWireInfo = buildWire(wdet->outerRadius(),wdet->halfLength(),shape_name_SD,vol_name_SD,wdet->materialNames(),wdet->shellsThicknesses(),activeWireSD,true);
//						  tmpSenseWireInfo.logical->SetVisAttributes(visAttSw);
//					  }
//					  SenseWireInfo.solid = tmpSenseWireInfo.solid;
//					  SenseWireInfo.logical = tmpSenseWireInfo.logical;
//					  sprintf(wire_name,"%s_%i",vol_name_SD,iwire->Id().getUId());
//					  SenseWireInfo.name = wire_name;
//					  SenseWireInfo.physical = new G4PVPlacement(iwire->get3DTransfrom(),
//							  SenseWireInfo.logical,         // its logical volume
//							  wire_name,                     // its name
//							  LayerInfo.logical,             // its mother  volume
//							  false,                         // no boolean operations
//							  iwire->Id().getUId(),          // copy number
//							  detailedCheck);
//				  }
//			  }

			  }
		  } else {
			  std::cerr<<"Service volume for PHCV shell not found in CDCH"<<std::endl;
		  }
	  }


	  //    phcvInfo.physical =  new G4PVPlacement( 0,
	  //                    trackerOffset,
	  //                    phcvInfo.logical,
	  //                    phcvName,
	  //                    mother,
	  //                    0,
	  //                    0,
	  //                    checkOverlap);
	  //
	  //    if ( checkOverlap ) { cout<<"PHCV Overlap Checking "<<phcvInfo.physical->CheckOverlaps(100000,0.0001,true)<<endl; }

  }

  return phcvInfo;

}

void PHCVBuilder::instantiateSensitiveDetectors( const std::string hitsCollectionName){

  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  G4SDManager* SDman      = G4SDManager::GetSDMpointer();

  // G4 takes ownership and will delete the detectors at the job end

  GeomHandle<PHCVtcounter> phcvtc;
  G4ThreeVector phcvPos(0.,0.,CLHEP::mm * phcvtc->z0()/*-zOff*/);

  PHCVscntSD* phcvtcSD=0x0;
  phcvtcSD = new PHCVscntSD(SensitiveDetectorName::PHCVTCounterRO(), hitsCollectionName,  config);
  phcvtcSD->setPHCVCenterInDetSys(phcvPos);
  SDman->AddNewDetector(phcvtcSD);
  if (config.getBool("phcv.ActiveRadiatorSD",false)) {
	  PHCVradSD* phcvrdSD=0x0;
	  phcvrdSD = new PHCVradSD(SensitiveDetectorName::PHCVRadiator(), "PHCVRadHitsCollection",  config);
	  phcvrdSD->setPHCVCenterInDetSys(phcvPos);
	  SDman->AddNewDetector(phcvrdSD);
  }

}

//void PHCVBuilder::constructRadiator( G4LogicalVolume* phcvmother/*, double zOff*/ ){
//
//  // Master geometry for the tracker.
//  GeomHandle<PHCVtracker> phcvtracker;
//  GeomHandle<PHCVRadiator> phcvradiator;
//  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();
//
//  checkOverlap = config.getBool("g4.doSurfaceCheck",false);
//
//  if (phcvtracker->isExternal()) {
////                throw cet::exception("GEOM") <<"This GDML file option is temporarily disabled\n";
//    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//    e<<"PHCV: This GDML file option is temporarily disabled\n";
//    e.error();
//  } else {
//
//    G4VisAttributes* visAtt = new G4VisAttributes(true, G4Colour::Grey() );
//    visAtt->SetForceSolid(true);
//    visAtt->SetForceAuxEdgeVisible (true);
//    visAtt->SetVisibility(true);
//    visAtt->SetDaughtersInvisible(false);
//
////    G4VisAttributes* visAtt1 = new G4VisAttributes(true, G4Colour::Green() );
////    visAtt1->SetForceSolid(true);
////    visAtt1->SetForceAuxEdgeVisible (true);
////    visAtt1->SetVisibility(true);
////    visAtt1->SetDaughtersInvisible(false);
//
//    G4Material* matMother = gmc::findMaterialOrThrow( config.getString("phcv.motherVolMat","G4_AIR") );
////    bool debugLayer =  config.getBool("phcv.debugLayer",false);
//
//    char shape[50], vol[50];
//
////    if (phcvtracker->geomType()==0) {
//
//      for (int iLy = 0; iLy < phcvradiator->getRadiatLayers(); ++iLy){
//
//        VolumeInfo LayerInfo;
//        sprintf(shape,"pswradly-L%d",iLy);
//        sprintf(vol,"pswradlyvol-L%02d",iLy);
//        if (phcvradiator->getRadiatType()[iLy]==0) { //Barrel layers
//
//          LayerInfo.solid = new G4Tubs(shape,phcvradiator->getRadiatInRasius()[iLy],
//              phcvradiator->getRadiatInRasius()[iLy]+phcvradiator->getRadiatorsThickness()[iLy],
//              phcvradiator->getRadiatHalfLengths()[iLy],
//              0.0,360.0*CLHEP::degree);
//          LayerInfo.logical = new G4LogicalVolume(LayerInfo.solid,matMother,vol,0,0,0);
//
//          char tShapeName[50], tVolName[50];
//
//          double iInRad = phcvradiator->getRadiatInRasius()[iLy];
//
//          for (int ishell=0; ishell<phcvradiator->getRadiatNmShells()[iLy]; ++ishell){
//            sprintf(tShapeName,"%s_sub%i",shape,ishell);
//            sprintf(tVolName,"%s_sub%i",vol,ishell);
//
//            G4Tubs *tradsh = new G4Tubs(tShapeName,iInRad,
//                iInRad+phcvradiator->getRadiatShellsThick()[iLy][ishell],
//                phcvradiator->getRadiatHalfLengths()[iLy],
//                0.0,360.0*CLHEP::degree);
//
//            G4LogicalVolume *tlogicRadSh = new G4LogicalVolume(tradsh,gmc::findMaterialOrThrow(phcvradiator->getRadiatShellsMaterial()[iLy][ishell]),tVolName,0,0,0);
//
//            G4VPhysicalVolume *tphysRadSh = new G4PVPlacement(0,
//                G4ThreeVector(0,0,0),
//                tlogicRadSh,        // its logical volume
//                tVolName,           // its name
//                LayerInfo.logical,  // its mother  volume
//                false,              // no boolean operations
//                0,                  // copy number
//                checkOverlap);
//            tphysRadSh->GetCopyNo(); //just to remove the warning during compiling
//            tlogicRadSh->SetVisAttributes(visAtt);
//            iInRad += phcvradiator->getRadiatShellsThick()[iLy][ishell];
//          }
//
//          LayerInfo.physical = new G4PVPlacement(0,               // no rotation
//              G4ThreeVector(0,0,0),    // at (x,y,z)
//              LayerInfo.logical,       // its logical volume
//              vol,                     // its name
//              phcvmother,              // its mother volume
//              false,                   // no boolean operations
//              0,                       // copy number
//              checkOverlap);
//        } else if (phcvradiator->getRadiatType()[iLy]==1) { //Forward layers
//
//          LayerInfo.solid = new G4Tubs(shape,phcvradiator->getRadiatInRasius()[iLy],
//              phcvradiator->getRadiatOutRasius()[iLy],
//              0.5*phcvradiator->getRadiatorsThickness()[iLy],
//              0.0,360.0*CLHEP::degree);
//          LayerInfo.logical = new G4LogicalVolume(LayerInfo.solid,matMother,vol,0,0,0);
//
//          char tShapeName[50], tVolName[50];
//
//          double iZpos = -0.5*phcvradiator->getRadiatorsThickness()[iLy];
//
//          for (int ishell=0; ishell<phcvradiator->getRadiatNmShells()[iLy]; ++ishell){
//            sprintf(tShapeName,"%s_sub%i",shape,ishell);
//            sprintf(tVolName,"%s_sub%i",vol,ishell);
//
//            iZpos += 0.5*phcvradiator->getRadiatShellsThick()[iLy][ishell];
//
//            G4Tubs *tradsh = new G4Tubs(tShapeName,phcvradiator->getRadiatInRasius()[iLy],
//                phcvradiator->getRadiatOutRasius()[iLy],
//                0.5*phcvradiator->getRadiatShellsThick()[iLy][ishell],
//                0.0,360.0*CLHEP::degree);
//
//            G4LogicalVolume *tlogicRadSh = new G4LogicalVolume(tradsh,gmc::findMaterialOrThrow(phcvradiator->getRadiatShellsMaterial()[iLy][ishell]),tVolName,0,0,0);
//
//            G4VPhysicalVolume *tphysRadSh = new G4PVPlacement(0,
//                G4ThreeVector(0,0,iZpos),
//                tlogicRadSh,        // its logical volume
//                tVolName,           // its name
//                LayerInfo.logical,  // its mother  volume
//                false,              // no boolean operations
//                0,                  // copy number
//                checkOverlap);
//            tphysRadSh->GetCopyNo(); //just to remove the warning during compiling
//            /*if (ishell==0)*/ tlogicRadSh->SetVisAttributes(visAtt);
////            if (ishell==1) tlogicRadSh->SetVisAttributes(visAtt1);
//            iZpos += 0.5*phcvradiator->getRadiatShellsThick()[iLy][ishell];
//          }
//
//          LayerInfo.physical = new G4PVPlacement(0,               // no rotation
//              G4ThreeVector(0,0,phcvradiator->getRadiatHalfLengths()[iLy]),    // at (x,y,z)
//              LayerInfo.logical,       // its logical volume
//              vol,                     // its name
//              phcvmother,              // its mother volume
//              false,                   // no boolean operations
//              0,                       // copy number
//              checkOverlap);
//          HepGeom::Transform3D posBckw(HepGeom::TranslateZ3D(-phcvradiator->getRadiatHalfLengths()[iLy])*HepGeom::RotateY3D(CLHEP::pi));
//          LayerInfo.physical = new G4PVPlacement(posBckw,
//              LayerInfo.logical,       // its logical volume
//              vol,                     // its name
//              phcvmother,              // its mother volume
//              false,                   // no boolean operations
//              1,                       // copy number
//              checkOverlap);
//
//        }
//      } // Layer loop
//
//  }
//
//}

} // end namespace phcv
