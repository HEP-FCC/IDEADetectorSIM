/*
 * GMCG4StopTargetBuilder.cc
 *
 *  Created on: Mar 22, 2017
 *      Author: tassiell
 */

#include "GMCG4StopTargetBuilder.hh"

#include <findMaterialOrThrow.hh>
#include <GeomService.hh>
#include "manageException.hh"

#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
//#include "G4Material.hh"
//#include "G4NistManager.hh"
#include "G4Colour.hh"
//#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

#include <string>
#include <vector>

void GMCG4StopTargetBuilder::construct(G4LogicalVolume* motherLogV) {

  if (motherLogV==NULL) G4cerr<<"GMCG4StopTargetBuilder mother volume not defined!"<<G4endl;

  G4bool  checkOverlap=true;

  GeomService *geoms = GeomService::Instance();
  const crd::SimpleConfig &cRd = geoms->getConfig();


  checkOverlap = cRd.getBool("g4.doSurfaceCheck",false);

  bool isExternal     = false;
  std::string extFile        = cRd.getString("strgt.extFile");
  if ( extFile.size()>1 && ( extFile.find_last_of(".gdml") || extFile.find_last_of(".GDML") )!=0 ) isExternal = true;

  if (isExternal) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"This GDML file option is temporarily disabled\n";
    e.error();
  } else {
    int geomType       = cRd.getInt("strgt.geomType");
    double r0          = cRd.getDouble("strgt.r0");
//    double halfLength  = cRd.getDouble("strgt.halfLength");
    double phiangle    = cRd.getDouble("strgt.phiangle");

    double totThick = 0.0;

    int nShells        = cRd.getInt("strgt.nShells");

    std::vector<double> shellsThickness;
    std::vector< std::string > shellsMaterial;

    cRd.getVectorDouble("strgt.ShellsThickness", shellsThickness, nShells );
    cRd.getVectorString("strgt.ShellsMaterial", shellsMaterial, nShells);

    for ( int ishell=0; ishell<nShells; ++ishell ) {
      totThick+=shellsThickness[ishell];
    }

    G4VisAttributes* visAtt = new G4VisAttributes(true, G4Colour::Green() );
    visAtt->SetForceSolid(true);
    visAtt->SetForceAuxEdgeVisible (false);
    visAtt->SetVisibility(true);
    visAtt->SetDaughtersInvisible(false);

    G4Material* matMother = gmc::findMaterialOrThrow( cRd.getString("strgt.motherVolMat","G4_AIR") );
    G4Tubs *strgtShp       = new G4Tubs("StopTarget", 0,r0+0.001,totThick+0.001,0.0,360.0*CLHEP::degree);
    G4LogicalVolume *strgtLog = new G4LogicalVolume(strgtShp , matMother, "StopTargetMother",0,0,0);
//    strgtLog->SetVisAttributes(visAtt);

    if (geomType==0) {

      double shRelPos = -totThick;
      char shape[50], vol[50];

      for ( int ishell=0; ishell<nShells; ++ishell ) {
        sprintf(shape,"StopTarget-Shl%d",ishell);
        sprintf(vol,"StopTargetVol-Shl%d",ishell);

        shRelPos+=shellsThickness[ishell]*0.5;

        G4Material* shellMat = gmc::findMaterialOrThrow( shellsMaterial[ishell] );
        G4Tubs *strgtShelShp          = new G4Tubs(shape, 0,r0,shellsThickness[ishell]*0.5,0.0,360.0*CLHEP::degree);
        G4LogicalVolume *strgtShelLog = new G4LogicalVolume(strgtShelShp , shellMat, vol,0,0,0);
        strgtShelLog->SetVisAttributes(visAtt);
        G4PVPlacement *strgtShellVol  = new G4PVPlacement(0,  // no rotation
            G4ThreeVector(0,0,shRelPos),                      // at (x,y,z)
            strgtShelLog,                               // its logical volume
            vol,                                       // its name
            strgtLog,                                   // its mother  volume
            false,                                     // no boolean operations
            0,                                         // copy number
            checkOverlap);
        strgtShellVol->GetCopyNo(); //Just to remove compiling warning

        shRelPos+=shellsThickness[ishell]*0.5;
      }
    }

//    G4PVPlacement *strgtVol = new G4PVPlacement(new CLHEP::HepRotationY(phiangle*CLHEP::degree),  // no rotation
//        G4ThreeVector(0,0,0),                      // at (x,y,z)
    G4PVPlacement *strgtVol = new G4PVPlacement( HepGeom::Translate3D(0,0,0)*HepGeom::RotateY3D(phiangle*CLHEP::degree),
    	strgtLog,                                   // its logical volume
        "StopTargetMother",                          // its name
        motherLogV,                                // its mother  volume
        false,                                     // no boolean operations
        0,                                         // copy number
        checkOverlap);
    strgtVol->GetCopyNo(); //Just to remove compiling warning
  }

}
