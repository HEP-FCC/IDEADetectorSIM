// interface to manage the geometries of the CDCH cells
//
// Original author G. Tassielli
//

#include "ROGeometryHandle.hh"
#include <utilFunctions.hh>

namespace svx {

void ROGeometryHandle::SelectLadder(int Layer, int PhiSec, int Ladder) {
  bool change = false;
  bool isFw = false;

  if (Layer!=_fLayer) {
    _fLayer = Layer;
    _lay = svxtracker->getLayer(_fLayer);
    change = true;
  }
  if (PhiSec!=_fPhiSec) {
    _fPhiSec = PhiSec;
    change = true;
  }
  if (Ladder!=_fLadder) {
    _fLadder = Ladder;
    change = true;
  }
  if (change) {
    isFw = (_lay->getLayerZone()==Layer::forward);
    if (isFw){ _fLadderID = _fLadder*_lay->nPhiSectors() + _fPhiSec;  }
    else { _fLadderID = _fPhiSec*_lay->nLaddersPerSector() + _fLadder; }
    _lad = _lay->getLadder(_fLadderID);

    _matrx = _lad->get3DTransfrom();
    _invmatrx = _lad->get3DInvTransfrom();
  }

}

void ROGeometryHandle::SelectRO(int Layer, int PhiSec, int Ladder, int fChFstSd, int fChSndSd, int fSubShell) {
  bool change = false;
  bool isFw = false;
  if (Layer!=_fLayer) {
    _fLayer = Layer;
    _lay = svxtracker->getLayer(_fLayer);
    change = true;
  }
  if (PhiSec!=_fPhiSec) {
    _fPhiSec = PhiSec;
    change = true;
  }
  if (Ladder!=_fLadder) {
    _fLadder = Ladder;
    change = true;
  }
  if (change) {
    isFw = (_lay->getLayerZone()==Layer::forward);
    if (isFw){ _fLadderID = _fLadder*_lay->nPhiSectors() + _fPhiSec;  }
    else { _fLadderID = _fPhiSec*_lay->nLaddersPerSector() + _fLadder; }
    _lad = _lay->getLadder(_fLadderID);
  }

  if (fSubShell!=_fSubShell) {
    _fSubShell = fSubShell;
    change = true;
  }
  if (fChFstSd!=_fChFstSd) {
    _fChFstSd = fChFstSd;
    change = true;
  }
  if (fChSndSd!=_fChSndSd) {
    _fChSndSd = fChSndSd;
    change = true;
  }

  if (change) {
    _fROChanID = computeChannelID(_fChFstSd,_fChSndSd,_fSubShell); //_fChFstSd*_lad->nReadOutsSndSd() + _fChSndSd;
//std::cout<<"Layer "<<Layer<<" PhiSec "<<PhiSec<<" Ladder "<<Ladder<<" fChFstSd "<<fChFstSd<<" fChSndSd "<<fChSndSd<<std::endl;
//std::cout<<"nReadOutsSndSd "<<_lad->nReadOutsSndSd()<<std::endl;
//std::cout<<"_fROChanID "<<_fROChanID<<std::endl;
    _matrx = _lad->get3DTransfrom();
    _invmatrx = _lad->get3DInvTransfrom();
    _fROChMidLcl = _lad->getROChLoclMidPoint(_fROChanID);
    Local2Global(_fROChMidLcl,_fROChMidGbl);
  }

}

void ROGeometryHandle::SelectRODet(unsigned long det) {
  if (det<1e+17) {
        // Return the Layer
        int fLayer = det/1e+13;
        det -= ((fLayer)*1e+13);
        --fLayer;
//                _lay = svxtracker->getLayer(fLayer);

        //Return the Ladder
        int fPhiSec = det/1e+11;
        det -= fPhiSec*1e+11;
        --fPhiSec;

        int fLadder = det/1e+9;
        det -= fLadder*1e+9;
        --fLadder;
//                fLadder += fPhiSec*(_lay->nLaddersPerSector());

        //Return the Channel RO
        int fSubShell = det/1e+8;
        det -= fSubShell*1e+8;
        --fSubShell;

        int fChFstSd = det/1e+4;
        det -= fChFstSd*1e+4;
        --fChFstSd;

        int fChSndSd = det;

        //Call the upper method
        /*ROGeometryHandle::*/SelectRO(fLayer,fPhiSec,fLadder,fChFstSd,fChSndSd,fSubShell);
  } else {
      exc::exceptionG4 e("GEOM","Waring",4);
      e<<"Requestesd Channel ID exceed the maximum allowed number\n";
      e.error();
  }

}

void ROGeometryHandle::SelectRO(int chambID, int channelID) {

//          _chambID=chambID;

  // Return the Layer
  int fLayer = chambID/1e+4;
  chambID -= ((fLayer)*1e+4);
  --fLayer;
  //                _lay = svxtracker->getLayer(fLayer);

  //Return the Ladder
  int fPhiSec = chambID/1e+2;
  chambID -= fPhiSec*1e+2;
  --fPhiSec;

  int fLadder = chambID;
  --fLadder;

  //Return the channelID RO
  int fSubShell = channelID/1e+8;
  channelID -= fSubShell*1e+8;
  --fSubShell;

  int fChFstSd = channelID/1e+4;
  channelID -= fChFstSd*1e+4;
  --fChFstSd;

  int fChSndSd = channelID;


  /*ROGeometryHandle::*/SelectRO(fLayer,fPhiSec,fLadder,fChFstSd,fChSndSd,fSubShell);
}

unsigned long ROGeometryHandle::computeDet(int Layer, int PhiSec, int Ladder, int ChFstSd,int ChSndSd, int subShell) {
        unsigned long det = 0;
        if (subShell>8) {
          exc::exceptionG4 e("GEOM","Error",4);
          e<<"Requestesd Sub Ladder Shell ID exceed the maximum allowed number (9)\n";
          e.error();
        }
        det = ((Layer+1)*1e+13) + ((PhiSec+1)*1e+11) + ((Ladder+1)*1e+9) + ((subShell+1)*1e+8) + ((ChFstSd+1)*1e+4) + ChSndSd ;
        return det;
}

unsigned long ROGeometryHandle::computeDet(int chambID, int channelID) {
        unsigned long det = 0;
        det = ((chambID/*+1*/)*1e+9) + channelID ;
        return det;
}

int ROGeometryHandle::computeChambID(int Layer, int PhiSec, int Ladder) {
        int chambID = 0;
        chambID = (Layer+1)*10000 + (PhiSec+1)*100 + Ladder+1;
        return chambID;
}

int ROGeometryHandle::computeChannelID(int ChFstSd,int ChSndSd, int subShell) {
        int channelID = 0;
        if (subShell>8) {
          exc::exceptionG4 e("GEOM","Error",4);
          e<<"Requestesd Sub Ladder Shell ID exceed the maximum allowed number (9)\n";
          e.error();
        }
        channelID = (subShell+1)*1e+8 + (ChFstSd+1)*10000 + ChSndSd;
        return channelID;
}

void ROGeometryHandle::Global2Local(double *global, double *local)
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

void ROGeometryHandle::Global2Local(HepGeom::Point3D<double> &global, HepGeom::Point3D<double> &local)
{
        tmpGlobal.set(global.x(),global.y(),global.z());
        tmpLocal=_invmatrx*tmpGlobal;
        local.set(tmpLocal.x(),tmpLocal.y(),tmpLocal.z());
}

void ROGeometryHandle::Local2Global(double *local, double *global)
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

void ROGeometryHandle::Local2Global(HepGeom::Point3D<double> &local, HepGeom::Point3D<double> &global)
{
        tmpLocal.set(local.x(),local.y(),local.z());
        tmpGlobal=_matrx*tmpLocal;
        global.set(tmpGlobal.x(),tmpGlobal.y(),tmpGlobal.z());
}

float ROGeometryHandle::GetLayerOutsideRad()
{
        return (float) _lay->getDetail()->OuterRadius();
}

float ROGeometryHandle::GetLayerInsideRad()
{
        return (float) _lay->getDetail()->InnerRadius();
}

const CLHEP::Hep3Vector ROGeometryHandle::GetROChanCenterLcl() const {
        return _fROChMidLcl;
}

const CLHEP::Hep3Vector ROGeometryHandle::GetROChanCenterGbl() const {
        return _fROChMidGbl;
}

const CLHEP::Hep3Vector ROGeometryHandle::GetROLayerMidPoint() {
        HepGeom::Point3D<double> ladGblMid(0.0,0.0,0.0);
        HepGeom::Point3D<double> ladLclMid(0.0,_fROChMidLcl.y(),0.0);
        Local2Global(ladLclMid,ladGblMid);
        return ladGblMid;
}

double ROGeometryHandle::GetROChanFstSdDim() const {
        return _lad->getReadOut()->getPitch();
}

double ROGeometryHandle::GetROChanSndSdDim() const {
        return _lad->getReadOut()->getLength();
}

const CLHEP::Hep3Vector ROGeometryHandle::GetLadderMidPoint() const {
        return _lad->getMidPoint();
}

const CLHEP::Hep3Vector ROGeometryHandle::GetLadderFstSdDir() const {
        return _lad->getFstSdDirection();
}

const CLHEP::Hep3Vector ROGeometryHandle::GetLadderSndSdDir() const {
        return _lad->getSndSdDirection();
}

bool ROGeometryHandle::isFwLayer() const {
        return (_lay->getLayerZone()==Layer::forward);
}

bool ROGeometryHandle::isVertLayer() const {
        return (_lay->getLayerOrient()==Layer::vertical);
}

} // namespace svx
