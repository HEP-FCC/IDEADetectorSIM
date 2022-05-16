// MUSPCRadiator manager class

#ifndef MUSPCRadiator_hh
#define MUSPCRadiator_hh

#include "Detector.hh"

#include <vector>

namespace muspc {

class MUSPCRadiator : public Detector {

  friend class MUSPCMaker;

public:
  MUSPCRadiator();
  ~MUSPCRadiator() {}

  std::vector<double> getRadiatorsThickness() const {
    return _RadiatorsThickness;
  }

  std::vector<int> getRadiatType() const {
    return _RadiatorType;
  }

  std::vector<double> getRadiatInRasius() const {
    return _RadiatInRasius;
  }

  std::vector<double> getRadiatOutRasius() const {
    return _RadiatOutRasius;
  }

  std::vector<double> getRadiatHalfLengths() const {
    return _RadiatHalfLengths;
  }

  std::vector<int> getRadiatNmShells() const {
    return _RadiatNmShells;
  }

  std::vector< std::vector<std::string> > getRadiatShellsMaterial() const {
    return _RadiatShellsMaterial;
  }

  std::vector< std::vector<double> > getRadiatShellsThick() const {
    return _RadiatShellsThick;
  }

  int getRadiatLayers() const {
    return _nRadiatLayers;
  }

protected:
  //Radiater material description
  int _nRadiatLayers;
  std::vector<int> _RadiatorType; //0: Barrel (Cylinder) 1: Forward (Disk)
  std::vector<double> _RadiatInRasius;
  std::vector<double> _RadiatOutRasius;
  std::vector<double> _RadiatHalfLengths;
  std::vector<int>    _RadiatNmShells;
  std::vector< std::vector<std::string> > _RadiatShellsMaterial;
  std::vector< std::vector<double> > _RadiatShellsThick;
  std::vector< double > _RadiatorsThickness;

};

} //namespace muspc

#endif /* MUSPCRadiator_hh */
