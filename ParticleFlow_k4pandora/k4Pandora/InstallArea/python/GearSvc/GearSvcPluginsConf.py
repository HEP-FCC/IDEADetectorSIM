#Tue May 24 15:07:28 2022"""Automatically generated. DO NOT EDIT please"""
import sys
if sys.version_info >= (3,):
    # Python 2 compatibility
    long = int
from GaudiKernel.Proxy.Configurable import *

class GearSvc( ConfigurableService ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'AuditServices' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AutoRetrieveTools' : True, # bool
    'CheckToolDeps' : True, # bool
    'GearXMLFile' : '', # str
  }
  _propertyDocDct = { 
    'OutputLevel' : """ output level [Service] """,
    'AuditServices' : """ [[deprecated]] unused [Service] """,
    'AuditInitialize' : """ trigger auditor on initialize() [Service] """,
    'AuditStart' : """ trigger auditor on start() [Service] """,
    'AuditStop' : """ trigger auditor on stop() [Service] """,
    'AuditFinalize' : """ trigger auditor on finalize() [Service] """,
    'AuditReinitialize' : """ trigger auditor on reinitialize() [Service] """,
    'AuditRestart' : """ trigger auditor on restart() [Service] """,
    'AutoRetrieveTools' : """ retrieve all AlgTools during initialize [Service] """,
    'CheckToolDeps' : """ check data dependencies of AlgTools (error if any found) [Service] """,
    'GearXMLFile' : """  [GearSvc] """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(GearSvc, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'GearSvcPlugins'
  def getType( self ):
      return 'GearSvc'
  pass # class GearSvc
