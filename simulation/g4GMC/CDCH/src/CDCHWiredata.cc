// CDCH wire description
//
// Original author G. Tassielli
//

#include "CDCHWiredata.hh"

CDCHWiredata::CDCHWiredata(){
        PosMatrix = new TObjArray(0);
        NcelLayer = 0;
        epsilon = 0x0;
        alfa = 0x0;
        radius_z0 = 0x0;
}

CDCHWiredata::~CDCHWiredata(){
        PosMatrix->Delete();

        NcelLayer = 0;
        delete [] epsilon;
        delete [] alfa;
        delete [] radius_z0;
}
