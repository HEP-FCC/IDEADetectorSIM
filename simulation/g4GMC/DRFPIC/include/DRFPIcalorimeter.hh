// DRFPIcalorimeter manager class
//
// Original author G. Tassielli
//

#ifndef DRFPIcalorimeter_hh
#define DRFPIcalorimeter_hh

#include <deque>
#include <vector>
#include <map>
#include <iterator>
#include <memory>

#include <boost/shared_array.hpp>

#include "Detector.hh"

namespace drc {

class Tower;
//class ROGeometryHandle;

class DRFPIcalorimeter: public Detector {

        friend class DRFPICMaker;

public:
        DRFPIcalorimeter();

        double z0()            const { return _z0;}

        std::string extFile()  const { return _extFile; }
        bool isExternal()      const { return _isExternal; }

        int nTower()              const { return _nTower;}
        double zHalfLength()      const { return _zhalfLength;}
        double zInHalfLength()      const { return _zInHalfLength;}

		double innerR()         const { return _innerR; }
		double outerR()         const { return _outerR; }
		double tower_height()   const { return _tower_height; }
		double endc_holeAng()   const { return _endc_holeAng; }
		int NbOfBarrel()        const { return _NbOfBarrel; }
		int DltNbB_E()          const { return _DltNbB_E; }
		int NbOfEndcap()        const { return _NbOfEndcap; }
		int NbOfZRot()          const { return _NbOfZRot; }
		double PMTT()           const { return _PMTT; }

        int geomType()         const { return _geomType; }

//        ROGeometryHandle* getROGeometryHandle() const;

        Tower* getTower(int n) const /*throw(cet::exception)*/;

        boost::shared_array<Tower> getTowersArray() const {
                return _tower;
        }

//        double getShellThick(int n)  const;
//        double getShellRadIn(int n)  const;
//        double getRadThick(int n)    const;
//        double getScintThick(int n)  const;

protected:

        double _z0;

        // Name of external gdml geometry file description.
        std::string _extFile;
        bool _isExternal;

        int _nTower;
        double _zhalfLength;
        double _zInHalfLength;

		double _innerR;
		double _outerR;
		double _tower_height; 
		double _endc_holeAng;
		int _NbOfBarrel;
		int _DltNbB_E;
		int _NbOfEndcap;
		int _NbOfZRot;
		double _PMTT;

        //decimal digit:magior version, unit digit:minor version  00: Dummy (Simplified Si tracker, just cylindrical shell for layer)
        int _geomType;

        boost::shared_array<Tower> _tower;

//        boost::shared_ptr<ROGeometryHandle> _roChhnd;

//        std::vector<double> _shellThick;
//        std::vector<double> _shellRadIn;
//        std::vector<double> _radThick;
//        std::vector<double> _scintThick;

};

} //namespace drc

#endif /* DRFPIcalorimeter_hh */
