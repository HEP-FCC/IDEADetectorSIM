// PSHWtracker manager class
//
// Original author G. Tassielli
//

#ifndef PSHWtracker_hh
#define PSHWtracker_hh

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

namespace pshw {

class PSHWtracker: public svx::SVXtracker /*Detector*/ {

        friend class PSHWMaker;

public:
        PSHWtracker();
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

} //namespace pshw

#endif /* PSHWtracker_hh */
