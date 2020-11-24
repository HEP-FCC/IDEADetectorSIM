// SVXtracker manager class
//
// Original author G. Tassielli
//

#ifndef SVXtracker_hh
#define SVXtracker_hh

#include <deque>
#include <vector>
#include <map>
#include <iterator>
#include <memory>
#include <string>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

//#include "Layer.hh"
//#include "ROGeometryHandle.hh"

#include "Detector.hh"

namespace svx {

class Layer;
class ROGeometryHandle;

class SVXtracker: public Detector {

        friend class SVXMaker;

public:
        SVXtracker(std::string name="SVXtracker");
        ~SVXtracker() {}

//        enum SensorType        { Pixel=0, Strip };

        double r0()            const { return _r0;}
        double z0()            const { return _z0;}
        double rOut()          const { return _rOut;}

        std::string extFile()  const { return _extFile; }
        bool isExternal()      const { return _isExternal; }

        int nLayers()          const { return _nLayers; }
        int nFwdLayers()       const { return _nFwdLayers; }

        double zHalfLength()   const { return _zHalfLength;}

        int geomType()         const { return _geomType; }

//        SensorType sensType()  const { return _sensType; }

        ROGeometryHandle* getROGeometryHandle() const;

        Layer* getLayer(int n) const /*throw(cet::exception)*/;

        boost::shared_array<Layer> getLayersArray() const {
                return _lr;
        }

protected:

        // Nominal values.
        // _r0 = Nominal radius of the center of the sector.
        // _z0 = position of the center of the tracker relative to the origin
        double _r0;
        double _z0;

        // Outer radius of a logical volume that will just contain the entire tracker.
        double _rOut;

        // Name of external gdml geometry file description.
        std::string _extFile;
        bool _isExternal;

        int _nLayers;
        int _nFwdLayers;

        double _zHalfLength;

        //decimal digit:magior version, unit digit:minor version  00: Dummy (Simplified Si tracker, just cylindrical shell for layer)

        int _geomType;

        //Sensor type: 0:Pixel, 1:Strip
//        SensorType _sensType;

        boost::shared_array<Layer> _lr;

//        std::unique_ptr<ROGeometryHandle> _roChhnd;
        boost::shared_ptr<ROGeometryHandle> _roChhnd;

};

} //namespace svx

#endif /* SVXtracker_hh */
