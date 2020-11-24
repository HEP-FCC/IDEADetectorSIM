// CDCH layer description
//
// Original author G. Tassielli
//

#include <Layer.hh>

#ifndef __CINT__

using namespace std;

namespace cdch {

  Layer::Layer():
    _id(LayerId()),
    _detailIndex(0),
    _layerType(undefined),
    _nCells(0),
    _nFiledWires(0),
    _voxelizationFactor(2),
    _cells(0x0),
    _fieldWires(0x0)
  {
  }

  Layer::Layer(LayerId& id,
                         boost::shared_ptr<LayerDetail> &detail,
                         int detailIndex,
                         std::vector<boost::shared_ptr<Cell> > &cells,
                         std::vector<boost::shared_ptr<Wire> > &fieldWires

               ):
    _id(id),
    _detailIndex(detailIndex),
    _layerType(undefined),
    _voxelizationFactor(2),
    _cells(cells),
    _fieldWires(fieldWires)
  {
          _detail=detail;

          _nCells=cells.size();
          _nFiledWires=fieldWires.size();
//          _cells.clear();
//          std::vector<Cell*>::iterator icell;
//          icell=cells.begin();
//          while ( icell!=cells.end() ){
//                  _cells.push_bach(boost::shared_ptr<Cell>(*icell));
//                  ++icell;
//          }
//          _fieldWires.clear();
//          std::vector<Wire*>::iterator iwire;
//          iwire=fieldWires.begin();
//          while ( iwire!=fieldWires.end() ){
//                _fieldWires.push_back(boost::shared_ptr<Wire>(*iwire));
//                  ++iwire;
//          }
          _cells=cells;
          _fieldWires=fieldWires;

  }

  Layer::~Layer(){
//                try {
//                        std::vector<const Cell*>::iterator icell;
//                        icell=_cells.begin();
//                        while ( icell!=_cells.end() ){
//                                delete (*icell);
//                                ++icell;
//                        }
//                        std::vector<const Wire*>::iterator iwire;
//                        iwire=_fieldWires.begin();
//                        while ( iwire!=_fieldWires.end() ){
//                                delete (*iwire);
//                                ++iwire;
//                        }
//                } catch (cet::exception e) {
//                    throw cet::exception("GEOM")
//                        << "Error during deleting layer data \n";
//                }

          //delete _detail;
          //std::cout<<_id<<std::endl;

  }

} // namespace cdch

#endif

