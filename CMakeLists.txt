cmake_minimum_required(VERSION 3.13)
# ^ needed because we pass normal variables into subprojects (https://cmake.org/cmake/help/latest/policy/CMP0077.html)

################################################################################
# libNDOF
################################################################################

# project name
project( libNDOF )


################################################################################
# options

option( LIBNDOF_DEBUG               "enable stdout debug" ON )
option( LIBNDOF_BACKEND_HDIAPI      "use 'hdiapi' backend" OFF )
option( LIBNDOF_BUILD_EXAMPLES      "build example programs" ON )


################################################################################
# dependencies


################################################################################
# settings

# specify the C++ standard
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

# version
set( LIBNDOF_VERSION_MAJOR  0 )
set( LIBNDOF_VERSION_MINOR  2 )
set( LIBNDOF_VERSION_STRING "${LIBNDOF_VERSION_MAJOR}.${LIBNDOF_VERSION_MINOR}" )


################################################################################
# files

set( LIBNDOF_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source" )
set( LIBNDOF_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" )
set( LIBNDOF_LIBS_DIR "${PROJECT_SOURCE_DIR}/libs" )

# source files to build
set( LIBNDOF_SOURCES
     "${LIBNDOF_SOURCE_DIR}/libNDOF.cpp"
     "${LIBNDOF_SOURCE_DIR}/NDOF.cpp"
     "${LIBNDOF_SOURCE_DIR}/Connection.cpp"
   )

# source include files 
# TODO: into source folder
set( LIBNDOF_INCLUDES
     "${LIBNDOF_INCLUDE_DIR}/libNDOF.hpp"
     "${LIBNDOF_INCLUDE_DIR}/libNDOF/devices.hpp"
     "${LIBNDOF_SOURCE_DIR}/LibNDOF/Connexion.hpp"
   )

# add macOS specific files 
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    list( APPEND LIBNDOF_SOURCES "${LIBNDOF_SOURCE_DIR}/LibNDOF/MacOSNDOF.mm" )

    list( APPEND LIBNDOF_INCLUDES "${LIBNDOF_SOURCE_DIR}/LibNDOF/MacOSNDOF.hpp")
    list( APPEND LIBNDOF_INCLUDES "${LIBNDOF_SOURCE_DIR}/LibNDOF/ConnexionMacOS.hpp")
endif()

# add Windows specific files
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    list( APPEND LIBNDOF_SOURCES "${LIBNDOF_SOURCE_DIR}/LibNDOF/WinNDOF.cpp" )

    list( APPEND LIBNDOF_INCLUDES "${LIBNDOF_SOURCE_DIR}/LibNDOF/WinNDOF.hpp")
endif()


################################################################################
# dependencies ('libs/')

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    # build `hidapi`
    set( HIDAPI_BUILD_HIDTEST OFF ) 
    set( HIDAPI_WITH_LIBUSB ON )
    set( HIDAPI_WITH_HIDRAW OFF )
    #set( BUILD_SHARED_LIBS OFF ) FIXME: subproject only
    add_subdirectory( ${LIBNDOF_LIBS_DIR}/hidapi )
    list( APPEND LIBNDOF_TARGET_LIBRARIES hidapi::hidapi )
endif()


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

# add preprocessor definitions
target_compile_definitions( libNDOF PRIVATE  $<$<BOOL:LIBNDOF_DEBUG>:"LIBNDOF_DEBUG"> )
# (the rest is set in configure.hpp.in; think what preprocessor directives to expose for library users)

# https://stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake+
# TODO:   target_compile_definitions(MathFunctions PRIVATE "USE_MYMATH")
# TODO: properties:
#set_target_properties(mylib PROPERTIES VERSION ${PROJECT_VERSION})
#set_target_properties(mylib PROPERTIES PUBLIC_HEADER include/mylib.h)

# add local include paths here:
target_include_directories( libNDOF PRIVATE "${LIBNDOF_SOURCE_DIR}" )   # 
target_include_directories( libNDOF PRIVATE "${PROJECT_BINARY_DIR}" )   # ^ add configure.hpp
target_include_directories( libNDOF PUBLIC "${LIBNDOF_SOURCE_DIR}" )   #  FIXME!! test on examples
# add exposed include paths here:
target_include_directories( libNDOF PUBLIC "${LIBNDOF_INCLUDE_DIR}" )  # ^ expose 'include/' TODO: into configured, binary dir?. for PUBLIC vs INTERFACE, see https://stackoverflow.com/questions/26243169/cmake-target-include-directories-meaning-of-scope

# link target to libraries
# https://cmake.org/cmake/help/latest/guide/tutorial/Selecting%20Static%20or%20Shared%20Libraries.html
target_link_libraries( libNDOF ${LIBNDOF_TARGET_LIBRARIES} )

if ( LIBNDOF_BUILD_EXAMPLES )
    add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/examples" )
endif()

