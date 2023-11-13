#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestRecord_imu_cam) {
    auto ric = record_imu_cam("record_imu_cam", pb);
}
