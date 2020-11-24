// CDCH superlayer description
//
// Original author G. Tassielli
//

#ifndef SuperLayer_hh
#define SuperLayer_hh

//#include <Layer.hh>
#include <deque>
#include <vector>

#include "SuperLayerId.hh"
#include "SuperLayerInfo.hh"

#include "manageException.hh"

#include <boost/shared_ptr.hpp>

namespace cdch {

class Layer;
class LayerId;

class SuperLayer{

  friend class Layer;
  friend class CDCHtracker;
  friend class CDCHMaker;

public:

  // A free function, returning void, that takes a const Layer& as an argument.
  typedef void (*SuperLayerFunction)( const SuperLayer& s);

  SuperLayer();

  SuperLayer(SuperLayerId& id);

  SuperLayer(SuperLayerId&   id,
                  std::vector< boost::shared_ptr<Layer> > &layer);

  SuperLayer( int &id);

  SuperLayer( int &id,
                  std::vector< boost::shared_ptr<Layer> > &layer);

  ~SuperLayer ();

  const SuperLayerId& Id() const { return _id;}

  int nLayers() const { return _nLayers; }

  boost::shared_ptr<Layer> getSwLayer( int n ) const;

  boost::shared_ptr<Layer> getLayer( int n ) const;
//  {
//    //if (n>=0 && n<_nLayers) return _layers.at(n);
//    //else { //throw  cet::exception("GEOM")<< "Layer number: "<< n <<" not present in "<<_id;
//    if (n<0 || n>=_nLayers) {
//      exc::exceptionG4 e("GEOM","Fatal Error",0);
//      e<< "Layer number: "<< n <<" not present in "<<_id;
//      e.error();
//    }
//    return _layers.at(n);
//  }

  boost::shared_ptr<Layer> getLayer( LayerId& id ) const;
//  {
//    return getLayer(id.getLayer());
//  }

  const std::vector< boost::shared_ptr<Layer> >& getLayers() const;
//  {
//    return _layers;
//  }

//  SuperLayer& operator=(const SuperLayer &sl) {
//          if (this!=&sl) {
//                  _id = sl.Id();
//                  _nLayers = sl.nLayers();
//                  _layers = sl.
//          }
//  }

protected:

  SuperLayerId _id;

  int _nLayers;

  std::vector< boost::shared_ptr<Layer> > _layers;
  std::vector<int> _swLayPos;

  void addLayer(Layer *cdchl, bool isSwLy=false);
//  {
//          _layers.push_back(boost::shared_ptr<Layer>(cdchl));
//          _nLayers++;
//  }

};
}

#endif /* SuperLayer_hh */
