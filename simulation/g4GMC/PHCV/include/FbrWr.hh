// PHCV fiber and wire description
//
// Original author G. Tassielli
//

#ifndef FbrWr_hh
#define FbrWr_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "FbrWrDetail.hh"
#include "FbrWrId.hh"

namespace phcv {

class FbrWr{

        friend class PHCVtcounter;
        friend class PHCVMaker;


public:

        // A free function, returning void, that takes a const FbrWr& as an argument.
        typedef void (*FbrWrFunction)( const FbrWr& s);

        enum FWtype {undefined=-1, fiber, wire};

        FbrWr();

        // Constructor using wire 3d transformation.
        FbrWr( FbrWrId id,
                        boost::shared_ptr<FbrWrDetail> detail,
                        HepGeom::Transform3D *pos,
                        double epsilon,
                        double alpha,
                        FWtype wireType=undefined
        );

        ~FbrWr ();

        FbrWrId Id() const { return _id;}

        boost::shared_ptr<FbrWrDetail> getDetail() const { return _detail;}

        const HepGeom::Transform3D& get3DTransfrom() const {return *_pos;}

        const HepGeom::Transform3D& get3DInvTransfrom() const {return _invpos;}

        const CLHEP::Hep3Vector& getMidPoint() const {return _c;}

        const CLHEP::Hep3Vector& getDirection() const { return _w;}

        double getEpsilon() const { return _epsilon;}
        double getAlpha() const { return _alpha;}

        FWtype getFbrWrType() const { return _wireType; }

protected:

        // Identifier
        FbrWrId _id;

        FWtype _wireType;

        // Mid-point of the wire.
        CLHEP::Hep3Vector _c;

        // Detailed description of a wire.
        boost::shared_ptr<FbrWrDetail> _detail;

        // Unit vector along the wire direction.
        // Need to add unit vectors along local u and v also.
        // Use Euler angle convention from G4.
        CLHEP::Hep3Vector _w;

        const HepGeom::Transform3D *_pos;
        HepGeom::Transform3D _invpos;
        const double _epsilon;
        const double _alpha;

};

inline std::ostream& operator<<(std::ostream& ost, const FbrWr& w ){
        ost <<w.Id()<<" type "<<w.getFbrWrType()<<" radius "<< w.getDetail()->outerRadius() << " length "<< w.getDetail()->length() <<std::endl;
        ost<<"epsilon "<< w.getEpsilon()<<" pos matrix: "<<std::endl;
        ost<<w.get3DTransfrom().xx()<<" "<<w.get3DTransfrom().xy()<<" "<<w.get3DTransfrom().xz()<<" "<<w.get3DTransfrom().dx()<<std::endl;
        ost<<w.get3DTransfrom().yx()<<" "<<w.get3DTransfrom().yy()<<" "<<w.get3DTransfrom().yz()<<" "<<w.get3DTransfrom().dy()<<std::endl;
        ost<<w.get3DTransfrom().zx()<<" "<<w.get3DTransfrom().zy()<<" "<<w.get3DTransfrom().zz()<<" "<<w.get3DTransfrom().dz()<<std::endl;
        return ost;
}

}  //namespace phcv

#endif /* FbrWr_hh */
