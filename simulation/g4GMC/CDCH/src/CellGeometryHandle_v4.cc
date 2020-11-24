// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#include "CellGeometryHandle_v4.hh"
#include "SuperLayer.hh"
#include "Layer.hh"
#include "Cell.hh"

#include <utilFunctions.hh>
#include "manageException.hh"

#include <iostream>

namespace cdch {

CellGeometryHandle_v4::CellGeometryHandle_v4(CDCHtracker *cdchtr) :
    _cdchtr(cdchtr)
{
}

CellGeometryHandle_v4::~CellGeometryHandle_v4() {
}

void CellGeometryHandle_v4::SelectCell() {
        SuperLayer *sl=_cdchtr->getSuperLayer(_fSuperLayer);
//        int swLayer=3*_fLayer+1;
//        if (_fSuperLayer==0) swLayer+=2;
//        std::cout<<"SelectCell: "<<_fSuperLayer<<" "<<_fLayer<<" "<<swLayer<<" "<<_fWire<<std::endl;
//        _lay=sl->getLayer(swLayer);
        _lay=sl->getSwLayer(_fLayer);
        _cell=_lay->getCell(_fWire);
        _cell->_tmpMidPoint  = _cell->_senseWire.get()->getMidPoint();
        _cell->_tmpDirection = _cell->_senseWire.get()->getDirection();
        _matrx = _cell->getWire()->get3DTransfrom();
        _invmatrx = _cell->getWire()->get3DInvTransfrom();
}

void CellGeometryHandle_v4::SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm) {
  if (!isUpstrm) {
    _fSuperLayer=SupLayer;
    _fLayer=CelLayer;
    _fWire=Cell;
    _absRadID = SupLayer*_cdchtr->nRing()+CelLayer;
    SelectCell();
  } else {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"CellGeometryHandle_v4::SelectCell is not usable for Dumbbell configuration\n";
    e.error();
  }

}

void  CellGeometryHandle_v4::SelectCell(int absRadID, int Cell, bool isUpstrm) {
  if (!isUpstrm) {
        _absRadID=absRadID;
        _fSuperLayer=absRadID/_cdchtr->nRing();
        _fLayer=absRadID-_cdchtr->nRing()*_fSuperLayer;
        _fWire=Cell;
        SelectCell();
  } else {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"CellGeometryHandle_v4::SelectCell is not usable for Dumbbell configuration\n";
    e.error();
  }
}

int CellGeometryHandle_v4::computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm) {
        int absRadID = 0;
        if (!isUpstrm) {
          absRadID = SupLayer*_cdchtr->nRing()+CelLayer;
        } else {
          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
          e<<"CellGeometryHandle_v4::SelectCell is not usable for Dumbbell configuration\n";
          e.error();
        }
        return absRadID;
}

bool  CellGeometryHandle_v4::canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm) const {
        int compSupLayer=compAbsRadID/_cdchtr->nRing();
        int compCelLayer=compAbsRadID-_cdchtr->nRing()*compSupLayer;
        return CellGeometryHandle_v4::canIntersectInZ(zCorss,distWires,compSupLayer,compCelLayer,compICell,compIsUpstrm);
}

