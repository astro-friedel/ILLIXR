#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestGround_truth_slam) {
    auto grnd = ground_truth_slam("ground_truth_slam", pb);
}
