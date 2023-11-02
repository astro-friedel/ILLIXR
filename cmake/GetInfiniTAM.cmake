# module to download, build and install the InfiniTAM ILLIXR plugin

# get dependencies
#get_external(PortAudio)
#get_external(SpatialAudio)

set(INFINITAM_CMAKE_ARGS "")

ExternalProject_Add(InfiniTAM
        GIT_REPOSITORY https://github.com/ILLIXR/InfiniTAM.git        # Git repo for source code
        #        GIT_TAG cb686800157b7bc68ab46f564abb4a0e24ffbeec              # via Yihan on slack
        GIT_TAG 264de7919c0c2c06ee8b7d5284535d84a3f54c06 # MVM's first push
        PREFIX ${CMAKE_BINARY_DIR}/_deps/infinitam                    # the build directory
        #DEPENDS ${PortAudio_DEP_STR} ${SpatialAudio_DEP_STR} ${OpenCV_DEP_STR}  # dependencies of this module
        #arguments to pass to CMake
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_FLAGS=-L${CMAKE_INSTALL_PREFIX}/lib\ -L${CMAKE_INSTALL_PREFIX}/lib64 -DILLIXR_ROOT=${PROJECT_SOURCE_DIR}/include -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX} -DILLIXR_BUILD_SUFFIX=${ILLIXR_BUILD_SUFFIX} ${INFINITAM_CMAKE_ARGS}
        )
