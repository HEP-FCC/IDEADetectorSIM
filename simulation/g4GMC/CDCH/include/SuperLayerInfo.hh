//
// Original author G. Tassielli
//

#ifndef SuperLayerInfo_hh
#define SuperLayerInfo_hh

namespace cdch {

class SuperLayerInfo{

  // The cells type
  enum Stype {undefined=-1, hexagonal, square};

public:
  SuperLayerInfo():
    _nLayers(-1),
    _cellType(undefined)
  {
  }
  SuperLayerInfo( int nLayers, Stype cellType
             ):
    _nLayers(nLayers),
    _cellType(cellType){
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

private:
  int _nLayers;
  Stype _cellType;

};

}  //namespace cdch

#endif /* SuperLayerInfo_hh */
