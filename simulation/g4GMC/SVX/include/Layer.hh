// SVX superlayer description
//
// Original author G. Tassielli
//

#ifndef Layer_hh
#define Layer_hh

#include <LayerId.hh>
#include <LayerDetail.hh>
#include <Ladder.hh>
//#include <LadderId.hh>

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "manageException.hh"

namespace pshw {
class PSHWMaker;
}

namespace svx {

class Layer{

  friend class Ladder;
  friend class SVXtracker;
  friend class SVXMaker;

  friend class pshw::PSHWMaker;

public:

  enum LyrOrient {horizontal=0, vertical};
  enum LyrZone {barrel=0, forward}; //redundant information, generally is equivalent to the Layer Orientation

  // A free function, returning void, that takes a const Layer& as an argument.
  typedef void (*LayerFunction)( const Layer& s);

  Layer();

  Layer(LayerId& id,
      boost::shared_ptr<LayerDetail> &detail,
      LyrOrient layerOrnt=horizontal,
      LyrZone layerZone=barrel
      );

  Layer(LayerId&   id,
      boost::shared_ptr<LayerDetail> &detail,
      std::vector< boost::shared_ptr<Ladder> > &ladder,
      LyrOrient layerOrnt=horizontal,
      LyrZone layerZone=barrel
      );

  Layer( int &id,
      boost::shared_ptr<LayerDetail> &detail,
      LyrOrient layerOrnt=horizontal,
      LyrZone layerZone=barrel
      );

  Layer( int &id,
      boost::shared_ptr<LayerDetail> &detail,
      std::vector< boost::shared_ptr<Ladder> > &ladder,
      LyrOrient layerOrnt=horizontal,
      LyrZone layerZone=barrel
  );

  ~Layer ();

  const LayerId& Id() const { return _id;}

  boost::shared_ptr<LayerDetail> getDetail() const { return _detail;}

  /*const*/ LyrOrient getLayerOrient() const { return _layerOrnt; }
  /*const*/ LyrZone getLayerZone() const { return _layerZone; }

  unsigned long nLadders() const { return _nLadders; }
  int nPhiSectors() const { return _nPhiSectors; }
  int nLaddersPerSector() const { return _nLaddersPerSector; }

  boost::shared_ptr<Ladder> getLadder( unsigned long n ) const {
    //if (n>=0 && n<_nLadders) return _ladders.at(n);
    //else { //throw  cet::exception("GEOM")<< "Ladder number: "<< n <<" not present in "<<_id;
    if (/*n<0 ||*/ n>=_nLadders) {
      exc::exceptionG4 e("GEOM","Fatal Error",0);
      e<< "Ladder number: "<< n <<" not present in "<<_id;
      e.error();
    }
    return _ladders.at(n);
  }

  boost::shared_ptr<Ladder> getLadder( LadderId& id ) const {
    return getLadder(id.getLadder());
  }

  const std::vector< boost::shared_ptr<Ladder> >& getLadders() const {
    return _ladders;
  }

//  Layer& operator=(const Layer &sl) {
//          if (this!=&sl) {
//                  _id = sl.Id();
//                  _nLadders = sl.nLadders();
//                  _ladders = sl.
//          }
//  }

protected:

  LayerId _id;

  // Detailed description of a layer.
  boost::shared_ptr<LayerDetail> _detail;

  LyrOrient _layerOrnt;
  LyrZone _layerZone;

  unsigned long _nLadders;
  int _nPhiSectors;
  int _nLaddersPerSector;

  std::vector< boost::shared_ptr<Ladder> > _ladders;

  void addLadder(Ladder *svxld){
          _ladders.push_back(boost::shared_ptr<Ladder>(svxld));
          _nLadders++;
  }

};

}  // namespace svx

#endif /* Layer_hh */
