#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "k4Pandora::k4GaudiPandora" for configuration "RelWithDebInfo"
set_property(TARGET k4Pandora::k4GaudiPandora APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(k4Pandora::k4GaudiPandora PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO ""
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libk4GaudiPandora.so"
  IMPORTED_NO_SONAME_RELWITHDEBINFO "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS k4Pandora::k4GaudiPandora )
list(APPEND _IMPORT_CHECK_FILES_FOR_k4Pandora::k4GaudiPandora "${_IMPORT_PREFIX}/lib/libk4GaudiPandora.so" )

# Import target "k4Pandora::GearSvcPlugins" for configuration "RelWithDebInfo"
set_property(TARGET k4Pandora::GearSvcPlugins APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(k4Pandora::GearSvcPlugins PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO ""
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libGearSvcPlugins.so"
  IMPORTED_NO_SONAME_RELWITHDEBINFO "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS k4Pandora::GearSvcPlugins )
list(APPEND _IMPORT_CHECK_FILES_FOR_k4Pandora::GearSvcPlugins "${_IMPORT_PREFIX}/lib/libGearSvcPlugins.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
