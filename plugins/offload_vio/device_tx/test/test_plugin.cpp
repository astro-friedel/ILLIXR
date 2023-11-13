#include <gtest/gtest.h>
#include "../plugin.hpp"
#include "test_macros.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestOffload_writer) {
    auto dtx = offload_writer("offload_writer", pb);
}
