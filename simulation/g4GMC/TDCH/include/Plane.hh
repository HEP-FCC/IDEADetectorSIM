// TDCH slayer description
//
// Original author G. Tassielli
//

#ifndef Plane_hh
#define Plane_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "manageException.hh"
#include "Panel.hh"
#include "PlaneDetail.hh"
#include "PlaneId.hh"

namespace tdch {

class Plane{

  friend class Station;
  friend class TDCHtracker;
  friend class TDCHMaker;


public:

  enum PLtype {undefined=-1, meas, serv};

//  // A free function, returning void, that takes a const Cell& as an argument.
//  typedef void (*CellFunction)( const Plane& s);

  Plane();

  // Constructor using Panel/wire/layer info.
  Plane( PlaneId& id,
         boost::shared_ptr<PlaneDetail> &detail,
         int detailIndex,
         std::vector<boost::shared_ptr<Panel> > &Pnls
         );

  ~Plane ();

  const PlaneId& Id() const { return _id;}

  boost::shared_ptr<PlaneDetail> getDetail() const { return _detail;}

  int nPanels() const { return _nPanels; }

  double voxelizationFactor() const { return _voxelizationFactor; }

  boost::shared_ptr<Panel> getPanel( int n ) const /*throw(cet::exception)*/ {
        if (n<0 || n>=_nPanels){
          exc::exceptionG4 e("GEOM","Fatal Error",0);
          e<< "Panel number: "<< n <<" not present in "<<_id;
          e.error();
        }
        return _Panels.at(n);
  }

  boost::shared_ptr<Panel> getPanel( const PanelId& id ) const {
    return getPanel(id.getPanel());
  }

protected:

  // Identifier
  PlaneId _id;

  // Detailed description of a layer.
  boost::shared_ptr<PlaneDetail> _detail;
  int _detailIndex;

  PLtype _planeType;
  int _nPanels;

  double _voxelizationFactor;

  // Pointers to the Panels and wires in this layer.
  std::vector<boost::shared_ptr<Panel> > _Panels;

  void addPanel(Panel *tmpPanel){
          _Panels.push_back(boost::shared_ptr<Panel> (tmpPanel));
          _nPanels++;
  }


};

}  //namespace tdch

#endif /* Plane_hh */
