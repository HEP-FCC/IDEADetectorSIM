// PHCV superlayer description
//
// Original author G. Tassielli
//

#ifndef Shell_hh
#define Shell_hh

//#include <Layer.hh>
#include <deque>
#include <vector>

#include "manageException.hh"

#include <boost/shared_ptr.hpp>
#include "ShellId.hh"

namespace phcv {

class SLayer;
class SLayerId;

class Shell{

  friend class SLayer;
  friend class PHCVtcounter;
  friend class PHCVMaker;

public:

  // A free function, returning void, that takes a const Layer& as an argument.
  typedef void (*ShellFunction)( const Shell& s);

  Shell();

  Shell(ShellId& id);

  Shell(ShellId&   id,
                  std::vector< boost::shared_ptr<SLayer> > &layer);

  Shell( int &id);

  Shell( int &id,
                  std::vector< boost::shared_ptr<SLayer> > &layer);

  ~Shell();

  const ShellId& Id() const { return _id;}

  int nSLayers() const { return _nLayers; }

  boost::shared_ptr<SLayer> getSLayer( int n ) const;

//  boost::shared_ptr<SLayer> getSLayer( SLayerId& id ) const;

  const std::vector< boost::shared_ptr<SLayer> >& getSLayers() const;

protected:

  ShellId _id;

  int _nLayers;

  std::vector< boost::shared_ptr<SLayer> > _layers;

  void addSLayer(SLayer *phcvl);

};
}

#endif /* Shell_hh */
