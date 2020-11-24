// PHCVtcounter manager class
//
// Original author G. Tassielli
//

#ifndef PHCVtcounter_hh
#define PHCVtcounter_hh

#include <deque>
#include <vector>
#include <map>
#include <iterator>
#include <memory>

#include <boost/shared_array.hpp>

#include "Detector.hh"

namespace phcv {

class Shell;
//class ROGeometryHandle;

class PHCVtcounter: public Detector {

        friend class PHCVMaker;

public:
        PHCVtcounter();

        double z0()            const { return _z0;}

        std::string extFile()  const { return _extFile; }
        bool isExternal()      const { return _isExternal; }

        int nShell()              const { return _nShell;}
        double zHalfLength()      const { return _zhalfLength;}

        int geomType()         const { return _geomType; }

//        ROGeometryHandle* getROGeometryHandle() const;

        Shell* getShell(int n) const /*throw(cet::exception)*/;

        boost::shared_array<Shell> getShellsArray() const {
                return _shell;
        }

        double getShellThick(int n)  const;
        double getShellRadIn(int n)  const;
        double getRadThick(int n)    const;
        double getScintThick(int n)  const;

protected:

        double _z0;

        // Name of external gdml geometry file description.
        std::string _extFile;
        bool _isExternal;

        int _nShell;
        double _zhalfLength;

        //decimal digit:magior version, unit digit:minor version  00: Dummy (Simplified Si tracker, just cylindrical shell for layer)
        int _geomType;

        boost::shared_array<Shell> _shell;

//        boost::shared_ptr<ROGeometryHandle> _roChhnd;

        std::vector<double> _shellThick;
        std::vector<double> _shellRadIn;
        std::vector<double> _radThick;
        std::vector<double> _scintThick;

};

} //namespace phcv

#endif /* PHCVtcounter_hh */
