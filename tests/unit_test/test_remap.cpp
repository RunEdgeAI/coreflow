/**
 * @file test_remap.cpp
 * @brief Test Internal Remap Object
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

class RemapTest : public ::testing::Test
{
protected:
    vx_context context;
    Remap* remap;
    const vx_uint32 src_width = 128;
    const vx_uint32 src_height = 128;
    const vx_uint32 dst_width = 64;
    const vx_uint32 dst_height = 64;

    void SetUp() override
    {
        context = vxCreateContext();
        remap = vxCreateRemap(context, src_width, src_height, dst_width, dst_height);
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(RemapTest, Constructor)
{
    ASSERT_NE(remap, nullptr);
    EXPECT_EQ(remap->type, VX_TYPE_REMAP);
    EXPECT_EQ(remap->src_width, src_width);
    EXPECT_EQ(remap->src_height, src_height);
    EXPECT_EQ(remap->dst_width, dst_width);
    EXPECT_EQ(remap->dst_height, dst_height);
    for (vx_uint32 p = 0; p < VX_PLANE_MAX; p++)
        EXPECT_EQ(remap->memory.ptrs[p], nullptr);
}

TEST_F(RemapTest, SetAndGetPoint)
{
    vx_uint32 dst_x = 10, dst_y = 10;
    vx_float32 src_x = 20.5f, src_y = 20.5f;

    // Set point
    vx_status status = remap->setCoordValue(dst_x, dst_y, src_x, src_y);
    EXPECT_EQ(status, VX_SUCCESS);

    // Get point
    vx_float32 get_src_x, get_src_y;
    status = remap->getCoordValue(dst_x, dst_y, &get_src_x, &get_src_y);
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_FLOAT_EQ(get_src_x, src_x);
    EXPECT_FLOAT_EQ(get_src_y, src_y);
}

TEST_F(RemapTest, SetPointOutOfBounds)
{
    vx_uint32 dst_x = dst_width + 1, dst_y = dst_height + 1;
    vx_float32 src_x = 20.5f, src_y = 20.5f;

    // Set point out of bounds
    vx_status status = remap->setCoordValue(dst_x, dst_y, src_x, src_y);
    EXPECT_EQ(status, VX_ERROR_INVALID_VALUE);
}

TEST_F(RemapTest, GetPointOutOfBounds)
{
    vx_uint32 dst_x = dst_width + 1, dst_y = dst_height + 1;
    vx_float32 get_src_x, get_src_y;

    // Get point out of bounds
    vx_status status = remap->getCoordValue(dst_x, dst_y, &get_src_x, &get_src_y);
    EXPECT_EQ(status, VX_ERROR_INVALID_VALUE);
}

TEST_F(RemapTest, Destruct)
{
    remap->destruct();
    EXPECT_EQ(remap->memory.allocated, vx_false_e);
    for (vx_uint32 p = 0; p < remap->memory.nptrs; p++)
        EXPECT_EQ(remap->memory.ptrs[p], nullptr);
}