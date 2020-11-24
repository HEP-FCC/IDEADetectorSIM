// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#ifndef CellGeometryHandle_v4_hh
#define CellGeometryHandle_v4_hh

#include "CellGeometryHandle.hh"
#include "CDCHtracker.hh"

namespace cdch {

class CellGeometryHandle_v4 : public CellGeometryHandle{

    friend class CDCHMaker;

protected:
    CellGeometryHandle_v4(CDCHtracker *cdchtr=0x0);

public:

    ~CellGeometryHandle_v4();

    using CellGeometryHandle::canIntersectInZ;

    virtual void  SelectCell(int SupLayer, int CelLayer, int Cell, bool isUpstrm=false);
    virtual void  SelectCell(int absRadID, int Cell, bool isUpstrm=false);
    virtual int   computeAbsRadID(int SupLayer, int CelLayer, bool isUpstrm=false);
    virtual bool  canIntersectInZ(float &zCorss, float &distWires, int compAbsRadID, int compICell, bool compIsUpstrm=false) const;
    virtual bool  canIntersectInZ(float &zCorss, float &distWires, int compSupLayer, int compCelLayer, int compCell, bool compIsUpstrm=false) const;

protected:
    const CDCHtracker *_cdchtr;
    void  SelectCell();

private:
    // no copying:
    CellGeometryHandle_v4( CellGeometryHandle_v4 const & );
    void operator = ( CellGeometryHandle_v4 const & );

};

}

#endif /* CellGeometryHandle_v4_hh */
