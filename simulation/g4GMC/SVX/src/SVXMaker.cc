// SVX geometry maker
//
// Original author G. Tassielli
//

#include "SVXMaker.hh"
//#include <utilFunctions.hh>
#include "manageException.hh"

#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"

#include "ROGeometryHandle.hh"
#include "SVXtracker.hh"
#include "Layer.hh"

#include "SimpleConfig.hh"
//#include "messagefacility/MessageLogger/MessageLogger.h"

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>

#ifndef __CINT__

using CLHEP::Hep3Vector;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

using namespace std;

namespace svx {

//void cellPrinter( const Cell& s){
//        cout << s.Id() << endl;
//}
//
////void cellPrinter2( const Cell* s, int& i){
////        cout << s->Id() <<  " | "
////                        << s->hack << " "
////                        << ++i << endl;
////}
////
////void cellHacker( Cell* s, int& i){
////        s->hack = 2;
////}
//
//void layerPrinter( const SuperLayer& l){
//        cout << "    Layer: " << l.Id() << endl;
//}
//
// Constructor that gets information from the config file instead of
// from arguments.
SVXMaker::SVXMaker( crd::SimpleConfig const& config):
                                                    _center(){

        _isExternal     = false;
        _extFile        = config.getString("svx.extFile");
        if ( _extFile.size()>1 && ( _extFile.find_last_of(".gdml") || _extFile.find_last_of(".GDML") )!=0 ) _isExternal = true;

        _z0             = config.getDouble("svx.z0");
        _geomType       = config.getInt("svx.geomType");

        _r0=1.0e+6;
        _halfLength=0.0;
        _rOut=0.0;

        loadBarrelTracker(config);
        loadForwardTracker(config);

        cout<<"------------------- SVX -------------------"<<endl;
        cout<<"r0 "<<_r0<<" rOut "<<_rOut<<" leng "<<2.0*_halfLength<<endl;
        cout<<"nLayers "<<_nLayers<<endl;
        unsigned long totNumRO = 0;
        for (int il=0; il<_nLayers; ++il) {
          cout<<"Ly: "<<il<<endl;
          cout<<"InR "<<_LayersInnerRad[il]<<" Thick "<<_LaddersThickness[il]<<endl;
          cout<<"nRoX "<<_nROsFstSdPerLadder[il]<<" nRoY "<<_nROsSndSdPerLadder[il]<<" nRo "<<_nROsPerLadder[il]<<endl;
          totNumRO +=  _nROsPerLadder[il]*_nLaddersPerSector[il]*_nPhiSectors[il];
        }
        cout<<"nFwdLayers "<<_nFwdLayers<<endl;
        for (int il=0; il<_nFwdLayers; ++il) {
          cout<<"Ly: "<<_nLayers+il<<endl;
          cout<<"InR "<<_LayersInnerRad_fwd[il]<<" Thick "<<_LaddersThickness_fwd[il]<<endl;
          for( int ilad=0; ilad<_nLaddersPerSector_fwd[il]; ++ilad ){
            cout<<"Lad: "<<ilad<<endl;
            cout<<"nRoX "<<_nROsFstSdShrtPerLadder_fwd[il][ilad]<<" nRoX2 "<<_nROsFstSdLngPerLadder_fwd[il][ilad]<<" nRoY "<<_nROsSndSdPerLadder_fwd[il][ilad]<<" nRo "<<_nROsPerLadder_fwd[il][ilad]<<endl;
            totNumRO +=  _nROsPerLadder_fwd[il][ilad]*_nPhiSectors_fwd[il];
          }
        }
/*        for (int il=0; il<_nLayers+2*_nFwdLayers; ++il) {
          Layer* tLy = _lst->getLayer(il);
          cout<<"InR "<<tLy->getDetail()->InnerRadius()<<" Thick "<<2.0*tLy->getDetail()->halfLength()<<endl;
          if (tLy->getLayerZone()==Layer::forward){
            for (int iSec=0; iSec<tLy->nLaddersPerSector(); ++iSec){
              cout<<"Sec: "<<iSec<<endl;
              boost::shared_ptr<Ladder> tLd = tLy->getLadder(iSec*tLy->nPhiSectors());
              cout<<"nRoX "<<tLd->nReadOutsFstSd()<<"nRoX2 "<<tLd->nReadOutsFstSd2()<<" nRoY "<<tLd->nReadOutsSndSd()<<" nRo "<<tLd->nReadOuts()<<endl;
              totNumRO += tLd->nReadOuts()*tLy->nPhiSectors();
            }
          }
          else {
            boost::shared_ptr<Ladder> tLd = tLy->getLadder(0);
            cout<<"nRoX "<<tLd->nReadOutsFstSd()<<" nRoY "<<tLd->nReadOutsSndSd()<<" nRo "<<tLd->nReadOuts()<<endl;
            totNumRO += tLd->nReadOuts()*tLy->nLaddersPerSector()*tLy->nPhiSectors();
          }
        }*/

        cout<<"Tot Numb of Channels "<<totNumRO<<endl;

//        _r0-=1*CLHEP::mm;
//        _rOut+=1*CLHEP::mm;
//        _halfLength+=1*CLHEP::mm;


        // Do the real work.
        Build( );
}


SVXMaker::~SVXMaker (){}

void SVXMaker::loadBarrelTracker( crd::SimpleConfig const& config ){

  _nLayers        = config.getInt("svx.Brl.nLayers");
  if (_nLayers==0) {
    std::cout<<"SVX: no Barrel layers present!"<<std::endl;
    return;
  }

  config.getVectorDouble("svx.Brl.LayersInRad", _LayersInnerRad, _nLayers);
  config.getVectorInt("svx.Brl.nPhiSectors", _nPhiSectors, _nLayers);
  config.getVectorInt("svx.Brl.nLaddersPerSector",_nLaddersPerSector,_nLayers);
  config.getVectorDouble("svx.Brl.LaddersHalfLength",_LaddersHalfLength,_nLayers);
  config.getVectorDouble("svx.Brl.LaddersThickness",_LaddersThickness,_nLayers);
  config.getVectorInt("svx.Brl.ROTypes",_ROTypes,_nLayers);
  config.getVectorDouble("svx.Brl.ROfirstSideDim",_ROfirstSideDim,_nLayers);
  config.getVectorDouble("svx.Brl.ROfirstSideInsul",_ROfirstSideInsul,_nLayers);
  config.getVectorDouble("svx.Brl.ROSecondSideDim",_ROSecondSideDim,_nLayers);
  config.getVectorDouble("svx.Brl.ROSecondSideInsul",_ROSecondSideInsul,_nLayers);
  config.getVectorDouble("svx.Brl.ROangle",_ROangle,_nLayers);

  char tmpVarName[50];

  for (int il=0; il<_nLayers; ++il) {

    _phiAngles.push_back( CLHEP::twopi/((double)_nPhiSectors[il]) );
    if (_LayersInnerRad[il]<_r0) { _r0=_LayersInnerRad[il]; }
    double il_rOut=(_LayersInnerRad[il]+_LaddersThickness[il])/cos(0.5*_phiAngles[il]);
    if (il_rOut>_rOut) { _rOut=il_rOut; }
    if (_nLaddersPerSector[il]*_LaddersHalfLength[il]>_halfLength) { _halfLength=_nLaddersPerSector[il]*_LaddersHalfLength[il]; }

    //Ladders Parameters
    sprintf(tmpVarName,"svx.Brl.l%d.ld.nShells",il+1);
    const std::string nShlVarName(tmpVarName);
    _LaddersNmShells.push_back( config.getInt(nShlVarName) );

    sprintf(tmpVarName,"svx.Brl.l%d.ld.ShellsMaterial",il+1);
    const std::string ShlMatVarName(tmpVarName);
    std::vector< std::string > tmpldMat;
    config.getVectorString(ShlMatVarName,tmpldMat,_LaddersNmShells[il]);
    _LaddersShellsMaterial.push_back( tmpldMat );

    sprintf(tmpVarName,"svx.Brl.l%d.ld.ShellsThickness",il+1);
    const std::string ShlThickVarName(tmpVarName);
    std::vector<double> tmpldThick;
    config.getVectorDouble(ShlThickVarName,tmpldThick,_LaddersNmShells[il]);
    _LaddersShellsThick.push_back( tmpldThick );

    //readOuts Parameters
    sprintf(tmpVarName,"svx.Brl.l%d.ro.nShells",il+1);
    const std::string nROShlVarName(tmpVarName);
    _ReadoutsNmShells.push_back( config.getInt(nROShlVarName) );

    sprintf(tmpVarName,"svx.Brl.l%d.ro.ShellIds",il+1);
    const std::string ROShlIdsVarName(tmpVarName);
    std::vector<int> tmproIds;
    config.getVectorInt(ROShlIdsVarName,tmproIds,_ReadoutsNmShells[il]);
    _ReadOutsShellIds.push_back( tmproIds );

  }

  if (_geomType==0) {
    for (int il=0; il<_nLayers; ++il) {
//            double FsrSdLadderDim = CLHEP::twopi*(_LayersInnerRad[il]+_LaddersThickness[il])/((double)_nPhiSectors[il]);
      double FsrSdLadderDim = 2.0*_LayersInnerRad[il]*tan( 0.5*_phiAngles[il] );
      _LaddersWidth.push_back(FsrSdLadderDim);
      unsigned int nROFstSdperLad = ((FsrSdLadderDim-_ROfirstSideInsul[il])/(_ROfirstSideDim[il]+_ROfirstSideInsul[il]));
      _nROsFstSdPerLadder.push_back( nROFstSdperLad );
      unsigned int nROSndSdperLad = ((2.0*_LaddersHalfLength[il]-_ROSecondSideInsul[il])/(_ROSecondSideDim[il]+_ROSecondSideInsul[il]));
      _nROsSndSdPerLadder.push_back( nROSndSdperLad );
      unsigned long nROperLad = nROFstSdperLad;
      nROperLad *= nROSndSdperLad;
      _nROsPerLadder.push_back( nROperLad );
      if (nROFstSdperLad>10000 || nROSndSdperLad>10000) {
        //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
        exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
        e<<"SVX: Maximum number of Channels allowed per X or Y per Ladder is 10000!\n";
        e.error();

      }
    }
  }

}

void SVXMaker::loadForwardTracker( crd::SimpleConfig const& config ){

  _nFwdLayers        = config.getInt("svx.Fwd.nLayers",0);

  if (_nFwdLayers==0) {
    std::cout<<"SVX: no Forward layers present!"<<std::endl;
    return;
  }
  config.getVectorDouble("svx.Fwd.LayersInRad", _LayersInnerRad_fwd, _nFwdLayers);
  config.getVectorDouble("svx.Fwd.LayersZPos", _LayersZPos_fwd, _nFwdLayers);
//  config.getVectorDouble("svx.Fwd.LayersHeight",_LayersHeight_fwd,_nFwdLayers);
  config.getVectorInt("svx.Fwd.nPhiSectors", _nPhiSectors_fwd, _nFwdLayers);
  config.getVectorInt("svx.Fwd.nLaddersPerSector",_nLaddersPerSector_fwd,_nFwdLayers);
//  config.getVectorDouble("svx.Fwd.LaddersHeight",_LaddersHeight_fwd,_nFwdLayers);
  config.getVectorDouble("svx.Fwd.LaddersThickness",_LaddersThickness_fwd,_nFwdLayers);
  config.getVectorInt("svx.Fwd.ROTypes",_ROTypes_fwd,_nFwdLayers);
  config.getVectorDouble("svx.Fwd.ROfirstSideDim",_ROfirstSideDim_fwd,_nFwdLayers);
  config.getVectorDouble("svx.Fwd.ROfirstSideInsul",_ROfirstSideInsul_fwd,_nFwdLayers);
  config.getVectorDouble("svx.Fwd.ROSecondSideDim",_ROSecondSideDim_fwd,_nFwdLayers);
  config.getVectorDouble("svx.Fwd.ROSecondSideInsul",_ROSecondSideInsul_fwd,_nFwdLayers);
  config.getVectorDouble("svx.Fwd.ROangle",_ROangle_fwd,_nFwdLayers);

  char tmpVarName[50];

  for (int il=0; il<_nFwdLayers; ++il) {

    _phiAngles_fwd.push_back( CLHEP::twopi/((double)_nPhiSectors_fwd[il]) );
    if (_LayersInnerRad_fwd[il]<_r0) { _r0=_LayersInnerRad_fwd[il]; }
    double tmpLength = _LayersZPos_fwd[il]+0.5*_LaddersThickness_fwd[il];
    if (tmpLength>_halfLength) { _halfLength=tmpLength; }

    //Ladders Parameters

    sprintf(tmpVarName,"svx.Fwd.l%d.ld.LaddersHeight",il+1);
    const std::string LdHeightVarName(tmpVarName);
    std::vector<double> tmpldHeight;
    config.getVectorDouble(LdHeightVarName,tmpldHeight,_nLaddersPerSector_fwd[il]);
    _LaddersHeight_fwd.push_back( tmpldHeight );

    double tmpLyHeight=0.0;
    for (int iLd=0; iLd<_nLaddersPerSector_fwd[il]; ++iLd) { tmpLyHeight+=_LaddersHeight_fwd[il][iLd]; }
    _LayersHeight_fwd.push_back(tmpLyHeight);

    sprintf(tmpVarName,"svx.Fwd.l%d.ld.nShells",il+1);
    const std::string nShlVarName(tmpVarName);
    _LaddersNmShells_fwd.push_back( config.getInt(nShlVarName) );

    sprintf(tmpVarName,"svx.Fwd.l%d.ld.ShellsMaterial",il+1);
    const std::string ShlMatVarName(tmpVarName);
    std::vector< std::string > tmpldMat;
    config.getVectorString(ShlMatVarName,tmpldMat,_LaddersNmShells_fwd[il]);
    _LaddersShellsMaterial_fwd.push_back( tmpldMat );

    sprintf(tmpVarName,"svx.Fwd.l%d.ld.ShellsThickness",il+1);
    const std::string ShlThickVarName(tmpVarName);
    std::vector<double> tmpldThick;
    config.getVectorDouble(ShlThickVarName,tmpldThick,_LaddersNmShells_fwd[il]);
    _LaddersShellsThick_fwd.push_back( tmpldThick );

    //readOuts Parameters
    sprintf(tmpVarName,"svx.Fwd.l%d.ro.nShells",il+1);
    const std::string nROShlVarName(tmpVarName);
    _ReadoutsNmShells_fwd.push_back( config.getInt(nROShlVarName) );

    sprintf(tmpVarName,"svx.Fwd.l%d.ro.ShellIds",il+1);
    const std::string ROShlIdsVarName(tmpVarName);
    std::vector<int> tmproIds;
    config.getVectorInt(ROShlIdsVarName,tmproIds,_ReadoutsNmShells_fwd[il]);
    _ReadOutsShellIds_fwd.push_back( tmproIds );

    //
    double tmpROut = (_LayersInnerRad_fwd[il]+_LayersHeight_fwd[il])/cos(0.5*_phiAngles_fwd[il]);
    if (tmpROut>_rOut) { _rOut=tmpROut; }

  }

  if (_geomType==0) {
    for (int il=0; il<_nFwdLayers; ++il) {
      std::vector<double> LdsShrtSdDim;
      std::vector<double> LdsLngSdDim;
      std::vector<unsigned int> nROsFstSdShrtPerLd;
      std::vector<unsigned int> nROsFstSdLngPerLd;
      std::vector<unsigned int> nROsSndSdPerLd;
      std::vector<int> LaddersNRwDltRO;
      std::vector<int> LaddersNRwAddDltRO;
      std::vector<unsigned long> nROsPerLd;

//      cout<<"il "<<il<<endl;
//      cout<<_nLaddersPerSector_fwd[il]<<endl;
//      cout<<_LayersInnerRad_fwd[il]<<endl;
//      cout<<_phiAngles_fwd[il]<<endl;
//      cout<<_ROfirstSideDim_fwd[il]<<" "<<_ROfirstSideInsul_fwd[il]<<endl;
//      cout<<_ROSecondSideDim_fwd[il]<<" "<<_ROSecondSideInsul_fwd[il]<<endl;

      double tmpLdInnerRad=_LayersInnerRad_fwd[il];
      double FsrSdLadderLngDim = 2.0*tmpLdInnerRad*tan( 0.5*_phiAngles_fwd[il] );
      for (int iLd=0; iLd<_nLaddersPerSector_fwd[il]; ++iLd) {

//        cout<<" iLd "<<iLd<<endl;
//        cout<<_LaddersHeight_fwd[il][iLd]<<endl;

        double FsrSdLadderShrtDim = FsrSdLadderLngDim;
        tmpLdInnerRad += _LaddersHeight_fwd[il][iLd];
        FsrSdLadderLngDim = 2.0*tmpLdInnerRad*tan( 0.5*_phiAngles_fwd[il] );
        LdsShrtSdDim.push_back(FsrSdLadderShrtDim);
        LdsLngSdDim.push_back(FsrSdLadderLngDim);
        unsigned int nROFstSdShrtperLad = ((FsrSdLadderShrtDim-_ROfirstSideInsul_fwd[il])/(_ROfirstSideDim_fwd[il]+_ROfirstSideInsul_fwd[il]));
        nROsFstSdShrtPerLd.push_back( nROFstSdShrtperLad );
        unsigned int nROFstSdLngperLad = ((FsrSdLadderLngDim-_ROfirstSideInsul_fwd[il])/(_ROfirstSideDim_fwd[il]+_ROfirstSideInsul_fwd[il]));
        nROsFstSdLngPerLd.push_back( nROFstSdLngperLad );
        unsigned int nROSndSdperLad = ((_LaddersHeight_fwd[il][iLd]-_ROSecondSideInsul_fwd[il])/(_ROSecondSideDim_fwd[il]+_ROSecondSideInsul_fwd[il]));
        nROsSndSdPerLd.push_back( nROSndSdperLad );
//        cout<<_ROfirstSideDim_fwd[il]<<" "<<_ROfirstSideInsul_fwd[il]<<" "<<2.0*tan( 0.5*_phiAngles_fwd[il] )<<endl;
//        cout<< _ROSecondSideDim_fwd[il]<<" "<<_ROSecondSideInsul_fwd[il]<<endl;
        float lNRwPerOneDltRO = 1.0/( ( ( (_ROfirstSideDim_fwd[il]+_ROfirstSideInsul_fwd[il])/(2.0*tan( 0.5*_phiAngles_fwd[il]) ) ) / ( _ROSecondSideDim_fwd[il]+_ROSecondSideInsul_fwd[il] ) ) );
        int tmpNRwPerOneDltRO = floor(lNRwPerOneDltRO);
        int tmpNRwPerAddOneDltRO = 0;
        float tmpLeft = fabs(lNRwPerOneDltRO-tmpNRwPerOneDltRO);
//        cout<<"tmpLeft "<<tmpLeft<<endl;
        if (tmpLeft>0.0) {
          tmpLeft=1.0/tmpLeft;
          tmpNRwPerAddOneDltRO = floor( tmpLeft );
          if ( tmpNRwPerAddOneDltRO>0.0 ) { ++tmpNRwPerAddOneDltRO; }
        }
//        cout<<"tmpNRwPerOneDltRO "<<tmpNRwPerOneDltRO<<" tmpNRwPerAddOneDltRO "<<tmpNRwPerAddOneDltRO<<endl;
        LaddersNRwDltRO.push_back( tmpNRwPerOneDltRO );
        LaddersNRwAddDltRO.push_back( tmpNRwPerAddOneDltRO );
        unsigned long nROperLad = nROFstSdShrtperLad;
        unsigned long nROperRow = nROFstSdShrtperLad;
//        cout<<nROSndSdperLad<<" "<<tmpNRwPerOneDltRO<<endl;
//        int nDlROAdd=0;
        for (unsigned int iSdRo=1; iSdRo<nROSndSdperLad; ++iSdRo) {
          nROperRow+=tmpNRwPerOneDltRO;
//            if ( iSdRo%tmpNRwPerOneDltRO==0 ) { ++nROperRow; ++nDlROAdd; }
//            if ( tmpNRwPerAddOneDltRO!=0 && nDlROAdd>0 && nDlROAdd%tmpNRwPerAddOneDltRO==0 ) { ++nROperRow; }
          if ( tmpNRwPerAddOneDltRO!=0 && iSdRo%tmpNRwPerAddOneDltRO==0 ) { ++nROperRow; }
          nROperLad += nROperRow;
        }
        nROsPerLd.push_back( nROperLad );

        if (nROFstSdLngperLad>10000 || nROSndSdperLad>10000) {
          //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"SVX: Maximum number of Channels allowed per X or Y per Fwd Ladder is 10000!\n";
          e.error();

        }
      }
      _LaddersShrtSdDim_fwd.push_back( LdsShrtSdDim );
      _LaddersLngSdDim_fwd.push_back( LdsLngSdDim );
      _nROsFstSdShrtPerLadder_fwd.push_back( nROsFstSdShrtPerLd );
      _nROsFstSdLngPerLadder_fwd.push_back( nROsFstSdLngPerLd );
      _nROsSndSdPerLadder_fwd.push_back( nROsSndSdPerLd );
      _LaddersNRwDltRO_fwd.push_back( LaddersNRwDltRO );
      _LaddersNRwAddDltRO_fwd.push_back( LaddersNRwAddDltRO );
      _nROsPerLadder_fwd.push_back( nROsPerLd );
   }
  }

}

void SVXMaker::Build(){

  _lst = unique_ptr<SVXtracker>(new SVXtracker());
  _lst->_isExternal = _isExternal;
  _lst->_geomType=_geomType;

  if (_nLayers<1 && _nFwdLayers<1) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"SVX: Number of Layer is not acceptable\n";
    e.error();
  }

