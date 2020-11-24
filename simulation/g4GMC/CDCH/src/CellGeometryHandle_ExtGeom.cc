// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#include "CellGeometryHandle_ExtGeom.hh"


namespace cdch {

CellGeometryHandle_ExtGeom::CellGeometryHandle_ExtGeom(const char *WireDataFile) {
        _iWirePos = new CDCHWireposition(false,WireDataFile);

}

CellGeometryHandle_ExtGeom::~CellGeometryHandle_ExtGeom() {
        delete _iWirePos;
}

void CellGeometryHandle_ExtGeom::SelectCell(int SupLayer, int CelLayer, int Cell){
        _iWirePos->SelectWire(SupLayer,CelLayer,Cell);
        _fSuperLayer=_iWirePos->GetSuperLayer();
        _fLayer=_iWirePos->GetCelRing();
        _fWire=_iWirePos->GetWire();
}

void CellGeometryHandle_ExtGeom::SelectCellDet(unsigned long  det)
{
        _iWirePos->SelectWireDet(det);
        _fSuperLayer=_iWirePos->GetSuperLayer();
        _fLayer=_iWirePos->GetCelRing();
        _fWire=_iWirePos->GetWire();
}

void CellGeometryHandle_ExtGeom::Global2Local(double *global, double *local)
{
        tmpGlobal[0]=global[0];
        tmpGlobal[1]=global[1];
        tmpGlobal[2]=global[2];
        _iWirePos->Global2Local(tmpGlobal,tmpLocal);
        local[0]=tmpLocal[0];
        local[1]=tmpLocal[1];
        local[2]=tmpLocal[2];
}

void CellGeometryHandle_ExtGeom::Local2Global(double *local, double *global)
{
        local[0]=tmpLocal[0];
        local[1]=tmpLocal[1];
        local[2]=tmpLocal[2];
        _iWirePos->Local2Global(tmpLocal,tmpGlobal);
        tmpGlobal[0]=global[0];
        tmpGlobal[1]=global[1];
        tmpGlobal[2]=global[2];
}

void CellGeometryHandle_ExtGeom::WirePosAtEndcap(float *right, float *left)
{
        _iWirePos->WirePosAtEndcap(tmpRight,tmpLeft);
        right[0]=tmpRight[0];
        right[1]=tmpRight[1];
        right[2]=tmpRight[2];
        left[0]=tmpLeft[0];
        left[1]=tmpLeft[1];
        left[2]=tmpLeft[2];
}

void CellGeometryHandle_ExtGeom::WirePosAtZ(float z, float *pos)
{
        _iWirePos->WirePosAtZ(z,tmpPosAtZ);
        pos[0]=tmpPosAtZ[0];
        pos[1]=tmpPosAtZ[1];
        pos[2]=tmpPosAtZ[2];
}

float CellGeometryHandle_ExtGeom::GetWireAlfa()
{
        return _iWirePos->GetWireAlfa();
}

float CellGeometryHandle_ExtGeom::GetWireEpsilon()
{
        return _iWirePos->GetWireEpsilon();
}

float CellGeometryHandle_ExtGeom::GetCellRad()
{
        return _iWirePos->GetLayerRad();
}

float CellGeometryHandle_ExtGeom::GetCellInsideRad()
{
        return _iWirePos->GetLayerRad();
}

double CellGeometryHandle_ExtGeom::DistFromWireCenter(double *global)
{
        tmpGlobal[0]=global[0];
        tmpGlobal[1]=global[1];
        tmpGlobal[2]=global[2];
        return _iWirePos->DistFromWireCenter(tmpGlobal);
}

double CellGeometryHandle_ExtGeom::DistFromWire(double *global)
{
        tmpGlobal[0]=global[0];
        tmpGlobal[1]=global[1];
        tmpGlobal[2]=global[2];
        return _iWirePos->DistFromWire(tmpGlobal);
}

} // namespace cdch
