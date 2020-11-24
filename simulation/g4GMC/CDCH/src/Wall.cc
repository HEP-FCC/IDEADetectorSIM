// CDCH walls description
//
// Original author G. Tassielli
//

// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"

// CDCH includes
#include "Wall.hh"

#include "manageException.hh"

#ifndef __CINT__

namespace cdch {

Wall::Wall(){
        _type           = Wall::undefined;
        _pRmin          = 0.0;
        _pRmax          = 0.0;
        _pSPhi          = 0.0;
        _pDPhi          = 0.0;
        _pSTheta        = 0.0;
        _pDTheta        = 0.0;
        _pDz            = 0.0;
        _name           = "";
        //_c              = CLHEP::Hep3Vector(0.0,0.0,0.0);
        _nShells        = 0;
        _totalThickness = 0.0;
}

Wall::Wall(Walltype wt){
        _type           = wt;
        _pRmin          = 0.0;
        _pRmax          = 0.0;
        _pSPhi          = 0.0;
        _pDPhi          = 0.0;
        _pSTheta        = 0.0;
        _pDTheta        = 0.0;
        _pDz            = 0.0;
        _name           = "";
        //_c              = CLHEP::Hep3Vector(0.0,0.0,0.0);
        _nShells        = 0;
        _totalThickness = 0.0;
}

Wall::Wall(const Wall &wl) {
        if (this!=&wl) {
                _type=wl.getType();
                _nShells=wl.getNShells();
                _totalThickness=wl.getTotalThickness();
                _pRmin=wl.getRmin();
                _pRmax=wl.getRmax();
                _pSPhi=wl.getSPhi();
                _pDPhi=wl.getDPhi();
                _pSTheta=wl.getSTheta();
                _pDTheta=wl.getDTheta();
                _pDz=wl.getDz();
                _name=wl.getName();
                //_c=wl.getC();
                _pos=wl.getPos();
                _materialsName=wl.getMaterialsName();
                _thicknesses=wl.getThicknesses();
        }
}

void Wall::addMaterials(int &wShellNumber, std::vector<std::string> *wShellsMatName, std::vector<double> *wShellsThicknesses) /*throw(cet::exception)*/ {
        _nShells=wShellNumber;
        if ( _nShells!=((int)wShellsMatName->size()) && _nShells!=((int)wShellsThicknesses->size()) ) {
//                throw cet::exception("GEOM")<< "Error in Configuration file! There is a disagreement between the vectors dimensions of a CDCHtracker wall.\n";
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<< "Error in Configuration file! There is a disagreement between the vectors dimensions of a CDCHtracker wall.\n";
          e.error();
        }

        _materialsName.reset(wShellsMatName);
        _thicknesses.reset(wShellsThicknesses);
        _totalThickness = 0.0;
        for (int is = 0; is < _nShells; ++is) {
                _totalThickness += wShellsThicknesses->at(is);
        }
}

Wall& Wall::operator=(const Wall &wl) {
        if (this!=&wl) {
                _type=wl.getType();
                _nShells=wl.getNShells();
                _totalThickness=wl.getTotalThickness();
                _pRmin=wl.getRmin();
                _pRmax=wl.getRmax();
                _pSPhi=wl.getSPhi();
                _pDPhi=wl.getDPhi();
                _pSTheta=wl.getSTheta();
                _pDTheta=wl.getDTheta();
                _pDz=wl.getDz();
                _name=wl.getName();
                //_c=wl.getC();
                _pos=wl.getPos();
                _materialsName=wl.getMaterialsName();
                _thicknesses=wl.getThicknesses();
        }
        return *this;
}

} // namespace cdch

#endif
