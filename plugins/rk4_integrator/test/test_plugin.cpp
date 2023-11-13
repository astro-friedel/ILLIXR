#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestRK4_integrator) {
    auto rk4 = rk4_integrator("rk4_integrator", pb);
}
