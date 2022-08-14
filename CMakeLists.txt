#---------------------------------------------
# Project: Contol App for SIRIUS setup
# Author: Rikel CHAKMA
# Date : 06/06/2022
#-----------------------------------------------


#-------------------
# Setup the project
#-------------------
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(SIRIUS_CONTROL_APP)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} will be used in the project.")

if(CMAKE_INSTALL_PREFIX)
  message(STATUS "-- found CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
else()
message(WARNING "-- CMAKE_INSTALL_PREFIX not found")
endif()


include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})


#-------------------
# Find GTK+2
#-----------------

find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)

include_directories (${GTK2_INCLUDE_DIRS})
#link_directories (${GTK2_LIBRARY_DIRS})


find_package(GTK2 2.10 COMPONENTS gtk)
if(GTK2_FOUND)
    include_directories(${GTK2_INCLUDE_DIRS})
     message(STATUS "--------GTK2 found. version ${GTK2_VERSION}")
else()
  message(FATAL ERROR "--------GTK2 not found !!!!!!")
  endif()

#----------------------------------------------------------------------------
# Add the executable, and link it to the ROOT libraries
# direct the object to home directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "$ENV{HOME}/ganacq_manip/test/GRU_SIRIUS/GUser-build/")
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

add_executable(SiriusControlApp sirius_control_app.cc)
target_link_libraries(SiriusControlApp PUBLIC  ${GTK2_LIBRARIES})


add_custom_target(SIRIUS_CONTROL_APP DEPENDS SiriusControlApp)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#--------------------------------------------------------------------------
install(TARGETS SiriusControlApp DESTINATION bin
        LIBRARY    DESTINATION lib
        ARCHIVE    DESTINATION lib
        INCLUDES   DESTINATION include
        COMPONENT libraries
)

install (FILES sirius_control_app.cc DESTINATION include)

