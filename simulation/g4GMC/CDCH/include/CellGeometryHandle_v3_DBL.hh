// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#ifndef CellGeometryHandle_v3_DBL_hh
#define CellGeometryHandle_v3_DBL_hh

#include "CellGeometryHandle_v3.hh"
#include "CDCHtracker.hh"

namespace cdch {

class CellGeometryHandle_v3_DBL : public CellGeometryHandle_v3{

    friend class CDCHMaker;

protected:
    CellGeometryHandle_v3_DBL(CDCHtracker *cdchtr=0x0);

public:

    ~CellGeometryHandle_v3_DBL();

    using CellGeometryHandle_v3::canIntersectInZ;

    virtual void  SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
    virtual void  SelectCellDet(unsigned long det);
    virtual void  SelectCell(int absRadID, int Cell, bool isUpstrm=false);
    virtual unsigned long computeDet(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
    virtual int computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm=false);
    virtual const CLHEP::Hep3Vector& GetCellCenter() const;
    virtual const CLHEP::Hep3Vector& GetCellDirection() const;
    virtual double GetCellHalfLength() const;
    virtual void  SelectComp_Cell(int compSupLayer, int compCelLayer, int compICell, bool compIsUpStream=false);
    virtual void  SelectComp_CellDet(unsigned long compDet);
    virtual void  SelectComp_Cell(int compIAbsRadID, int compICell, bool compIsUpStream=false);
    virtual bool  canIntersectInZ(float &zCorss, float &distWires) const;

private:
    // no copying:
    CellGeometryHandle_v3_DBL( CellGeometryHandle_v3_DBL const & );
    void operator = ( CellGeometryHandle_v3_DBL const & );
    void  AdjustMatrix();
    int _nSLayer;
    double _DnStrmDeadWireLngt;
    double _UpStrmDeadWireLngt;
    double _tmpCellHalfLength;
    CLHEP::Hep3Vector _tmpDirection;
    CLHEP::Hep3Vector _tmpMidPoint;
    float wCntPos[3];

    inline void  Comp_AdjustMatrix();
    void  Comp_WirePosAtLength(float length, float *pos);
    bool _Comp_isUpStream;
    double _Comp_DnStrmDeadWireLngt;
    double _Comp_UpStrmDeadWireLngt;
    double _Comp_tmpCellHalfLength;
    CLHEP::Hep3Vector _Comp_tmpDirection;
    CLHEP::Hep3Vector _Comp_tmpMidPoint;
    float Comp_wCntPos[3];

};

}

#endif /* CellGeometryHandle_v3_DBL_hh */
