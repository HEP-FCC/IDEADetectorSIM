#####################################################################################
# (c) Copyright 1998-2019 CERN for the benefit of the LHCb and ATLAS collaborations #
#                                                                                   #
# This software is distributed under the terms of the Apache version 2 licence,     #
# copied verbatim in the file "LICENSE".                                            #
#                                                                                   #
# In applying this licence, CERN does not waive the privileges and immunities       #
# granted to it by virtue of its status as an Intergovernmental Organization        #
# or submit itself to any jurisdiction.                                             #
#####################################################################################
# - Locate gperftools library
# Defines:
#
#  GPERFTOOLS_FOUND
#  GPERFTOOLS_INCLUDE_DIR
#  GPERFTOOLS_INCLUDE_DIRS (not cached)
#  GPERFTOOLS_TCMALLOC_LIBRARY
#  GPERFTOOLS_TCMALLOC_DEBUG_LIBRARY
#  GPERFTOOLS_TCMALLOC_MINIMAL_LIBRARY
#  GPERFTOOLS_TCMALLOC_AND_PROFILER_LIBRARY
#  GPERFTOOLS_PROFILER_LIBRARY
#  GPERFTOOLS_LIBRARIES (not cached)
#  GPERFTOOLS_LIBRARY_DIRS (not cached)
#  GPERFTOOLS_PPROF_EXECUTABLE
#
# Imports:
#
#  gperftools::gperftools (tcmalloc+profiler)
#  gperftools::tcmalloc
#  gperftools::tcmalloc_debug
#  gperftools::tcmalloc_and_profiler
#  gperftools::tcmalloc_minimal
#  gperftools::profiler
#  gperftools::pprof (executable)
#
# Usage of the targets instead of the variables is advised

# Find quietly if already found before
if(DEFINED CACHE{GPERFTOOLS_INCLUDE_DIR})
  set(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY YES)
endif()

find_path(GPERFTOOLS_INCLUDE_DIR tcmalloc.h  HINTS ${GPERFTOOLS_INSTALL_DIR}
  PATH_SUFFIXES  "include/gperftools" "include/google" NO_DEFAULT_PATH)
find_path(GPERFTOOLS_INCLUDE_DIR tcmalloc.h  HINTS ${GPERFTOOLS_INSTALL_DIR}
  PATH_SUFFIXES  "gperftools" "google")
foreach(component IN ITEMS tcmalloc tcmalloc_debug tcmalloc_and_profiler tcmalloc_minimal profiler)
  string(TOUPPER ${component} COMPONENT)
  find_library(GPERFTOOLS_${COMPONENT}_LIBRARY NAMES ${component}
    HINTS ${GPERFTOOLS_INSTALL_DIR}/lib NO_DEFAULT_PATH)
  find_library(GPERFTOOLS_${COMPONENT}_LIBRARY NAMES ${component}
    HINTS ${GPERFTOOLS_INSTALL_DIR}/lib)
  mark_as_advanced(GPERFTOOLS_${COMPONENT}_LIBRARY)
endforeach()
get_filename_component(GPERFTOOLS_INCLUDE_BASE "${GPERFTOOLS_INCLUDE_DIR}" DIRECTORY)
find_program(GPERFTOOLS_PPROF_EXECUTABLE NAMES pprof
             HINTS ${GPERFTOOLS_INSTALL_DIR}/bin NO_DEFAULT_PATH)
find_program(GPERFTOOLS_PPROF_EXECUTABLE NAMES pprof
             HINTS ${GPERFTOOLS_INSTALL_DIR}/bin)

set(GPERFTOOLS_INCLUDE_DIRS ${GPERFTOOLS_INCLUDE_BASE})
set(GPERFTOOLS_LIBRARIES ${GPERFTOOLS_TCMALLOC_LIBRARY} ${GPERFTOOLS_PROFILER_LIBRARY})

#find GPERFTOOLS version
if( GPERFTOOLS_INCLUDE_DIR )
   FILE(STRINGS "${GPERFTOOLS_INCLUDE_DIR}/tcmalloc.h" TCMVERSION REGEX ".define TC_VERSION_[MP].*")
