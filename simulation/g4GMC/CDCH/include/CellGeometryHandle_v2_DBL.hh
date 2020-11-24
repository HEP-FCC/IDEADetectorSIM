// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#ifndef CellGeometryHandle_v2_DBL_hh
#define CellGeometryHandle_v2_DBL_hh

#include "CellGeometryHandle_v2.hh"
#include "CDCHtracker.hh"

namespace cdch {

class CellGeometryHandle_v2_DBL : public CellGeometryHandle_v2{

        friend class CDCHMaker;

protected:
        CellGeometryHandle_v2_DBL(CDCHtracker *cdchtr=0x0);

public:

        ~CellGeometryHandle_v2_DBL();

        virtual void  SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
        virtual void  SelectCellDet(unsigned long det);
        virtual void  SelectCell(int absRadID, int Cell, bool isUpstrm=false);
        virtual unsigned long computeDet(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
        virtual int computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm=false);
        virtual const CLHEP::Hep3Vector& GetCellCenter() const;
        virtual const CLHEP::Hep3Vector& GetCellDirection() const;
        virtual double GetCellHalfLength() const;

private:
        void  AdjustMatrix();

        int _nSLayer;
        double _DnStrmDeadWireLngt;
        double _UpStrmDeadWireLngt;
        double _tmpCellHalfLength;
        CLHEP::Hep3Vector _tmpDirection;
        CLHEP::Hep3Vector _tmpMidPoint;
        float wCntPos[3];

};

}

#endif /* CellGeometryHandle_v2_DBL_hh */
