// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#include <utilFunctions.hh>

#include "CellGeometryHandle.hh"
//#include <Layer.hh>
//#include "Cell.hh"

namespace cdch {

void CellGeometryHandle::Global2Local(double *global, double *local)
{
        tmpGlobal.set(global[0],global[1],global[2]);
        tmpLocal=_invmatrx*tmpGlobal;
        local[0]=tmpLocal.x();
        local[1]=tmpLocal.y();
        local[2]=tmpLocal.z();

//        CLHEP::Hep3Vector tr  = _matrx.getTranslation();
//        double mt0  = global[0]-tr[0];
//        double mt1  = global[1]-tr[1];
//        double mt2  = global[2]-tr[2];
//        CLHEP::HepRotation rot = _matrx.getRotation();
//        local[0] = mt0*rot[0][0] + mt1*rot[1][0] + mt2*rot[2][0];
//        local[1] = mt0*rot[0][1] + mt1*rot[1][1] + mt2*rot[2][1];
//        local[2] = mt0*rot[0][2] + mt1*rot[1][2] + mt2*rot[2][2];

}

void CellGeometryHandle::Global2Local(HepGeom::Point3D<double> &global, HepGeom::Point3D<double> &local)
{
        tmpGlobal.set(global.x(),global.y(),global.z());
        tmpLocal=_invmatrx*tmpGlobal;
        local.set(tmpLocal.x(),tmpLocal.y(),tmpLocal.z());
}

void CellGeometryHandle::Local2Global(double *local, double *global)
{
        tmpLocal.set(local[0],local[1],local[2]);
        tmpGlobal=_matrx*tmpLocal;
        global[0]=tmpGlobal.x();
        global[1]=tmpGlobal.y();
        global[2]=tmpGlobal.z();

//        int i;
//        CLHEP::Hep3Vector tr  = _matrx.getTranslation();
//        CLHEP::HepRotation rot = _matrx.getRotation();
//        for (i=0; i<3; i++) {
//                global[i] = tr[i]
//                            + local[0]*rot[i][0]
//                            + local[1]*rot[i][1]
//                            + local[2]*rot[i][2];
//        }

}

void CellGeometryHandle::Local2Global(HepGeom::Point3D<double> &local, HepGeom::Point3D<double> &global)
{
        tmpLocal.set(local.x(),local.y(),local.z());
        tmpGlobal=_matrx*tmpLocal;
        global.set(tmpGlobal.x(),tmpGlobal.y(),tmpGlobal.z());
}

void CellGeometryHandle::WirePosAtEndcap(float *right, float *left)
{
        tmpRight.set(0.0,0.0,_cell->getWire()->getDetail()->halfLength());
        tmpLeft.set(0.0,0.0,-tmpRight.z());
        tmpRight.transform(_matrx);
        tmpLeft.transform(_matrx);
        right[0]=tmpRight.x();
        right[1]=tmpRight.y();
        right[2]=tmpRight.z();
        left[0]=tmpLeft.x();
        left[1]=tmpLeft.y();
        left[2]=tmpLeft.z();
}

void CellGeometryHandle::WirePosAtZ(float z, float *pos)
{
        WirePosAtLength(z/cos(_cell->getWire()->getEpsilon()), pos);
}

void CellGeometryHandle::WirePosAtLength(float length, float *pos)
{
        tmpPos.set(0.0,0.0,length);
        tmpPos.transform(_matrx);
        pos[0]=tmpPos.x();
        pos[1]=tmpPos.y();
        pos[2]=tmpPos.z();
}

void CellGeometryHandle::WirePosAtEndcap(CLHEP::Hep3Vector &right, CLHEP::Hep3Vector &left)
{
        tmpRight.set(0.0,0.0,_cell->getWire()->getDetail()->halfLength());
        tmpLeft.set(0.0,0.0,-tmpRight.z());
        tmpRight.transform(_matrx);
        tmpLeft.transform(_matrx);
        right.set(tmpRight.x(),tmpRight.y(),tmpRight.z());
        left.set(tmpLeft.x(),tmpLeft.y(),tmpLeft.z());
}

void CellGeometryHandle::WirePosAtZ(float z, CLHEP::Hep3Vector &pos)
{
        WirePosAtLength(z/cos(_cell->getWire()->getEpsilon()), pos);
}

void CellGeometryHandle::WirePosAtLength(float length, CLHEP::Hep3Vector &pos)
{
        tmpPos.set(0.0,0.0,length);
        tmpPos.transform(_matrx);
        pos.set(tmpPos.x(),tmpPos.y(),tmpPos.z());
}

float CellGeometryHandle::GetWireAlfa()
{
        return (float) _cell->getWire()->getAlpha();
}

float CellGeometryHandle::GetWireEpsilon()
{
        return (float) _cell->getWire()->getEpsilon();
}

float CellGeometryHandle::GetCellRad()
{
        return (float) _cell->getDetail()->CirumscribedRadius();
}

float CellGeometryHandle::GetCellInsideRad()
{
        return (float) _cell->getDetail()->InscribedCircleRadius();
}

const CLHEP::Hep3Vector& CellGeometryHandle::GetWireCenter() const {
        return _cell->getMidPoint();
}

const CLHEP::Hep3Vector& CellGeometryHandle::GetWireDirection() const {
        return _cell->getDirection();
}

const CLHEP::Hep3Vector& CellGeometryHandle::GetCellCenter() const {
        return _cell->getMidPoint();
}

const CLHEP::Hep3Vector& CellGeometryHandle::GetCellDirection() const {
        return _cell->getDirection();
}

double CellGeometryHandle::GetCellHalfLength() const {
        return _cell->getHalfLength();
}

bool  CellGeometryHandle::canIntersectInZ(float &zCorss, float &distWires, unsigned long compDet) const {
        // Return the SuperLayer
         int compSuperLayer=(int)(compDet*0.00001);

         //Return the Layer
         int compLayer=(int)((compDet)-((compSuperLayer)*100000));

         compLayer=(int)(compLayer*0.001);

         //Return the Wire
         int compWire=(int)(((compDet)-((compSuperLayer)*100000))-compLayer*1000);

         compSuperLayer--;

         return canIntersectInZ (zCorss, distWires, compSuperLayer, compLayer, compWire);
}

bool  CellGeometryHandle::canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm) const {
  exc::exceptionG4 e("GEOM","Waring",4);
  e<<"canIntersectInZ is not usable in base clase CellGeometryHandle, use specific implementation\n";
  e<<"Ignored arguments "<< zCorss<<" "<<distWires<<" "<<compAbsRadID<<" "<<compICell<<" "<<compIsUpstrm<<"\n";
  e.error();
  return false;
}

