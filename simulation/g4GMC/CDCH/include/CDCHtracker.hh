// CDCHtracker manager class
//
// Original author G. Tassielli
//

#ifndef CDCHtracker_hh
#define CDCHtracker_hh

#include <deque>
#include <vector>
#include <map>
#include <iterator>
#include <memory>

#include <boost/shared_array.hpp>

//#include "SuperLayer.hh"
#include "Wall.hh"
//#include "Mu2eInterfaces/inc/Detector.hh"
//#include "TrackerGeom/inc/Tracker.hh"

#include "CellGeometryHandle.hh"

#include "Detector.hh"

namespace cdch {

//class Wall;
class SuperLayer;

class CDCHtracker: public Detector {

        friend class CDCHMaker;

public:
        CDCHtracker();
        ~CDCHtracker() {}

//        virtual std::string name() const { return "CDCHtracker";}

        enum CellType          { Hexagonal=2, Square };
        enum EnCapType         { Plane, Spherical };

        bool isDumbbell()      const { return _isDumbbell; }
        double* zZonesLimits() const { return _zZonesLimits.get(); }

        double r0()            const { return _r0;}
        double z0()            const { return _z0;}
        double rOut()          const { return _rOut;}
        int nSWire()           const { return _nSWire;}
        int nSDeltaWire()      const { return _nSDeltaWire;}
        int nRing()            const { return _nRing;}

        std::string extFile()  const { return _extFile; }
        bool isExternal()      const { return _isExternal; }

        int nSuperLayers()     const { return _nSuperLayers; }

        double zHalfLength()   const { return _zHalfLength;}

        double maxEndCapDim()  const { return _max_EndCap_dim; }

        int geomType()    const { return _geomType; }

        int maxNLayPerSL() const {
          if (_geomType==20) return 100;
          else if (_geomType==50||_geomType==60) return 10;
          else return 1;
        }

        CellType cellType()    const { return _cellType; }

        EnCapType endcapType() const { return _endcapType; }

        int getNWalls()        const { return _nWalls; }

        bool displayGasLayer() const { return _displayGasLayer; }
        bool displayWires()    const { return _displayWires; }

        CellGeometryHandle* getCellGeometryHandle() const { return _cellhnd.get(); }

        SuperLayer* getSuperLayer(int n) const /*throw(cet::exception)*/;

        boost::shared_array<SuperLayer> getSuperLayersArray() const {
                return _sprlr;
        }

// FAKE methods for Compatibility into Mu2e framework
//        const Straw& getStraw ( const StrawId& sid )      const throw(cet::exception) {
//                throw cet::exception("GEOM")<< "Fake method \"getStraw ( StrawId )\", not used for the CDCHtracker";
//                return fakeStraw;
//        }
////        const Straw& getStraw ( StrawIndex i )      const { return fakeStraw; }
//        const Straw&  getStraw ( StrawIndex i )            const {
//                getCellGeometryHandle()->SelectCellDet(i.asUint());
//                return dynamic_cast<Straw&>( *(getCellGeometryHandle()->GetCell().get()) );
//        }
//        const std::deque<Straw>& getAllStraws()           const throw(cet::exception) {
//               /* if (fakeStrawDeq.empty()) {
//                        for ( int iS=0; iS<_nSuperLayers; iS++) {
//                                for ( int iL=0; iL<_sprlr[iS].nLayers(); iL++ ) {
//                                        for ( int iC=0; iC<_sprlr[iS].getLayer(iL)->nCells(); iC++ ) {
//                                                fakeStrawDeq.push_back( static_cast<mu2e::Straw>( (*(_sprlr[iS].getLayer(iL)->getCell(iC).get())) ) );
//                                        }
//                                }
//                        }
//                }*/
//                throw cet::exception("GEOM")<< "Fake method \"getAllStraws()\", not used for the CDCHtracker";
//                return fakeStrawDeq;
//        }
//        const std::vector<StrawDetail>& getStrawDetails() const throw(cet::exception) {
//                throw cet::exception("GEOM")<< "Fake method \"getStrawDetails()\", not used for the CDCHtracker";
//                return fakeStrawVec;
//        }

//        const boost::shared_ptr<Wall> getWall(int n) throw(cet::exception);

        boost::shared_ptr<std::multimap<Wall::Walltype,boost::shared_ptr<Wall> > > getWalls() const {
                return _walls;
        }

protected:

        // Nominal values.
        // _r0 = Nominal radius of the center of the sector.
        // _z0 = position of the center of the tracker relative to the origin
        double _r0;
        double _z0;
        int _nSWire;
        int _nSDeltaWire;
        int _nRing;
        bool _isDumbbell;
        std::unique_ptr<double> _zZonesLimits;
        int _nOuterShell;

        // Outer radius of a logical volume that will just contain the entire tracker.
        double _rOut;

        // Name of external gdml geometry file description.
        std::string _extFile;
        bool _isExternal;

        int _nSuperLayers;
        int _nWalls;

        double _zHalfLength;
        double _max_EndCap_dim;

        //decimal digit:magior version, unit digit:minor version  20:Hexagonal (ILC 4-th),
        //                                         30:Square with constant cell dimension,
        //                                         31:Like 30 but with base fw in both directions and shared between two consecutive layers, and layers staggered
        //                                         41:Like 31 but with cell dimension that increases with radius
        //                                         42:Like 41 but (final Mu2e/MEG layout) (different cell staggering)
        //                                         50:Like 42 but organized in SuperLayer (the number of wire increase at each Superlayer)
        //                                         60:Like 50 but divided in inner and outer DCH and the outer is organized in group of trackers
        // if a new one is added please check maxNLayPerSL method
        int _geomType;

        //Cell geometry type: 2:Hexagonal, 3:Square
        CellType _cellType;

        //EndCap shape type: 0 plane, 1 spherical
        EnCapType _endcapType;

        //Allow to display the gas (and/or every wires inside gas) inside the chamber
        bool _displayGasLayer;
        bool _displayWires;

        boost::shared_array<SuperLayer> _sprlr;

        boost::shared_ptr<std::multimap<Wall::Walltype,boost::shared_ptr<Wall> > > _walls;

        void addWall(Wall *wall);

        std::unique_ptr<CellGeometryHandle> _cellhnd;

        //std::deque<Straw> fakeStrawDeq;

private:
        std::multimap <Wall::Walltype,boost::shared_ptr<Wall> >::iterator _walls_it;
        int _lastSeenWall; //last extracted wall from the _walls container using the getWall method

// FAKE members for Compatibility into Mu2e framework
//        const Straw fakeStraw;
//        const std::deque<Straw> fakeStrawDeq;
//        const std::vector<StrawDetail>  fakeStrawVec;
};

} //namespace cdch

#endif /* CDCHtracker_hh */
