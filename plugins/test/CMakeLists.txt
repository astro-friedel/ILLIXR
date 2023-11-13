find_package(GMock REQUIRED)

add_library(mock_pose STATIC plugins/test/mock_pose.cpp)
target_include_directories(mock_pose PUBLIC ${ILLIXR_SOURCE_DIR}/include ${GMOCK_INCLUDE_DIRS})
target_link_libraries(mock_pose PUBLIC ${GMOCK_LIBRARIES})
target_compile_features(mock_pose PRIVATE cxx_std_14)

add_library(mock_display STATIC plugins/test/mock_display.cpp)
target_include_directories(mock_display PUBLIC ${ILLIXR_SOURCE_DIR}/include ${GMOCK_INCLUDE_DIRS})
target_link_libraries(mock_display PUBLIC ${GMOCK_LIBRARIES})
target_compile_features(mock_display PRIVATE cxx_std_17)

add_library(mock_timewarp_vk STATIC plugins/test/mock_timewarp_vk.cpp)
target_include_directories(mock_timewarp_vk PUBLIC ${ILLIXR_SOURCE_DIR}/include ${GMOCK_INCLUDE_DIRS})
target_link_libraries(mock_timewarp_vk PUBLIC ${GMOCK_LIBRARIES})
target_compile_features(mock_timewarp_vk PRIVATE cxx_std_17)

add_library(mock_app STATIC plugins/test/mock_app.cpp)
target_include_directories(mock_app PUBLIC ${ILLIXR_SOURCE_DIR}/include ${GMOCK_INCLUDE_DIRS})
target_link_libraries(mock_app PUBLIC ${GMOCK_LIBRARIES})
target_compile_features(mock_app PRIVATE cxx_std_17)
