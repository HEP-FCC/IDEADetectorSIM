// CDCH superlayer description
//
// Original author G. Tassielli
//

#include "SuperLayer.hh"
#include "Layer.hh"

#ifndef __CINT__

using namespace std;

using CLHEP::Hep3Vector;

namespace cdch {

SuperLayer::SuperLayer():
                    _id(SuperLayerId()),
                    _nLayers(0),
                    _layers(*(new std::vector< boost::shared_ptr<Layer> >(0)))
                    {
                    }

SuperLayer::SuperLayer(SuperLayerId& id):
                        _id(id),
                        _nLayers(0),
                        _layers(*(new std::vector< boost::shared_ptr<Layer> >(0)))
                        {
                        }

SuperLayer::SuperLayer(SuperLayerId& id, std::vector< boost::shared_ptr<Layer> > &layers):
                        _id(id),
                        _layers(layers)
                        {
        _nLayers = layers.size();
                        }

SuperLayer::SuperLayer( int& id):
                        _id(SuperLayerId(id)),
                        _nLayers(0),
                        _layers(*(new std::vector< boost::shared_ptr<Layer> >(0)))
                        {
                        }

SuperLayer::SuperLayer( int& id, std::vector< boost::shared_ptr<Layer> > &layers):
                        _id(SuperLayerId(id)),
                        _layers(layers)
                        {
        _nLayers = layers.size();
                        }

SuperLayer::~SuperLayer(){
        //         for ( std::vector<Layer*>::iterator j=_layers.begin(); j != _layers.end(); j++){
        //                 delete (*j);
        //         }
}

boost::shared_ptr<Layer> SuperLayer::getSwLayer( int n ) const {
	  if (n<0 || n>=_nLayers) {
	    exc::exceptionG4 e("GEOM","Fatal Error",0);
	    e<< "Layer number: "<< n <<" not present in "<<_id;
	    e.error();
	  }
	  return _layers.at(_swLayPos.at(n));
}

boost::shared_ptr<Layer> SuperLayer::getLayer( int n ) const {
  //if (n>=0 && n<_nLayers) return _layers.at(n);
  //else { //throw  cet::exception("GEOM")<< "Layer number: "<< n <<" not present in "<<_id;
  if (n<0 || n>=_nLayers) {
    exc::exceptionG4 e("GEOM","Fatal Error",0);
    e<< "Layer number: "<< n <<" not present in "<<_id;
    e.error();
  }
  return _layers.at(n);
}

boost::shared_ptr<Layer> SuperLayer::getLayer( LayerId& id ) const {
  return getLayer(id.getLayer());
}

const std::vector< boost::shared_ptr<Layer> >& SuperLayer::getLayers() const {
  return _layers;
}

void SuperLayer::addLayer(Layer *cdchl, bool isSwLy){
        _layers.push_back(boost::shared_ptr<Layer>(cdchl));
        if (isSwLy) {_swLayPos.push_back(_nLayers);}
        _nLayers++;
}

} // namespace cdch

#endif

