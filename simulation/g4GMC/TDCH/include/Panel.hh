// TDCH fiber description
//
// Original author G. Tassielli
//

#ifndef Panel_hh
#define Panel_hh

#include <deque>
#include <vector>

#include <boost/shared_ptr.hpp>

// Framework includes
#include "manageException.hh"

// TDCH includes
#include "PanelDetail.hh"
#include "PanelId.hh"
#include "Cell.hh"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace tdch {

class Panel{

	friend class Plane;
	friend class TDCHtracker;
	friend class TDCHMaker;


public:

	enum Pntype {undefined=-1, allwire, foilwire, straw};

	// A free function, returning void, that takes a const Panel& as an argument.
	typedef void (*CellFunction)( const Panel& s);

	Panel();

	// Constructor using wire 3d transformation.
	Panel( PanelId id,
			boost::shared_ptr<PanelDetail> detail,
			HepGeom::Transform3D *pos,
			//                        double epsilon,
			//                        double alpha,
	         int detailIndex,
	         std::vector<boost::shared_ptr<Cell> > &cells,
	         std::vector<boost::shared_ptr<Wire> > &fieldWires
	);

	~Panel ();

	PanelId Id() const { return _id;}

	boost::shared_ptr<PanelDetail> getDetail() const { return _detail;}

	const HepGeom::Transform3D& get3DTransfrom() const {return *_pos;}

	const HepGeom::Transform3D& get3DInvTransfrom() const {return _invpos;}

	const CLHEP::Hep3Vector& getMidPoint() const {return _c;}

	const CLHEP::Hep3Vector& getDirection() const { return _w;}

	//        double getEpsilon() const { return _epsilon;}
	//        double getAlpha() const { return _alpha;}

	Pntype getPanelType() const { return _pnType; }

	  int nCells() const { return _nCells; }

	  int nFieldWires() const { return _nFiledWires; }

//	  double voxelizationFactor() const { return _voxelizationFactor; }

	  boost::shared_ptr<Cell> getCell( int n ) const /*throw(cet::exception)*/ {
	        //if (n>=0 && n<_nCells) return _cells.at(n);
	        //else {//throw cet::exception("GEOM")<< "Cell number: "<< n <<" not present in "<<_id;
	        if (n<0 || n>=_nCells) {
	          exc::exceptionG4 e("GEOM","Fatal Error",0 );
	          e<< "Cell number: "<< n <<" not present in "<<_id;
	          e.error();
	        }
	        return _cells.at(n);
	  }

	  boost::shared_ptr<Cell> getCell( const CellId& id ) const {
	    return getCell(id.getCell());
	  }

	  boost::shared_ptr<Wire> getFWire( int n ) const /*throw(cet::exception)*/ {
	        //if (n>=0 && n<_nFiledWires) return _fieldWires.at(n);
	        //else { //throw cet::exception("GEOM")<< "Field wire number: "<< n <<" not present in "<<_id;
	        if (n<0 || n>=_nFiledWires){
	          exc::exceptionG4 e("GEOM","Fatal Error",0);
	          e<< "Field wire number: "<< n <<" not present in "<<_id;
	          e.error();
	        }
	        return _fieldWires.at(n);
	  }

	  boost::shared_ptr<Wire> getFWire( const WireId& id ) const {
	    return getFWire(id.getWire());
	  }

protected:

	// Identifier
	PanelId _id;

	Pntype _pnType;

	// Mid-point of the wire.
	CLHEP::Hep3Vector _c;

	// Detailed description of a wire.
	boost::shared_ptr<PanelDetail> _detail;

	// Unit vector along the wire direction.
	// Need to add unit vectors along local u and v also.
	// Use Euler angle convention from G4.
	CLHEP::Hep3Vector _w;

	const HepGeom::Transform3D *_pos;
	HepGeom::Transform3D _invpos;
	//        const double _epsilon;
	//        const double _alpha;

	int _detailIndex;

	int _nCells;
	int _nFiledWires;

//	double _voxelizationFactor;

	// Pointers to the cells and field wires in this layer.
	std::vector<boost::shared_ptr<Cell> > _cells;
	std::vector<boost::shared_ptr<Wire> > _fieldWires;

	void addCell(Cell *cell){
		_cells.push_back(boost::shared_ptr<Cell>(cell));
		_nCells++;
	}

	void addFieldWire(Wire *tmpWire){
		_fieldWires.push_back(boost::shared_ptr<Wire> (tmpWire));
		_nFiledWires++;
	}

};

inline std::ostream& operator<<(std::ostream& ost, const Panel& pn ){
        ost <<pn.Id()<<" type "<<pn.getPanelType()<<" radius "<< pn.getDetail()->outerRadius() << " length "<< pn.getDetail()->length() <<std::endl;
        ost/*<<"epsilon "<< pn.getEpsilon()*/<<" pos matrix: "<<std::endl;
        ost<<pn.get3DTransfrom().xx()<<" "<<pn.get3DTransfrom().xy()<<" "<<pn.get3DTransfrom().xz()<<" "<<pn.get3DTransfrom().dx()<<std::endl;
        ost<<pn.get3DTransfrom().yx()<<" "<<pn.get3DTransfrom().yy()<<" "<<pn.get3DTransfrom().yz()<<" "<<pn.get3DTransfrom().dy()<<std::endl;
        ost<<pn.get3DTransfrom().zx()<<" "<<pn.get3DTransfrom().zy()<<" "<<pn.get3DTransfrom().zz()<<" "<<pn.get3DTransfrom().dz()<<std::endl;
        return ost;
}

}  //namespace tdch

#endif /* Panel_hh */
