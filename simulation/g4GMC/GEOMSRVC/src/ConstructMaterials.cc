/*
 * GMCG4ConstructMaterials.cc
 *
 *  Created on: Mar 29, 2017
 *      Author: tassiell
 */

#include "ConstructMaterials.hh"

#include <GeomService.hh>

//c++ includes
#include <iostream>

// G4 includes
#include "globals.hh"
#include <G4Material.hh>
#include "G4NistManager.hh"
#include "findMaterialOrThrow.hh"

// Value used to request that all specific materials be made.
const std::string DoAllValue = "DoAll";

namespace gmc {

ConstructMaterials::ConstructMaterials() {

  GeomService *geoms = GeomService::Instance();
  cRd = &geoms->getConfig();

}

void ConstructMaterials::construct() {

  contructGeneralMaterial();

  if (cRd->getBool("hasCDCH",false)) contructCDCHMaterial();
}

// Decide if we need to build this material.
// If additional tests are required, call them from within this method.
CheckedG4String ConstructMaterials::isNeeded( std::vector<std::string> const& V, std::string const& s){

  // Default return value is not to build it.
  CheckedG4String val(false,s);

  // Throw if the material already exists.
  uniqueMaterialOrThrow(val.name);

  // Is this material requested explicitly?
  val.doit = isRequested( V, s );

  // Is this material requested implicitly?
  if ( !val.doit ) val.doit = isRequested(V, DoAllValue);

  return val;

}

// Return true if the requested string is present in the container.
// The match must be exact.
bool ConstructMaterials::isRequested( std::vector<std::string> const& V, std::string const& s){
  for ( std::vector<std::string>::const_iterator i=V.begin(), e=V.end();
      i != e; ++i ){
    if ( *i == s ) return true;
  }
  return false;
}

// Check to see if the named material already exists.
void ConstructMaterials::uniqueMaterialOrThrow( G4String const& name){
  if ( G4Material::GetMaterial(name,false) != 0 ){
    G4Exception("GEOM","Fatal error in Argument",
        FatalErrorInArgument,
        G4String("GMCG4ConstructMaterial: The requested material is already defined: " + name));
  }
}

// Wrapper around FindOrBuildElement.
// This is protection against spelling mistakes in the name.
G4Element* ConstructMaterials::getElementOrThrow( G4String const& name){

  // G4 manager for elements and materials.
  G4NistManager* nistMan = G4NistManager::Instance();

  G4Element* answer = nistMan->FindOrBuildElement(name,true);

  // Throw if we could not find a requested element.
  if ( !answer ){
    G4Exception("GEOM","Fatal error in Argument",
        FatalErrorInArgument,
        G4String("ConstructMaterials::constructMaterials(): Could not load predefined G4 element named: " + name));
  }

  return answer;
}

void ConstructMaterials::doGasHe(std::string matName, double pressure, double temperature){

  G4int nel;
  G4double density   = 0.000166 *CLHEP::g/CLHEP::cm3;

  density *= pressure/(1.0*CLHEP::atmosphere);

  G4Material *GasMix = new G4Material( matName, density, nel=1,
      kStateGas, temperature, pressure);

  G4Element* He = getElementOrThrow("He");
  GasMix->AddElement(He, 1.0 );

}


void ConstructMaterials::doGasHeIsobMix(double frcHe, std::string matName, double pressure, double temperature){

  G4double density;
  G4int nel;

  G4double densityHe   = 0.000166 *CLHEP::g/CLHEP::cm3;
  G4double densityIsoB = 0.00249  *CLHEP::g/CLHEP::cm3;
  G4double fractionHe  = frcHe*CLHEP::perCent;

  density = fractionHe*densityHe + (1.0-fractionHe)*densityIsoB;
  density *= pressure/(1.0*CLHEP::atmosphere);

  G4Material *GasMix = new G4Material( matName, density, nel=3,
      kStateGas, temperature, pressure);

  G4Element* He = getElementOrThrow("He");
  G4Element* C  = getElementOrThrow("C");
  G4Element* H  = getElementOrThrow("H");

  G4double atomicWeight_He =  4.002602 *CLHEP::g/CLHEP::mole;
  G4double atomicWeight_C  = 12.0107   *CLHEP::g/CLHEP::mole;
  G4double atomicWeight_H  =  1.00794  *CLHEP::g/CLHEP::mole;
  G4double pwHe = fractionHe*atomicWeight_He;
  G4double pwC  = (1.0-fractionHe) *  4.0*atomicWeight_C;
  G4double pwH  = (1.0-fractionHe) * 10.0*atomicWeight_H;
  G4double atomicWeightMix = pwHe + pwC + pwH ;

  pwHe/=atomicWeightMix;
  pwH/=atomicWeightMix;
  GasMix->AddElement(He, pwHe );
  GasMix->AddElement(H , pwH  );
  GasMix->AddElement(C , 1.0-pwHe-pwH  );

}

void ConstructMaterials::doGasHeCF4Mix(double frcHe, std::string matName, double pressure, double temperature){

  G4double density;
  G4int nel;

  G4double densityHe   = 0.000166 *CLHEP::g/CLHEP::cm3;
  G4double densityCF4  = 0.003780 *CLHEP::g/CLHEP::cm3;
  G4double fractionHe  = frcHe*CLHEP::perCent;

  density = fractionHe*densityHe + (1.0-fractionHe)*densityCF4;
  density *= pressure/(1.0*CLHEP::atmosphere);

  G4Material *GasMix = new G4Material( matName, density, nel=3,
      kStateGas, temperature, pressure);

  G4Element* He = getElementOrThrow("He");
  G4Element* C  = getElementOrThrow("C");
  G4Element* F  = getElementOrThrow("F");

  G4double atomicWeight_He =  4.002602 *CLHEP::g/CLHEP::mole;
  G4double atomicWeight_C  = 12.0107   *CLHEP::g/CLHEP::mole;
  G4double atomicWeight_F  = 18.9984032 *CLHEP::g/CLHEP::mole;
  G4double pwHe = fractionHe*atomicWeight_He;
  G4double pwC  = (1.0-fractionHe) *  4.0*atomicWeight_C;
  G4double pwF  = (1.0-fractionHe) *  4.0*atomicWeight_F;
  G4double atomicWeightMix = pwHe + pwC + pwF ;

  pwHe/=atomicWeightMix;
  pwF/=atomicWeightMix;
  GasMix->AddElement(He, pwHe );
  GasMix->AddElement(F , pwF  );
  GasMix->AddElement(C , 1.0-pwHe-pwF  );

}

void ConstructMaterials::contructGeneralMaterial() {

  // List of requested specific materials from the config file.
  std::vector<std::string> materialsToLoad;
  cRd->getVectorString("gmc.materials",materialsToLoad);

  CheckedG4String mat = isNeeded(materialsToLoad, "GasHe");
  if ( mat.doit ){
    doGasHe(mat.name);
  }

  mat = isNeeded(materialsToLoad, "GasHe_90Isob_10" );
  if ( mat.doit ) {
    doGasHeIsobMix(90.0,mat.name);
  }

  mat = isNeeded(materialsToLoad, "GasHe_85Isob_15" );
  if ( mat.doit ) {
    doGasHeIsobMix(85.0,mat.name);
  }

  mat = isNeeded(materialsToLoad, "Kapton");
  if ( mat.doit ){
    //
    // Kapton: from NIST: physics.nist.gov/cgi-bin/Star/compos.pl?matno=179
    //
    G4Material* Kapton = new G4Material(mat.name, 1.42*CLHEP::g/CLHEP::cm3, 4);
    Kapton->AddElement( getElementOrThrow("H"), 0.026362);
    Kapton->AddElement( getElementOrThrow("C"), 0.691133);
    Kapton->AddElement( getElementOrThrow("N"), 0.073270);
    Kapton->AddElement( getElementOrThrow("O"), 0.209235);
  }

}

void ConstructMaterials::contructCDCHMaterial()
{

  // List of requested specific materials from the config file.
  std::vector<std::string> materialsToLoad;
  cRd->getVectorString("cdch.materials",materialsToLoad);

  CheckedG4String mat = isNeeded(materialsToLoad, "GasHe_90CF4_10");
  if ( mat.doit ){
    doGasHeCF4Mix(90.0,mat.name);
  }

  mat = isNeeded(materialsToLoad, "CarbonFiber_resin");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material* CFresin =
      new G4Material(mat.name, density = 1.1*CLHEP::g/CLHEP::cm3, nel=3);
    G4int natoms;
    CFresin->AddElement(getElementOrThrow("H"),natoms=5);
    CFresin->AddElement(getElementOrThrow("C"),natoms=5);
    CFresin->AddElement(getElementOrThrow("O"),natoms=2);
  }

