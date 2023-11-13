#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestGtsam_integrator) {
    auto gts = gtsam_integrator("gtsam_integrator", pb);
}
