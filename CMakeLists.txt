project(cloud-chamber-particle-tracking)
cmake_minimum_required(VERSION 3.14)

################################################################################
## Configurations
################################################################################

# From: https://blog.kitware.com/cmake-and-the-default-build-type/
# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

################################################################################
## Default install location
################################################################################

# Set the default install location to build dir/ bin
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "default install path" FORCE )
message("Setting install prefix to ${CMAKE_INSTALL_PREFIX} as none was specified")
endif()

################################################################################
## Compiler arguments
################################################################################

if(MSVC)
    add_compile_options("$<$<CONFIG:RELEASE>:/O2>")
else()
    add_compile_options("-Wall" "$<$<CONFIG:RELEASE>:-O3>")
endif()

################################################################################
## Build setup
################################################################################

# Create the touput directory in the build/src/ location
# TODO: remove when code is made more modular
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/src/output")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/output")

# Copy raw data
file(COPY "${CMAKE_SOURCE_DIR}/data" "${CMAKE_SOURCE_DIR}/images" 
     DESTINATION "${CMAKE_BINARY_DIR}")
# Create symbolic links
file(CREATE_LINK "${CMAKE_BINARY_DIR}/images" "${CMAKE_BINARY_DIR}/src/images" SYMBOLIC)

# Unzip data/Run2.tar.gz
add_custom_target(unzip ALL)
add_custom_command(TARGET unzip PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar -xzf ${CMAKE_SOURCE_DIR}/data/Run2.tar.gz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/data
    # DEPENDS ${CMAKE_SOURCE_DIR}/abc.zip
    COMMENT "Unpacking data/Run2.tar.gz"
    VERBATIM)

################################################################################
## Sources
################################################################################

add_subdirectory(src)
# add_subdirectory(test)


# Now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here.
set(CPACK_PACKAGE_NAME cloud-chamber-particle-tracking)
set(CPACK_PACKAGE_VERSION "0.1.0")

# We don't want to split our program up into several incomplete pieces.
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)