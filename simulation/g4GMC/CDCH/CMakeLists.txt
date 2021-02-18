file(GLOB CDCH_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/CDCH/include)
add_library(CDCH SHARED ${CDCH_SOURCES})
