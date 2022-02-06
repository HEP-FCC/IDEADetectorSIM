// TDCHtracker manager class
//
// Original author G. Tassielli
//

#ifndef TDCHtracker_hh
#define TDCHtracker_hh

#include <deque>
#include <vector>
#include <map>
#include <iterator>
#include <memory>

#include <boost/shared_array.hpp>

#include "Wall.hh"
#include "CellGeometryHandle.hh"

#include "Detector.hh"

//namespace cdch {
//class Wall;
//}

namespace tdch {

class Station;
//class CellGeometryHandle;

class TDCHtracker: public Detector {

        friend class TDCHMaker;

public:
        TDCHtracker();
        ~TDCHtracker() {}

        enum CellType          { Square=2, Cylind };
        enum EnCapType         { Plane, Spherical };

        double r0()            const { return _r0;}
        double z0()            const { return _z0;}
        double rOut()          const { return _rOut;}
        int nSWire()           const { return _nSWire;}
//        int nSDeltaWire()      const { return _nSDeltaWire;}
        int nLayer()           const { return _nLayer;}
        int nStation()         const { return _nStations;}
        int nPlane()           const { return _nPlane;}
        int nPanel()           const { return _nPanel;}

        std::string extFile()  const { return _extFile; }
        bool isExternal()      const { return _isExternal; }

        double zHalfLength()   const { return _zHalfLength;}

        double maxEndCapDim()  const { return _max_EndCap_dim; }

        int geomType()         const { return _geomType; }

        int maxNPlnPerSttn() const {
//          if (_geomType==20) return 100;
          return 100;
        }

        CellType cellType()    const { return _cellType; }

        EnCapType endcapType() const { return _endcapType; }

        bool displayGasLayer() const { return _displayGasLayer; }
        bool displayWires()    const { return _displayWires; }

        cdch::CellGeometryHandle* getCellGeometryHandle() const { return _cellhnd.get(); }

        Station* getStation(int n) const /*throw(cet::exception)*/;

        boost::shared_array<Station> getStationsArray() const {
                return _station;
        }

        boost::shared_ptr<std::multimap<cdch::Wall::Walltype,boost::shared_ptr<cdch::Wall> > > getWalls() const {
                return _walls;
        }

protected:

        double _r0;                   //Nominal Inner radius of the tracker
        double _rOut;                 //Nominal Outer radius of the tracker - Outer radius of a logical volume that will just contain the entire tracker
        double _z0;                   //Shift along z of the center of the tracker

        double _cellDimension;        //Cell dimension in the case of Square cells
        int _nSWire;                  //Number of sense wire of a panel layer
        int _nShift;                  //n. of cell quarter shift between the cell layers
        int _nLayer;                  //Number of cell layers  per Panel
        int _nPanel;                  //Number of Panel per Plane
        int _nMeasPlane;              //Number of (active) Planes per Station
        double  _planeRot;            //rotation angle between consecutive planes in a Station
        int _nSrvPlane;               //Number of (inactive/for service) Planes per Station
        int _nPlane;                  //Number of Planes per Station
        int _nStations;               //Number of Station
        double  _stationRot;          //rotation angle between consecutive Stations

        int _nWalls;


        // Name of external gdml geometry file description.
        std::string _extFile;
        bool _isExternal;

        double _zHalfLength;

        bool _hasWalls;
        double _max_EndCap_dim;

        //decimal digit:magior version, unit digit:minor version  20:Square cell all wires,
        //                                         30:Square with foils instead of field wires,
        //                                         40:Cylindrical, straw tube tracker
        // if a new one is added please check maxNPlnPerSttn method
        int _geomType;

        //Cell geometry type: 2:Square, 3:Cylind
        CellType _cellType;

        //EndCap shape type: 0 plane, 1 spherical
        EnCapType _endcapType;

        //Allow to display the gas (and/or every wires inside gas) inside the chamber
        bool _displayGasLayer;
        bool _displayWires;

        boost::shared_array<Station> _station;

        boost::shared_ptr<std::multimap<cdch::Wall::Walltype,boost::shared_ptr<cdch::Wall> > > _walls;

        void addWall(cdch::Wall *wall);

        std::unique_ptr<cdch::CellGeometryHandle> _cellhnd;

private:
        std::multimap <cdch::Wall::Walltype,boost::shared_ptr<cdch::Wall> >::iterator _walls_it;
        int _lastSeenWall; //last extracted wall from the _walls container using the getWall method

};

} //namespace tdch

#endif /* TDCHtracker_hh */
