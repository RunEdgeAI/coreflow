/**
 * @file test_parameter.cpp
 * @brief Test Internal Parameter Object
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

class ParameterTest : public ::testing::Test
{
protected:
    vx_context context;
    Parameter* parameter;

    void SetUp() override
    {
        context = vxCreateContext();
        parameter = new Parameter(context, nullptr);
    }

    void TearDown() override
    {
        delete parameter;
        vxReleaseContext(&context);
    }
};

TEST_F(ParameterTest, IsValidDirection)
{
    // Test valid directions
    EXPECT_EQ(Parameter::isValidDirection(VX_INPUT), vx_true_e);
    EXPECT_EQ(Parameter::isValidDirection(VX_OUTPUT), vx_true_e);
    EXPECT_EQ(Parameter::isValidDirection(VX_BIDIRECTIONAL), vx_true_e);

    // Test invalid direction
    EXPECT_EQ(Parameter::isValidDirection((vx_enum)100), vx_false_e);
}

TEST_F(ParameterTest, IsValidTypeMatch)
{
    // Test matching types
    EXPECT_EQ(Parameter::isValidTypeMatch(VX_TYPE_UINT8, VX_TYPE_UINT8), vx_true_e);
    EXPECT_EQ(Parameter::isValidTypeMatch(VX_TYPE_INT32, VX_TYPE_INT32), vx_true_e);

    // Test non-matching types
    EXPECT_EQ(Parameter::isValidTypeMatch(VX_TYPE_UINT8, VX_TYPE_INT32), vx_false_e);
    EXPECT_EQ(Parameter::isValidTypeMatch(VX_TYPE_FLOAT32, VX_TYPE_UINT8), vx_false_e);

    // Test edge cases
    EXPECT_EQ(Parameter::isValidTypeMatch((vx_enum)0, (vx_enum)0), vx_true_e);
    EXPECT_EQ(Parameter::isValidTypeMatch((vx_enum)0, VX_TYPE_UINT8), vx_false_e);
}

TEST_F(ParameterTest, IsValidState)
{
    // Test valid states
    EXPECT_EQ(Parameter::isValidState(VX_PARAMETER_STATE_REQUIRED), vx_true_e);
    EXPECT_EQ(Parameter::isValidState(VX_PARAMETER_STATE_OPTIONAL), vx_true_e);

    // Test invalid state
    EXPECT_EQ(Parameter::isValidState((vx_enum)0), vx_false_e);
}

TEST_F(ParameterTest, Destruct)
{
    parameter->destruct();
    EXPECT_EQ(parameter->node, nullptr);
    EXPECT_EQ(parameter->kernel, nullptr);
}
