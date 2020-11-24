// SVX layer description
//
// Original author G. Tassielli
//

#include "Layer.hh"

#ifndef __CINT__

using namespace std;

using CLHEP::Hep3Vector;

namespace svx {

Layer::Layer():
                  _id(LayerId()),
                  _layerOrnt(horizontal),
                  _layerZone(barrel),
                  _nLadders(0),
                  _ladders(*(new std::vector< boost::shared_ptr<Ladder> >(0)))
{
}

Layer::Layer(LayerId& id, boost::shared_ptr<LayerDetail> &detail,
            LyrOrient layerOrnt, LyrZone layerZone):
                  _id(id),
                  _layerOrnt(layerOrnt),
                  _layerZone(layerZone),
                  _nLadders(0),
                  _ladders(*(new std::vector< boost::shared_ptr<Ladder> >(0)))
{
  _detail=detail;
}

Layer::Layer(LayerId& id, boost::shared_ptr<LayerDetail> &detail, std::vector< boost::shared_ptr<Ladder> > &ladders,
             LyrOrient layerOrnt, LyrZone layerZone):
                  _id(id),
                  _layerOrnt(layerOrnt),
                  _layerZone(layerZone),
                  _ladders(ladders)
{
  _detail=detail;
  _nLadders = ladders.size();
}

Layer::Layer( int& id, boost::shared_ptr<LayerDetail> &detail,
    LyrOrient layerOrnt, LyrZone layerZone):
                  _id(LayerId(id)),
                  _layerOrnt(layerOrnt),
                  _layerZone(layerZone),
                  _nLadders(0),
                  _ladders(*(new std::vector< boost::shared_ptr<Ladder> >(0)))
{
  _detail=detail;
}

Layer::Layer( int& id, boost::shared_ptr<LayerDetail> &detail, std::vector< boost::shared_ptr<Ladder> > &ladders,
    LyrOrient layerOrnt, LyrZone layerZone):
                  _id(LayerId(id)),
                  _layerOrnt(layerOrnt),
                  _layerZone(layerZone),
                  _ladders(ladders)
{
  _detail=detail;
  _nLadders = ladders.size();
}

Layer::~Layer(){
  //         for ( std::vector<Ladder*>::iterator j=_ladders.begin(); j != _ladders.end(); j++){
  //                 delete (*j);
  //         }
}

} // namespace svx

#endif

