include(CMakeFindDependencyMacro)
find_dependency(podio REQUIRED)
find_dependency(Gaudi REQUIRED)
find_dependency(k4FWCore REQUIRED)
find_dependency(EDM4HEP REQUIRED)
find_dependency(ROOT REQUIRED)

# - Include the targets file to create the imported targets that a client can
# link to (libraries) or execute (programs)
include("${CMAKE_CURRENT_LIST_DIR}/k4PandoraTargets.cmake")

get_property(TEST_K4PANDORA_LIBRARY TARGET k4Pandora::k4GaudiPandora PROPERTY LOCATION)
find_package_handle_standard_args(k4Pandora DEFAULT_MSG CMAKE_CURRENT_LIST_FILE TEST_CEPCSW_LIBRARY)