bool  CellGeometryHandle::canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compICell, bool compIsUpstrm) const {
  exc::exceptionG4 e("GEOM","Waring",4);
  e<<"canIntersectInZ is not usable in base clase CellGeometryHandle, use specific implementation\n";
  e<<"Ignored arguments "<< zCorss<<" "<<distWires<<" "<<compSupLayer<<" "<<compCelLayer<<" "<<compICell<<" "<<compIsUpstrm<<"\n";
  e.error();
  return false;
}

bool  CellGeometryHandle::canIntersectInZ(float &zCorss, float &distWires) const {
  exc::exceptionG4 e("GEOM","Waring",4);
  e<<"canIntersectInZ is not usable in base clase CellGeometryHandle, use specific implementation\n";
  e<<"Ignored arguments "<< zCorss<<" "<<distWires<<"\n";
  e.error();
  return false;
}

double CellGeometryHandle::DistFromWire(double *global)
{
        return (DistFromWireCenter(global)-_cell->getDetail()->wireRadius());
}

double CellGeometryHandle::DistFromWireCenter(double *global)
{
        tmpGlobal.set(global[0],global[1],global[2]);
        tmpLocal=_invmatrx*tmpGlobal;
        return sqrt(sum_of_squares(tmpLocal.x(), tmpLocal.y()));
}

double CellGeometryHandle::DistFromWire(CLHEP::Hep3Vector &global)
{
        return (DistFromWireCenter(global)-_cell->getDetail()->wireRadius());
}

double CellGeometryHandle::DistFromWireCenter(CLHEP::Hep3Vector &global)
{
        tmpGlobal.set(global[0],global[1],global[2]);
        tmpLocal=_invmatrx*tmpGlobal;
        return sqrt(sum_of_squares(tmpLocal.x(), tmpLocal.y()));
}

double CellGeometryHandle::DistFromWire(CLHEP::Hep3Vector const &global)
{
        return (DistFromWireCenter(global)-_cell->getDetail()->wireRadius());
}

double CellGeometryHandle::DistFromWireCenter(CLHEP::Hep3Vector const &global)
{
        tmpGlobal.set(global[0],global[1],global[2]);
        tmpLocal=_invmatrx*tmpGlobal;
        return sqrt(sum_of_squares(tmpLocal.x(), tmpLocal.y()));
}

double CellGeometryHandle::CrossingPathOnFieldWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
                                        FWireSide &sideFlag, CLHEP::Hep3Vector &fwPca, double tolerance) const {
  exc::exceptionG4 e("GEOM","Waring",4);
  e<<"CrossingPathOnFieldWires is not usable in base clase CellGeometryHandle, use specific implementation\n";
  e<<"Ignored arguments "<< point<<" "<<dir<<" "<<sideFlag<<" "<<fwPca<<" "<<tolerance<<"\n";
  e.error();
  return 0.0;
}

double CellGeometryHandle::CrossingPathOnSenseWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
                                        CLHEP::Hep3Vector &swPca, double tolerance) const {
  exc::exceptionG4 e("GEOM","Waring",4);
  e<<"CrossingPathOnSenseWires is not usable in base clase CellGeometryHandle, use specific implementation\n";
  e<<"Ignored arguments "<< point<<" "<<dir<<" "<<swPca<<" "<<tolerance<<"\n";
  e.error();
  return 0.0;
}

boost::shared_ptr<Layer> CellGeometryHandle::GetLayer() {
  return _lay;
}
boost::shared_ptr<Cell> CellGeometryHandle::GetCell() {
  return _cell;
}

void  CellGeometryHandle::SelectComp_Cell(int compSupLayer, int compCelLayer, int compICell, bool isUpstrm) {
//                throw cet::exception("GEOM")<< "SelectCell Method not implemented for the interface class CellGeometryHandle, please use one of the real implementation"<<std::endl;
  exc::exceptionG4 e("GEOM","Fatal Error",0);
  e<<"SelectCell Method not implemented for the interface class CellGeometryHandle, please use one of the real implementation \n";
  e.error();
  //Fake line just to remove worning
  e<<compSupLayer<<" "<<compCelLayer<<" "<<compICell<<" "<<isUpstrm<<"\n";
}

} // namespace cdch
