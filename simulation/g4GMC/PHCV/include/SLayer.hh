// PHCV slayer description
//
// Original author G. Tassielli
//

#ifndef SLayer_hh
#define SLayer_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "manageException.hh"
#include "SLayerDetail.hh"
#include "SLayerId.hh"

// PHCV includes
#include "FbrWr.hh"

namespace phcv {

class SLayer{

  friend class Shell;
  friend class PHCVtcounter;
  friend class PHCVMaker;


public:

  enum SLtype {undefined=-1, scint, radiat};

//  // A free function, returning void, that takes a const Cell& as an argument.
//  typedef void (*CellFunction)( const SLayer& s);

  SLayer();

  // Constructor using fiber/wire/layer info.
  SLayer( SLayerId& id,
         boost::shared_ptr<SLayerDetail> &detail,
         int detailIndex,
         std::vector<boost::shared_ptr<FbrWr> > &fbrWrs
         );

  ~SLayer ();

  const SLayerId& Id() const { return _id;}

  boost::shared_ptr<SLayerDetail> getDetail() const { return _detail;}

  /*const*/ SLtype getSLayerType() const { return _layerType; }

  int nFbrWrs() const { return _nFbrWr; }

  double voxelizationFactor() const { return _voxelizationFactor; }

  boost::shared_ptr<FbrWr> getFbrWr( int n ) const /*throw(cet::exception)*/ {
        if (n<0 || n>=_nFbrWr){
          exc::exceptionG4 e("GEOM","Fatal Error",0);
          e<< "Field FbrWr number: "<< n <<" not present in "<<_id;
          e.error();
        }
        return _FieberWires.at(n);
  }

  boost::shared_ptr<FbrWr> getFbrWr( const FbrWrId& id ) const {
    return getFbrWr(id.getFbrWr());
  }

protected:

  // Identifier
  SLayerId _id;

  // Detailed description of a layer.
  boost::shared_ptr<SLayerDetail> _detail;
  int _detailIndex;

  SLtype _layerType;
  int _nFbrWr;

  double _voxelizationFactor;

  // Pointers to the fibers and wires in this layer.
  std::vector<boost::shared_ptr<FbrWr> > _FieberWires;

  void addFbrWr(FbrWr *tmpFbrWr){
          _FieberWires.push_back(boost::shared_ptr<FbrWr> (tmpFbrWr));
          _nFbrWr++;
  }


};

}  //namespace phcv

#endif /* SLayer_hh */
