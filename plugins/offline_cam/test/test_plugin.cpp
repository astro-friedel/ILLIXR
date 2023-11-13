#include <gtest/gtest.h>
#include "test_macros.hpp"
#include "../plugin.hpp"

START_SETUP

END_SETUP


TEST_F(ILLIXRTest, TestOffline_cam) {
    auto olc = offline_cam("offline_cam", pb);
}
