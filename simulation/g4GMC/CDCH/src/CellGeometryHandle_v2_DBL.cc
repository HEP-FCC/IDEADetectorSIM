// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#include "CellGeometryHandle_v2_DBL.hh"
#include "Cell.hh"

namespace cdch {

CellGeometryHandle_v2_DBL::CellGeometryHandle_v2_DBL(CDCHtracker *cdchtr):CellGeometryHandle_v2(cdchtr) {
        _nSLayer=cdchtr->nSuperLayers();
}

CellGeometryHandle_v2_DBL::~CellGeometryHandle_v2_DBL() {
}

inline void CellGeometryHandle_v2_DBL::AdjustMatrix(){
        if (_isDownStream) {
                _DnStrmDeadWireLngt=_cdchtr->zZonesLimits()[1]/cos(_cell->getWire()->getEpsilon());
                _tmpCellHalfLength=_cell->getHalfLength()-_DnStrmDeadWireLngt;
                _tmpCellHalfLength*=0.5;
                WirePosAtLength(_tmpCellHalfLength+_DnStrmDeadWireLngt,wCntPos);
                _tmpMidPoint.set(wCntPos[0],wCntPos[1],wCntPos[2]);
                _tmpDirection.set(_cell->getDirection().x(),_cell->getDirection().y(),_cell->getDirection().z());
        }
        else if (_isUpStream) {
                _UpStrmDeadWireLngt=_cdchtr->zZonesLimits()[0]/cos(_cell->getWire()->getEpsilon());
                _tmpCellHalfLength=_cell->getHalfLength()+_UpStrmDeadWireLngt;
                _tmpCellHalfLength*=0.5;
                WirePosAtLength(-_tmpCellHalfLength+_UpStrmDeadWireLngt,wCntPos);
                _tmpMidPoint.set(wCntPos[0],wCntPos[1],wCntPos[2]);
                _tmpDirection.set(-_cell->getDirection().x(),-_cell->getDirection().y(),-_cell->getDirection().z());
        }

}

void CellGeometryHandle_v2_DBL::SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm) {
        if (SupLayer>=_nSLayer || isUpstrm){
                _isUpStream=true;
                _isDownStream=false;
                SupLayer=SupLayer%_nSLayer;
        }
        else {
                _isUpStream=false;
                _isDownStream=true;
        }

        CellGeometryHandle_v2::SelectCell(SupLayer,CelLayer,Cell);
        AdjustMatrix();
}

void CellGeometryHandle_v2_DBL::SelectCellDet(unsigned long  det) {
        // Return the SuperLayer
        int fSuperLayer=(int)(det*0.00001);

        //Return the Layer
        int fLayer=(int)((det)-((fSuperLayer)*100000));

        fLayer=(int)(fLayer*0.001);

        //Return the Wire
        int fWire=(int)(((det)-((fSuperLayer)*100000))-fLayer*1000);

        fSuperLayer--;
        if (fSuperLayer>=_nSLayer){
                _isUpStream=true;
                _isDownStream=false;
                fSuperLayer=fSuperLayer%_nSLayer;
        }
        else {
                _isUpStream=false;
                _isDownStream=true;
        }

        CellGeometryHandle_v2::SelectCell(fSuperLayer,fLayer,fWire);
        AdjustMatrix();
}

void  CellGeometryHandle_v2_DBL::SelectCell(int absRadID, int Cell, bool isUpstrm) {
        _absRadID=absRadID;
        _fSuperLayer=absRadID/_cdchtr->nRing();
        _fLayer=absRadID-_cdchtr->nRing()*_fSuperLayer;
        _fWire=Cell;
        if (_fSuperLayer>=_nSLayer || isUpstrm){
                _isUpStream=true;
                _isDownStream=false;
                _fSuperLayer=_fSuperLayer%_nSLayer;
        }
        else {
                _isUpStream=false;
                _isDownStream=true;
        }

        CellGeometryHandle_v2::SelectCell();
        AdjustMatrix();
}

unsigned long CellGeometryHandle_v2_DBL::computeDet(int SupLayer, int CelLayer, int Cell, bool isUpstrm) {
        if (isUpstrm) {
                SupLayer+=_nSLayer;
        }
        return CellGeometryHandle_v2::computeDet(SupLayer,CelLayer,Cell);
}

int CellGeometryHandle_v2_DBL::computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm) {
        if (SupLayer>=_nSLayer || isUpstrm){
                SupLayer=SupLayer%_nSLayer;
        }
        int absRadID = SupLayer*_cdchtr->nRing()+CelLayer;
        return absRadID;
}

const CLHEP::Hep3Vector& CellGeometryHandle_v2_DBL::GetCellCenter() const {
        return _tmpMidPoint;
}

const CLHEP::Hep3Vector& CellGeometryHandle_v2_DBL::GetCellDirection() const {
        return _tmpDirection;
}

double CellGeometryHandle_v2_DBL::GetCellHalfLength() const {
        return _tmpCellHalfLength;
}

} // namespace cdch
