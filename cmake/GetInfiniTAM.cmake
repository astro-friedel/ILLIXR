# module to download, build and install the InfiniTAM ILLIXR plugin

set(INFINITAM_CMAKE_ARGS "")

ExternalProject_Add(InfiniTAM
        GIT_REPOSITORY https://github.com/ILLIXR/InfiniTAM.git        # Git repo for source code
        GIT_TAG 82b931520a9e29339e21a8ae16dcde5570a013f6              # 2023-11-02 commit that builds and installs
        PREFIX ${CMAKE_BINARY_DIR}/_deps/infinitam                    # the build directory
        #arguments to pass to CMake
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_FLAGS=-L${CMAKE_INSTALL_PREFIX}/lib\ -L${CMAKE_INSTALL_PREFIX}/lib64 -DILLIXR_ROOT=${PROJECT_SOURCE_DIR}/include -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX} -DILLIXR_BUILD_SUFFIX=${ILLIXR_BUILD_SUFFIX} ${INFINITAM_CMAKE_ARGS}
        )
