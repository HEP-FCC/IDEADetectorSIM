// $Id$
// $

#ifndef G4GMC_GMCG4MEGTAR_HH_
#define G4GMC_GMCG4MEGTAR_HH_

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;
class G4Region;


const G4int    kDefaultTargetType        = 4;
const G4double kDefaultTargetSize[]      = {28.00*cm,
                                             7.98*cm,
                                             0.0140*cm };
const G4double kDefaultTargetAngle[]     = {15.00*degree,
                                             0.00*degree,
                                             0.00*degree};
const G4double kDefaultTargetPosition[]  = { 0.00*mm,
                                             0.00*mm,
                                             0.00*mm};

class GMCG4MegTAR {

public:

   virtual ~GMCG4MegTAR();

//private:
   GMCG4MegTAR();

public:
//   void Construct(G4LogicalVolume *parent);
   void construct(G4LogicalVolume* motherLogV=NULL);

protected:
   void SetTargetSize(G4ThreeVector&);
   void SetTargetAngle(G4ThreeVector&);
   void SetTargetPosition(G4ThreeVector&);

private:
   void ConstructMaterials();
   void CreateRegion();
//   void SetRegionCuts();

private:
//   G4int    idx;
   G4bool   materialConstructed;
   G4int    ITAR;
   G4double TRGT[3];
   G4double TARPOS[3];
   G4double TRGTSLA;
   G4double TRGTPHI;
   G4double TRGTPSI;

   // Materials for target
   G4Material *matarc;
   G4Material *mataroha;
   G4Material *matape;
   G4Material *matapp;
   G4Material *matamy;
   G4Material *matarsus;

   // visualizer for target
   G4VisAttributes *TRGTVisAtt;
   G4VisAttributes *TARSUPVisAtt;

   G4Region *tarRegion;
};

#endif /* G4GMC_GMCG4MEGTAR_HH_ */
