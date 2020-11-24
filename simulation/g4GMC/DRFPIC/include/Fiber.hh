// DRFPI fiber description
//
// Original author G. Tassielli
//

#ifndef Fiber_hh
#define Fiber_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "FiberDetail.hh"
#include "FiberId.hh"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace drc {

class Fiber{

        friend class DRFPIcalorimeter;
        friend class DRFPICMaker;


public:

        // A free function, returning void, that takes a const Fiber& as an argument.
        typedef void (*FbrFunction)( const Fiber& s);

        enum Fbrtype {undefined=-1, scint, cher};

        Fiber();

        // Constructor using wire 3d transformation.
        Fiber( FiberId id,
                        boost::shared_ptr<FiberDetail> detail,
                        HepGeom::Transform3D *pos,
//                        double epsilon,
//                        double alpha,
                        Fbrtype fbrType=undefined
        );

        ~Fiber ();

        FiberId Id() const { return _id;}

        boost::shared_ptr<FiberDetail> getDetail() const { return _detail;}

        const HepGeom::Transform3D& get3DTransfrom() const {return *_pos;}

        const HepGeom::Transform3D& get3DInvTransfrom() const {return _invpos;}

        const CLHEP::Hep3Vector& getMidPoint() const {return _c;}

        const CLHEP::Hep3Vector& getDirection() const { return _w;}

//        double getEpsilon() const { return _epsilon;}
//        double getAlpha() const { return _alpha;}

        Fbrtype getFbrWrType() const { return _fbrType; }

protected:

        // Identifier
        FiberId _id;

        Fbrtype _fbrType;

        // Mid-point of the wire.
        CLHEP::Hep3Vector _c;

        // Detailed description of a wire.
        boost::shared_ptr<FiberDetail> _detail;

        // Unit vector along the wire direction.
        // Need to add unit vectors along local u and v also.
        // Use Euler angle convention from G4.
        CLHEP::Hep3Vector _w;

        const HepGeom::Transform3D *_pos;
        HepGeom::Transform3D _invpos;
//        const double _epsilon;
//        const double _alpha;

};

inline std::ostream& operator<<(std::ostream& ost, const Fiber& w ){
        ost <<w.Id()<<" type "<<w.getFbrWrType()<<" radius "<< w.getDetail()->outerRadius() << " length "<< w.getDetail()->length() <<std::endl;
        ost/*<<"epsilon "<< w.getEpsilon()*/<<" pos matrix: "<<std::endl;
        ost<<w.get3DTransfrom().xx()<<" "<<w.get3DTransfrom().xy()<<" "<<w.get3DTransfrom().xz()<<" "<<w.get3DTransfrom().dx()<<std::endl;
        ost<<w.get3DTransfrom().yx()<<" "<<w.get3DTransfrom().yy()<<" "<<w.get3DTransfrom().yz()<<" "<<w.get3DTransfrom().dy()<<std::endl;
        ost<<w.get3DTransfrom().zx()<<" "<<w.get3DTransfrom().zy()<<" "<<w.get3DTransfrom().zz()<<" "<<w.get3DTransfrom().dz()<<std::endl;
        return ost;
}

}  //namespace drc

#endif /* Fiber_hh */
