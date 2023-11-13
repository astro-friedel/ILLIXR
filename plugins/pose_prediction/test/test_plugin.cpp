#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestPose_prediction_impl) {
    auto pp = pose_prediction_impl(pb);
}

TEST_F(ILLIXRTest, TestPose_prediction_plugin) {
    auto pp = pose_prediction_plugin("pose_prediction_plugin", pb);
}
