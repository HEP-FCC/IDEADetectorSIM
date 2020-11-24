// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#ifndef CellGeometryHandle_v3_hh
#define CellGeometryHandle_v3_hh

#include "CellGeometryHandle.hh"
#include "CDCHtracker.hh"

namespace cdch {

class CellGeometryHandle_v3 : public CellGeometryHandle{

    friend class CDCHMaker;

protected:
    CellGeometryHandle_v3(CDCHtracker *cdchtr=0x0);

public:

    ~CellGeometryHandle_v3();

    virtual void  SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
    virtual void  SelectCellDet(unsigned long det);
    virtual void  SelectCell(int absRadID, int Cell, bool isUpstrm=false);
    virtual unsigned long computeDet(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
    virtual void  SelectComp_Cell(int compSupLayer, int compCelLayer, int compICell, bool compIsUpStream=false);
    virtual void  SelectComp_CellDet(unsigned long compDet);
    virtual void  SelectComp_Cell(int compIAbsRadID, int compICell, bool compIsUpStream=false);
    virtual bool  canIntersectInZ(float &zCorss, float &distWires) const;
    virtual bool  canIntersectInZ(float &zCorss, float &distWires, unsigned long compDet) const;
    virtual bool  canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm=false) const;
    virtual bool  canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compCell, bool compIsUpstrm=false) const;

    virtual double CrossingPathOnFieldWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
                                            FWireSide &sideFlag, CLHEP::Hep3Vector &fwPca, double tolerance=0.002) const;
    virtual double CrossingPathOnSenseWires(CLHEP::Hep3Vector const &point, CLHEP::Hep3Vector const &dir,
                                            CLHEP::Hep3Vector &swPca, double tolerance=0.002) const;

protected:
    const CDCHtracker *_cdchtr;
    int _nLayer;

private:
    // no copying:
    CellGeometryHandle_v3( CellGeometryHandle_v3 const & );
    void operator = ( CellGeometryHandle_v3 const & );

};

}

#endif /* CellGeometryHandle_v3_hh */
