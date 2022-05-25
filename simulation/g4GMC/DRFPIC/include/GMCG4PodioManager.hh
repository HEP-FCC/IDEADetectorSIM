#ifndef GMCG4PodioManager_H
#define GMCG4PodioManager_H

#include <map>

#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

#include "globals.hh"

#include "G4AutoLock.hh"

namespace { G4Mutex GMCG4PodioActionMutex = G4MUTEX_INITIALIZER; }

class GMCG4PodioManager
{
 public:
  static GMCG4PodioManager * Instance();
  void SetFilePrefix (G4String filename){m_filename_prefix = filename;}
  void SetFileSuffix(G4String filename){m_filename_suffix = filename;}
  bool Finish();
  podio::EventStore * GetEvtStore();
  podio::ROOTWriter * GetWriter();
    

 protected:
  G4String m_filename_prefix;
  G4String m_filename_suffix; 
  std::map<int,podio::EventStore *> m_map_store;
  std::map<int,podio::ROOTWriter *> m_map_writer;  
 private:
  static GMCG4PodioManager * m_inst_;
  GMCG4PodioManager();

};

#endif 
