//
// Original author G. Tassielli
//

#ifndef StationInfo_hh
#define StationInfo_hh

namespace tdch {

class StationInfo{

  // The cells type
  enum Stype {undefined=-1, square, straw};

public:
  StationInfo():
    _nPlanes(-1),
    _cellType(undefined)
  {
  }
  StationInfo( int nPlanes, Stype cellType
             ):
    _nPlanes(nPlanes),
    _cellType(cellType){
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

private:
  int _nPlanes;
  Stype _cellType;

};

}  //namespace tdch

#endif /* StationInfo_hh */
