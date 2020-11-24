// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#include "CellGeometryHandle_v2.hh"
#include "SuperLayer.hh"
#include "Layer.hh"
#include "Cell.hh"

#include <utilFunctions.hh>

namespace cdch {

CellGeometryHandle_v2::CellGeometryHandle_v2(CDCHtracker *cdchtr) {
        _cdchtr=cdchtr;
}

CellGeometryHandle_v2::~CellGeometryHandle_v2() {
}

void CellGeometryHandle_v2::SelectCell() {
        SuperLayer *sl=_cdchtr->getSuperLayer(_fSuperLayer);
        int SelectedCellLayer = _fLayer*2;
        if (_fLayer==0) SelectedCellLayer+=2;
        if (_fSuperLayer==0) SelectedCellLayer++;
        _lay=sl->getLayer(SelectedCellLayer);
        _cell=_lay->getCell(_fWire);
        _cell->_tmpMidPoint  = _cell->_senseWire.get()->getMidPoint();
        _cell->_tmpDirection = _cell->_senseWire.get()->getDirection();
 //        _cell = _cdchtr->getSuperLayer(_fSuperLayer)->getLayer(CelLayer)->getCell(Cell);
        _matrx = _cell->getWire()->get3DTransfrom();
        _invmatrx = _cell->getWire()->get3DInvTransfrom();
}

void CellGeometryHandle_v2::SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm) {
  if (!isUpstrm) {
    _fSuperLayer=SupLayer;
    _fLayer=CelLayer;
    _fWire=Cell;
    _absRadID = SupLayer*_cdchtr->nRing()+CelLayer;
    SelectCell();
  } else {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"CellGeometryHandle_v2::SelectCell is not usable for Dumbbell configuration\n";
    e.error();
  }

}

void  CellGeometryHandle_v2::SelectCell(int absRadID, int Cell, bool isUpstrm) {
  if (!isUpstrm) {
        _absRadID=absRadID;
        _fSuperLayer=absRadID/_cdchtr->nRing();
        _fLayer=absRadID-_cdchtr->nRing()*_fSuperLayer;
        _fWire=Cell;
        SelectCell();
  } else {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"CellGeometryHandle_v2::SelectCell is not usable for Dumbbell configuration\n";
    e.error();
  }
}

int CellGeometryHandle_v2::computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm) {
        int absRadID = 0;
        if (!isUpstrm) {
          absRadID = SupLayer*_cdchtr->nRing()+CelLayer;
        } else {
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"CellGeometryHandle_v2::SelectCell is not usable for Dumbbell configuration\n";
          e.error();
        }
        return absRadID;
}

bool  CellGeometryHandle_v2::canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm) const {
        int compSupLayer=compAbsRadID/_cdchtr->nRing();
        int compCelLayer=compAbsRadID-_cdchtr->nRing()*compSupLayer;
        return CellGeometryHandle_v2::canIntersectInZ(zCorss,distWires,compSupLayer,compCelLayer,compICell,compIsUpstrm);
}

bool  CellGeometryHandle_v2::canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compICell, bool compIsUpstrm) const {
        if (_isUpStream!=compIsUpstrm) {
                return false;
        }
        if ( _fSuperLayer == compSupLayer ) {
                return false;
        }

        SuperLayer *sl=_cdchtr->getSuperLayer(compSupLayer);
        int SelectedCellLayer = compCelLayer*2;
        if (compCelLayer==0) SelectedCellLayer+=2;
        if (compSupLayer==0) SelectedCellLayer++;
        boost::shared_ptr<Layer> compItl=sl->getLayer(SelectedCellLayer);
        boost::shared_ptr<Cell> compCell= compItl->getCell(compICell);

        if (_cell->getWire()->getEpsilon()*compCell->getWire()->getEpsilon()>0.0) {
                return false;
        }

        float deltaPhi, maxDistAvail;
        bool crossing = false;

        maxDistAvail = 0.5*_cell->getWire()->getAlpha() + 0.5*compCell->getWire()->getAlpha();

        deltaPhi = _cell->getMidPoint().getPhi() - compCell->getMidPoint().getPhi();

        if ( deltaPhi==0.0 ) {
                crossing = true;
        } else {
                deltaPhi = fabs (deltaPhi);
                if (deltaPhi>CLHEP::pi) deltaPhi = CLHEP::twopi - deltaPhi;
                if (deltaPhi<maxDistAvail) {
                        crossing = true;
                }
        }
        if ( crossing ) {
                TwoLinePCA pca(_cell->getMidPoint() , _cell->getDirection(), compCell->getMidPoint(), compCell->getDirection());
                if (pca.s1()<-_cell->getWire()->getDetail()->halfLength() || pca.s1()>_cell->getWire()->getDetail()->halfLength() ) {
                        return false;
                }
                zCorss = pca.point1().z();//0.5*(pca.point1().z()+pca.point2().z());
                distWires = pca.dca();
        }
        return crossing;
}

} // namespace cdch
