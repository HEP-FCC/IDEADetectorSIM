// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#ifndef CellGeometryHandle_hh
#define CellGeometryHandle_hh

#include "Layer.hh"
#include "Cell.hh"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "manageException.hh"

#include <boost/shared_ptr.hpp>

namespace tdch {
class TDCHMaker;
}

namespace cdch {

//class Layer;
//class Cell;

class CellGeometryHandle {

    friend class CDCHMaker;
    friend class tdch::TDCHMaker;

protected:
        CellGeometryHandle() {
          _absRadID=0;
          _fWire=0;
          _fSuperLayer=0;
          _Comp_SuperLayer=0;
          _fLayer=0;

          _isDownStream=true;
          _isUpStream=false;

          _Comp_isDownStream=true;
          _Comp_isUpStream=false;
        }

public:

        enum FWireSide {noFWCross=-1, bottom, side, top};

        virtual ~CellGeometryHandle() {}

        virtual void  SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false) {
          //                throw cet::exception("GEOM")<< "SelectCell Method not implemented for the interface class CellGeometryHandle, please use one of the real implementation"<<std::endl;
          exc::exceptionG4 e("GEOM","Fatal Error",0);
          e<<"SelectCell Method not implemented for the interface class CellGeometryHandle, please use one of the real implementation \n";
          e<<"Ignored arguments "<<SupLayer<<" "<<CelLayer<<" "<<Cell<<" "<<isUpstrm<<"\n";
          e.error();
        }
        virtual void  SelectCellDet(unsigned long det) {
                // Return the SuperLayer
                int fSuperLayer=(int)(det*0.00001);

                //Return the Layer
                int fLayer=(int)((det)-((fSuperLayer)*100000));

                fLayer=(int)(fLayer*0.001);

                //Return the Wire
                int fWire=(int)(((det)-((fSuperLayer)*100000))-fLayer*1000);

                fSuperLayer--;

                //Call the upper method
                /*CellGeometryHandle::*/SelectCell(fSuperLayer,fLayer,fWire);
        }
        virtual void  SelectCell(int absRadID, int Cell, bool isUpstrm=false) {
                _absRadID=absRadID;
                /*CellGeometryHandle::*/SelectCell(absRadID,0,Cell,isUpstrm);
        }
        virtual unsigned long computeDet(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false) {
                unsigned long det = 0;
                if (!isUpstrm) { det = ((SupLayer+1)*100000)+((CelLayer)*1000)+(Cell);}
                else {
                  exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
                  e<<"CellGeometryHandle is not usable for Dumbbell configuration\n";
                  e.error();
                }
                return det;
        }
        virtual int computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm=false) {
                int absRadID = 0;
                if (!isUpstrm) { absRadID=SupLayer; }
                else {
                  exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
                  e<<"CellGeometryHandle is not usable for Dumbbell configuration\n";
                  e.error();
                }
                exc::exceptionG4 e("GEOM","Waring",4);
                e<<"CellGeometryHandle::computeAbsRadID CelLayer ignored: "<<CelLayer<<"\n";
                e.error();
                return absRadID;
        }
        virtual void  Global2Local(double *global, double *local) ;
        virtual void  Local2Global(double *local, double *global) ;
        virtual void  Global2Local(HepGeom::Point3D<double> &global, HepGeom::Point3D<double> &local) ;
        virtual void  Local2Global(HepGeom::Point3D<double> &local, HepGeom::Point3D<double> &global) ;
        virtual void  WirePosAtEndcap(float *right, float *left)  ;
        virtual void  WirePosAtZ(float z, float *pos)             ;
        virtual void  WirePosAtLength(float length, float *pos)   ;
        virtual void  WirePosAtEndcap(CLHEP::Hep3Vector &right, CLHEP::Hep3Vector &left)  ;
        virtual void  WirePosAtZ(float z, CLHEP::Hep3Vector &pos)             ;
        virtual void  WirePosAtLength(float length, CLHEP::Hep3Vector &pos)   ;
        virtual int   GetSuperLayer()                             { return _fSuperLayer; }
        virtual int   GetCelRing()                                { return _fLayer; }
        virtual int   GetWire()                                   { return _fWire; }
        virtual float GetWireAlfa()                               ;
        virtual float GetWireEpsilon()                            ;
        virtual float GetCellRad()                                ;
        virtual float GetCellInsideRad()                          ;
        virtual int   GetCellAbsRadID()                           { return _absRadID; }
        virtual const CLHEP::Hep3Vector& GetWireCenter() const    ;
        virtual const CLHEP::Hep3Vector& GetWireDirection() const ;
        virtual const CLHEP::Hep3Vector& GetCellCenter() const    ;
        virtual const CLHEP::Hep3Vector& GetCellDirection() const ;
        virtual double GetCellHalfLength() const                  ;
        virtual bool  canIntersectInZ(float &zCorss, float &distWires, unsigned long compDet) const;
        virtual bool  canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm=false) const;
        virtual bool  canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compICell, bool compIsUpstrm=false) const;
        virtual bool  canIntersectInZ(float &zCorss, float &distWires) const;

        virtual double DistFromWire(double *global)               ;
        virtual double DistFromWireCenter(double *global)         ;
        virtual double DistFromWire(CLHEP::Hep3Vector &global)            ;
        virtual double DistFromWireCenter(CLHEP::Hep3Vector &global)      ;
        virtual double DistFromWire(CLHEP::Hep3Vector const &global)      ;
        virtual double DistFromWireCenter(CLHEP::Hep3Vector const &global);

        virtual double CrossingPathOnFieldWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
                                                FWireSide &sideFlag, CLHEP::Hep3Vector &fwPca, double tolerance=0.002) const;
        virtual double CrossingPathOnSenseWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
                                                CLHEP::Hep3Vector &swPca, double tolerance=0.002) const;

        virtual boost::shared_ptr<Layer> GetLayer();//           { return _lay; }
        virtual boost::shared_ptr<Cell> GetCell();//               { return _cell; }
        bool isUpStream()   { return _isUpStream; }
        bool isDownStream() { return _isDownStream; }

        virtual void  SelectComp_Cell(int compSupLayer, int compCelLayer, int compICell, bool isUpstrm=false);

        virtual void  SelectComp_CellDet(unsigned long compDet) {
                // Return the SuperLayer
                int compSupLayer=(int)(compDet*0.00001);

                //Return the Layer
                int compCelLayer=(int)((compDet)-((compSupLayer)*100000));

                compCelLayer=(int)(compCelLayer*0.001);

                //Return the Wire
                int compICell=(int)(((compDet)-((compSupLayer)*100000))-compCelLayer*1000);

                compSupLayer--;

                //Call the upper method
                /*CellGeometryHandle::*/SelectCell(compSupLayer,compCelLayer,compICell);
        }
        virtual void  SelectComp_Cell(int compIAbsRadID, int compICell, bool isUpstrm=false) {
          /*CellGeometryHandle::*/SelectCell(compIAbsRadID,0,compICell,isUpstrm);
        }

protected:
        int       _fSuperLayer;
        int       _fLayer;
        int       _fWire;
        int       _absRadID;
        boost::shared_ptr<Layer> _lay;
        HepGeom::Transform3D _matrx;
        HepGeom::Transform3D _invmatrx;
        boost::shared_ptr<Cell> _cell;
        HepGeom::Point3D<double> tmpGlobal;
        HepGeom::Point3D<double> tmpLocal;
        HepGeom::Point3D<float>  tmpRight;
        HepGeom::Point3D<float>  tmpLeft;
        HepGeom::Point3D<float>  tmpPos;

        bool _isUpStream;
        bool _isDownStream;

        bool _Comp_isUpStream;
        bool _Comp_isDownStream;
        int  _Comp_SuperLayer;
        boost::shared_ptr<Cell> _Comp_cell;
        HepGeom::Transform3D _Comp_matrx;

};

}

#endif /* CellGeometryHandle_hh */
