#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestPose_lookup_impl) {
    auto plu = pose_lookup_impl(pb);
}

TEST_F(ILLIXRTest, TestPose_lookup_plugin) {
    auto plu = pose_lookup_plugin("pose_lookup_plugin", pb);
}
