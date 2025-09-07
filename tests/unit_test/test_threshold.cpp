/**
 * @file test_threshold.cpp
 * @brief Test Internal Threshold Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025 Edge.AI
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

using namespace coreflow;

class ThresholdTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_threshold threshold;

    void SetUp() override
    {
        context = vxCreateContext();
        threshold = vxCreateThreshold(context, VX_THRESHOLD_TYPE_BINARY, VX_TYPE_UINT8);
    }

    void TearDown() override
    {
        vxReleaseThreshold(&threshold);
        vxReleaseContext(&context);
    }
};

TEST_F(ThresholdTest, CreateThreshold)
{
    ASSERT_NE(threshold, nullptr);
    EXPECT_EQ(threshold->thresh_type, VX_THRESHOLD_TYPE_BINARY);
    EXPECT_EQ(threshold->data_type, VX_TYPE_UINT8);
}

TEST_F(ThresholdTest, IsValidThresholdType)
{
    // Test valid types
    EXPECT_EQ(Threshold::isValidThresholdType(VX_THRESHOLD_TYPE_BINARY), vx_true_e);
    EXPECT_EQ(Threshold::isValidThresholdType(VX_THRESHOLD_TYPE_RANGE), vx_true_e);

    // Test invalid type
    EXPECT_EQ(Threshold::isValidThresholdType((vx_enum)0), vx_false_e);
}

TEST_F(ThresholdTest, IsValidThresholdDataType)
{
    // Test valid data types
    EXPECT_EQ(Threshold::isValidThresholdDataType(VX_TYPE_BOOL), vx_true_e);
    EXPECT_EQ(Threshold::isValidThresholdDataType(VX_TYPE_INT8), vx_true_e);
    EXPECT_EQ(Threshold::isValidThresholdDataType(VX_TYPE_UINT8), vx_true_e);

    // Test invalid data type
    EXPECT_EQ(Threshold::isValidThresholdDataType((vx_enum)0), vx_false_e);
}
