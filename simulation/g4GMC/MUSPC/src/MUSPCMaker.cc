// MUSPC geometry maker
//
// Original author G. Tassielli
//

#include <MUSPCRadiator.hh>
#include "MUSPCMaker.hh"
#include "ROGeometryHandle.hh"
#include "MUSPCtracker.hh"
#include "Layer.hh"
#include "manageException.hh"
#include "SimpleConfig.hh"
//#include "messagefacility/MessageLogger/MessageLogger.h"

#include "CLHEP/Vector/RotationY.h"
#include "CLHEP/Vector/RotationZ.h"

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>

#ifndef __CINT__

using CLHEP::Hep3Vector;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

using namespace std;
using namespace svx;

namespace muspc {

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
MUSPCMaker::MUSPCMaker( crd::SimpleConfig const& config):
                                                    _center(){

        _isExternal     = false;
        _extFile        = config.getString("muspc.extFile");
        if ( _extFile.size()>1 && ( _extFile.find_last_of(".gdml") || _extFile.find_last_of(".GDML") )!=0 ) _isExternal = true;

        _z0             = config.getDouble("muspc.z0");
        _geomType       = config.getInt("muspc.geomType");

        _r0=1.0e+6;
        _halfLength=0.0;
        _rOut=0.0;

        loadBarrelTracker(config);

        _r0_fwd=1.0e+6;
        _halfLength_fwd=1.0e+6;
        _rOut_fwd=0.0;
        _zPos_fwd=0.0;

        loadForwardTracker(config);

        loadBarrelRadiator(config);

        loadForwardRadiator(config);

        if (_nFwdLayers>0) {
          _halfLength_fwd=(_zPos_fwd-_halfLength_fwd)*0.5;
          _zPos_fwd-=_halfLength_fwd;
        }

        cout<<"------------------- MUSPC -------------------"<<endl;
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

        cout<<"Tot Numb of Channels "<<totNumRO<<endl;
        cout<<"nRadiator Layers "<<_nRadiatLayers<<endl;

//        _r0-=1*CLHEP::mm;
//        _rOut+=1*CLHEP::mm;
//        _halfLength+=1*CLHEP::mm;


        // Do the real work.
        Build( );
        BuildRadiator();
}

MUSPCMaker::~MUSPCMaker (){}

void MUSPCMaker::loadBarrelTracker( crd::SimpleConfig const& config ){

  _nLayers        = config.getInt("muspc.Brl.nLayers");

  config.getVectorDouble("muspc.Brl.LayersInRad", _LayersInnerRad, _nLayers);
  config.getVectorInt("muspc.Brl.nPhiSectors", _nPhiSectors, _nLayers);
  config.getVectorInt("muspc.Brl.nLaddersPerSector",_nLaddersPerSector,_nLayers);
  config.getVectorDouble("muspc.Brl.LaddersHalfLength",_LaddersHalfLength,_nLayers);
  config.getVectorDouble("muspc.Brl.LaddersThickness",_LaddersThickness,_nLayers);
  config.getVectorInt("muspc.Brl.ROTypes",_ROTypes,_nLayers);
  config.getVectorDouble("muspc.Brl.ROfirstSideDim",_ROfirstSideDim,_nLayers);
  config.getVectorDouble("muspc.Brl.ROfirstSideInsul",_ROfirstSideInsul,_nLayers);
  config.getVectorDouble("muspc.Brl.ROSecondSideDim",_ROSecondSideDim,_nLayers);
  config.getVectorDouble("muspc.Brl.ROSecondSideInsul",_ROSecondSideInsul,_nLayers);
  config.getVectorDouble("muspc.Brl.ROangle",_ROangle,_nLayers);

 char tmpVarName[50];

  for (int il=0; il<_nLayers; ++il) {

    _phiAngles.push_back( CLHEP::twopi/((double)_nPhiSectors[il]) );
    if (_LayersInnerRad[il]<_r0) { _r0=_LayersInnerRad[il]; }
    double il_rOut=(_LayersInnerRad[il]+_LaddersThickness[il])/cos(0.5*_phiAngles[il]);
    if (il_rOut>_rOut) { _rOut=il_rOut; }
    if (_nLaddersPerSector[il]*_LaddersHalfLength[il]>_halfLength) { _halfLength=_nLaddersPerSector[il]*_LaddersHalfLength[il]; }

    //Ladders Parameters
    sprintf(tmpVarName,"muspc.Brl.l%d.ld.nShells",il+1);
    const std::string nShlVarName(tmpVarName);
    _LaddersNmShells.push_back( config.getInt(nShlVarName) );

    sprintf(tmpVarName,"muspc.Brl.l%d.ld.ShellsMaterial",il+1);
    const std::string ShlMatVarName(tmpVarName);
    std::vector< std::string > tmpldMat;
    config.getVectorString(ShlMatVarName,tmpldMat,_LaddersNmShells[il]);
    _LaddersShellsMaterial.push_back( tmpldMat );

    sprintf(tmpVarName,"muspc.Brl.l%d.ld.ShellsThickness",il+1);
    const std::string ShlThickVarName(tmpVarName);
    std::vector<double> tmpldThick;
    config.getVectorDouble(ShlThickVarName,tmpldThick,_LaddersNmShells[il]);
    _LaddersShellsThick.push_back( tmpldThick );

    //readOuts Parameters
    sprintf(tmpVarName,"muspc.Brl.l%d.ro.nShells",il+1);
    const std::string nROShlVarName(tmpVarName);
    _ReadoutsNmShells.push_back( config.getInt(nROShlVarName) );

    sprintf(tmpVarName,"muspc.Brl.l%d.ro.ShellIds",il+1);
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
        e<<"MUSPC: Maximum number of Channels allowed per X or Y per Ladder is 10000!\n";
        e.error();

      }
    }
  }

}

