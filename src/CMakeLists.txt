message(building sources)

################################################################################
## dependencies ################################################################
################################################################################

add_executable(track-particles
               ImportImage.cpp
               ProcessImage.cpp
               EasyBMP/EasyBMP.cpp
               main.cpp)

target_include_directories(track-particles PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EasyBMP>
)

################################################################################
## packaging ###################################################################
################################################################################
install(TARGETS track-particles DESTINATION bin)
