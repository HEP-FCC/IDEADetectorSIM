#ifndef Detector_hh
#define Detector_hh

//
// A base class for detector components to be stored in GeomService.
//
// Original author G. F. Tassielli
//


#include <string>

class Detector
{
public:
  Detector(std::string name="");
  virtual ~Detector();
  const std::string& name() {return _name;}
protected:
  std::string _name;
};

#endif /* Detector_hh */
