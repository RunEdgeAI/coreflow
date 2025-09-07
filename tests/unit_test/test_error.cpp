/**
 * @file test_error.cpp
 * @brief Test Internal Error Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

using namespace coreflow;

class ErrorTest : public ::testing::Test
{
protected:
    vx_context context;

    void SetUp() override
    {
        context = vxCreateContext();
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(ErrorTest, CreateConstErrors)
{
    vx_bool result = Error::createConstErrors(context);
    EXPECT_EQ(result, vx_true_e);
}

TEST_F(ErrorTest, AllocateError)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    vx_error error = Error::allocateError(context, status);
    ASSERT_NE(error, nullptr);
    EXPECT_EQ(error->status, status);
}