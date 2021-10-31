cmake_minimum_required(VERSION 3.10)

################################################################################
# libNDOF
################################################################################

# project name
project( libNDOF )


################################################################################
# options

option( LIBNDOF_DEBUG_OUTPUT_ENABLE "build with stdout debug output" ON )
option( LIBNDOF_BACKEND_HDIAPI      "use 'hdiapi' backend" ON )
option( LIBNDOF_BACKEND_SDK         "use 3Dconnexion drivers as backend (not implemented)" OFF )
option( LIBNDOF_TIMETAG             "tag events with time points using <chrono> STL" ON )


################################################################################
# dependencies


################################################################################
# settings

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11) # FIXME: 14?
set(CMAKE_CXX_STANDARD_REQUIRED True)

# version
set( LIBNDOF_VERSION_MAJOR  0 )
set( LIBNDOF_VERSION_MINOR  0 )
set( LIBNDOF_VERSION_STRING "${LIBNDOF_VERSION_MAJOR}.${LIBNDOF_VERSION_MINOR}" )


################################################################################
# files

set( LIBNDOF_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source" )
set( LIBNDOF_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" )
set( LIBNDOF_LIBS_DIR "${PROJECT_SOURCE_DIR}/libs" )

# source files to build
set( LIBNDOF_SOURCES
     "${LIBNDOF_SOURCE_DIR}/libNDOF.cpp"
   )

# source include files 
set( LIBNDOF_INCLUDES
     "${LIBNDOF_INCLUDE_DIR}/libNDOF.hpp"
   )


################################################################################
# dependencies ('libs/')

# libs/hidapi TODO: set CACHE?

# no test program
set( HIDAPI_BUILD_HIDTEST OFF ) 
# only usb backend (linux only)
set( HIDAPI_WITH_LIBUSB ON )
set( HIDAPI_WITH_HIDRAW OFF )
# HIDAPI as static library. 
#set( BUILD_SHARED_LIBS OFF ) FIXME: subproject only
# build
add_subdirectory( ${LIBNDOF_LIBS_DIR}/hidapi )
list( APPEND LIBNDOF_TARGET_LIBRARIES hidapi::hidapi )


################################################################################
# build

# confugire file
configure_file( "${PROJECT_SOURCE_DIR}/configure.hpp.in" "${PROJECT_BINARY_DIR}/configure.hpp" )

# append headers to sources. this is necessary to recompile changed headers, see
#   * https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake#13703725
#   * https://stackoverflow.com/questions/17653738/recursive-cmake-search-for-header-and-source-files
#   * https://stackoverflow.com/questions/32411963/why-is-cmake-file-glob-evil#32412044
# ^ FIXME: is this old-skool?
list( APPEND LIBNDOF_SOURCES ${LIBNDOF_INCLUDES} )

# create library 'libNDOF'
add_library( libNDOF ${LIBNDOF_SOURCES} )

# add local include paths here:
target_include_directories( libNDOF PRIVATE "${PROJECT_BINARY_DIR}" )   # ^ add configure.hpp
# add exposed include paths here:
target_include_directories( libNDOF PUBLIC "${LIBNDOF_INCLUDE_DIR}" )  # ^ expose 'include/' TODO: into configured, binary dir?. for PUBLIC vs INTERFACE, see https://stackoverflow.com/questions/26243169/cmake-target-include-directories-meaning-of-scope

# link target to libraries
target_link_libraries( libNDOF ${LIBNDOF_TARGET_LIBRARIES} )

