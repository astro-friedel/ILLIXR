#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestOffline_imu) {
    auto oli = offline_imu("offline_imu", pb);
}
