#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestOpenni_plugin) {
    auto oni = openni_plugin("openni_plugin", pb);
}
