#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestRecord_rgb_depth) {
    auto rgb = record_rgb_depth("record_rgb_depth", pb);
}
