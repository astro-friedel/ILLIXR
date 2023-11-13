#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

ADD_DEPENDENCY(mock_pose)
ADD_DEPENDENCY(mock_display)

END_SETUP


TEST_F(ILLIXRTest, TestTimewarp_vk) {
    auto tvk = timewarp_vk(pb);
}

TEST_F(ILLIXRTest, TestTimewarp_vk_plugin) {
    auto tvk = timewarp_vk_plugin("timewarp_vk_plugin", pb);
}
