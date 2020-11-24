// PHCV superlayer description
//
// Original author G. Tassielli
//

#include "Shell.hh"

#include "SLayer.hh"

#ifndef __CINT__

using namespace std;

using CLHEP::Hep3Vector;

namespace phcv {

Shell::Shell():
                    _id(ShellId()),
                    _nLayers(0),
                    _layers(*(new std::vector< boost::shared_ptr<SLayer> >(0)))
                    {
                    }

Shell::Shell(ShellId& id):
                        _id(id),
                        _nLayers(0),
                        _layers(*(new std::vector< boost::shared_ptr<SLayer> >(0)))
                        {
                        }

Shell::Shell(ShellId& id, std::vector< boost::shared_ptr<SLayer> > &layers):
                        _id(id),
                        _layers(layers)
                        {
        _nLayers = layers.size();
                        }

Shell::Shell( int& id):
                        _id(ShellId(id)),
                        _nLayers(0),
                        _layers(*(new std::vector< boost::shared_ptr<SLayer> >(0)))
                        {
                        }

Shell::Shell( int& id, std::vector< boost::shared_ptr<SLayer> > &layers):
                        _id(ShellId(id)),
                        _layers(layers)
                        {
        _nLayers = layers.size();
                        }

Shell::~Shell(){
        //         for ( std::vector<Layer*>::iterator j=_layers.begin(); j != _layers.end(); j++){
        //                 delete (*j);
        //         }
}

boost::shared_ptr<SLayer> Shell::getSLayer( int n ) const {
  if (n<0 || n>=_nLayers) {
    exc::exceptionG4 e("GEOM","Fatal Error",0);
    e<< "Layer number: "<< n <<" not present in "<<_id;
    e.error();
  }
  return _layers.at(n);
}

//boost::shared_ptr<SLayer> Shell::getSLayer( SLayerId& id ) const {
//  return getSLayer(id.getLayer());
//}

const std::vector< boost::shared_ptr<SLayer> >& Shell::getSLayers() const {
  return _layers;
}

void Shell::addSLayer(SLayer *phcvl){
        _layers.push_back(boost::shared_ptr<SLayer>(phcvl));
        ++_nLayers;
}

} // namespace phcv

#endif

