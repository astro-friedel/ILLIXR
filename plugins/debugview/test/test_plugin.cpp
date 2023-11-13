#include <gtest/gtest.h>
#include "../plugin.hpp"
#include "test_macros.hpp"

START_SETUP

ADD_DEPENDENCY(mock_pose)

END_SETUP


TEST_F(ILLIXRTest, TestDebugview) {
    auto dbv = debugview("debugview", pb);
}
