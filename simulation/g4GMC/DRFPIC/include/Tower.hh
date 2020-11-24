// DRFPI slayer description
//
// Original author G. Tassielli
//

#ifndef Tower_hh
#define Tower_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "Fiber.hh"
#include "TowerDetail.hh"
#include "TowerId.hh"
#include "manageException.hh"

// #include "G4ThreeVector.hh"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
 
namespace drc {

class Tower{

  friend class DRFPIcalorimeter;
  friend class DRFPICMaker;


public:

  enum TWtype {undefined=-1, barrel, endcap};
  enum TWSide {undef=-1, right, left};

//  // A free function, returning void, that takes a const Cell& as an argument.
//  typedef void (*CellFunction)( const Tower& s);

  Tower();

  // Constructor using fiber/wire/layer info.
  Tower( TowerId& id
         ,boost::shared_ptr<TowerDetail> &detail
         ,int detailIndex
//         ,std::vector<boost::shared_ptr<Fiber> > &fbrWrs
         );

  ~Tower ();

  const TowerId& Id() const { return _id;}

  boost::shared_ptr<TowerDetail> getDetail() const { return _detail;}

  /*const*/ TWtype getTowerType() const { return _towerType; }

  /*const*/ TWSide getTowerSide() const { return _towerSide; }

//  int nFbrWrs() const { return _nFibers; }

  double voxelizationFactor() const { return _voxelizationFactor; }

//  boost::shared_ptr<Fiber> getFiber( int n ) const /*throw(cet::exception)*/ {
//        if (n<0 || n>=_nFibers){
//          exc::exceptionG4 e("GEOM","Fatal Error",0);
//          e<< "Field FbrWr number: "<< n <<" not present in "<<_id;
//          e.error();
//        }
//        return _Fiebers.at(n);
//  }

//  boost::shared_ptr<Fiber> getFiber( const FiberId& id ) const {
//    return getFiber(id.getFiber());
//  }
  
  double getDeltatheta() const { return _deltatheta; }
  double getThetaofcenter() const { return _thetaofcenter; }
  CLHEP::Hep3Vector *getPt() { return _pt; }
  CLHEP::HepRotation getRotM() { return _rotM; }
  CLHEP::Hep3Vector getCenter() const { return _center; }
  CLHEP::Hep3Vector *getV() { return _v; }
  double getInnerR_new() { return _innerR_new; }

protected:
  
  //  int _ndimension;
  // Identifier
  TowerId _id;

  // Detailed description of a layer.
  boost::shared_ptr<TowerDetail> _detail;
  int _detailIndex;

  TWtype _towerType;
  TWSide _towerSide;
//  int _nFibers;
  double _voxelizationFactor;

//  // Pointers to the fibers and wires in this layer.
//  std::vector<boost::shared_ptr<Fiber> > _Fiebers;
//
//  void addFbrWr(Fiber *tmpFbrWr){
//          _Fiebers.push_back(boost::shared_ptr<Fiber> (tmpFbrWr));
//          _nFibers++;
//  }

// added by Nicola
  double _deltatheta;
  double _thetaofcenter;
  CLHEP::Hep3Vector _pt[8];
  CLHEP::HepRotation _rotM;
  CLHEP::Hep3Vector _center;

  CLHEP::Hep3Vector _v[4];
  double _innerR_new;

};

}  //namespace drc

#endif /* Tower_hh */
