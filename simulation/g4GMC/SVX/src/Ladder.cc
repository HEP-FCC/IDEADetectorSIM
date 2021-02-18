// SVX ladder description
//
// Original author G. Tassielli
//

#include "Ladder.hh"

#ifndef __CINT__

using namespace std;

namespace svx {

  Ladder::Ladder():
    _id(LadderId()),
//    _detailIndex(0),
    _ladderType(pixel),
    _ladderGeomType(plane),
    _MidPoint(CLHEP::Hep3Vector(0.,0.,0.)),
//    _Direction(CLHEP::Hep3Vector(0.,0.,1.)),
    _pos(0x0),
    _invpos(HepGeom::Transform3D()),
    _nReadOuts(0),
    _nReadOutsFstSd(0),
    _nReadOutsSndSd(0),
    _nReadOutsFstSd2(0),
    _nSndSdDltRO(0),
    _nSndSdAddDltRO(0)
//    _voxelizationFactor(2),
//    _ros(0x0)
  {
  }

  Ladder::Ladder(LadderId& id,
                         boost::shared_ptr<LadderDetail> &detail,
//                         int detailIndex,
//                         CLHEP::Hep3Vector &center,
//                         CLHEP::Hep3Vector &direction,
                         HepGeom::Transform3D *pos,
//                         std::vector<boost::shared_ptr<ReadOut> > &ros,
                         boost::shared_ptr<ReadOut> &ros,
                         Ltype ladderType,
                         Lgtype ladderGType
               ):
    _id(id),
//    _detailIndex(detailIndex),
    _ladderType(ladderType),
    _ladderGeomType(ladderGType),
    _pos(pos)
//    _MidPoint(center),
//    _Direction(direction)
//    _voxelizationFactor(2),
  {
    _detail=detail;
    _MidPoint.set(_pos->dx(), _pos->dy(), _pos->dz());
    _invpos=_pos->inverse();
//    _Direction.set(_pos->xz(),_pos->yz(),_pos->zz());
//    _Direction=_pos->getRotation().getAxis();
//    _nReadOuts=ros.size();
    _nReadOuts=0;
    _nReadOutsFstSd=0;
    _nReadOutsSndSd=0;
    _nReadOutsFstSd2=0;
    _nSndSdDltRO=0;
    _nSndSdAddDltRO=0;
    _ros=ros;

  }

  Ladder::~Ladder(){
  }

  void Ladder::setPosMatrix( HepGeom::Transform3D &lPos ) {
    _pos = new HepGeom::Transform3D (lPos);
    _MidPoint.set(_pos->dx(), _pos->dy(), _pos->dz());
    _invpos=_pos->inverse();
    //_FstSdDirection=(*_pos)*HepGeom::Point3D<double>(1,0,0)-_MidPoint;
    //_FstSdDirection = _FstSdDirection.unit();
    //_FstSdDirection.set(_pos->xx(),_pos->yx(),_pos->zx());
    _FstSdDirection = _pos->getRotation().colX();
    //_SndSdDirection=(*_pos)*HepGeom::Point3D<double>(0,0,1)-_MidPoint;
    //_SndSdDirection = _SndSdDirection.unit();
    //_SndSdDirection.set(_pos->xz(),_pos->yz(),_pos->zz());
    _SndSdDirection = _pos->getRotation().colZ();
  }

  const CLHEP::Hep3Vector& Ladder::getROChLoclMidPoint(unsigned long id)  const {
	  //    cout<<"In getROChLoclMidPoint"<<endl;
	  //    cout<<_id<<endl;
	  //    cout<<"getROChLoclMidPoint of "<<id<<endl;
	  if (_ros->selectChannel(id)){
		  unsigned long idX=_ros->getSelChFstSd(); //id/_nReadOutsSndSd;
		  unsigned long idZ=_ros->getSelChSndSd(); //id-idX*_nReadOutsSndSd;
		  //      double chX=0.0;
		  //      double chZ=0.0;
		  double frstSdWidth = _detail->width();
		  if (isWedgeType()) {
			  /*if(_nSndSdDltRO!=0) {
          unsigned long nDltROAdd=(idZ/_nSndSdDltRO);
          frstSdWidth += nDltROAdd*(_ros->getPitch()+_ros->getFstSideInsul());
          if (_nSndSdAddDltRO!=0) {
            frstSdWidth += (nDltROAdd/_nSndSdAddDltRO)*(_ros->getPitch()+_ros->getFstSideInsul());
          }
        }*/
			  frstSdWidth += idZ*_nSndSdDltRO*(_ros->getPitch()+_ros->getFstSideInsul());
			  if (_nSndSdAddDltRO!=0) {
				  frstSdWidth += idZ/_nSndSdAddDltRO*(_ros->getPitch()+_ros->getFstSideInsul());
			  }
		  }
		  double chX = 0.5*frstSdWidth-( (2*idX+1)*_ros->getPitch()*0.5 + (idX+1)*_ros->getFstSideInsul() );
		  double chY = _detail->shellYPos(_ros->getSelSubShell());//-0.5*_detail->thickness()+0.5*_detail->shellThickness(_ros->getSelSubShell());
		  double chZ = _detail->halfLength()-( (2*idZ+1)*_ros->getLength()*0.5 + (idZ+1)*_ros->getSndSideInsul() );
		  if (isWedgeType()) {
			  //chY *= -1.0;
			  chZ *= -1.0;
		  }
		  _ros->_tmpMidPoint.set(chX,chY,chZ);
	  }
	  return _ros->_tmpMidPoint;
  }

} // namespace svx

#endif