void MUSPCMaker::loadForwardTracker( crd::SimpleConfig const& config ){

  _nFwdLayers        = config.getInt("muspc.Fwd.nLayers",0);

  if (_nFwdLayers==0) {
    std::cout<<"MUSPC: no Forward layers present!"<<std::endl;
    return;
  }
  config.getVectorDouble("muspc.Fwd.LayersInRad", _LayersInnerRad_fwd, _nFwdLayers);
  config.getVectorDouble("muspc.Fwd.LayersZPos", _LayersZPos_fwd, _nFwdLayers);
//  config.getVectorDouble("muspc.Fwd.LayersHeight",_LayersHeight_fwd,_nFwdLayers);
  config.getVectorInt("muspc.Fwd.nPhiSectors", _nPhiSectors_fwd, _nFwdLayers);
  config.getVectorInt("muspc.Fwd.nLaddersPerSector",_nLaddersPerSector_fwd,_nFwdLayers);
//  config.getVectorDouble("muspc.Fwd.LaddersHeight",_LaddersHeight_fwd,_nFwdLayers);
  config.getVectorDouble("muspc.Fwd.LaddersThickness",_LaddersThickness_fwd,_nFwdLayers);
  config.getVectorInt("muspc.Fwd.ROTypes",_ROTypes_fwd,_nFwdLayers);
  config.getVectorDouble("muspc.Fwd.ROfirstSideDim",_ROfirstSideDim_fwd,_nFwdLayers);
  config.getVectorDouble("muspc.Fwd.ROfirstSideInsul",_ROfirstSideInsul_fwd,_nFwdLayers);
  config.getVectorDouble("muspc.Fwd.ROSecondSideDim",_ROSecondSideDim_fwd,_nFwdLayers);
  config.getVectorDouble("muspc.Fwd.ROSecondSideInsul",_ROSecondSideInsul_fwd,_nFwdLayers);
  config.getVectorDouble("muspc.Fwd.ROangle",_ROangle_fwd,_nFwdLayers);

  char tmpVarName[50];

  for (int il=0; il<_nFwdLayers; ++il) {

    _phiAngles_fwd.push_back( CLHEP::twopi/((double)_nPhiSectors_fwd[il]) );
    if (_LayersInnerRad_fwd[il]<_r0_fwd) { _r0_fwd=_LayersInnerRad_fwd[il]; }
    double tmpZmin = _LayersZPos_fwd[il]-0.5*_LaddersThickness_fwd[il];
    double tmpZmax = _LayersZPos_fwd[il]+0.5*_LaddersThickness_fwd[il];
    if (tmpZmin<_halfLength_fwd) { _halfLength_fwd=tmpZmin; }
    if (tmpZmax>_zPos_fwd) { _zPos_fwd=tmpZmax; }

    //Ladders Parameters

    sprintf(tmpVarName,"muspc.Fwd.l%d.ld.LaddersHeight",il+1);
    const std::string LdHeightVarName(tmpVarName);
    std::vector<double> tmpldHeight;
    config.getVectorDouble(LdHeightVarName,tmpldHeight,_nLaddersPerSector_fwd[il]);
    _LaddersHeight_fwd.push_back( tmpldHeight );

    double tmpLyHeight=0.0;
    for (int iLd=0; iLd<_nLaddersPerSector_fwd[il]; ++iLd) { tmpLyHeight+=_LaddersHeight_fwd[il][iLd]; }
    _LayersHeight_fwd.push_back(tmpLyHeight);

    sprintf(tmpVarName,"muspc.Fwd.l%d.ld.nShells",il+1);
    const std::string nShlVarName(tmpVarName);
    _LaddersNmShells_fwd.push_back( config.getInt(nShlVarName) );

    sprintf(tmpVarName,"muspc.Fwd.l%d.ld.ShellsMaterial",il+1);
    const std::string ShlMatVarName(tmpVarName);
    std::vector< std::string > tmpldMat;
    config.getVectorString(ShlMatVarName,tmpldMat,_LaddersNmShells_fwd[il]);
    _LaddersShellsMaterial_fwd.push_back( tmpldMat );

    sprintf(tmpVarName,"muspc.Fwd.l%d.ld.ShellsThickness",il+1);
    const std::string ShlThickVarName(tmpVarName);
    std::vector<double> tmpldThick;
    config.getVectorDouble(ShlThickVarName,tmpldThick,_LaddersNmShells_fwd[il]);
    _LaddersShellsThick_fwd.push_back( tmpldThick );

    //readOuts Parameters
    sprintf(tmpVarName,"muspc.Fwd.l%d.ro.nShells",il+1);
    const std::string nROShlVarName(tmpVarName);
    _ReadoutsNmShells_fwd.push_back( config.getInt(nROShlVarName) );

    sprintf(tmpVarName,"muspc.Fwd.l%d.ro.ShellIds",il+1);
    const std::string ROShlIdsVarName(tmpVarName);
    std::vector<int> tmproIds;
    config.getVectorInt(ROShlIdsVarName,tmproIds,_ReadoutsNmShells_fwd[il]);
    _ReadOutsShellIds_fwd.push_back( tmproIds );

    //
    double tmpROut = (_LayersInnerRad_fwd[il]+_LayersHeight_fwd[il])/cos(0.5*_phiAngles_fwd[il]);
    if (tmpROut>_rOut_fwd) { _rOut_fwd=tmpROut; }
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
          //if ( (tmpLeft-tmpNRwPerAddOneDltRO)>0.0 ) { ++tmpNRwPerAddOneDltRO; }
        }
//        if (tmpLeft>0.0) { tmpNRwPerAddOneDltRO = rint( 10.0*tmpLeft ); }
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
          e<<"MUSPC: Maximum number of Channels allowed per X or Y per Fwd Ladder is 10000!\n";
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

void MUSPCMaker::Build(){

  _lmst = unique_ptr<MUSPCtracker>(new MUSPCtracker());
  _lmst->_isExternal = _isExternal;
  _lmst->_geomType=_geomType;

  if (_nLayers<1) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"MUSPC: Number of Layer is not acceptable\n";
    e.error();
  }

  if (_isExternal) {
    //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"MUSPC: Using GDML file option is temporarily disabled\n";
    e.error();

  } else {

    _lmst->_nLayers = _nLayers + 2*_nFwdLayers;
    _lmst->_nFwdLayers = _nFwdLayers;
    _lmst->_r0 = _r0;
    _lmst->_rOut = _rOut;
    _lmst->_z0 = _z0;
    _lmst->_zHalfLength = _halfLength;

    _lmst->_r0_fwd = _r0_fwd;
    _lmst->_halfLength_fwd = _halfLength_fwd;
    _lmst->_rOut_fwd = _rOut_fwd;
    _lmst->_zPos_fwd = _zPos_fwd;

    _lmst->_roChhnd.reset( new ROGeometryHandle(_lmst.get()) );

    Layer *_lr     = new Layer[_nLayers + 2*_nFwdLayers];

    boost::shared_ptr<Ladder> lad;

    //         -----------------------------

    for ( int ily=0; ily<_nLayers; ++ily ) {
      std::cout <<"Building MUSPC layer: "<<ily+1<<std::endl;

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
      std::cout <<"Building MUSPC FW layer: "<<jly+1<<std::endl;

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

    _lmst->_lr.reset(_lr);

//    for ( int ily=0; ily<_nLayers; ++ily ) {
//      Layer *iLy = _lmst->getLayer(ily);
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

void MUSPCMaker::loadBarrelRadiator( crd::SimpleConfig const& config ){

  char tmpVarName[50];

  _nRadiatLayers   = config.getInt("muspc.Brl.nRadLayers",0);
  for (int il=0; il<_nRadiatLayers; ++il) {
    //Radiator Layers Parameters
    sprintf(tmpVarName,"muspc.Brl.rad.l%d.InRad",il+1);
    const std::string inRadVarName(tmpVarName);
    _RadiatInRasius.push_back(config.getDouble(inRadVarName));

    if (_RadiatInRasius[il]<_r0) { _r0=_RadiatInRasius[il]; }

    sprintf(tmpVarName,"muspc.Brl.rad.l%d.halfLength",il+1);
    const std::string halflVarName(tmpVarName);
    _RadiatHalfLengths.push_back(config.getDouble(halflVarName));

    if (_RadiatHalfLengths[il]>_halfLength) { _halfLength=_RadiatHalfLengths[il]; }

    sprintf(tmpVarName,"muspc.Brl.rad.l%d.nShells",il+1);
    const std::string nShlVarName(tmpVarName);
    _RadiatNmShells.push_back(config.getInt(nShlVarName));

    sprintf(tmpVarName,"muspc.Brl.rad.l%d.ShellsMaterial",il+1);
    const std::string AbShlMatVarName(tmpVarName);
    std::vector< std::string > tmpabMat;
    config.getVectorString(AbShlMatVarName,tmpabMat,_RadiatNmShells[il]);
    _RadiatShellsMaterial.push_back( tmpabMat );

    sprintf(tmpVarName,"muspc.Brl.rad.l%d.ShellsThickness",il+1);
    const std::string AbShlThickVarName(tmpVarName);
    std::vector<double> tmpabThick;
    config.getVectorDouble(AbShlThickVarName,tmpabThick,_RadiatNmShells[il]);
    _RadiatShellsThick.push_back( tmpabThick );

    double tmpThick=0.0;
    for (int iShl=0; iShl<_RadiatNmShells[il]; ++iShl) {
      tmpThick += _RadiatShellsThick[il][iShl];
    }
    _RadiatersThickness.push_back(tmpThick);

    if ((_RadiatInRasius[il]+_RadiatersThickness[il])>_rOut) { _rOut=(_RadiatInRasius[il]+_RadiatersThickness[il]) ; }

  }

}

void MUSPCMaker::loadForwardRadiator( crd::SimpleConfig const& config ) {

  char tmpVarName[50];

  _nRadiatLayers_fwd = config.getInt("muspc.Fwd.nRadLayers",0);
  for (int il=0; il<_nRadiatLayers_fwd; ++il) {
    //Radiator Layers Parameters
    sprintf(tmpVarName,"muspc.Fwd.rad.l%d.InRad",il+1);
    const std::string inRadVarName(tmpVarName);
    _RadiatInRasius_fwd.push_back(config.getDouble(inRadVarName));

    if (_RadiatInRasius_fwd[il]<_r0_fwd) { _r0_fwd=_RadiatInRasius_fwd[il]; }

    sprintf(tmpVarName,"muspc.Fwd.rad.l%d.OutRad",il+1);
    const std::string outRadVarName(tmpVarName);
    _RadiatOutRasius_fwd.push_back(config.getDouble(outRadVarName));

    if (_RadiatOutRasius_fwd[il]>_rOut_fwd) { _rOut_fwd=_RadiatOutRasius_fwd[il]; }

    sprintf(tmpVarName,"muspc.Fwd.rad.l%d.zPos",il+1);
    const std::string zPosVarName(tmpVarName);
    _RadiatZpos_fwd.push_back(config.getDouble(zPosVarName));

    sprintf(tmpVarName,"muspc.Fwd.rad.l%d.nShells",il+1);
    const std::string nShlVarName(tmpVarName);
    _RadiatNmShells_fwd.push_back(config.getInt(nShlVarName));

    sprintf(tmpVarName,"muspc.Fwd.rad.l%d.ShellsMaterial",il+1);
    const std::string AbShlMatVarName(tmpVarName);
    std::vector< std::string > tmpabMat;
    config.getVectorString(AbShlMatVarName,tmpabMat,_RadiatNmShells_fwd[il]);
    _RadiatShellsMaterial_fwd.push_back( tmpabMat );

    sprintf(tmpVarName,"muspc.Fwd.rad.l%d.ShellsThickness",il+1);
    const std::string AbShlThickVarName(tmpVarName);
    std::vector<double> tmpabThick;
    config.getVectorDouble(AbShlThickVarName,tmpabThick,_RadiatNmShells_fwd[il]);
    _RadiatShellsThick_fwd.push_back( tmpabThick );

    double tmpThick=0.0;
    for (int iShl=0; iShl<_RadiatNmShells_fwd[il]; ++iShl) {
      tmpThick += _RadiatShellsThick_fwd[il][iShl];
    }
    _RadiatersThickness_fwd.push_back(tmpThick);

    //_zPos_fwd-_halfLength_fwd
    double tmpMinZ = _RadiatZpos_fwd[il]-0.5*_RadiatersThickness_fwd[il];
    if (tmpMinZ<_halfLength_fwd) { _halfLength_fwd=tmpMinZ; }

    double tmpMaxZ = _RadiatZpos_fwd[il]+0.5*_RadiatersThickness_fwd[il];
    if (tmpMaxZ>_zPos_fwd) { _zPos_fwd=tmpMaxZ; }

  }

}

void MUSPCMaker::BuildRadiator(){

  _lmsrad = unique_ptr<MUSPCRadiator>(new MUSPCRadiator());

  if (_nRadiatLayers<1) {
    exc::exceptionG4 e("GEOM","Warning in Argument",4);
    e<<"MUSPC: Number of Radiator Shell is 0\n";
    e.error();
  }
  if (_nRadiatLayers_fwd<0) {
    exc::exceptionG4 e("GEOM","Warning in Argument",4);
    e<<"MUSPC: Number of Forwad Radiator Shell is negative\n";
    e.error();
  }

  if (_isExternal) {
    //                throw cet::exception("GEOM") <<"Using GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"MUSPC: Using GDML file option is temporarily disabled\n";
    e.error();

  } else {
    _lmsrad->_nRadiatLayers = _nRadiatLayers+_nRadiatLayers_fwd;
    _lmsrad->_RadiatInRasius = _RadiatInRasius;
    _lmsrad->_RadiatHalfLengths = _RadiatHalfLengths;
    _lmsrad->_RadiatNmShells = _RadiatNmShells;
    _lmsrad->_RadiatShellsMaterial = _RadiatShellsMaterial;
    _lmsrad->_RadiatShellsThick = _RadiatShellsThick;
    _lmsrad->_RadiatorsThickness = _RadiatersThickness;
    for (int ily=0; ily<_nRadiatLayers; ++ily) {
      _lmsrad->_RadiatOutRasius.push_back(0.0);
      _lmsrad->_RadiatorType.push_back(0);
    }
    for (int ily=0; ily<_nRadiatLayers_fwd; ++ily) {
      _lmsrad->_RadiatorType.push_back(1);
      _lmsrad->_RadiatInRasius.push_back(_RadiatInRasius_fwd[ily]);
      _lmsrad->_RadiatOutRasius.push_back(_RadiatOutRasius_fwd[ily]);
      _lmsrad->_RadiatHalfLengths.push_back(_RadiatZpos_fwd[ily]);
      _lmsrad->_RadiatNmShells.push_back(_RadiatNmShells_fwd[ily]);
      _lmsrad->_RadiatShellsMaterial.push_back(_RadiatShellsMaterial_fwd[ily]);
      _lmsrad->_RadiatShellsThick.push_back(_RadiatShellsThick_fwd[ily]);
      _lmsrad->_RadiatorsThickness.push_back(_RadiatersThickness_fwd[ily]);
    }

  }
}

} // namespace muspc

#endif