  if (_isExternal) {
    //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"SVX: Using GDML file option is temporarily disabled\n";
    e.error();

  } else {

    _lst->_nLayers = _nLayers + 2*_nFwdLayers;
    _lst->_nFwdLayers = _nFwdLayers;
    _lst->_r0 = _r0;
    _lst->_rOut = _rOut;
    _lst->_z0 = _z0;
    _lst->_zHalfLength = _halfLength;

    _lst->_roChhnd.reset( new ROGeometryHandle(_lst.get()) );

    Layer *_lr     = new Layer[_nLayers + 2*_nFwdLayers];

    boost::shared_ptr<Ladder> lad;

    //         -----------------------------

    for ( int ily=0; ily<_nLayers; ++ily ) {
      std::cout <<"Building SVX layer: "<<ily+1<<std::endl;

      _lr[ily]._id = LayerId(ily);
      double layHalfLength = _LaddersHalfLength[ily]*_nLaddersPerSector[ily];
      _lr[ily]._detail.reset( new LayerDetail( _LayersInnerRad[ily], (_LayersInnerRad[ily]+_LaddersThickness[ily])/cos(0.5*_phiAngles[ily]), layHalfLength, 0/*_z0*/ ) );
      _lr[ily]._nPhiSectors=_nPhiSectors[ily];
      _lr[ily]._nLaddersPerSector=_nLaddersPerSector[ily];

      long ldid=0;
      for (int isec=0; isec<_nPhiSectors[ily]; ++isec ) {

        ldid=(isec+1)*100;

        for( int ilad=0; ilad<_nLaddersPerSector[ily]; ++ilad ){
          //std::cout <<"Building ladder: "<<ilad+1<<" in Sector "<<isec+1<<std::endl;

          _lr[ily].addLadder(new Ladder());
          lad = _lr[ily]._ladders.back();
          lad->_detail.reset( new LadderDetail( _LaddersWidth[ily], _LaddersHalfLength[ily], _LaddersShellsThick[ily], _LaddersShellsMaterial[ily] ) );
          lad->_id = LadderId(&_lr[ily]._id, ldid);
          if (_ROTypes[ily]==1) { lad->_ladderType=Ladder::pixel; }
          else if (_ROTypes[ily]==2) { lad->_ladderType=Ladder::strip; }
          //lad->_ladderGeomType=Ladder::spherical;
          lad->_ladderGeomType=Ladder::plane;

          double ladZpos = layHalfLength -(2*ilad+1)*_LaddersHalfLength[ily];
//          double ladZpos = 0.0;

          HepGeom::Translate3D ladRZpos (_lr[ily].getDetail()->InnerRadius()+0.5*lad->_detail->thickness(),
                                         0.0,
                                         ladZpos); //in case of cylindrical ladder remove the X translation
          //                cout<<"iSec "<<isec<<" Angle "<<isec*_phiAngles[ily]<<endl;
          HepGeom::RotateZ3D ladPutVert(-CLHEP::halfpi);
          HepGeom::RotateZ3D ladPhipos (isec*_phiAngles[ily]);
          HepGeom::Transform3D ladTransf ( ladPhipos * ladRZpos * ladPutVert );
          lad->setPosMatrix ( ladTransf );

          lad->_ros.reset( new ReadOut() );
          lad->_ros->_detail.reset( new ReadOutDetail(
              _ROfirstSideDim[ily],_ROfirstSideInsul[ily],
              _ROSecondSideDim[ily],_ROSecondSideInsul[ily],
              _ROangle[ily],_ReadOutsShellIds[ily]
          )
          );
          lad->_nReadOuts=_nROsPerLadder[ily];
          lad->_nReadOutsFstSd=_nROsFstSdPerLadder[ily];
          lad->_nReadOutsSndSd=_nROsSndSdPerLadder[ily];

          ++ldid;
        }

      }

    }

    for (int jly=0; jly<2*_nFwdLayers; ++jly) {
      int ily = jly%_nFwdLayers;
      bool isNegative=(jly>=_nFwdLayers);
      int ilyId=_nLayers+jly;
      std::cout <<"Building SVX FW layer: "<<jly+1<<std::endl;

      _lr[ilyId]._id = LayerId(ilyId);
      double layHalfLength = 0.5*_LaddersThickness_fwd[ily];
      if (isNegative) {
        _lr[ilyId]._detail.reset( new LayerDetail( _LayersInnerRad_fwd[ily], (_LayersInnerRad_fwd[ily]+_LayersHeight_fwd[ily])/cos(0.5*_phiAngles_fwd[ily]), layHalfLength, /*_z0*/-_LayersZPos_fwd[ily] ) );
      } else {
        _lr[ilyId]._detail.reset( new LayerDetail( _LayersInnerRad_fwd[ily], (_LayersInnerRad_fwd[ily]+_LayersHeight_fwd[ily])/cos(0.5*_phiAngles_fwd[ily]), layHalfLength, /*_z0+*/_LayersZPos_fwd[ily] ) );
      }
      _lr[ilyId]._layerOrnt = Layer::vertical;
      _lr[ilyId]._layerZone = Layer::forward;
      _lr[ilyId]._nPhiSectors=_nPhiSectors_fwd[ily];
      _lr[ilyId]._nLaddersPerSector=_nLaddersPerSector_fwd[ily];


      long ldid=0;
//      for (int isec=1; isec<2/*_nPhiSectors_fwd[ily]*/; ++isec ) {
//
//        ldid=(isec+1)*100;

      for( int ilad=0; ilad<_nLaddersPerSector_fwd[ily]; ++ilad ){
        //std::cout <<"Building FW ladder: "<<ilad+1<<" in Sector "<<isec+1<<std::endl;

        for (int isec=0; isec<_nPhiSectors_fwd[ily]; ++isec ) {

          ldid=(isec+1)*100+ilad;

          _lr[ilyId].addLadder(new Ladder());
          lad = _lr[ilyId]._ladders.back();
          lad->_detail.reset( new LadderDetail( _LaddersShrtSdDim_fwd[ily][ilad], 0.5*_LaddersHeight_fwd[ily][ilad], _LaddersShellsThick_fwd[ily], _LaddersShellsMaterial_fwd[ily], _LaddersLngSdDim_fwd[ily][ilad] ) );
          lad->_id = LadderId(&_lr[ilyId]._id, ldid);
          if (_ROTypes_fwd[ily]==1) { lad->_ladderType=Ladder::pixel; }
          else if (_ROTypes_fwd[ily]==2) { lad->_ladderType=Ladder::strip; }
          //lad->_ladderGeomType=Ladder::spherical;
          lad->_ladderGeomType=Ladder::plane;

          //          double ladZpos = layHalfLength -(2*ilad+1)*_LaddersHalfLength[ily];
          double ladZpos = _LayersInnerRad_fwd[ily];
          for (int jlad=0; jlad<ilad; ++jlad) {
            ladZpos+=_LaddersHeight_fwd[ily][jlad];
          }
          ladZpos+=0.5*_LaddersHeight_fwd[ily][ilad];

          HepGeom::Translate3D ladRZpos (0.0,
              0.0,
              ladZpos); //in case of cylindrical ladder remove the X translation
          //                cout<<"iSec "<<isec<<" Angle "<<isec*_phiAngles[ily]<<endl;
          HepGeom::RotateZ3D ladPutVert1(CLHEP::halfpi);
          HepGeom::RotateX3D ladPutVert2(CLHEP::halfpi);
          HepGeom::RotateZ3D *putBackWd = new HepGeom::RotateZ3D(0.0);
          if (isNegative) { delete putBackWd; putBackWd=new HepGeom::RotateZ3D(CLHEP::pi); }
          HepGeom::RotateZ3D ladPhipos (isec*_phiAngles_fwd[ily]);
          HepGeom::TranslateZ3D layZpos(_lr[ilyId].getDetail()->zPosition());
          HepGeom::Transform3D ladTransf ( layZpos*ladPhipos * ladPutVert1* ladPutVert2 * ladRZpos * (*putBackWd) );
          lad->setPosMatrix ( ladTransf );

          lad->_ros.reset( new ReadOut() );
          lad->_ros->_detail.reset( new ReadOutDetail(
              _ROfirstSideDim_fwd[ily],_ROfirstSideInsul_fwd[ily],
              _ROSecondSideDim_fwd[ily],_ROSecondSideInsul_fwd[ily],
              _ROangle_fwd[ily],_ReadOutsShellIds_fwd[ily]
          )
          );
          lad->_nReadOuts=_nROsPerLadder_fwd[ily][ilad];
          lad->_nReadOutsFstSd=_nROsFstSdShrtPerLadder_fwd[ily][ilad];
          lad->_nReadOutsSndSd=_nROsSndSdPerLadder_fwd[ily][ilad];
          lad->_nReadOutsFstSd2=_nROsFstSdLngPerLadder_fwd[ily][ilad];
          lad->_nSndSdDltRO=_LaddersNRwDltRO_fwd[ily][ilad];
          lad->_nSndSdAddDltRO=_LaddersNRwAddDltRO_fwd[ily][ilad];

          //++ldid;
          delete putBackWd;
        }

      }


    }

    _lst->_lr.reset(_lr);

//    for ( int ily=0; ily<_nLayers; ++ily ) {
//      Layer *iLy = _lst->getLayer(ily);
//      cout<<iLy->Id()<<": Rin "<<iLy->getDetail()->InnerRadius()<<" ROut "<<iLy->getDetail()->OuterRadius()<<" HL "<<iLy->getDetail()->halfLength()<<" zPos "<<iLy->getDetail()->zPosition()<<" nLadder "<<iLy->nLadders()<<endl;
//      for (int ild=0; ild<iLy->nLadders(); ++ild) {
//        cout<<*iLy->getLadder(ild)<<endl;
//        for (unsigned long ird=0; ird<2.1*_nROsSndSdPerLadder[ily]; ++ird) {
//          cout<<iLy->getLadder(ild)->getROChLoclMidPoint(ird)<<endl;
//        }
//      }
//    }

  }

}

} // namespace svx

#endif
