/*
 * GMCG4ModupixTrkrBuilder.cc
 *
 *  Created on: Mar 28, 2017
 *      Author: tassiell
 */

#include <findMaterialOrThrow.hh>
#include <GeomService.hh>
#include <GMCG4ModupixTrkrBuilder.hh>
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
//#include "G4Material.hh"
//#include "G4NistManager.hh"
#include "G4Colour.hh"
//#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
//#include "G4ios.hh"

void GMCG4ModupixTrkrBuilder::constructVrtxTracker(G4LogicalVolume* motherLogV) {

  if (motherLogV==NULL) G4cerr<<"GMCG4RomaProtoBuilder mother volume not defined!"<<G4endl;

  G4bool  fCheckOverlaps=true;

  GeomService *geoms = GeomService::Instance();
  const crd::SimpleConfig &cRd = geoms->getConfig();


  const G4ThreeVector &fAngleR=cRd.getHep3Vector("mdpvrtx.relAngle", CLHEP::Hep3Vector(0,0,0) );
  const G4ThreeVector &fPosPixelA=cRd.getHep3Vector("mdpvrtx.posModA", CLHEP::Hep3Vector(0,0,0) );
  const G4ThreeVector &fPosPixelB=cRd.getHep3Vector("mdpvrtx.posModB", CLHEP::Hep3Vector(0,0,0) );


  //modupix shape
  G4double mdpix_x = 1.425*cm;
  G4double mdpix_y = 1.425*cm;
  G4double mdpix_z = 0.030*cm;

  //define rotation matrix with respect the Main Reference Frame
//  G4double phi = (90.+fAngleR.x())*deg;
//  G4double theta = (90.-fAngleR.y())*deg;
//  G4double psi = fAngleR.z()*deg;

  //  G4ThreeVector ll = G4ThreeVector(0., 1., 0.);
  //  G4ThreeVector mm = G4ThreeVector(0., 1., 0.);
  //  G4ThreeVector nn = G4ThreeVector(0., 0., 1.);
//  G4RotationMatrix *tRot = new G4RotationMatrix(phi,theta,psi);

  G4Material* si_mat = gmc::findMaterialOrThrow("G4_Si");
  G4Box* modupix = new G4Box("modupix",0.5*mdpix_x,0.5*mdpix_y,0.5*mdpix_z);
  G4LogicalVolume* modupix_log = new G4LogicalVolume(modupix,si_mat,"modupix_log");

  new G4PVPlacement(0,
            G4ThreeVector(fPosPixelA.x(),fPosPixelA.y(),fPosPixelA.z()*cm),
            modupix_log,"modupix_1",motherLogV,false,0,fCheckOverlaps);

  new G4PVPlacement(0,
            G4ThreeVector(fPosPixelB.x(),fPosPixelB.y(),fPosPixelB.z()*cm),
            modupix_log,"modupix_2",motherLogV,false,1,fCheckOverlaps);

  G4Colour  yellow  (1.0, 1.0, 0.0) ;
  G4VisAttributes* att_yellow = new G4VisAttributes(yellow);
  modupix_log->SetVisAttributes(att_yellow);

}
