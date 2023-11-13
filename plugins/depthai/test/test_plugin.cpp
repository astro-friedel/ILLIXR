#include <gtest/gtest.h>
#include "../plugin.hpp"
#include "test_macros.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestDepthai) {
    auto dai = depthai("depthai", pb);
}
