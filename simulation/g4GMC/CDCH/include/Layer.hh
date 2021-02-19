// CDCH layer description
//
// Original author G. Tassielli
//

#ifndef CDCH_Layer_hh
#define CDCH_Layer_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

// Framework includes
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include "manageException.hh"

// CDCH includes
#include "LayerDetail.hh"
#include "LayerId.hh"
#include "Cell.hh"

namespace cdch {

class Layer{

  friend class SuperLayer;
  // friend class CDCHDevice;
  friend class CDCHtracker;
  friend class CDCHMaker;


public:

  enum Ltype {undefined=-1, wire, gas, serv};

  // A free function, returning void, that takes a const Cell& as an argument.
  typedef void (*CellFunction)( const Layer& s);

  Layer();

  // Constructor using cell/wire/layer info.
  Layer( LayerId& id,
         boost::shared_ptr<LayerDetail> &detail,
         int detailIndex,
         std::vector<boost::shared_ptr<Cell> > &cells,
         std::vector<boost::shared_ptr<Wire> > &fieldWires
         );

  ~Layer ();

  const LayerId& Id() const { return _id;}

  boost::shared_ptr<LayerDetail> getDetail() const { return _detail;}

  /*const*/ Ltype getLayerType() const { return _layerType; }

  int nCells() const { return _nCells; }

  int nFieldWires() const { return _nFiledWires; }

  double voxelizationFactor() const { return _voxelizationFactor; }

  boost::shared_ptr<Cell> getCell( int n ) const /*throw(cet::exception)*/ {
        //if (n>=0 && n<_nCells) return _cells.at(n);
        //else {//throw cet::exception("GEOM")<< "Cell number: "<< n <<" not present in "<<_id;
        if (n<0 || n>=_nCells) {
          exc::exceptionG4 e("GEOM","Fatal Error",0 );
          e<< "Cell number: "<< n <<" not present in "<<_id;
          e.error();
        }
        return _cells.at(n);
  }

  boost::shared_ptr<Cell> getCell( const CellId& id ) const {
    return getCell(id.getCell());
  }

  boost::shared_ptr<Wire> getFWire( int n ) const /*throw(cet::exception)*/ {
        //if (n>=0 && n<_nFiledWires) return _fieldWires.at(n);
        //else { //throw cet::exception("GEOM")<< "Field wire number: "<< n <<" not present in "<<_id;
        if (n<0 || n>=_nFiledWires){
          exc::exceptionG4 e("GEOM","Fatal Error",0);
          e<< "Field wire number: "<< n <<" not present in "<<_id;
          e.error();
        }
        return _fieldWires.at(n);
  }

  boost::shared_ptr<Wire> getFWire( const WireId& id ) const {
    return getFWire(id.getWire());
  }

protected:

  // Identifier
  LayerId _id;

  // Detailed description of a layer.
  boost::shared_ptr<LayerDetail> _detail;
  int _detailIndex;

  Ltype _layerType;
  int _nCells;
  int _nFiledWires;

  double _voxelizationFactor;

  // Pointers to the cells and field wires in this layer.
  std::vector<boost::shared_ptr<Cell> > _cells;
  std::vector<boost::shared_ptr<Wire> > _fieldWires;

  void addCell(Cell *cell){
          _cells.push_back(boost::shared_ptr<Cell>(cell));
          _nCells++;
  }

  void addFieldWire(Wire *tmpWire){
          _fieldWires.push_back(boost::shared_ptr<Wire> (tmpWire));
          _nFiledWires++;
  }


};

}  //namespace cdch

#endif /* CDCH_Layer_hh */
