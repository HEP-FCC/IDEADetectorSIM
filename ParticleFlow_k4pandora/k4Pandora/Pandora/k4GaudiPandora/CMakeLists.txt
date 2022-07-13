
# Modules
gaudi_add_module(k4GaudiPandora
                 SOURCES src/PandoraSCAlg.cpp
                         src/MCParticleCreator.cpp
                         src/GeometryCreator.cpp
                         src/CaloHitCreator.cpp
			 src/DumperHelperAlg.cpp	 
                         src/TrackCreator.cpp
                         src/PfoCreator.cpp
                         src/Utility.cpp
                         ../../Utility/MarlinUtil/01-08/source/ClusterShapes.cc
                         ../../Utility/MarlinUtil/01-08/source/HelixClass.cc
                         ../../Utility/MarlinUtil/01-08/source/LineClass.cc
                 LINK 
                      GearSvc
                      Gaudi::GaudiKernel
                      k4FWCore::k4FWCore
                      ${PandoraSDK_LIBRARIES}
                      ${LCContent_LIBRARIES} 
                      ${GSL_LIBRARIES} 
                      ${CLHEP_LIBRARIES} 
                      ${ROOT_LIBRARIES}
                      ${LCIO_LIBRARIES}
                      ${GEAR_LIBRARIES}
                      ${DD4hep_COMPONENT_LIBRARIES}
                      EDM4HEP::edm4hep EDM4HEP::edm4hepDict
)

target_include_directories(k4GaudiPandora PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>/include
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/Utility/MarlinUtil/01-08/source
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(TARGETS k4GaudiPandora
  EXPORT k4PandoraTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  COMPONENT dev)

