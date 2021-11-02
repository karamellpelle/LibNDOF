project( examples )

set( EXAMPLES_SOURCE_DIR "${PROJECT_SOURCE_DIR}" )


add_executable( events "${EXAMPLES_SOURCE_DIR}/events.cpp" )

target_link_libraries( events libNDOF )
