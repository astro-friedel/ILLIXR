get_external(opengv)
get_external(KimeraRPGO)

set(KIMERA_VIO_CMAKE_ARGS "")
if(HAVE_CENTOS)
    set(KIMERA_VIO_CMAKE_ARGS "-DINTERNAL_OPENCV=${OpenCV_DIR}")
endif()
ExternalProject_Add(Kimera_VIO
        GIT_REPOSITORY https://github.com/ILLIXR/Kimera-VIO.git
        GIT_TAG 3165aa6658413bef8280a7e5ce13c1caad8a1d06
        GIT_SUBMODULES_RECURSE TRUE
        DEPENDS ${KimeraRPGO_DEP_STR} ${DBoW2_DEP_STR} ${GTSAM_DEP_STR} ${opengv_DEP_STR} ${OpenCV_DEP_STR}
        PREFIX ${CMAKE_BINARY_DIR}/_deps/kimera_vio
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_SHARED_LINKER_FLAGS=-L${CMAKE_INSTALL_PREFIX}/lib -DCMAKE_EXE_LINKER_FLAGS=-L${CMAKE_INSTALL_PREFIX}/lib -DBUILD_TESTS=OFF -DILLIXR_BUILD_SUFFIX=${ILLIXR_BUILD_SUFFIX} -DCMAKE_INSTALL_LIBDIR=lib -DILLIXR_ROOT=${CMAKE_SOURCE_DIR}/include ${KIMERA_VIO_CMAKE_ARGS}
        )