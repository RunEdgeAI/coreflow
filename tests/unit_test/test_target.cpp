/**
 * @file test_target.cpp
 * @brief Test Internal Target Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025 Edge.AI
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

class TargetTest : public ::testing::Test
{
protected:
    vx_context context;
    Target* target;

    void SetUp() override
    {
        context = vxCreateContext();
        target = new Target(context, nullptr);
    }

    void TearDown() override
    {
        delete target;
        vxReleaseContext(&context);
    }
};