bool  CellGeometryHandle_v4::canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compICell, bool compIsUpstrm) const {
        if (_isUpStream!=compIsUpstrm) {
                return false;
        }
        if ( _fSuperLayer == compSupLayer ) {
                return false;
        }

        SuperLayer *sl=_cdchtr->getSuperLayer(compSupLayer);
        boost::shared_ptr<Layer> compItl=sl->getLayer(compCelLayer);
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


//void CellGeometryHandle_v4::SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm) {
//  if (!isUpstrm) {
//    SuperLayer *sl=_cdchtr->getSuperLayer(SupLayer);
//    if (SupLayer==_nLayer && CelLayer==1) CelLayer=0;
//    if (CelLayer>0) {
//      //          throw cet::exception("GEOM")<< "The requested cell layer is not allowed in the Square cell geometry"<<std::endl;
//      exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//      e<< "The requested cell layer is not allowed in the Square cell geometry"<<std::endl;
//      e.error();
//    }
//    int SelectedCellLayer = 1;
//    if (SupLayer==0) SelectedCellLayer++;
//    _lay=sl->getLayer(SelectedCellLayer);
//    _cell=_lay->getCell(Cell);
//    _cell->_tmpMidPoint  = _cell->_senseWire.get()->getMidPoint();
//    _cell->_tmpDirection = _cell->_senseWire.get()->getDirection();
//    _matrx = _cell->getWire()->get3DTransfrom();
//    _invmatrx = _cell->getWire()->get3DInvTransfrom();
//    _fSuperLayer=SupLayer;
//    _fLayer=CelLayer;
//    _fWire=Cell;
//    _absRadID = _fSuperLayer;
//  } else {
//    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//    e<<"CellGeometryHandle_v4::SelectCell is not usable for Dumbbell configuration\n";
//    e.error();
//  }
//}
//
//void CellGeometryHandle_v4::SelectCellDet(unsigned long  det) {
//        // Return the SuperLayer
//        int fSuperLayer=(int)(det*0.0001);
//
//        //Return the Wire
//        int fWire=(int)((det)-((fSuperLayer)*10000));
//
//        fSuperLayer--;
//
//        //Call the upper method
//        SelectCell(fSuperLayer,0,fWire);
//}
//
//void  CellGeometryHandle_v4::SelectCell(int absRadID, int Cell, bool isUpstrm) {
//        _absRadID=absRadID;
//        SelectCell(absRadID,0,Cell,isUpstrm);
//}
//
//unsigned long CellGeometryHandle_v4::computeDet(int SupLayer, int CelLayer, int Cell, bool isUpstrm) {
//  unsigned long det = 0;
//  if (!isUpstrm) {
//    det = ((SupLayer+1)*10000)+(Cell);
//  } else {
//    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//    e<<"CellGeometryHandle_v4::computeDet is not usable for Dumbbell configuration\n";
//    e.error();
//  }
//  return det;
//  exc::exceptionG4 e("GEOM","Waring",4);
//  e<<"CellGeometryHandle_v4::computeDet CelLayer ignored: "<<CelLayer<<"\n";
//  e.error();
//
//}
//
//void  CellGeometryHandle_v4::SelectComp_Cell(int compSupLayer, int compCelLayer, int compICell, bool compIsUpStream) {
//        SuperLayer *sl=_cdchtr->getSuperLayer(compSupLayer);
//        if (compSupLayer==_nLayer && compCelLayer==1) compCelLayer=0;
//        if (compCelLayer>0) {
////          throw cet::exception("GEOM")<< "The requested cell layer is not allowed in the Square cell geometry"<<std::endl;
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<< "The requested cell layer is not allowed in the Square cell geometry"<<std::endl;
//          e.error();
//        }
//        int SelectedCellLayer = 1;
//        if (compSupLayer==0) SelectedCellLayer++;
//        boost::shared_ptr<Layer> compItl=sl->getLayer(SelectedCellLayer);
//        _Comp_cell = compItl->getCell(compICell);
//        _Comp_matrx = _Comp_cell->getWire()->get3DTransfrom();
//        _Comp_isUpStream = compIsUpStream;
//        _Comp_SuperLayer = compSupLayer;
//}
//
//void  CellGeometryHandle_v4::SelectComp_CellDet(unsigned long compDet){
//        // Return the SuperLayer
//        int compSuperLayer=(int)(compDet*0.0001);
//
//        //Return the Wire
//        int compWire=(int)((compDet)-((compSuperLayer)*10000));
//
//        compSuperLayer--;
//
//        SelectComp_Cell(compSuperLayer,0,compWire,false);
//}
//
//void  CellGeometryHandle_v4::SelectComp_Cell(int compIAbsRadID, int compICell, bool compIsUpStream) {
//        SelectComp_Cell(compIAbsRadID,0,compICell,compIsUpStream);
//}
//
//bool  CellGeometryHandle_v4::canIntersectInZ(float &zCorss, float &distWires) const {
//
//        if (_isUpStream!=_Comp_isUpStream) {
//                return false;
//        }
//        if ( _fSuperLayer == _Comp_SuperLayer ) {
//                return false;
//        }
//
//        if (_cell->getWire()->getEpsilon()*_Comp_cell->getWire()->getEpsilon()>0.0) {
//                return false;
//        }
//
//        float deltaPhi, maxDistAvail;
//        bool crossing = false;
//
//        maxDistAvail = 0.5*_cell->getWire()->getAlpha() + 0.5*_Comp_cell->getWire()->getAlpha();
//        deltaPhi = _cell->_tmpMidPoint.getPhi() - _Comp_cell->_tmpMidPoint.getPhi();
//
//        if ( deltaPhi==0.0 ) {
//                crossing = true;
//        } else {
//                deltaPhi = fabs (deltaPhi);
//                if (deltaPhi>CLHEP::pi) deltaPhi = CLHEP::twopi - deltaPhi;
//                if (deltaPhi<maxDistAvail) {
//                        crossing = true;
//                }
//        }
//        if ( crossing ) {
//                TwoLinePCA pca( _cell->_tmpMidPoint, _cell->_tmpDirection, _Comp_cell->_tmpMidPoint, _Comp_cell->_tmpDirection);
//                if (pca.s1()<-_cell->getHalfLength() || pca.s1()>_cell->getHalfLength() ) {
//                        return false;
//                }
//                zCorss = pca.point1().z();//0.5*(pca.point1().z()+pca.point2().z());
//                distWires = pca.dca();
//        }
//        return crossing;
//}
//
//bool  CellGeometryHandle_v4::canIntersectInZ(float &zCorss, float &distWires, unsigned long compDet) const {
//        // Return the SuperLayer
//        int compSuperLayer=(int)(compDet*0.0001);
//
//        //Return the Wire
//        int compWire=(int)((compDet)-((compSuperLayer)*10000));
//
//        compSuperLayer--;
//
//        return canIntersectInZ(zCorss,distWires,compSuperLayer,0,compWire);
//}
//
//bool  CellGeometryHandle_v4::canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm) const {
//        return canIntersectInZ(zCorss,distWires,compAbsRadID,0,compICell,compIsUpstrm);
//}
//
//bool  CellGeometryHandle_v4::canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compICell, bool compIsUpstrm) const {
//        if (_isUpStream!=compIsUpstrm) {
//                return false;
//        }
//        if ( _fSuperLayer == compSupLayer ) {
//                return false;
//        }
//        SuperLayer *sl=_cdchtr->getSuperLayer(compSupLayer);
//        if (compSupLayer==_nLayer && compCelLayer==1) compCelLayer=0;
//        if (compCelLayer>0) {
////          throw cet::exception("GEOM")<< "The requested cell layer is not allowed in the Square cell geometry"<<std::endl;
//          exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
//          e<< "The requested cell layer is not allowed in the Square cell geometry"<<std::endl;
//          e.error();
//        }
//        int SelectedCellLayer = 1;
//        if (compSupLayer==0) SelectedCellLayer++;
//        boost::shared_ptr<Layer> compItl=sl->getLayer(SelectedCellLayer);
//        boost::shared_ptr<Cell> compCell= compItl->getCell(compICell);
//
//        if (_cell->getWire()->getEpsilon()*compCell->getWire()->getEpsilon()>0.0) {
//                return false;
//        }
//
//        float deltaPhi, maxDistAvail;
//        bool crossing = false;
//
//        maxDistAvail = 0.5*_cell->getWire()->getAlpha() + 0.5*compCell->getWire()->getAlpha();
//
//        deltaPhi = _cell->getMidPoint().getPhi() - compCell->getMidPoint().getPhi();
//
//        if ( deltaPhi==0.0 ) {
//                crossing = true;
//        } else {
//                deltaPhi = fabs (deltaPhi);
//                if (deltaPhi>CLHEP::pi) deltaPhi = CLHEP::twopi - deltaPhi;
//                if (deltaPhi<maxDistAvail) {
//                        crossing = true;
//                }
//        }
//        if ( crossing ) {
//                TwoLinePCA pca(_cell->getMidPoint() , _cell->getDirection(), compCell->getMidPoint(), compCell->getDirection());
//                if (pca.s1()<-_cell->getWire()->getDetail()->halfLength() || pca.s1()>_cell->getWire()->getDetail()->halfLength() ) {
//                        return false;
//                }
//                zCorss = pca.point1().z();//0.5*(pca.point1().z()+pca.point2().z());
//                distWires = pca.dca();
//        }
//        return crossing;
//}
//
//double CellGeometryHandle_v4::CrossingPathOnFieldWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
//                CellGeometryHandle::FWireSide &sideFlag, CLHEP::Hep3Vector &fwPca, double tolerance) const {
//
//        HepGeom::Point3D<double> tmpPntGlob, tmpPntLocal;
//        double fwRad(0.0);
//        double crossPath(0.0);
//        size_t ifw=0;
//        size_t maxIfw=_cell->nFWires();
//
////        for ( ; ifw<maxIfw; ++ifw) {
////                boost::shared_ptr<Wire> fw = _cell->getFWire(ifw);
////                std::cout<<"fwwire "<<ifw<<" R center "<<sqrt(sum_of_squares(fw->getMidPoint().x(), fw->getMidPoint().y()))<<std::endl;
////        }
//
//        if ( sideFlag == side ) {
//                maxIfw=2;
//        } else if ( sideFlag == bottom ) {
//                ifw=2;
//                maxIfw=5;
//        } else if ( sideFlag == top ) {
//                ifw=5;
//        }
////        std::cout<<"Point "<<point<<" ifw "<<ifw<<" maxIfw "<< maxIfw<<std::endl;
//        sideFlag = noFWCross;
//        for ( ; ifw<maxIfw; ++ifw) {
//                boost::shared_ptr<Wire> fw = _cell->getFWire(ifw);
//                tmpPntGlob.set(point[0],point[1],point[2]);
//                tmpPntLocal=fw->get3DInvTransfrom()*tmpPntGlob;
//
//                //A starting point close to the wire is needed to evaluate the crossing path between track and wire
//                //because it is computed using a linear approximation.
//
////                std::cout<<"dist from wire "<<ifw<<" center "<<sqrt(sum_of_squares(tmpPntLocal.x(), tmpPntLocal.y()))<<std::endl;
//                if ( sqrt(sum_of_squares(tmpPntLocal.x(), tmpPntLocal.y())) <0.8) {
//                        TwoLinePCA pca(fw->getMidPoint(),fw->getDirection(), point, dir);
//                        fwRad = fw->getDetail()->outerRadius();
//                        if (pca.dca2d()<fwRad){
//                                crossPath = 2.0*sqrt(diff_of_squares(fwRad,pca.dca2d()));
//                                fwPca=pca.point2();
//                        } else if ( pca.dca2d()<(fwRad+tolerance) ) {
//                                crossPath = 0.1*fwRad;
//                                fwPca=pca.point2();
//                        } else {
//                                continue;
//                        }
//                        //crossPath/=sin(fw->getDirection().angle(dir));
//                        double cos2t = fw->getDirection().cos2Theta(dir);
//                        if(cos2t<0.999) {
//                                crossPath/=sqrt(1.0-cos2t);
//                        }
//                        if (ifw<2) { sideFlag = side; }
//                        else if (ifw<5) { sideFlag = bottom; }
//                        else { sideFlag = top; }
//                        break;
//                }
//        }
//        return crossPath;
//}
//
//double CellGeometryHandle_v4::CrossingPathOnSenseWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir, CLHEP::Hep3Vector &swPca, double tolerance) const {
//
//        HepGeom::Point3D<double> tmpPntGlob, tmpPntLocal;
//        double swRad(0.0);
//        double crossPath(0.0);
//        tmpPntGlob.set(point[0],point[1],point[2]);
//        tmpPntLocal=_cell->getWire()->get3DInvTransfrom()*tmpPntGlob;
//        //A starting point close to the wire is needed to evaluate the crossing path between track and wire
//        //because it is computed using a linear approximation.
//        if ( sqrt(sum_of_squares(tmpPntLocal.x(), tmpPntLocal.y())) <0.8) {
//                TwoLinePCA pca(GetCellCenter(),GetCellDirection(), point, dir);
//                swRad = _cell->getWire()->getDetail()->outerRadius();
//                if (pca.dca2d()<swRad){
//                        crossPath = 2.0*sqrt(diff_of_squares(swRad,pca.dca2d()));
//                        swPca=pca.point2();
//                } else if ( pca.dca2d()<(swRad+tolerance) ) {
//                        crossPath = 0.1*swRad;
//                        swPca=pca.point2();
//                }
//                if (crossPath>0.0) {
//                        //crossPath/=sin(GetCellDirection().angle(dir));
//                        double cos2t = GetCellDirection().cos2Theta(dir);
//                        if(cos2t<0.999) {
//                                crossPath/=sqrt(1.0-cos2t);
//                        }
//                }
//        }
//        return crossPath;
//}

} // namespace cdch
