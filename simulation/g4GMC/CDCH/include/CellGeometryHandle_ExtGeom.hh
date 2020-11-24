// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#ifndef CellGeometryHandle_ExtGeom_hh
#define CellGeometryHandle_ExtGeom_hh

#include "CellGeometryHandle.hh"
#include "CDCHWireposition.hh"

namespace cdch {

class CellGeometryHandle_ExtGeom : public CellGeometryHandle {

  friend class CDCHMaker;

protected:
  CellGeometryHandle_ExtGeom(const char *WireDataFile = "CDCHWireData.root");

public:

  ~CellGeometryHandle_ExtGeom();

  using CellGeometryHandle::SelectCell;
  using CellGeometryHandle::Global2Local;
  using CellGeometryHandle::Local2Global;
  using CellGeometryHandle::WirePosAtEndcap;
  using CellGeometryHandle::WirePosAtZ;
  using CellGeometryHandle::DistFromWire;
  using CellGeometryHandle::DistFromWireCenter;

  virtual void SelectCell(int SupLayer, int CelLayer, int Cell);
  virtual void SelectCellDet(unsigned long det);// Det Method
  virtual void Global2Local(double *global, double *local);
  virtual void Local2Global(double *local, double *global);
  virtual void WirePosAtEndcap(float *right, float *left);
  virtual void WirePosAtZ(float z, float *pos);
  virtual float GetWireAlfa();
  virtual float GetWireEpsilon();
  virtual float GetCellRad();
  virtual float GetCellInsideRad();

  virtual double DistFromWire(double *global);
  virtual double DistFromWireCenter(double *global);

  const TGeoHMatrix *GetGeoMatrix() { return _iWirePos->GetGeoMatrix(); }

private:

  CDCHWireposition *_iWirePos;

  Double_t tmpGlobal[3];
  Double_t tmpLocal[3];
  Float_t  tmpRight[3];
  Float_t  tmpLeft[3];
  Float_t  tmpPosAtZ[3];

};

}

#endif /* CellGeometryHandle_ExtGeom_hh */
