/*
 * GMCG4BeamPipeBuilder.cc
 *
 *  Created on: Mar 22, 2017
 *      Author: tassiell
 */

#include <GMCG4BeamPipeBuilder.hh>

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

void GMCG4BeamPipeBuilder::constructPipe(G4LogicalVolume* motherLogV) {

  if (motherLogV==NULL) G4cerr<<"GMCG4BeamPipeBuilder mother volume not defined!"<<G4endl;

  G4bool  checkOverlap=true;

  GeomService *geoms = GeomService::Instance();
  const crd::SimpleConfig &cRd = geoms->getConfig();


  checkOverlap = cRd.getBool("g4.doSurfaceCheck",false);

  bool isExternal     = false;
  std::string extFile        = cRd.getString("bmpp.extFile");
  if ( extFile.size()>1 && ( extFile.find_last_of(".gdml") || extFile.find_last_of(".GDML") )!=0 ) isExternal = true;

  if (isExternal) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"This GDML file option is temporarily disabled\n";
    e.error();
  } else {
    int geomType       = cRd.getInt("bmpp.geomType");
    double r0          = cRd.getDouble("bmpp.r0");
    double halfLength  = cRd.getDouble("bmpp.halfLength");

    double totThick = 0.0;

    int nShells        = cRd.getInt("bmpp.nShells");

    std::vector<double> shellsThickness;
    std::vector< std::string > shellsMaterial;

    cRd.getVectorDouble("bmpp.ShellsThickness", shellsThickness, nShells );
    cRd.getVectorString("bmpp.ShellsMaterial", shellsMaterial, nShells);

    for ( int ishell=0; ishell<nShells; ++ishell ) {
      totThick+=shellsThickness[ishell];
    }

    G4VisAttributes* visAtt = new G4VisAttributes(true, G4Colour::Grey() );
    visAtt->SetForceSolid(true);
    visAtt->SetForceAuxEdgeVisible (false);
    visAtt->SetVisibility(true);
    visAtt->SetDaughtersInvisible(false);

    G4Material* matMother = gmc::findMaterialOrThrow( cRd.getString("bmpp.motherVolMat","G4_AIR") );
    G4Tubs *bmppShp       = new G4Tubs("BeamPipe", 0,r0+totThick+0.001,halfLength+0.001,0.0,360.0*CLHEP::degree);
    G4LogicalVolume *bmppLog = new G4LogicalVolume(bmppShp , matMother, "BeamPipeMother",0,0,0);
//    bmppLog->SetVisAttributes(visAtt);

    G4Material* matVac = gmc::findMaterialOrThrow( "G4_Galactic" );
    G4Tubs *bmppVacShp          = new G4Tubs("BeamPipeVac", 0,r0,halfLength,0.0,360.0*CLHEP::degree);
    G4LogicalVolume *bmppVacLog = new G4LogicalVolume(bmppVacShp , matVac, "BeamPipeVacVol",0,0,0);
    G4PVPlacement *bmppVacVol = new G4PVPlacement(0,  // no rotation
        G4ThreeVector(0,0,0),                         // at (x,y,z)
        bmppVacLog,                                   // its logical volume
        "BeamPipeVacVol",                             // its name
        bmppLog,                                      // its mother  volume
        false,                                        // no boolean operations
        0,                                            // copy number
        checkOverlap);
    bmppVacVol->GetCopyNo(); //Just to remove compiling warning

    if (geomType==0) {

      double shRin = r0;
      char shape[50], vol[50];

      for ( int ishell=0; ishell<nShells; ++ishell ) {
        sprintf(shape,"BeamPipe-Shl%d",ishell);
        sprintf(vol,"BeamPipeVol-Shl%d",ishell);

        G4Material* shellMat = gmc::findMaterialOrThrow( shellsMaterial[ishell] );
        G4Tubs *bmppShelShp          = new G4Tubs(shape, shRin,shRin+shellsThickness[ishell],halfLength,0.0,360.0*CLHEP::degree);
        G4LogicalVolume *bmppShelLog = new G4LogicalVolume(bmppShelShp , shellMat, vol,0,0,0);
        bmppShelLog->SetVisAttributes(visAtt);
        G4PVPlacement *bmppShellVol  = new G4PVPlacement(0,  // no rotation
            G4ThreeVector(0,0,0),                      // at (x,y,z)
            bmppShelLog,                               // its logical volume
            vol,                                       // its name
            bmppLog,                                   // its mother  volume
            false,                                     // no boolean operations
            0,                                         // copy number
            checkOverlap);
        bmppShellVol->GetCopyNo(); //Just to remove compiling warning

        shRin+=shellsThickness[ishell];
      }
    }

    G4PVPlacement *bmppVol = new G4PVPlacement(0,  // no rotation
        G4ThreeVector(0,0,0),                      // at (x,y,z)
        bmppLog,                                   // its logical volume
        "BeamPipeMother",                          // its name
        motherLogV,                                // its mother  volume
        false,                                     // no boolean operations
        0,                                         // copy number
        checkOverlap);
    bmppVol->GetCopyNo(); //Just to remove compiling warning
  }

}
