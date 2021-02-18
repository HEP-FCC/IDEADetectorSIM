file(GLOB PSHW_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/PSHW/include
                    ${PROJECT_SOURCE_DIR}/SVX/include)
add_library(PSHW SHARED ${PSHW_SOURCES})

