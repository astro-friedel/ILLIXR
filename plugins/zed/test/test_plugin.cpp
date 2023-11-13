#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestZed) {
    auto zd = zed_imu_thread("zed_imu_thread", pb);
}
