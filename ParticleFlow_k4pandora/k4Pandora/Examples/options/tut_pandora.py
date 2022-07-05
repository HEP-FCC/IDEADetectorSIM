#!/usr/bin/env python

import os
import sys

from Configurables import PodioInput
from Gaudi.Configuration import *



##############################################################################
# Event Data Svc
##############################################################################
from Configurables import k4DataSvc
podioevent = k4DataSvc("EventDataSvc", input="/storage/local/home/atlas/onofrioa/Updated_ParticleFlow_k4pandora/k4Pandora/Input/run_0001_pi-_ene_5_tower_1_openAngle_0_2_digi_podio.root")
#podioevent = k4DataSvc("EventDataSvc", input="/storage/local/home/atlas/onofrioa/Updated_ParticleFlow_k4pandora/k4Pandora/Input/EDMOutput00001.root")


##############################################################################
from Configurables import PodioInput ## set the input collection data
podioinput = PodioInput("PodioReader", collections=[ 
    "S_CalorimeterHits",
    "C_CalorimeterHits"
    ])
##############################################################################
from Configurables import GearSvc
gearSvc  = GearSvc("GearSvc")
gearSvc.GearXMLFile = "../Pandora/FullDetGear.xml"

##############################################################################
from Configurables import PandoraSCAlg

pandoralg = PandoraSCAlg("PandoraSCAlg")
pandoralg.collections = [
        "CalorimeterHit:S_CalorimeterHits",
        "CalorimeterHit:C_CalorimeterHits"
        ]
pandoralg.WriteCaloHitCollection               = "SC_CalorimeterHitCollection"
pandoralg.PandoraSettingsDefault_xml = "../Pandora/PandoraSettingsDefault.xml"
pandoralg.HCalCaloHitCollections=  ["S_CalorimeterHits", "C_CalorimeterHits"]

##############################################################################


# write PODIO file
from Configurables import PodioOutput
write = PodioOutput("write")
write.filename = "test.root"
write.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr(
        TopAlg = [podioinput,pandoralg,write],
        EvtSel = 'NONE',
        EvtMax = 3,
        ExtSvc = [podioevent, gearSvc],
        OutputLevel=INFO
)
