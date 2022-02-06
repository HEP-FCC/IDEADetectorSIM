// MUSPCtracker manager class
//
// Original author G. Tassielli
//

#ifndef MUSPCtracker_hh
#define MUSPCtracker_hh

#include <deque>
#include <vector>
#include <map>
#include <iterator>
#include <memory>

#include <boost/shared_array.hpp>

//#include "Layer.hh"
//#include "ROGeometryHandle.hh"

//#include "Detector.hh"

#include "SVXtracker.hh"

namespace svx {
class Layer;
class ROGeometryHandle;
}

namespace muspc {

class MUSPCtracker: public svx::SVXtracker /*Detector*/ {

        friend class MUSPCMaker;

public:
        MUSPCtracker();
        double r0_Fw()            const { return _r0_fwd;}
        double zHalfLength_Fw()   const { return _halfLength_fwd;}
        double rOut_FW()          const { return _rOut_fwd;}
        double zPos_FW()          const { return _zPos_fwd;}

protected:
        double _r0_fwd;
        double _halfLength_fwd;
        double _rOut_fwd;
        double _zPos_fwd;

};

} //namespace muspc

#endif /* MUSPCtracker_hh */
