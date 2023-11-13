#include <gtest/gtest.h>
#include "../plugin.hpp"
#define USE_EXTEND
#include "test_macros.hpp"

START_SETUP

WITH_GL

ADD_DEPENDENCY(mock_pose)

END_SETUP


TEST_F(ILLIXRTest, TestGldemo) {
    auto gld = gldemo("gldemo", pb);
}
#undef USE_EXTEND