endif()
if(TCMVERSION)
  LIST(GET TCMVERSION 0 VERMAJ)
  string(REGEX MATCH ".* ([0-9]+)$" MAJ ${VERMAJ})
  if(CMAKE_MATCH_1)
    set(GPERFTOOLS_VERSION_MAJOR ${CMAKE_MATCH_1})
  else()
    set(GPERFTOOLS_VERSION_MAJOR 0)
  endif()
  LIST(GET TCMVERSION 1 VERMIN)
  string(REGEX MATCH ".* ([0-9]+)$" MIN ${VERMIN})
  if(CMAKE_MATCH_1)
    set(GPERFTOOLS_VERSION_MINOR ${CMAKE_MATCH_1})
  else()
    set(GPERFTOOLS_VERSION_MINOR 0)
  endif()
  LIST(GET TCMVERSION 2 VERPATCH)
  string(REGEX MATCH ".* ([0-9]+)$" PAT ${VERPATCH})
  if(CMAKE_MATCH_1)
    set(GPERFTOOLS_VERSION_PATCH ${CMAKE_MATCH_1})
  else()
    set(GPERFTOOLS_VERSION_PATCH 0)
  endif()
  set(GPERFTOOLS_VERSION ${GPERFTOOLS_VERSION_MAJOR}.${GPERFTOOLS_VERSION_MINOR}.${GPERFTOOLS_VERSION_PATCH})
endif()
# handle the QUIETLY and REQUIRED arguments and set GPERFTOOLS_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(gperftools FOUND_VAR GPERFTOOLS_FOUND 
  REQUIRED_VARS GPERFTOOLS_INCLUDE_DIRS GPERFTOOLS_LIBRARIES
  VERSION_VAR GPERFTOOLS_VERSION)

mark_as_advanced(GPERFTOOLS_FOUND GPERFTOOLS_INCLUDE_DIR GPERFTOOLS_PPROF_EXECUTABLE GPERFTOOLS_VERSION )

if(GPERFTOOLS_TCMALLOC_LIBRARY)
  get_filename_component(GPERFTOOLS_LIBRARY_DIRS ${GPERFTOOLS_TCMALLOC_LIBRARY} PATH)
elseif(GPERFTOOLS_PROFILER_LIBRARY)
  get_filename_component(GPERFTOOLS_LIBRARY_DIRS ${GPERFTOOLS_PROFILER_LIBRARY} PATH)
endif()

# Modernisation: create interface targets to link against and use
if(TARGET gperftools::gperftools)
    return()
endif()
if(GPERFTOOLS_FOUND)
  if(NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    message(STATUS "  found components: gperftools tcmalloc tcmalloc_debug "
      "tcmalloc_and_profiler tcmalloc_minimal profiler")
  endif()
  add_library(gperftools::gperftools IMPORTED INTERFACE)
  target_include_directories(gperftools::gperftools SYSTEM INTERFACE "${GPERFTOOLS_INCLUDE_DIRS}")
  target_link_libraries(gperftools::gperftools INTERFACE "${GPERFTOOLS_LIBRARIES}")
  foreach(component IN ITEMS tcmalloc tcmalloc_debug tcmalloc_and_profiler tcmalloc_minimal profiler)
    string(TOUPPER ${component} COMPONENT)
    add_library(gperftools::${component} IMPORTED INTERFACE)
    target_include_directories(gperftools::${component} SYSTEM INTERFACE "${GPERFTOOLS_INCLUDE_DIR}")
    target_link_libraries(gperftools::${component} INTERFACE "${GPERFTOOLS_${COMPONENT}_LIBRARY}")
    if(NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    endif()
  endforeach()
  if(NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    message(STATUS "  Import executable target: gperftools::pprof")
  endif()
  add_executable(gperftools::pprof IMPORTED)
  set_target_properties(gperftools::pprof PROPERTIES IMPORTED_LOCATION ${GPERFTOOLS_PPROF_EXECUTABLE})
endif()

if(COMMAND __deprecate_var_for_target)
  foreach(v IN ITEMS GPERFTOOLS_INCLUDE_DIR
                     # GPERFTOOLS_INCLUDE_DIRS
                     GPERFTOOLS_TCMALLOC_LIBRARY
                     GPERFTOOLS_TCMALLOC_DEBUG_LIBRARY
                     GPERFTOOLS_TCMALLOC_MINIMAL_LIBRARY
                     GPERFTOOLS_TCMALLOC_AND_PROFILER_LIBRARY
                     GPERFTOOLS_PROFILER_LIBRARY GPERFTOOLS_LIBRARIES
                     GPERFTOOLS_LIBRARY_DIRS GPERFTOOLS_PPROF_EXECUTABLE)
    variable_watch(${v} __deprecate_var_for_target)
  endforeach()
endif()