#include <GMCG4MegTAR.hh>

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4EllipticalTube.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4UserLimits.hh"
#include "G4AssemblyVolume.hh"
#include "G4Transform3D.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"

#include "G4PVReplica.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"


GMCG4MegTAR::~GMCG4MegTAR()
{
   // target
   delete TRGTVisAtt;
   delete TARSUPVisAtt;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

GMCG4MegTAR::GMCG4MegTAR(/*const char* n, GEMPhysicsList *p, G4int i*/) :
//:GEMAbsTargetModule(n)
//,idx(i)
//,physics(p)
materialConstructed(false)
,ITAR(kDefaultTargetType)
,TRGTSLA(kDefaultTargetAngle[0])
,TRGTPHI(kDefaultTargetAngle[1])
,TRGTPSI(kDefaultTargetAngle[2])
,matarc(0)
,mataroha(0)
,matape(0)
,matapp(0)
,matamy(0)
,matarsus(0)
,TRGTVisAtt(0)
,TARSUPVisAtt(0)
,tarRegion(0)
{
   TRGT[0]   = kDefaultTargetSize[0];
   TRGT[1]   = kDefaultTargetSize[1];
   TRGT[2]   = kDefaultTargetSize[2];
   TARPOS[0] = kDefaultTargetPosition[0];
   TARPOS[1] = kDefaultTargetPosition[1];
   TARPOS[2] = kDefaultTargetPosition[2];
//   eventAction    = new TAREventAction();
//   steppingAction = new TARSteppingAction();
//   static_cast<TAREventAction*>(eventAction)->SetIndex(idx);
//   static_cast<TARSteppingAction*>(steppingAction)->SetIndex(idx);
}

void GMCG4MegTAR::construct(G4LogicalVolume *parent)
{
   // All managed (deleted) by SDManager

   if (!materialConstructed) {
      ConstructMaterials();
      materialConstructed = true;
   }
   G4String name;
   G4ThreeVector trans;

   // Reset
   G4VPhysicalVolume* targetVolume = 0;

   // rotation matrix for no rotation
   G4RotationMatrix *norot = new G4RotationMatrix();

   // parameters for target
   const G4double PIBY2  = 90.*deg;
   const G4int ntarholes = 6;
   const G4int IRCIT = 0;
   // const G4int MTAR = 4;

   // target geometry and material
   // holes on the target
   const G4double TARHOLER = 0.5*cm;
   const G4double TARHOLEPOS[6][2] = { // target position of x/y
      { 6.65*cm,  0.000*cm},
      { 3.35*cm,  0.000*cm},
      { 0.00*cm,  1.475*cm},
      { 0.00*cm, -1.475*cm},
      {-3.35*cm,  0.000*cm},
      {-6.65*cm,  0.000*cm}} ;
   // target support
   const G4double TARSUPP1[2] = {0.650*cm, 0.525*cm};
   const G4double TARSUPP2[2] = {5.08*cm, 13.54*cm};
   const G4double TARSUPP3[3] = {0.5*cm, 1.5*cm, 0.25*cm};
   const G4double TARSUPP4[4] = {0.237*cm, 0.01*cm, 50.0*cm, -12.8*deg};
   // Rohacell insertion tube
   const G4double RCITGEO[4] = {5.0*cm, 5.2*cm, 61.0*cm, 81.0*cm}; // rmin, rmax, dz of tube, z of the center of tube

   G4double vptrgt[3], vptarg0[6], vphole[3];
   for (G4int i=0;i<3;i++) {
      vptrgt[i] = TRGT[i]/2.;
   }

   G4AssemblyVolume *TAR0Assembly;

   G4VSolid *TAR1Solid  = 0;
   G4VSolid *TAR2Solid  = 0;
   G4VSolid *TAR12Solid = 0;
   G4VSolid *TAR3Solid  = 0;
   G4VSolid *TAR4Solid  = 0;
   G4VSolid *TAR5Solid  = 0;
   G4VSolid *TGHLSolid  = 0;
   G4VSolid *RCITSolid  = 0;
   G4LogicalVolume *TAR12Logical = 0;
   G4LogicalVolume *TAR3Logical = 0;
   G4LogicalVolume *TAR4Logical = 0;
   G4LogicalVolume *TAR5Logical = 0;
   G4LogicalVolume *RCITLogical = 0;
   G4VSolid *TRGTBaseSolid = new G4EllipticalTube("TRGTBaseSolid", vptrgt[0], vptrgt[1], vptrgt[2]);
   G4LogicalVolume *TRGTLogical = 0;

   const G4double ETHETA = -(PIBY2 - TRGTSLA);
#if 0
   const G4double EPHI   =  (TRGTPHI + PIBY2);
   const G4double EPSI   = -(TRGTPSI + PIBY2);
   const G4double THX = acos(sin(EPHI)*sin(ETHETA));
   const G4double PHX =
      (TRGTPHI!=0.||TRGTPSI!=0.) ?
      atan2(cos(EPHI)*sin(EPSI)+sin(EPHI)*cos(EPSI)*cos(ETHETA),
            cos(EPHI)*cos(EPSI)-sin(EPSI)*sin(EPHI)*cos(ETHETA)) :
      0;
   const G4double THY = acos(cos(EPHI)*sin(ETHETA));
   const G4double PHY =
      (TRGTPHI!=0.||TRGTPSI!=0.) ?
      atan2(-sin(EPHI)*sin(EPSI)+cos(EPHI)*cos(EPSI)*cos(ETHETA),
            -sin(EPHI)*cos(EPSI)-sin(EPSI)*cos(EPHI)*cos(ETHETA)) :
      PIBY2;
   const G4double THZ = acos(cos(ETHETA));
   const G4double PHZ =
      (TRGTPSI!=0.) ?
      atan2(cos(EPSI), -sin(EPSI)) :
      0.;
#endif

   G4RotationMatrix *TRGTRot;
   if (ITAR<=3) { // target without support
      TRGTRot = new G4RotationMatrix();
      TRGTRot->rotateY(ETHETA);
      TRGTLogical  = new G4LogicalVolume(TRGTBaseSolid, matapp, "TRGTLogical", 0, 0, 0);
      targetVolume = new G4PVPlacement(TRGTRot,G4ThreeVector(TARPOS[0], TARPOS[1], TARPOS[2]),TRGTLogical,
                                       "TRGTPhys",parent,false,0);
   } else { // target with support
      // parameters for target support rohacell rods
      G4double yellip
         = ((1.-((TARSUPP2[0]+TARSUPP1[0])/(TRGT[0]/2.)))*(1.+((TARSUPP2[0]+TARSUPP1[0])/(TRGT[0]/2.))));
      if (yellip>=0.) {
         yellip = (TRGT[1]/2.)*sqrt(yellip);
      } else {
         yellip = 0.;
      }
      G4double dysuprod  = TARSUPP2[1]-yellip;
      G4double suprodang = atan2(TARSUPP2[0], dysuprod);

      // assembly volume
      TAR0Assembly = new G4AssemblyVolume();

      // target hole
      G4VSolid *TRGTSolid = TRGTBaseSolid;
      vphole[0] = 0.;
      vphole[1] = TARHOLER;
      vphole[2] = TRGT[2]; // not to share surface
      TGHLSolid = new G4Tubs("TGHLSolid", vphole[0], vphole[1], vphole[2], 0.*deg, 360.*deg);
      G4double holex, holey, holez;
      for (G4int ihole=0;ihole<ntarholes;ihole++) {
         holex = TARHOLEPOS[ihole][0];
         holey = TARHOLEPOS[ihole][1];
         holez = 0.;
         name = "TRGTSolid";
         if (ihole != ntarholes - 1) {
            name += ihole;
         }
         TRGTSolid = new G4SubtractionSolid(name, TRGTSolid,
                                            TGHLSolid, 0, G4ThreeVector(holex, holey, holez));
      }
      // target itself
      TRGTLogical = new G4LogicalVolume(TRGTSolid, matapp, "TRGTLogical", 0, 0, 0);
      trans.setX(0);
      trans.setY(0);
      trans.setZ(0);
      TAR0Assembly->AddPlacedVolume(TRGTLogical, trans, norot);

      // target elliptical rohacell frame
      vptarg0[0] = vptrgt[0];
      vptarg0[1] = vptrgt[1];
      vptarg0[2] = (TARSUPP1[1]-TRGT[2]/2.)/2.;
      TAR1Solid = new G4EllipticalTube("TAR1Solid", vptarg0[0], vptarg0[1], vptarg0[2]);
      vptarg0[0] = vptrgt[0]-TARSUPP1[0];
      vptarg0[1] = vptrgt[1]-TARSUPP1[0];
      vptarg0[2] = (TARSUPP1[1]-TRGT[2]/2.)/2.+0.001*cm;
      TAR2Solid = new G4EllipticalTube("TAR2Solid", vptarg0[0], vptarg0[1], vptarg0[2]);

      // TAR1 & TAR2 boolean Solid
      TAR12Solid = new G4SubtractionSolid("TAR12Solid", TAR1Solid, TAR2Solid);
      TAR12Logical = new G4LogicalVolume(TAR12Solid, mataroha, "TAR12Logical", 0, 0, 0);
      trans.setX(0);
      trans.setY(0);
      trans.setZ(+(vptarg0[2]+vptrgt[2]));
      TAR0Assembly->AddPlacedVolume(TAR12Logical, trans, norot);
      trans.setX(0);
      trans.setY(0);
      trans.setZ(-(vptarg0[2]+vptrgt[2]));
      TAR0Assembly->AddPlacedVolume(TAR12Logical, trans, norot);

      // target support rohacell rods
      G4RotationMatrix *rotTAR3[2];
      vptarg0[0] = TARSUPP1[0]/2.;
      vptarg0[1] = dysuprod/2.;
      vptarg0[2] = TARSUPP1[1];
      vptarg0[3] = -suprodang;
      vptarg0[4] = 0.;
      vptarg0[5] = 0.;
      G4double ysuprod = yellip + dysuprod/2.;
      G4VSolid *TAR3BaseSolid[2] = {
         new G4Trap("TAR3BaseSolid0",
                    vptarg0[0],
                    0, 0,
                    vptarg0[1], vptarg0[2], vptarg0[2], vptarg0[3],
                    vptarg0[1], vptarg0[2], vptarg0[2], vptarg0[3]),
         new G4Trap("TAR3BaseSolid1",
                    vptarg0[0] - 1*micrometer,// not to share surface,
                    0, 0,
                    vptarg0[1] - 1*micrometer,// not to share surface
                    vptarg0[2],
                    vptarg0[2],
                    vptarg0[3],
                    vptarg0[1] - 1*micrometer,// not to share surface
                    vptarg0[2],
                    vptarg0[2],
                    vptarg0[3]),
      };
      vptarg0[0] = vptrgt[0];
      vptarg0[1] = vptrgt[1];
      vptarg0[2] = (TARSUPP1[1]) * 2; // not to share faces
      G4VSolid *TAR3SubSolid = new G4EllipticalTube("TAR3SubSolid", vptarg0[0], vptarg0[1], vptarg0[2]);
      trans.setX(-TARSUPP2[0]/2);
      trans.setY(-ysuprod);
      trans.setZ(0);
      G4VSolid *TAR3SolidTmp[2] = {
         new G4SubtractionSolid("TAR3SolidTmp0", TAR3BaseSolid[0], TAR3SubSolid,
                                0, trans),
         new G4SubtractionSolid("TAR3SolidTmp1", TAR3BaseSolid[1], TAR3SubSolid,
                                0, trans),
      };
      rotTAR3[1] = new G4RotationMatrix();
      rotTAR3[1]->rotateY(+180.*deg);
      trans.setX(-TARSUPP2[0]);
      trans.setY(0);
      trans.setZ(0);
      TAR3Solid = new G4UnionSolid("TAR3Solid", TAR3SolidTmp[0], TAR3SolidTmp[1], rotTAR3[1], trans);
      TAR3Logical = new G4LogicalVolume(TAR3Solid, mataroha, "TAR3Logical", 0, 0, 0);

      rotTAR3[0] = new G4RotationMatrix();
      trans.setX(+TARSUPP2[0]/2);
      trans.setY(ysuprod);
      trans.setZ(0);
      TAR0Assembly->AddPlacedVolume(TAR3Logical, trans, rotTAR3[0]);

      // steel target support tubes
      G4double ybox = TARSUPP2[1] + TARSUPP3[1];

      vptarg0[0] = TARSUPP4[0] - TARSUPP4[1];
      vptarg0[1] = TARSUPP4[0];
      vptarg0[2] = TARSUPP4[2];
      TAR5Solid = new G4Tubs("TAR5Solid", vptarg0[0], vptarg0[1], vptarg0[2], 0.*deg, 360.*deg);
      TAR5Logical = new G4LogicalVolume(TAR5Solid, matarsus, "TAR5Logical", 0, 0, 0);

      G4RotationMatrix *rotTAR5;
      rotTAR5 = new G4RotationMatrix();
      rotTAR5->rotateY(-TARSUPP4[3]+PIBY2);
      G4double ytube;
      ytube = ybox - TARSUPP3[1]/3.;
      trans.setX(0);
      trans.setY(ytube);
      trans.setZ(0);
#if 0
      TAR0Assembly->AddPlacedVolume(TAR5Logical, trans, rotTAR5);
#endif
      ytube = ybox + TARSUPP3[1]/3.;
      trans.setX(0);
      trans.setY(ytube);
      trans.setZ(0);
#if 0
      TAR0Assembly->AddPlacedVolume(TAR5Logical, trans, rotTAR5);
#endif

      // plastic connecting steel support tubes and rohacell support rods
      // use mylar material until the correct will be known
      G4VSolid *TAR4BaseSolid = new G4Box("TAR4BaseSolid", TARSUPP3[0], TARSUPP3[1], TARSUPP3[2]);
      vptarg0[0] = 0;
      vptarg0[1] = TARSUPP4[0];
      vptarg0[2] = TARSUPP4[2];
      G4VSolid *TAR4SubSolid = new G4Tubs("TAR4SubSolid", vptarg0[0], vptarg0[1], vptarg0[2], 0.*deg, 360.*deg);
      G4RotationMatrix *rotTAR4Sub = new G4RotationMatrix();
      rotTAR4Sub->rotateY(+TARSUPP4[3]-PIBY2);
      ytube = ybox - TARSUPP3[1]/3.;
      trans.setX(0);
      trans.setY(ytube - ybox);
      trans.setZ(0);
      TAR4Solid = new G4SubtractionSolid("TAR4SolidTmp0", TAR4BaseSolid, TAR4SubSolid,
                                         rotTAR4Sub, trans);
      ytube = ybox + TARSUPP3[1]/3.;
      trans.setX(0);
      trans.setY(ytube - ybox);
      trans.setZ(0);
      TAR4Solid = new G4SubtractionSolid("TAR4SolidTmp0", TAR4Solid, TAR4SubSolid,
                                         rotTAR4Sub, trans);
      TAR4Logical = new G4LogicalVolume(TAR4Solid, matamy, "TAR4Logical", 0, 0, 0);
      trans.setX(0);
      trans.setY(ybox);
      trans.setZ(0);
#if 0
      TAR0Assembly->AddPlacedVolume(TAR4Logical, trans, norot);
#endif

      // Rohacell Insertion Tube for proton calibration
      if (ITAR!=3&&IRCIT==1) {
         G4double vprcit[3] = {RCITGEO[0], RCITGEO[1], RCITGEO[2]};
         G4double zrcit = RCITGEO[3];
         RCITSolid = new G4Tubs("RCITSolid", vprcit[0], vprcit[1], vprcit[2], 0.*deg, 360.*deg);
         RCITLogical = new G4LogicalVolume(RCITSolid, matarc, "RCITLogical", 0, 0, 0);
         new G4PVPlacement(0, G4ThreeVector(0., 0., zrcit), RCITLogical, "RCITPhys", parent,true, 0);
      }

      //////////////////////////
      // target mother volume //
      //////////////////////////
      TRGTRot = new G4RotationMatrix();
      TRGTRot->rotateY(-ETHETA);
      trans.setX(TARPOS[0]);
      trans.setY(TARPOS[1]);
      trans.setZ(TARPOS[2]);
      TAR0Assembly->MakeImprint(parent, trans, TRGTRot, 0);

      std::vector<G4VPhysicalVolume*>::iterator physVol = TAR0Assembly->GetVolumesIterator();
      unsigned int nPhysVol = TAR0Assembly->TotalImprintedVolumes();
      unsigned int iPhysVol;
      for (iPhysVol = 0; iPhysVol < nPhysVol; iPhysVol++) {
         if ((*physVol) &&  (*physVol)->GetName().contains("TRGTLogical")) {
            targetVolume = (*physVol);
            break;
         }
         ++physVol;
      }
      if (!targetVolume) {
         std::ostringstream o;
         o<<"Target volume not found";
         G4Exception(__func__, "", FatalException, o.str().c_str());
      }
   }
//   static_cast<TARSteppingAction*>(steppingAction)->SetTargetVolume(targetVolume);

   CreateRegion();
   TRGTLogical->SetRegion(tarRegion);
   tarRegion->AddRootLogicalVolume(TRGTLogical);

   // visualization attributes ------------------------------------------------
   TRGTVisAtt = new G4VisAttributes(G4Colour(1., 0., 0.));  // Red
   TRGTVisAtt->SetForceSolid(true);
   TRGTLogical->SetVisAttributes(TRGTVisAtt);

   TARSUPVisAtt = new G4VisAttributes(G4Colour(1.,1.,1.));
   if (TARSUPVisAtt) TARSUPVisAtt->SetForceSolid(true);
   if (TAR12Logical) TAR12Logical->SetVisAttributes(TARSUPVisAtt);
   if (TAR3Logical ) TAR3Logical->SetVisAttributes(TARSUPVisAtt);
   if (TAR4Logical ) TAR4Logical->SetVisAttributes(TARSUPVisAtt);
   if (TAR5Logical ) TAR5Logical->SetVisAttributes(TARSUPVisAtt);
   if (RCITLogical ) RCITLogical->SetVisAttributes(TARSUPVisAtt);

//   SetRegionCuts();

   // return the world physical volume ----------------------------------------

   //  G4cout << G4endl << "The geometrical tree defined are : " << G4endl << G4endl;
   //  DumpGeometricalTree(worldPhysical);
}

void GMCG4MegTAR::ConstructMaterials()
{
   G4double density;
   G4String name;
   G4String symbol;
   G4int nElem;

   // elements for mixtures and compounds
   G4Element *elH  = G4NistManager::Instance()->FindOrBuildElement("H");
   G4Element *elC  = G4NistManager::Instance()->FindOrBuildElement("C");
   G4Element *elN  = G4NistManager::Instance()->FindOrBuildElement("N");
   G4Element *elO  = G4NistManager::Instance()->FindOrBuildElement("O");
   G4Element *elCr = G4NistManager::Instance()->FindOrBuildElement("Cr");
   G4Element *elFe = G4NistManager::Instance()->FindOrBuildElement("Fe");
   G4Element *elNi = G4NistManager::Instance()->FindOrBuildElement("Ni");

   // Rohacell51 (polymethacrylimide hard foam, H13-C9-N1-O2)
   density = 0.05*g/cm3;
   matarc = new G4Material(name="Rohacell51", density, nElem=4);
   matarc->AddElement(elH, 13);
   matarc->AddElement(elC, 9);
   matarc->AddElement(elN, 1);
   matarc->AddElement(elO, 2);

   /////////////////////////////
   // Target Materials        //
   /////////////////////////////

   // Polyethylene n-(CH2=CH2)
#if 1
   density = 0.922*g/cm3;
   matape = new G4Material("PolyethyleneTAR", density, nElem=2);
   matape->AddElement(elH, 2);
   matape->AddElement(elC, 1);
#else
   matape = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE");
#endif

   // Rohacell (polymethacrylimide- (PMI-) hard foam, H11-C8-N1-O2)
   density = 0.052*g/cm3;
   mataroha = new G4Material(name="Rohacell", density, nElem=4);
   mataroha->AddElement(elH, 11);
   mataroha->AddElement(elC, 8);
   mataroha->AddElement(elN, 1);
   mataroha->AddElement(elO, 2);

   // Polyethylene/polyester sandwitch n-(CH2=CH2)
   density = 0.895*g/cm3;
   matapp = new G4Material("PolySandwitch", density, nElem=2);
   matapp->AddElement(elH, 2);
   matapp->AddElement(elC, 1);

   // Maylar (polyethylene terephthlate, C5H4O2)
   density = 1.395*g/cm3;
   matamy = new G4Material("Maylar", density, nElem=3);
   matamy->AddElement(elH, 4);
   matamy->AddElement(elC, 5);
   matamy->AddElement(elO, 2);

   // Composition of SUS-310 (Fe and Cr and Ni)
#if 1
   density = 7.98*g/cm3;
   matarsus = new G4Material("SUS-301", density, nElem=3);
   matarsus->AddElement(elCr, 0.25);
   matarsus->AddElement(elFe, 0.55);
   matarsus->AddElement(elNi, 0.20);
#else
   matarsus = G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
#endif
}

////_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
//
//GEMAbsUserEventInformation* GMCG4MegTAR::MakeEventInformation(const G4Event* /*anEvent*/)
//{
//   return new TARUserEventInformation();
//}
//
////_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
//
//GEMAbsUserTrackInformation* GMCG4MegTAR::MakeTrackInformation(const G4Track* /*aTrack*/)
//{
//   return 0;
//}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

void GMCG4MegTAR::SetTargetSize(G4ThreeVector &v)
{
   TRGT[0] = v.x();
   TRGT[1] = v.y();
   TRGT[2] = v.z();
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void GMCG4MegTAR::SetTargetAngle(G4ThreeVector &v)
{
   TRGTSLA = v.x();
   TRGTPHI = v.y();
   TRGTPSI = v.z();
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void GMCG4MegTAR::SetTargetPosition(G4ThreeVector &v)
{
   TARPOS[0] = v.x();
   TARPOS[1] = v.y();
   TARPOS[2] = v.z();
}

////_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
//void GMCG4MegTAR::ReadDatabase(ROMESQLDataBase *db, G4int run)
//{
//   ROMEStr2DArray *values = new ROMEStr2DArray(1, 1);
//   ROMEString path;
//
//   G4cout<<"___ TAR reading from DB ___"<<G4endl;
//
//   path.SetFormatted("/RunCatalog[id=%d]/MEGConf_id", run);
//   db->Read(values, path, run, 0);
//   G4int megid = strtol(values->At(0, 0).Data(), 0, 10);
//
//   path.SetFormatted("/MEGConf[id=%d]/TGTGeometry_id", megid);
//   db->Read(values, path, run, 0);
//   G4int tarid = strtol(values->At(0, 0).Data(), 0, 10);
//   G4cout<<"TGTGeometry_id = "<<tarid<<G4endl;
//
//   path.SetFormatted("/TGTGeometry[id=%d]/TargetDescriptionId", tarid);
//   db->Read(values, path, run, 0);
//   ITAR = strtol(values->At(0, 0).Data(), 0, 10);
//   G4cout<<"ITAR = "<<ITAR<<G4endl;
//
//   G4cout<<"___________________________"<<G4endl;
//
//   delete values;
//}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

void GMCG4MegTAR::CreateRegion()
{
   if (tarRegion) {
      delete tarRegion;
   }
   tarRegion = new G4Region("TARRegion");
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

//void GMCG4MegTAR::SetRegionCuts()
//{
//   G4ProductionCuts *cuts = new G4ProductionCuts;
//   cuts->SetProductionCut(physics->GetDefaultCutValue(), G4ProductionCuts::GetIndex("gamma"));
//   cuts->SetProductionCut(physics->GetDefaultCutValue(), G4ProductionCuts::GetIndex("e-"));
//#if 1
//   cuts->SetProductionCut(physics->GetDefaultCutValue(), G4ProductionCuts::GetIndex("e+"));
//#else
//   cuts->SetProductionCut(1*um, G4ProductionCuts::GetIndex("e+"));
//#endif
//   cuts->SetProductionCut(physics->GetDefaultCutValue(), G4ProductionCuts::GetIndex("proton"));
//   tarRegion->SetProductionCuts(cuts);
//}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