  mat = isNeeded(materialsToLoad, "CarbonFiber");
  if ( mat.doit ){
    G4double density, fiberFrac=46.0*CLHEP::perCent;
    G4int nel, natoms;
    G4Material* CFresin = findMaterialOrThrow("CarbonFiber_resin");
    G4Material* CFibers = new G4Material("CFibers",density = 1.8*CLHEP::g/CLHEP::cm3,nel=1);
    CFibers->AddElement(getElementOrThrow("C"),natoms=1);

    density = fiberFrac*CFibers->GetDensity()+(1.0-fiberFrac)*CFresin->GetDensity();
    G4Material* CarbonFiber =
      new G4Material(mat.name, density /*= 1.384*CLHEP::g/CLHEP::cm3*/, nel=2);
    CarbonFiber->AddMaterial(CFibers, fiberFrac );
    CarbonFiber->AddMaterial(CFresin, (1.0-fiberFrac) );
  }

  // http://personalpages.to.infn.it/~tosello/EngMeet/ITSmat/SDD/Epotek-301-1.html
  // C_19_H_20_O_4

  mat = isNeeded(materialsToLoad, "C_19_H_20_O_4");
  if ( mat.doit ){
    G4Material* C_19_H_20_O_4 = new G4Material( mat.name, 1.16*CLHEP::g/CLHEP::cm3, 3);
    C_19_H_20_O_4->AddElement( getElementOrThrow("C"), 19);
    C_19_H_20_O_4->AddElement( getElementOrThrow("H"), 20);
    C_19_H_20_O_4->AddElement( getElementOrThrow("O"),  4);
  }

