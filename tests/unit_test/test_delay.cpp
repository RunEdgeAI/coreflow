/**
 * @file test_delay.cpp
 * @brief Test Internal Delay Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025 Edge.AI
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

class DelayTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_image imagedelay;
    vx_delay delay;

    void SetUp() override
    {
        context = vxCreateContext();
        imagedelay = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8);
        delay = vxCreateDelay(context, imagedelay, 10);
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(DelayTest, CreateDelay) {
    ASSERT_NE(delay, nullptr);
    EXPECT_EQ(delay->type, VX_TYPE_IMAGE);
    EXPECT_EQ(delay->count, 10);
}

TEST_F(DelayTest, RemoveAssociationToDelay) {
    vx_reference value = vxGetReferenceFromDelay(delay, 0);
    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_BOX_3x3);
    vx_graph graph = vxCreateGraph(context);
    vx_node node = vxCreateGenericNode(graph, kernel);
    vx_uint32 index = 0;

    vx_bool result = Delay::addAssociationToDelay(value, node, index);
    EXPECT_EQ(result, vx_true_e);

    result = Delay::removeAssociationToDelay(value, node, index);
    EXPECT_EQ(result, vx_true_e);

    vxReleaseNode(&node);
}

TEST_F(DelayTest, AddAssociationToDelay) {
    vx_reference value = vxGetReferenceFromDelay(delay, 0);
    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_BOX_3x3);
    vx_graph graph = vxCreateGraph(context);
    vx_node node = vxCreateGenericNode(graph, kernel);
    vx_uint32 index = 0;

    vx_bool result = Delay::addAssociationToDelay(value, node, index);
    EXPECT_EQ(result, vx_true_e);

    vxReleaseNode(&node);
}

TEST_F(DelayTest, DestructDelay) {
    delay->destruct();
    EXPECT_EQ(delay->set, nullptr);
    EXPECT_EQ(delay->refs, nullptr);
}
