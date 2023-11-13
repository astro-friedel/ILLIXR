#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

ADD_DEPENDENCY(mock_display)

END_SETUP


TEST_F(ILLIXRTest, TestVkdemo) {
    auto vkd = vkdemo(pb);
}

TEST_F(ILLIXRTest, TestVkdemoPlugin) {
    auto vkd = vkdemo_plugin("vkdemo_plugin", pb);
}