  // C_10_H_18_O_4

  mat = isNeeded(materialsToLoad, "C_10_H_18_O_4");
  if ( mat.doit ){
    G4Material* C_10_H_18_O_4 = new G4Material( mat.name, 1.10*CLHEP::g/CLHEP::cm3, 3);
    C_10_H_18_O_4->AddElement( getElementOrThrow("C"), 10);
    C_10_H_18_O_4->AddElement( getElementOrThrow("H"), 18);
    C_10_H_18_O_4->AddElement( getElementOrThrow("O"),  4);

  }

  // C_9_H_22_N_2

  mat = isNeeded(materialsToLoad, "C_9_H_22_N_2");
  if ( mat.doit ){
    G4Material* C_9_H_22_N_2 = new G4Material( mat.name, 0.865*CLHEP::g/CLHEP::cm3, 3);
    C_9_H_22_N_2->AddElement( getElementOrThrow("C"),  9);
    C_9_H_22_N_2->AddElement( getElementOrThrow("H"), 22);
    C_9_H_22_N_2->AddElement( getElementOrThrow("N"),  2);

  }

  // http://personalpages.to.infn.it/~tosello/EngMeet/ITSmat/SDD/Epotek-301-1.html
  mat = isNeeded(materialsToLoad, "Epotek301");
  if ( mat.doit ){
    G4Material* Epotek301 = new G4Material( mat.name, 1.19*CLHEP::g/CLHEP::cm3, 3);
    Epotek301->AddMaterial(findMaterialOrThrow("C_19_H_20_O_4"), 0.56);
    Epotek301->AddMaterial(findMaterialOrThrow("C_10_H_18_O_4"), 0.24);
    Epotek301->AddMaterial(findMaterialOrThrow("C_9_H_22_N_2"),  0.20);
  }

