#include <gtest/gtest.h>
#include "../plugin.hpp"
#include "test_macros.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestPassthrough_integrator) {
    auto pti = passthrough_integrator("passthrough_integrator", pb);
}
