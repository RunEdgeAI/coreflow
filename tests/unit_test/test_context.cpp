/**
 * @file test_context.cpp
 * @brief Test Context Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025 Edge.AI
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

class ContextTest : public ::testing::Test
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

TEST_F(ContextTest, CreateContext)
{
    ASSERT_NE(context, nullptr);
    EXPECT_EQ(context->type, VX_TYPE_CONTEXT);
}

TEST_F(ContextTest, SetContextAttribute)
{
    vx_border_t border = { VX_BORDER_CONSTANT, { {128} } };
    vx_status status = vxSetContextAttribute(context, VX_CONTEXT_IMMEDIATE_BORDER, &border, sizeof(border));
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_EQ(context->imm_border.mode, VX_BORDER_CONSTANT);
    EXPECT_EQ(context->imm_border.constant_value.U8, 128);
}

TEST_F(ContextTest, QueryContext)
{
    vx_uint16 vendor_id;
    vx_status status = vxQueryContext(context, VX_CONTEXT_VENDOR_ID, &vendor_id, sizeof(vendor_id));
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_EQ(vendor_id, VX_ID_KHRONOS);

    vx_uint16 version;
    status = vxQueryContext(context, VX_CONTEXT_VERSION, &version, sizeof(version));
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_EQ(version, VX_VERSION);
}

TEST_F(ContextTest, AddAndRemoveReference)
{
    vx_image image = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8);
    ASSERT_NE(image, nullptr);

    vx_bool added = context->addReference((vx_reference)image);
    EXPECT_EQ(added, vx_true_e);

    vx_reference ref = (vx_reference)image;
    vx_bool removed = context->removeReference(ref);
    EXPECT_EQ(removed, vx_true_e);

    vxReleaseImage(&image);
}

TEST_F(ContextTest, MemoryMapAndUnmap)
{
    vx_image image = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8);
    ASSERT_NE(image, nullptr);

    vx_map_id map_id;
    void* ptr = nullptr;
    vx_size size = 128 * 128;
    vx_memory_map_extra extra;
    vx_bool mapped = context->memoryMap((vx_reference)image, size, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0, &extra, &ptr, &map_id);
    EXPECT_EQ(mapped, vx_true_e);
    EXPECT_NE(ptr, nullptr);

    context->memoryUnmap(map_id);

    vxReleaseImage(&image);
}

TEST_F(ContextTest, IsValidContext)
{
    EXPECT_TRUE(Context::isValidContext(context));
    vx_context invalid_context = nullptr;
    EXPECT_FALSE(Context::isValidContext(invalid_context));
}

TEST_F(ContextTest, IsValidType)
{
    EXPECT_TRUE(Context::isValidType(VX_TYPE_IMAGE));
    EXPECT_FALSE(Context::isValidType(VX_TYPE_INVALID));
}

TEST_F(ContextTest, IsValidImport)
{
    EXPECT_TRUE(Context::isValidImport(VX_MEMORY_TYPE_HOST));
    EXPECT_FALSE(Context::isValidImport(VX_MEMORY_TYPE_NONE));
}

TEST_F(ContextTest, AddAccessor)
{
    vx_uint32 index;
    void* ptr = nullptr;
    vx_bool added = context->addAccessor(128, VX_READ_ONLY, ptr, nullptr, &index, nullptr);
    EXPECT_TRUE(added);
    context->removeAccessor(index);
}

TEST_F(ContextTest, FindAccessor)
{
    vx_uint32 index;
    void* ptr = nullptr;
    vx_bool added = context->addAccessor(128, VX_READ_ONLY, ptr, nullptr, &index, nullptr);
    EXPECT_EQ(vx_true_e, added);
    EXPECT_NE(ptr, nullptr);

    vx_uint32 found_index;
    vx_bool found = context->findAccessor(ptr, &found_index);
    EXPECT_EQ(vx_true_e, found);
    EXPECT_EQ(index, found_index);

    context->removeAccessor(index);
}

TEST_F(ContextTest, FindMemoryMap)
{
    vx_image image = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8);
    ASSERT_NE(image, nullptr);

    vx_map_id map_id;
    void* ptr = nullptr;
    vx_size size = 128 * 128;
    vx_memory_map_extra extra;
    vx_bool mapped = context->memoryMap((vx_reference)image, size, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0, &extra, &ptr, &map_id);
    EXPECT_TRUE(mapped);

    vx_bool found = context->findMemoryMap((vx_reference)image, map_id);
    EXPECT_TRUE(found);

    context->memoryUnmap(map_id);
    vxReleaseImage(&image);
}

TEST_F(ContextTest, RemoveAccessor)
{
    vx_uint32 index;
    void* ptr = nullptr;
    vx_bool added = context->addAccessor(128, VX_READ_ONLY, ptr, nullptr, &index, nullptr);
    EXPECT_TRUE(added);

    context->removeAccessor(index);
    vx_uint32 found_index;
    vx_bool found = context->findAccessor(ptr, &found_index);
    EXPECT_FALSE(found);
}
