#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestFaux_pose_impl) {
    auto faux = faux_pose_impl(pb);
}

TEST_F(ILLIXRTest, TestFaux_pose) {
    auto faux = faux_pose("faux_pose", pb);
}
