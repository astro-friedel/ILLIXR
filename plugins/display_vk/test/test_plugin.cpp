#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestDisplay_vk) {
    auto dvk = display_vk(pb);
}

TEST_F(ILLIXRTest, TestDisplay_vk_plugin) {
    auto dvk = display_vk_plugin("display_vk", pb);
}
