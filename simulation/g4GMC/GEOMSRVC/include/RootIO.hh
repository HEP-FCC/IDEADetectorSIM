//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file persistency/P01/include/RootIO.hh
/// \brief Definition of the RootIO class
//
// $Id: RootIO.hh 71791 2013-06-24 14:08:28Z gcosmo $
#ifndef INCLUDE_ROOTIO_HH 
#define INCLUDE_ROOTIO_HH 1

// Include files
//#include "TROOT.h"
#include "TFile.h"
//#include "TSystem.h"
#include "TTree.h"
//#include "TBranch.h"
//#include "TClonesArray.h"

#include <map>
#include <string>
#include "GMCG4Particle.hh"
#include "GMCG4TrackerHit.hh"



/** @class RootIO
 *   
 *
 *  @author Witold POKORSKI
 *  @date   2005-10-27
 *  modified by G.F: Tassielli
 */

/// Root IO implementation for the persistency

class RootIO {
public: 

  virtual ~RootIO();
  
  static RootIO* GetInstance(Int_t runN=0, TString outFold=".", bool hasDRFPIC=false);
  void Close();
//  void CreateTreeObject();
  void CreateMCStepBranches(std::string , const char *);
  void CreateDRCaloBranches();
  void FillEvent();  
//  void PutMCStepHitCh(std::vector<GMCG4TrackerHit*>&);
//  void PutMCStepHitPx(std::vector<GMCG4TrackerHit*>&);
  void PutMCStepHit(std::string , std::vector<GMCG4TrackerHit*>&);
//  void PutMCTracks(TClonesArray &);
  void PutMCTracks(std::vector<GMCG4Particle*>&);

protected:
  RootIO(Int_t runN=0, TString outFold=".", bool hasDRFPIC=false);
  
private:

  void CreateTreeObject();
  bool fHasDRFPIC;

  TFile* fFile;
  int fNevents;

//  std::vector<GMCG4TrackerHit*> fHitsVectorCh;
//  std::vector<GMCG4TrackerHit*> fHitsVectorPx;
  std::map<std::string,std::vector<GMCG4TrackerHit*>  > fHitsVector;
  std::vector<GMCG4Particle*> fTracks;
  
  TTree *fMCStep;
  TTree *fMCTracks;
  TTree *fDRCalo;
};
#endif // INCLUDE_ROOTIO_HH
