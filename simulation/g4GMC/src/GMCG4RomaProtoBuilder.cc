/*
 * GMCG4RomaProtoBuilder.cc
 *
 *  Created on: Mar 22, 2017
 *      Author: tassiell
 */

#include <GMCG4RomaProtoBuilder.hh>

#include <findMaterialOrThrow.hh>
#include <GeomService.hh>

#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
//#include "G4Material.hh"
//#include "G4NistManager.hh"
#include "G4Colour.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

void GMCG4RomaProtoBuilder::constructTracker(G4LogicalVolume* motherLogV) {

  if (motherLogV==NULL) G4cerr<<"GMCG4RomaProtoBuilder mother volume not defined!"<<G4endl;

  G4bool  fCheckOverlaps=true;

  GeomService *geoms = GeomService::Instance();
  const crd::SimpleConfig &cRd = geoms->getConfig();


//  const G4ThreeVector &fShift=cRd.getHep3Vector("rmproto.relCenter");
  const G4ThreeVector &fAngleR=cRd.getHep3Vector("rmproto.relAngle", CLHEP::Hep3Vector(0,0,0) );

  //Describe RomeDriftChamber

  //..............................box
  G4int total_tube_nb = 16;
  G4double box_in_x = (total_tube_nb/2. + 2.)*cm;
  G4double box_in_y = 10.*cm;
  G4double box_in_z = 25.*cm;

  G4double kaptonWall = 100.*um;

  G4Material* fChamberGas = gmc::findMaterialOrThrow(cRd.getString("rmproto.fillMaterial"));

  //outer box
  G4Box* box_out = new G4Box("box_out",0.5*box_in_x + kaptonWall,0.5*box_in_y,0.5*box_in_z);
  G4LogicalVolume* box_out_log = new G4LogicalVolume(box_out,fChamberGas,"box_out_log");


  //create box wall
  G4Material* out_mat = gmc::findMaterialOrThrow("G4_KAPTON");
  G4Box* kapt_wall = new G4Box("kapt_wall",0.5*kaptonWall,0.5*box_in_y,0.5*box_in_z);
  G4LogicalVolume* kapt_wall_log = new G4LogicalVolume(kapt_wall,out_mat,"kapt_wall_log");

  new G4PVPlacement(0,
      G4ThreeVector(-0.5*(box_in_x+kaptonWall),0.,0.),
      kapt_wall_log,"box_wall_1",box_out_log,false,0,fCheckOverlaps);

  new G4PVPlacement(0,
      G4ThreeVector(0.5*(box_in_x+kaptonWall),0.,0.),
      kapt_wall_log,"box_wall_2",box_out_log,false,1,fCheckOverlaps);

  //***** finish the box kapton wall

  //create the gas volume
  G4Box* box_gas = new G4Box("box_gas",0.5*box_in_x,0.5*box_in_y,0.5*box_in_z);

  G4LogicalVolume* box_gas_log =
      new G4LogicalVolume(box_gas,fChamberGas,"box_gas_log");

  //***** finish the chamber gas volume

  G4double TubeSize = 0.975*cm;
  G4double MylarWidth = (36.*1e-4)*cm;
  G4double TubeLength = box_in_z;
  G4double AnodeWireSize = 0.0020*cm;
  G4double startAngle = 0.*deg;
  G4double endAngle = 360.*deg;

  G4Tubs* tracker_tube = new G4Tubs("tracker_tube",
      0.,0.5*TubeSize+MylarWidth,0.5*TubeLength,startAngle,endAngle);

  G4LogicalVolume* tracker_log =
      new G4LogicalVolume(tracker_tube,fChamberGas,"tracker_log");

  //tracker wall
  G4Material* twall_mat = gmc::findMaterialOrThrow("G4_MYLAR");
  G4Tubs* wall_tracker = new G4Tubs("wall_tracker",
      0.5*TubeSize,0.5*TubeSize+MylarWidth,0.5*TubeLength,startAngle,endAngle);

  G4LogicalVolume* wall_tracker_log =
      new G4LogicalVolume(wall_tracker,twall_mat,"wall_tracker_log");

  new G4PVPlacement(0,G4ThreeVector(),wall_tracker_log,"walltube",tracker_log,false,0);

  //anode wire
  G4Material* wire_mat = gmc::findMaterialOrThrow("G4_W");
  G4Tubs* anode_wire = new G4Tubs("anode_wire",0.,0.5*AnodeWireSize,0.5*TubeLength,startAngle,endAngle);

  G4LogicalVolume* anode_wire_log =
      new G4LogicalVolume(anode_wire,wire_mat,"anode_wire_log");

  new G4PVPlacement(0,G4ThreeVector(),anode_wire_log,"anodewire",tracker_log,false,0);

  G4double staggering = 0.5*TubeSize + MylarWidth;
  G4int numberOfTubes = total_tube_nb/2;
  G4double trackerPos_x = 0*cm;
  G4double trackerPos_y = 0*cm;
  G4double trackerPos_z = 0*cm;

  //displacement of the tube in the box
  for (G4int irows=0; irows<2; irows++) {

    for(G4int itb=0; itb<numberOfTubes; itb++){

      trackerPos_x = (1. - numberOfTubes + itb*2.)*staggering;

      if(itb%2 == 0) trackerPos_y = (1 - irows)*2.*staggering;
      else trackerPos_y -= staggering;

      trackerPos_z = 0.;

      char namephys[100];
      sprintf(namephys,"tracker_tube_%d",numberOfTubes*irows+itb);
      new G4PVPlacement(0,
          G4ThreeVector(trackerPos_x,trackerPos_y,trackerPos_z),
          tracker_log,
          namephys,
          box_gas_log,
          false,
          numberOfTubes*irows+itb);
    }
  }

  new G4PVPlacement(0,G4ThreeVector(),box_gas_log,"Chamber",box_out_log,false,0,fCheckOverlaps);

  //define rotation matrix with respect the Main Reference Frame
  G4RotationMatrix* tRot = new G4RotationMatrix();
  tRot->rotateX(0.*deg);
  tRot->rotateY((90.+fAngleR.y())*deg);
  tRot->rotateZ(0.);

  new G4PVPlacement(tRot,G4ThreeVector(),box_out_log,"ChBox",motherLogV,false,0,fCheckOverlaps);


  //--------------------------------------------color
  G4Colour  magenta (1.0, 0.0, 1.0) ;
  G4Colour  blue    (0.0, 0.0, 1.0) ;
  G4Colour  black   (0.0, 0.0, 0.0) ;
  G4Colour  yellow  (1.0, 1.0, 0.0) ;
  G4Colour  white   (1.0, 1.0, 1.0);

//  G4VisAttributes* att_magenta = new G4VisAttributes(magenta);
  G4VisAttributes* att_blue = new G4VisAttributes(blue);
  G4VisAttributes* att_yellow = new G4VisAttributes(yellow);
//  G4VisAttributes* att_white = new G4VisAttributes(white);

  G4VisAttributes* visGreen = new G4VisAttributes();
  visGreen->SetColour(0./255.,255./255.,0./255.,0.);
  visGreen->SetVisibility(false);
  visGreen->SetDaughtersInvisible(0);
  //  visGreen->SetForceWireframe(true);

  box_gas_log->SetVisAttributes(visGreen);
  tracker_log->SetVisAttributes(att_yellow);
  //gas_tracker_log->SetVisAttributes(att_white);
  kapt_wall_log->SetVisAttributes(att_blue);
  //  experimentalHall_log->SetVisAttributes(att_yellow);

  G4double fmaxStep = cRd.getDouble("rmproto.freePath",0.0);
  // Sets a max step length in the tracker region, with G4StepLimiter
  if (fmaxStep > 0.) {
    G4UserLimits *fStepLimit = new G4UserLimits(fmaxStep);
    tracker_log->SetUserLimits(fStepLimit);
  }


}