 // http://personalpages.to.infn.it/~tosello/EngMeet/ITSmat/SDD/E_glass.html
  mat = isNeeded(materialsToLoad, "EGlass");
  if ( mat.doit ){
    G4Material* EGlass = new G4Material (mat.name, 2.61*CLHEP::g/CLHEP::cm3, 10);
    EGlass->AddMaterial(findMaterialOrThrow("G4_SILICON_DIOXIDE"), 0.54);
    EGlass->AddMaterial(findMaterialOrThrow("G4_CALCIUM_OXIDE"), 0.19 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_ALUMINUM_OXIDE"), 0.13 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_MAGNESIUM_OXIDE"), 0.025 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_BORON_OXIDE"), 0.075 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_TITANIUM_DIOXIDE"), 0.008 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_SODIUM_MONOXIDE"), 0.01 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_POTASSIUM_OXIDE"), 0.01 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_FERRIC_OXIDE"), 0.005 );
    EGlass->AddMaterial(findMaterialOrThrow("G4_F"), 0.007 );
  }

  //G10-FR4 used for printed board
  // G10 http://personalpages.to.infn.it/~tosello/EngMeet/ITSmat/SDD/SDD_G10FR4.html
  // http://pdg.lbl.gov/2002/atomicrpp.pdf
  mat = isNeeded(materialsToLoad, "G10_FR4");
  if ( mat.doit ) {
    G4double density;

    G4Material* G10_FR4 =
      new G4Material(mat.name, density = 1.8*CLHEP::g/CLHEP::cm3, 2);
    G10_FR4->AddMaterial(findMaterialOrThrow("EGlass"), 0.60);
    G10_FR4->AddMaterial(findMaterialOrThrow("Epotek301"), 0.40);
  }

  mat = isNeeded(materialsToLoad, "PolypropyleneFoam");
  if ( mat.doit ){
    //Polypropylene (CH3)
    G4double density;
    G4int nel;
    G4Material *Polypropylene = new G4Material(mat.name, density = 0.04*CLHEP::g/CLHEP::cm3, nel=2);
    G4Element* H  = getElementOrThrow("H");
    G4Element* C  = getElementOrThrow("C");
    Polypropylene->AddElement(H, 3 );
    Polypropylene->AddElement(C, 1 );
  }

  mat = isNeeded(materialsToLoad, "BeFoam_018");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material *BeFoam_018 = new G4Material(mat.name, density = 0.018*CLHEP::g/CLHEP::cm3, nel=1);
    G4Element* Be  = getElementOrThrow("Be");
    BeFoam_018->AddElement(Be, 100.0*CLHEP::perCent );
  }

  mat = isNeeded(materialsToLoad, "CFoam_332");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material *CFoam = new G4Material(mat.name, density = 0.332*CLHEP::g/CLHEP::cm3, nel=1);
    G4Element* C  = getElementOrThrow("C");
    CFoam->AddElement(C, 100.0*CLHEP::perCent );
  }

  mat = isNeeded(materialsToLoad, "CFoam_166");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material *CFoam = new G4Material(mat.name, density = 0.166*CLHEP::g/CLHEP::cm3, nel=1);
    G4Element* C  = getElementOrThrow("C");
    CFoam->AddElement(C, 100.0*CLHEP::perCent );
  }

  mat = isNeeded(materialsToLoad, "CFoam_080");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material *CFoam = new G4Material(mat.name, density = 0.080*CLHEP::g/CLHEP::cm3, nel=1);
    G4Element* C  = getElementOrThrow("C");
    CFoam->AddElement(C, 100.0*CLHEP::perCent );
  }

  mat = isNeeded(materialsToLoad, "CFoam");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material *CFoam = new G4Material(mat.name, density = 0.030*CLHEP::g/CLHEP::cm3, nel=1);
    G4Element* C  = getElementOrThrow("C");
    CFoam->AddElement(C, 100.0*CLHEP::perCent );
  }

  mat = isNeeded(materialsToLoad, "KptFoam_030");
  if ( mat.doit ){
    G4double density;
    G4int nel;
    G4Material *KptFoam = new G4Material(mat.name, density = 0.030*CLHEP::g/CLHEP::cm3, nel=1);
    KptFoam->AddMaterial(findMaterialOrThrow("G4_KAPTON"), 100.0*CLHEP::perCent );
  }

}

}
