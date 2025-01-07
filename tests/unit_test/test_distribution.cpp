/**
 * @file test_distribution.cpp
 * @brief Test Internal Distribution Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

class DistributionTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_distribution distribution;
    vx_size numBins;
    vx_int32 offset;
    vx_uint32 range;

    void SetUp() override
    {
        context = vxCreateContext();
        numBins = 256;
        offset = 0;
        range = 255;
        distribution = vxCreateDistribution(context, numBins, offset, range);
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(DistributionTest, CreateDistribution)
{
    ASSERT_NE(distribution, nullptr);
    EXPECT_EQ(distribution->range_x, range);
    EXPECT_EQ(distribution->range_y, 1);
    EXPECT_EQ(distribution->offset_x, offset);
    EXPECT_EQ(distribution->offset_y, 0);
    EXPECT_EQ(distribution->memory.dims[0][VX_DIM_X], numBins);
}

TEST_F(DistributionTest, DestructDistribution)
{
    distribution->destruct();
    for (vx_uint32 p = 0; p < distribution->memory.nptrs; p++)
        EXPECT_EQ(distribution->memory.ptrs[p], nullptr);
}
