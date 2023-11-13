#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

ADD_DEPENDENCY(mock_app)
ADD_DEPENDENCY(mock_display)
ADD_DEPENDENCY(mock_pose)
ADD_DEPENDENCY(mock_timewarp_vk)

END_SETUP


TEST_F(ILLIXRTest, TestNative_renderer) {
    auto nrd = native_renderer("native_renderer", pb);
}
