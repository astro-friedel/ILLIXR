# module to build and install the debugview ILLIXR plugin


set(PLUGIN_NAME plugin.new_pl${ILLIXR_BUILD_SUFFIX})

# source files, listed individually so that any changes will trigger a rebuild
set(DEBUGVIEW_SOURCES plugin.cpp
        ../../include/illixr/shaders/demo_shader.hpp
        ../../include/illixr/data_format.hpp
        ../../include/illixr/opencv_data_types.hpp
        ../../include/illixr/error_util.hpp
        ../../include/illixr/gl_util/obj.hpp
        ../../include/illixr/global_module_defs.hpp
        ../../include/illixr/math_util.hpp
        ../../include/illixr/pose_prediction.hpp
        ../../include/illixr/shader_util.hpp
        ../../include/illixr/switchboard.hpp
        ../../include/illixr/threadloop.hpp
        )

add_library(${PLUGIN_NAME} SHARED ${DEBUGVIEW_SOURCES})

# if building on CentOS make sure we use the correct OpenCV
if(BUILD_OPENCV)
    add_dependencies(${PLUGIN_NAME} OpenCV_Viz)
endif()

target_include_directories(${PLUGIN_NAME} PRIVATE ${X11_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLU_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS} ${gl_INCLUDE_DIRS} ${ILLIXR_SOURCE_DIR}/include ${Eigen3_INCLUDE_DIRS})
target_link_libraries(${PLUGIN_NAME} ${X11_LIBRARIES} ${GLEW_LIBRARIES} ${glu_LDFLAGS} ${OpenCV_LIBRARIES} glfw OpenGL::GL ${Eigen3_LIBRARIES} dl Threads::Threads spdlog::spdlog)
target_compile_features(${PLUGIN_NAME} PRIVATE cxx_std_17)

install(TARGETS ${PLUGIN_NAME} DESTINATION lib)
