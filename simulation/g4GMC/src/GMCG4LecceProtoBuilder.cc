/*
 * GMCG4LecceProtoBuilder.cc
 *
 *  Created on: Mar 27, 2017
 *      Author: tassiell
 */

#include <GMCG4LecceProtoBuilder.hh>

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

void GMCG4LecceProtoBuilder::constructTracker(G4LogicalVolume* motherLogV) {

  if (motherLogV==NULL) G4cerr<<"GMCG4RomaProtoBuilder mother volume not defined!"<<G4endl;

  G4bool  fCheckOverlaps=true;

  GeomService *geoms = GeomService::Instance();
  const crd::SimpleConfig &cRd = geoms->getConfig();


  const G4ThreeVector &fShift=cRd.getHep3Vector("leproto.relCenter");
  const G4ThreeVector &fAngleR=cRd.getHep3Vector("leproto.relAngle", CLHEP::Hep3Vector(0,0,0) );

  //describe LeccePrototype

  //outer box
  G4double box_in_x = 28.*cm;
  G4double box_in_y = 24.*cm;
  G4double box_in_z = 65.*cm;

  G4double petWall = 0.0190*cm;

  G4Material* fChamberGas = gmc::findMaterialOrThrow(cRd.getString("leproto.fillMaterial"));

  G4Box* box_out = new G4Box("box_out",0.5*box_in_x+petWall,0.5*box_in_y+petWall,0.5*box_in_z);
  G4LogicalVolume* box_out_log = new G4LogicalVolume(box_out,fChamberGas,"box_out_log");

  //create box wall
  G4Material* out_mat = gmc::findMaterialOrThrow("G4_POLYETHYLENE");

  G4Box* pet_wall = new G4Box("pet_wall",0.5*petWall,0.5*15.*cm,0.5*40.*cm);
  G4LogicalVolume* pet_wall_log = new G4LogicalVolume(pet_wall,out_mat,"pet_wall_log");

  G4ThreeVector u = G4ThreeVector(0., 1., 0.);
  G4ThreeVector v = G4ThreeVector(1., 0., 0.);
  G4ThreeVector w = G4ThreeVector(0., 0., 1.);
  G4RotationMatrix *rotm1 = new G4RotationMatrix(u, v, w);

  new G4PVPlacement(rotm1,
                G4ThreeVector(0.,0.5*(box_in_y+petWall),0.),
            pet_wall_log,"box_wall_0",box_out_log,false,0,
            fCheckOverlaps);

  new G4PVPlacement(0,
            G4ThreeVector(0.5*(box_in_x+petWall),0.,0.),
            pet_wall_log,"box_wall_1",box_out_log,false,1,
            fCheckOverlaps);

  new G4PVPlacement(rotm1,
            G4ThreeVector(0.,-0.5*(box_in_y+petWall),0.),
            pet_wall_log,"box_wall_2",box_out_log,false,2,
            fCheckOverlaps);

  new G4PVPlacement(0,
            G4ThreeVector(-0.5*(box_in_x+petWall),0.,0.),
            pet_wall_log,"box_wall_3",box_out_log,false,3,
            fCheckOverlaps);

  //***** finish the box Pet wall

  //create sensitive volume
  G4double box_x = 18.0*cm;
  G4double box_y = 18.0*cm;
  G4double box_z = 56.*cm;

  G4Box* chamber = new G4Box("chamber",0.5*box_x,0.5*box_y,0.5*box_z);
  G4LogicalVolume* chamber_log = new G4LogicalVolume(chamber,fChamberGas,"chamber_log");

  //aluminum brackets
  box_x = 0.5*cm;
  box_z = 58.0*cm;
  G4Box* bracket_vol = new G4Box("bracket_vol",0.5*box_x,0.5*box_y,0.5*box_z);
  G4LogicalVolume* bracket_log = new G4LogicalVolume(bracket_vol,fChamberGas,"bracket_log");

  G4Box* lag_ant= new G4Box("lag_ant",0.5*box_x,0.5*box_y,0.5*2.*cm);
  G4LogicalVolume* lagAnt_log = new G4LogicalVolume(lag_ant,gmc::findMaterialOrThrow("G4_Al"),"lagAnt_log");

  new G4PVPlacement(0,
            G4ThreeVector(0.,0.,-0.5*box_z+1.*cm),
            lagAnt_log,"Asse_1",bracket_log,false,1,
            fCheckOverlaps);

  new G4PVPlacement(0,
            G4ThreeVector(0.,0.,0.5*box_z-1.*cm),
            lagAnt_log,"Asse_3",bracket_log,false,2,
            fCheckOverlaps);

  G4Box* lag_sup= new G4Box("lag_sup",0.5*box_x,0.5*1.*cm,0.5*(box_z-4.*cm));
  G4LogicalVolume* lagSup_log = new G4LogicalVolume(lag_sup,gmc::findMaterialOrThrow("G4_Al"),"lagSup_log");

  new G4PVPlacement(0,
            G4ThreeVector(0.,0.5*box_y-0.5*cm,0.),
            lagSup_log,"Asse_2",bracket_log,false,1,
            fCheckOverlaps);

  new G4PVPlacement(0,
            G4ThreeVector(0.,-0.5*box_y+0.5*cm,0.),
            lagSup_log,"Asse_4",bracket_log,false,2,
            fCheckOverlaps);


  G4Box* in_box = new G4Box("lag_sup",0.5*box_x,0.5*5.*cm,0.5*7.*cm);
  G4LogicalVolume* inBox_log = new G4LogicalVolume(in_box,gmc::findMaterialOrThrow("G4_Al"),"inBox_log");

  new G4PVPlacement(0,
            G4ThreeVector(0.,0.,0.),
            inBox_log,"Central",bracket_log,false,0,
            fCheckOverlaps);

  G4Box* cross_lag= new G4Box("cross_lag",0.5*box_x,0.5*box_x,0.5*box_z);
  G4LogicalVolume* crossedLag_log = new G4LogicalVolume(cross_lag,gmc::findMaterialOrThrow("G4_Al"),"crossedLag_log");

  G4RotationMatrix* tRot_1 = new G4RotationMatrix();
  tRot_1->rotateX(atan(18./58.)*rad);
  tRot_1->rotateY(0.*deg);
  tRot_1->rotateZ(0.*deg);


  new G4PVPlacement(tRot_1,
            G4ThreeVector(0.,0.,0.),
            crossedLag_log,"Cross_0",bracket_log,false,0,
            0);

  G4RotationMatrix* tRot_2 = new G4RotationMatrix();
  tRot_2->rotateX(-atan(18./58.)*rad);
  tRot_2->rotateY(0.*deg);
  tRot_2->rotateZ(0.*deg);

  new G4PVPlacement(tRot_2,
            G4ThreeVector(0.,0.,0.),
            crossedLag_log,"Cross_1",bracket_log,false,0,
            0);

  //positioning the bracket
  new G4PVPlacement(0,
            G4ThreeVector(-0.5*box_y,0.,0.),
            bracket_log,"bracket_sx",box_out_log,false,0,
            fCheckOverlaps);

  new G4PVPlacement(0,
            G4ThreeVector(0.5*box_y,0.,0.),
            bracket_log,"bracket_dx",box_out_log,false,0,
            fCheckOverlaps);


  //create the cell volume
  box_x = 12.010*cm;
  box_y = 0.990*cm;
  box_z = 56.*cm;
  G4Box* tracker_cell = new G4Box("tracker_cell",0.5*box_x,0.5*box_y,0.5*box_z);
  G4LogicalVolume* tracker_log = new G4LogicalVolume(tracker_cell,fChamberGas,"tracker_log");

  box_y = 2.0*cm;
  G4Box* bottom_cap = new G4Box("bottom_cap",0.5*box_x,0.5*box_y,0.5*box_z);
  G4LogicalVolume* bottom_cap_log = new G4LogicalVolume(bottom_cap,fChamberGas,"bottom_cap_log");

  G4Box* top_cap = new G4Box("top_cap",0.5*box_x,0.5*box_y,0.5*box_z);
  G4LogicalVolume* top_cap_log = new G4LogicalVolume(top_cap,fChamberGas,"top_cap_log");


  //Cathode wire
  G4double WireSize = 0.0040*cm;
  G4double startAngle = 180.*deg;
  G4double endAngle = 360.*deg;

  G4Material* Al_wire = gmc::findMaterialOrThrow("G4_Al");
  G4Tubs* cathode_wire_inf = new G4Tubs("cathode_wire_inf",0.,0.5*WireSize,0.5*box_z,startAngle,endAngle);

  G4LogicalVolume* cathode_wire_inf_log =
    new G4LogicalVolume(cathode_wire_inf,Al_wire,"cathode_wire_inf_log");

  G4Tubs* cathode_wire_sup = new G4Tubs("cathode_wire_sup",0.,0.5*WireSize,0.5*box_z,0.,startAngle);

  G4LogicalVolume* cathode_wire_sup_log =
    new G4LogicalVolume(cathode_wire_sup,Al_wire,"cathode_wire_sup_log");

  WireSize = 0.0050*cm;
  G4Tubs* cathode_middle = new G4Tubs("cathode_middle",0.,0.5*WireSize,0.5*box_z,0.,endAngle);

  G4LogicalVolume* cathode_middle_log =
    new G4LogicalVolume(cathode_middle,Al_wire,"cathode_middle_log");

  //anode wire
  WireSize = 0.0020*cm;
  G4Material* W_wire = gmc::findMaterialOrThrow("G4_W");
  G4Tubs* anode_wire = new G4Tubs("anode_wire",0.,0.5*WireSize,0.5*box_z,0.,endAngle);

  G4LogicalVolume* anode_wire_log =
    new G4LogicalVolume(anode_wire,W_wire,"anode_wire_log");

  for (int iw=0;iw<=48;iw++) {
    new G4PVPlacement(0,
              G4ThreeVector((-6.+0.25*iw)*cm,1.*cm,0.),
              cathode_wire_inf_log,
              "cth_bottom",
              bottom_cap_log,
              false,iw);

    new G4PVPlacement(0,
              G4ThreeVector((-6.+0.25*iw)*cm,-1.*cm,0.),
              cathode_wire_sup_log,
              "cth_top",
              top_cap_log,
              false,iw);

    new G4PVPlacement(0,
              G4ThreeVector((-6.+0.25*iw)*cm,-0.495*cm,0.),
              cathode_wire_sup_log,
              "cathode_sup",
              tracker_log,
              false,iw);

    new G4PVPlacement(0,
              G4ThreeVector((-6.+0.25*iw)*cm,0.495*cm,0.),
              cathode_wire_inf_log,
              "cathode_inf",
              tracker_log,
              false,iw);

    if (iw%4 == 0)  {
      new G4PVPlacement(0,
            G4ThreeVector((-6.+0.25*iw)*cm,0.,0.),
            cathode_middle_log,
            "cathode",
            tracker_log,
            false,iw);
    }

    if (iw%4 == 2)  {
      new G4PVPlacement(0,
            G4ThreeVector((-6.+0.25*iw)*cm,0.,0.),
            anode_wire_log,
            "anode",
            tracker_log,
            false,iw);
    }
  }


  //assemblaggio camera
  G4double cell_offset = -5.*cm;//0.99*6.*cm;
  new G4PVPlacement(0,
            G4ThreeVector(0.,cell_offset-1.*cm,0.),
            bottom_cap_log,
            "gBottom",
            chamber_log,
            false,0);
  box_y = 0.990*cm;
  for (int icell=0;icell<12;icell++) {
    char namephys[100];
    sprintf(namephys,"cell_layer_%d",icell+1);

    new G4PVPlacement(0,
              G4ThreeVector(0.,cell_offset+0.5*box_y,0.),
              tracker_log,
              namephys,
              chamber_log,
              false,
              icell+1);

    //    new G4PVDivision(namephys,tracker_log,chamber_log,kXAxis,12,0.);
    cell_offset += box_y;

  }

  new G4PVPlacement(0,
            G4ThreeVector(0.,cell_offset+1.*cm,0.),
            top_cap_log,
            "gTop",
            chamber_log,
            false,13);

  //chamber positioning in the box
  new G4PVPlacement(0,G4ThreeVector(),chamber_log,"DCH",box_out_log,false,0);

  //define rotation matrix with respect the Main Reference Frame
  G4double phi = (-90.+fAngleR.x())*deg;
  G4double theta = (-90.-fAngleR.y())*deg;
  G4double psi = (0.+fAngleR.z())*deg;

  //  G4ThreeVector ll = G4ThreeVector(0., 1., 0.);
  //  G4ThreeVector mm = G4ThreeVector(0., 1., 0.);
  //  G4ThreeVector nn = G4ThreeVector(0., 0., 1.);
  G4RotationMatrix *tRot = new G4RotationMatrix(phi,theta,psi);

  //  tRot->rotateX((fAngleR.x())*deg);
  //  tRot->rotateY((90.+fAngleR)*deg);
  //  tRot->rotateZ(0.*deg);

  //  G4double shift = 7.5*sqrt(2.)*cm*0.5;
  //  G4double shift_z = 10.*cm;
  new G4PVPlacement(tRot,fShift,box_out_log,"ChBox",motherLogV,false,0,fCheckOverlaps);


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
  visGreen->SetForceWireframe(true);

  //  box_gas_log->SetVisAttributes(visGreen);
  //  tracker_log->SetVisAttributes(att_yellow);
  cathode_wire_inf_log->SetVisAttributes(att_blue);
  cathode_wire_sup_log->SetVisAttributes(att_blue);
  cathode_middle_log->SetVisAttributes(att_blue);
  anode_wire_log->SetVisAttributes(att_yellow);

  bracket_log->SetVisAttributes(att_blue);
  pet_wall_log->SetVisAttributes(att_blue);
  //  experimentalHall_log->SetVisAttributes(att_yellow);

  G4double fmaxStep = cRd.getDouble("leproto.freePath",0.0);
//  G4cout<<"--------->>>>>  che step utilizzo "<<fmaxStep<<G4endl;
  // Sets a max step length in the tracker region, with G4StepLimiter
  if (fmaxStep > 0.) {
    G4UserLimits *fStepLimit = new G4UserLimits();
    fStepLimit->SetMaxAllowedStep(fmaxStep*mm);
    tracker_log->SetUserLimits(fStepLimit);
  }

}

