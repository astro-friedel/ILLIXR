#include "../plugin.hpp"
#include <gtest/gtest.h>
#include "test_macros.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestOffload_reader) {
    auto drx = offload_reader("offload_reader", pb);
}
