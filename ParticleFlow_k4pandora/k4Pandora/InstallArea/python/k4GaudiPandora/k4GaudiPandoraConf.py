#Tue May 24 15:37:24 2022"""Automatically generated. DO NOT EDIT please"""
import sys
if sys.version_info >= (3,):
    # Python 2 compatibility
    long = int
from GaudiKernel.DataHandle import DataHandle
from GaudiKernel.Proxy.Configurable import *

class PandoraPFAlg( ConfigurableAlgorithm ) :
  __slots__ = { 
    'ExtraInputs' : [], # list
    'ExtraOutputs' : [], # list
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : True, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'Blocking' : False, # bool
    'FilterCircularDependencies' : True, # bool
    'RootInTES' : '', # str
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'PrintEmptyCounters' : False, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
    'PandoraSettingsDefault_xml' : 'PandoraSettingsDefault.xml', # str
    'NEventsToSkip' : 0, # int
    'TrackCollections' : [ 'Tracks' ], # list
    'ECalCaloHitCollections' : [ 'ECALBarrel' , 'ECALEndcap' , 'ECALOther' ], # list
    'HCalCaloHitCollections' : [ 'HCALBarrel' , 'HCALEndcap' , 'HCALOther' ], # list
    'LCalCaloHitCollections' : [ 'LCAL' ], # list
    'LHCalCaloHitCollections' : [ 'LHCAL' ], # list
    'MuonCaloHitCollections' : [ 'MUON' ], # list
    'MCParticleCollections' : [ 'MCParticle' ], # list
    'RelCaloHitCollections' : [ 'RelationCaloHit' , 'RelationMuonHit' ], # list
    'RelTrackCollections' : [ 'RecoTrackerAssociation' ], # list
    'KinkVertexCollections' : [ 'KinkVertices' ], # list
    'ProngVertexCollections' : [ 'ProngVertices' ], # list
    'SplitVertexCollections' : [ 'SplitVertices' ], # list
    'V0VertexCollections' : [ 'V0Vertices' ], # list
    'ClusterCollectionName' : 'PandoraClusters', # str
    'PFOCollectionName' : 'PandoraPFOs', # str
    'ECalToMipCalibration' : 160.000, # float
    'HCalToMipCalibration' : 34.8000, # float
    'ECalMipThreshold' : 0.500000, # float
    'HCalMipThreshold' : 0.300000, # float
    'ECalToEMGeVCalibration' : 1.00700, # float
    'HCalToEMGeVCalibration' : 1.00700, # float
    'ECalToHadGeVCalibrationBarrel' : 1.12000, # float
    'ECalToHadGeVCalibrationEndCap' : 1.12000, # float
    'HCalToHadGeVCalibration' : 1.07000, # float
    'MuonToMipCalibration' : 10.0000, # float
    'DigitalMuonHits' : 0, # int
    'MaxHCalHitHadronicEnergy' : 1.00000, # float
    'UseOldTrackStateCalculation' : 0, # int
    'AbsorberRadLengthECal' : 0.285400, # float
    'AbsorberIntLengthECal' : 0.0101000, # float
    'AbsorberRadLengthHCal' : 0.0569000, # float
    'AbsorberIntLengthHCal' : 0.00600000, # float
    'AbsorberRadLengthOther' : 0.0569000, # float
    'AbsorberIntLengthOther' : 0.00600000, # float
    'StartVertexCollectionName' : 'PandoraPFANewStartVertices', # str
    'StartVertexAlgorithmName' : 'PandoraPFANew', # str
    'EMStochasticTerm' : 0.170000, # float
    'HadStochasticTerm' : 0.600000, # float
    'EMConstantTerm' : 0.0100000, # float
    'HadConstantTerm' : 0.0300000, # float
    'MuonHitEnergy' : 0.500000, # float
    'NOuterSamplingLayers' : 3, # int
    'LayersFromEdgeMaxRearDistance' : 250.000, # float
    'MuonBarrelBField' : -1.50000, # float
    'MuonEndCapBField' : 0.0100000, # float
    'ShouldFormTrackRelationships' : 1, # int
    'MinTrackHits' : 5, # int
    'MinFtdTrackHits' : 0, # int
    'MaxTrackHits' : 5000, # int
    'D0TrackCut' : 50.0000, # float
    'Z0TrackCut' : 50.0000, # float
    'UseNonVertexTracks' : 1, # int
    'UseUnmatchedNonVertexTracks' : 0, # int
    'UseUnmatchedVertexTracks' : 1, # int
    'UnmatchedVertexTrackMaxEnergy' : 5.00000, # float
    'D0UnmatchedVertexTrackCut' : 5.00000, # float
    'Z0UnmatchedVertexTrackCut' : 5.00000, # float
    'ZCutForNonVertexTracks' : 250.000, # float
    'ReachesECalNTpcHits' : 11, # int
    'ReachesECalNFtdHits' : 4, # int
    'ReachesECalTpcOuterDistance' : -100.000, # float
    'ReachesECalMinFtdLayer' : 9, # int
    'ReachesECalTpcZMaxDistance' : -50.0000, # float
    'ReachesECalFtdZMaxDistance' : -1.00000, # float
    'CurvatureToMomentumFactor' : 0.000150000, # float
    'MinTrackECalDistanceFromIp' : 100.000, # float
    'MaxTrackSigmaPOverP' : 0.150000, # float
    'MinMomentumForTrackHitChecks' : 1.00000, # float
    'TpcMembraneMaxZ' : 10.0000, # float
    'MinTpcHitFractionOfExpected' : 0.200000, # float
    'MinFtdHitsForTpcHitFraction' : 2, # int
    'MaxTpcInnerRDistance' : 50.0000, # float
    'ECalEndCapInnerSymmetryOrder' : 4, # int
    'ECalEndCapInnerPhiCoordinate' : 0.00000, # float
    'ECalEndCapOuterSymmetryOrder' : 8, # int
    'ECalEndCapOuterPhiCoordinate' : 0.00000, # float
    'HCalEndCapInnerSymmetryOrder' : 4, # int
    'HCalEndCapInnerPhiCoordinate' : 0.00000, # float
    'HCalEndCapOuterSymmetryOrder' : 16, # int
    'HCalEndCapOuterPhiCoordinate' : 0.00000, # float
    'HCalRingInnerSymmetryOrder' : 8, # int
    'HCalRingInnerPhiCoordinate' : 0.00000, # float
    'HCalRingOuterSymmetryOrder' : 16, # int
    'HCalRingOuterPhiCoordinate' : 0.00000, # float
    'StripSplittingOn' : False, # bool
    'UseEcalScLayers' : False, # bool
    'ECalSiToMipCalibration' : 1.00000, # float
    'ECalScToMipCalibration' : 1.00000, # float
    'ECalSiMipThreshold' : 0.00000, # float
    'ECalScMipThreshold' : 0.00000, # float
    'ECalSiToEMGeVCalibration' : 1.00000, # float
    'ECalScToEMGeVCalibration' : 1.00000, # float
    'ECalSiToHadGeVCalibrationEndCap' : 1.00000, # float
    'ECalScToHadGeVCalibrationEndCap' : 1.00000, # float
    'ECalSiToHadGeVCalibrationBarrel' : 1.00000, # float
    'ECalScToHadGeVCalibrationBarrel' : 1.00000, # float
    'InputEnergyCorrectionPoints' : [  ], # list
    'OutputEnergyCorrectionPoints' : [  ], # list
    'collections' : [  ], # list
    'WriteClusterCollection' : DataHandle('PandoraClusters', 'W', 'DataWrapper<edm4hep::ClusterCollection>'), # DataHandle
    'WriteReconstructedParticleCollection' : DataHandle('PandoraPFOs', 'W', 'DataWrapper<edm4hep::ReconstructedParticleCollection>'), # DataHandle
    'WriteVertexCollection' : DataHandle('PandoraPFANewStartVertices', 'W', 'DataWrapper<edm4hep::VertexCollection>'), # DataHandle
  }
  _propertyDocDct = { 
    'ExtraInputs' : """  [DataHandleHolderBase<PropertyHolder<CommonMessaging<implements<IAlgorithm,IDataHandleHolder,IProperty,IStateful> > > >] """,
    'ExtraOutputs' : """  [DataHandleHolderBase<PropertyHolder<CommonMessaging<implements<IAlgorithm,IDataHandleHolder,IProperty,IStateful> > > >] """,
    'OutputLevel' : """ output level [Gaudi::Algorithm] """,
    'Enable' : """ should the algorithm be executed or not [Gaudi::Algorithm] """,
    'ErrorMax' : """ [[deprecated]] max number of errors [Gaudi::Algorithm] """,
    'AuditAlgorithms' : """ [[deprecated]] unused [Gaudi::Algorithm] """,
    'AuditInitialize' : """ trigger auditor on initialize() [Gaudi::Algorithm] """,
    'AuditReinitialize' : """ trigger auditor on reinitialize() [Gaudi::Algorithm] """,
    'AuditRestart' : """ trigger auditor on restart() [Gaudi::Algorithm] """,
    'AuditExecute' : """ trigger auditor on execute() [Gaudi::Algorithm] """,
    'AuditFinalize' : """ trigger auditor on finalize() [Gaudi::Algorithm] """,
    'AuditStart' : """ trigger auditor on start() [Gaudi::Algorithm] """,
    'AuditStop' : """ trigger auditor on stop() [Gaudi::Algorithm] """,
    'Timeline' : """ send events to TimelineSvc [Gaudi::Algorithm] """,
    'MonitorService' : """ name to use for Monitor Service [Gaudi::Algorithm] """,
    'RegisterForContextService' : """ flag to enforce the registration for Algorithm Context Service [Gaudi::Algorithm] """,
    'Cardinality' : """ how many clones to create - 0 means algo is reentrant [Gaudi::Algorithm] """,
    'NeededResources' : """ named resources needed during event looping [Gaudi::Algorithm] """,
    'Blocking' : """ if algorithm invokes CPU-blocking system calls (offloads computations to accelerators or quantum processors, performs disk or network I/O, is bound by resource synchronization, etc) [Gaudi::Algorithm] """,
    'FilterCircularDependencies' : """ filter out circular data dependencies [Gaudi::Algorithm] """,
    'RootInTES' : """ note: overridden by parent settings [FixTESPath<Algorithm>] """,
    'ErrorsPrint' : """ print the statistics of errors/warnings/exceptions [GaudiCommon<Algorithm>] """,
    'PropertiesPrint' : """ print the properties of the component [GaudiCommon<Algorithm>] """,
    'PrintEmptyCounters' : """ force printing of empty counters, otherwise only printed in DEBUG mode [GaudiCommon<Algorithm>] """,
    'TypePrint' : """ add the actual C++ component type into the messages [GaudiCommon<Algorithm>] """,
    'Context' : """ note: overridden by parent settings [GaudiCommon<Algorithm>] """,
    'StatTableHeader' : """ the header row for the output Stat-table [GaudiCommon<Algorithm>] """,
    'RegularRowFormat' : """ the format for regular row in the output Stat-table [GaudiCommon<Algorithm>] """,
    'EfficiencyRowFormat' : """ The format for "efficiency" row in the output Stat-table [GaudiCommon<Algorithm>] """,
    'UseEfficiencyRowFormat' : """ use the special format for printout of efficiency counters [GaudiCommon<Algorithm>] """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary [GaudiCommon<Algorithm>] """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary [GaudiCommon<Algorithm>] """,
    'VetoObjects' : """ skip execute if one or more of these TES objects exist [GaudiAlgorithm] """,
    'RequireObjects' : """ execute only if one or more of these TES objects exist [GaudiAlgorithm] """,
    'PandoraSettingsDefault_xml' : """  [PandoraPFAlg] """,
    'NEventsToSkip' : """  [PandoraPFAlg] """,
    'TrackCollections' : """  [PandoraPFAlg] """,
    'ECalCaloHitCollections' : """  [PandoraPFAlg] """,
    'HCalCaloHitCollections' : """  [PandoraPFAlg] """,
    'LCalCaloHitCollections' : """  [PandoraPFAlg] """,
    'LHCalCaloHitCollections' : """  [PandoraPFAlg] """,
    'MuonCaloHitCollections' : """  [PandoraPFAlg] """,
    'MCParticleCollections' : """  [PandoraPFAlg] """,
    'RelCaloHitCollections' : """  [PandoraPFAlg] """,
    'RelTrackCollections' : """  [PandoraPFAlg] """,
    'KinkVertexCollections' : """  [PandoraPFAlg] """,
    'ProngVertexCollections' : """  [PandoraPFAlg] """,
    'SplitVertexCollections' : """  [PandoraPFAlg] """,
    'V0VertexCollections' : """  [PandoraPFAlg] """,
    'ClusterCollectionName' : """  [PandoraPFAlg] """,
    'PFOCollectionName' : """  [PandoraPFAlg] """,
    'ECalToMipCalibration' : """  [PandoraPFAlg] """,
    'HCalToMipCalibration' : """  [PandoraPFAlg] """,
    'ECalMipThreshold' : """  [PandoraPFAlg] """,
    'HCalMipThreshold' : """  [PandoraPFAlg] """,
    'ECalToEMGeVCalibration' : """  [PandoraPFAlg] """,
    'HCalToEMGeVCalibration' : """  [PandoraPFAlg] """,
    'ECalToHadGeVCalibrationBarrel' : """  [PandoraPFAlg] """,
    'ECalToHadGeVCalibrationEndCap' : """  [PandoraPFAlg] """,
    'HCalToHadGeVCalibration' : """  [PandoraPFAlg] """,
    'MuonToMipCalibration' : """  [PandoraPFAlg] """,
    'DigitalMuonHits' : """  [PandoraPFAlg] """,
    'MaxHCalHitHadronicEnergy' : """  [PandoraPFAlg] """,
    'UseOldTrackStateCalculation' : """  [PandoraPFAlg] """,
    'AbsorberRadLengthECal' : """  [PandoraPFAlg] """,
    'AbsorberIntLengthECal' : """  [PandoraPFAlg] """,
    'AbsorberRadLengthHCal' : """  [PandoraPFAlg] """,
    'AbsorberIntLengthHCal' : """  [PandoraPFAlg] """,
    'AbsorberRadLengthOther' : """  [PandoraPFAlg] """,
    'AbsorberIntLengthOther' : """  [PandoraPFAlg] """,
    'StartVertexCollectionName' : """  [PandoraPFAlg] """,
    'StartVertexAlgorithmName' : """  [PandoraPFAlg] """,
    'EMStochasticTerm' : """  [PandoraPFAlg] """,
    'HadStochasticTerm' : """  [PandoraPFAlg] """,
    'EMConstantTerm' : """  [PandoraPFAlg] """,
    'HadConstantTerm' : """  [PandoraPFAlg] """,
    'MuonHitEnergy' : """  [PandoraPFAlg] """,
    'NOuterSamplingLayers' : """  [PandoraPFAlg] """,
    'LayersFromEdgeMaxRearDistance' : """  [PandoraPFAlg] """,
    'MuonBarrelBField' : """  [PandoraPFAlg] """,
    'MuonEndCapBField' : """  [PandoraPFAlg] """,
    'ShouldFormTrackRelationships' : """  [PandoraPFAlg] """,
    'MinTrackHits' : """  [PandoraPFAlg] """,
    'MinFtdTrackHits' : """  [PandoraPFAlg] """,
    'MaxTrackHits' : """  [PandoraPFAlg] """,
    'D0TrackCut' : """  [PandoraPFAlg] """,
    'Z0TrackCut' : """  [PandoraPFAlg] """,
    'UseNonVertexTracks' : """  [PandoraPFAlg] """,
    'UseUnmatchedNonVertexTracks' : """  [PandoraPFAlg] """,
    'UseUnmatchedVertexTracks' : """  [PandoraPFAlg] """,
    'UnmatchedVertexTrackMaxEnergy' : """  [PandoraPFAlg] """,
    'D0UnmatchedVertexTrackCut' : """  [PandoraPFAlg] """,
    'Z0UnmatchedVertexTrackCut' : """  [PandoraPFAlg] """,
    'ZCutForNonVertexTracks' : """  [PandoraPFAlg] """,
    'ReachesECalNTpcHits' : """  [PandoraPFAlg] """,
    'ReachesECalNFtdHits' : """  [PandoraPFAlg] """,
    'ReachesECalTpcOuterDistance' : """  [PandoraPFAlg] """,
    'ReachesECalMinFtdLayer' : """  [PandoraPFAlg] """,
    'ReachesECalTpcZMaxDistance' : """  [PandoraPFAlg] """,
    'ReachesECalFtdZMaxDistance' : """  [PandoraPFAlg] """,
    'CurvatureToMomentumFactor' : """  [PandoraPFAlg] """,
    'MinTrackECalDistanceFromIp' : """  [PandoraPFAlg] """,
    'MaxTrackSigmaPOverP' : """  [PandoraPFAlg] """,
    'MinMomentumForTrackHitChecks' : """  [PandoraPFAlg] """,
    'TpcMembraneMaxZ' : """  [PandoraPFAlg] """,
    'MinTpcHitFractionOfExpected' : """  [PandoraPFAlg] """,
    'MinFtdHitsForTpcHitFraction' : """  [PandoraPFAlg] """,
    'MaxTpcInnerRDistance' : """  [PandoraPFAlg] """,
    'ECalEndCapInnerSymmetryOrder' : """  [PandoraPFAlg] """,
    'ECalEndCapInnerPhiCoordinate' : """  [PandoraPFAlg] """,
    'ECalEndCapOuterSymmetryOrder' : """  [PandoraPFAlg] """,
    'ECalEndCapOuterPhiCoordinate' : """  [PandoraPFAlg] """,
    'HCalEndCapInnerSymmetryOrder' : """  [PandoraPFAlg] """,
    'HCalEndCapInnerPhiCoordinate' : """  [PandoraPFAlg] """,
    'HCalEndCapOuterSymmetryOrder' : """  [PandoraPFAlg] """,
    'HCalEndCapOuterPhiCoordinate' : """  [PandoraPFAlg] """,
    'HCalRingInnerSymmetryOrder' : """  [PandoraPFAlg] """,
    'HCalRingInnerPhiCoordinate' : """  [PandoraPFAlg] """,
    'HCalRingOuterSymmetryOrder' : """  [PandoraPFAlg] """,
    'HCalRingOuterPhiCoordinate' : """  [PandoraPFAlg] """,
    'StripSplittingOn' : """  [PandoraPFAlg] """,
    'UseEcalScLayers' : """  [PandoraPFAlg] """,
    'ECalSiToMipCalibration' : """  [PandoraPFAlg] """,
    'ECalScToMipCalibration' : """  [PandoraPFAlg] """,
    'ECalSiMipThreshold' : """  [PandoraPFAlg] """,
    'ECalScMipThreshold' : """  [PandoraPFAlg] """,
    'ECalSiToEMGeVCalibration' : """  [PandoraPFAlg] """,
    'ECalScToEMGeVCalibration' : """  [PandoraPFAlg] """,
    'ECalSiToHadGeVCalibrationEndCap' : """  [PandoraPFAlg] """,
    'ECalScToHadGeVCalibrationEndCap' : """  [PandoraPFAlg] """,
    'ECalSiToHadGeVCalibrationBarrel' : """  [PandoraPFAlg] """,
    'ECalScToHadGeVCalibrationBarrel' : """  [PandoraPFAlg] """,
    'InputEnergyCorrectionPoints' : """  [PandoraPFAlg] """,
    'OutputEnergyCorrectionPoints' : """  [PandoraPFAlg] """,
    'collections' : """ Places of collections to read [PandoraPFAlg] """,
    'WriteClusterCollection' : """ Handle of the ClusterCollection               output collection [unknown owner type] """,
    'WriteReconstructedParticleCollection' : """ Handle of the ReconstructedParticleCollection output collection [unknown owner type] """,
    'WriteVertexCollection' : """ Handle of the VertexCollection                output collection [unknown owner type] """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(PandoraPFAlg, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'k4GaudiPandora'
  def getType( self ):
      return 'PandoraPFAlg'
  pass # class PandoraPFAlg
