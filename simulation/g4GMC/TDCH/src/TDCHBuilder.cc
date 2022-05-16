//
// TDCHBuilder builder class for the TDCH in geant4
//
// Original author G. Tassielli
//

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

#include "TDCHBuilder.hh"
#include "Station.hh"
#include "TDCHcellSD.hh"
#include "TDCHtracker.hh"

using namespace std;

namespace tdch {

bool checkOverlap, detailedCheck;
string tdchName("TDCHMother");

VolumeInfo TDCHBuilder::construct( G4LogicalVolume* mother/*, double zOff*/ ){

  // Master geometry for the calorimeter.
  GeomHandle<TDCHtracker> tdch;
  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  VolumeInfo tdchInfo;

  double z0    = CLHEP::mm * tdch->z0();
  G4ThreeVector trackerOffset(0.,0.,z0/*-zOff*/);

  checkOverlap = config.getBool("g4.doSurfaceCheck",false);
  detailedCheck = checkOverlap&&config.getBool("tdch.doDetailedSurfCheck",false);
  bool debugLayer =  config.getBool("tdch.debugLayer",false);

  if (tdch->isExternal()) {
//                throw cet::exception("GEOM") <<"This GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"TDCH: This GDML file option is temporarily disabled\n";
    e.error();
  } else {

	  G4VisAttributes* visAtt=NULL;

	  G4VisAttributes* visAttUnd = new G4VisAttributes(true, G4Colour::Brown());
	  visAttUnd->SetForceSolid(true);
	  visAttUnd->SetForceAuxEdgeVisible (false);
	  visAttUnd->SetVisibility(false);
	  visAttUnd->SetDaughtersInvisible(false);

	  G4VisAttributes* visAttSct = new G4VisAttributes(true, G4Colour::Yellow() );
	  visAttSct->SetForceSolid(true);
	  visAttSct->SetForceAuxEdgeVisible (false);
	  visAttSct->SetDaughtersInvisible(false);

	  //    G4Material* matMother = gmc::findMaterialOrThrow( config.getString("tdch.motherVolMat","G4_AIR") );

	  char vol[30], shape[30];
	  G4ThreeVector relPos = G4ThreeVector(0,0,0);

	  for (int iTw = 0; iTw < tdch->nStation(); iTw++){

		  Station *station = tdch->getStation(iTw);
		  if ( debugLayer ) {
			  cout<<"Station: "<<station->Id()/*<<" contains n SLayers "<<station->nSLayers()*/<<endl;
		  }

//		  sprintf(vol,"svolS%03d",station->Id().getRefId());
//		  std::string cdchSrvShName(vol);
//
//
//		  for (int iLy=0; iLy < tower->nSLayers(); iLy++ ){
//
//			  VolumeInfo SLayerInfo;
//			  boost::shared_ptr<Station> ily = tower->getSLayer(iLy);
//			  int shellId = ily->Id().getShell();
//			  int slId = ily->Id().getSLayer();
//			  if (ily->getSLayerType() == Station::scint) {
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
//			  if (tdch->geomType()==0) {
//
//				  if (ily->getSLayerType() == Station::scint) {
//					  G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::DRFPICalorimeterRO());
//					  if(sd) SLayerInfo.logical->SetSensitiveDetector(sd);
//				  }
//
//			  }
//
//		  }
	  }


	  //    tdchInfo.physical =  new G4PVPlacement( 0,
	  //                    trackerOffset,
	  //                    tdchInfo.logical,
	  //                    tdchName,
	  //                    mother,
	  //                    0,
	  //                    0,
	  //                    checkOverlap);
	  //
	  //    if ( checkOverlap ) { cout<<"TDCH Overlap Checking "<<tdchInfo.physical->CheckOverlaps(100000,0.0001,true)<<endl; }

  }

  return tdchInfo;

}

void TDCHBuilder::constructStepLimiters(){

  // Master geometry for the CDCHtracker.
  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  bool physicalStep =  config.getBool("tdch.usePhysicalStep",false);
  // Maximum step length, in mm.
  double maxStep = 10.0;
  if (physicalStep){
          maxStep = 10.0/12.0;
  }else {
          maxStep = config.getDouble("tdch.freePath", 0.5);
  }
//  G4LogicalVolume* tracker        = _helper->locateVolInfo("CDCHTrackerMother").logical;
  G4LogicalVolume* tracker        = G4LogicalVolumeStore::GetInstance()->GetVolume(tdchName);

//  AntiLeakRegistry& reg = art::ServiceHandle<G4Helper>()->antiLeakRegistry();
//  G4UserLimits* stepLimit = reg.add( G4UserLimits(maxStep) );
  G4UserLimits* stepLimit = new G4UserLimits(maxStep);

  GeomHandle<TDCHtracker> tdchtracker;
// TO be Fixed!!
//  SuperLayer *iSpl;
//  int ilay;
//  for (int ispls=0; ispls<cdchtracker->nSuperLayers(); ispls++){
//          iSpl = cdchtracker->getSuperLayer(ispls);
//          for ( ilay=0; ilay<iSpl->nLayers(); ilay++){
//                  iSpl->getLayer(ilay)->getDetail();
//          }
//
//  }
//  G4VPhysicalVolume *iDau;
//  cout<<"N CDCH daughter: "<<tracker->GetNoDaughters()<<endl;
//  for (int iDaughter=0; iDaughter<tracker->GetNoDaughters(); iDaughter++){
//          iDau = tracker->GetDaughter(iDaughter);
//      if (!iDau) break;
//      //            cout<<"Vol Name "<< iDau->GetName()<<" is Tracking: "<<iDau->GetName().contains("volS")<<endl;
//          if ( iDau->GetName().contains("gvolS") || iDau->GetName().contains("wvolS") ) iDau->GetLogicalVolume()->SetUserLimits(stepLimit);
//  }

  cout<<"TDCH Step limits set"<<endl;


}

void TDCHBuilder::instantiateSensitiveDetectors( const std::string hitsCollectionName){

  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  G4SDManager* SDman      = G4SDManager::GetSDMpointer();

  // G4 takes ownership and will delete the detectors at the job end

  GeomHandle<TDCHtracker> tdch;
  G4ThreeVector tdchPos(0.,0.,CLHEP::mm * tdch->z0()/*-zOff*/);

  TDCHcellSD* tdchSD=0x0;
  tdchSD = new TDCHcellSD(SensitiveDetectorName::TrackerGas(), hitsCollectionName,  config);
//  tdchSD = new TDCHcellSD(SensitiveDetectorName::TDCHtracker(), hitsCollectionName,  config);
  tdchSD->setTDCHCenterInDetSys(tdchPos);
  SDman->AddNewDetector(tdchSD);

}

} // end namespace tdch
