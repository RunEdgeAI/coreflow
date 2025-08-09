/**
 * @file test_array.cpp
 * @brief Test Internal Array Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025 Edge.AI
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

using namespace corevx;

class ArrayTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_enum type;
    vx_enum item_type;
    vx_size capacity;
    vx_bool is_virtual;
    vx_array array;

    void SetUp() override
    {
        context = vxCreateContext();
        type = VX_TYPE_ARRAY;
        item_type = VX_TYPE_UINT8;
        capacity = 10;
        is_virtual = vx_false_e;
        array = Array::createArray(context, item_type, capacity, is_virtual, type);
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(ArrayTest, CreateArray)
{
    ASSERT_NE(array, nullptr);
    EXPECT_EQ(array->item_type, item_type);
    EXPECT_EQ(array->capacity, capacity);
    EXPECT_EQ(array->is_virtual, is_virtual);
}

TEST_F(ArrayTest, ValidateArray)
{
    vx_bool valid = array->validateArray(item_type, capacity);
    EXPECT_EQ(valid, vx_true_e);
}

TEST_F(ArrayTest, AllocateArray)
{
    vx_bool allocated = array->allocateArray();
    EXPECT_EQ(allocated, vx_true_e);
}

TEST_F(ArrayTest, ItemSize)
{
    vx_size size = Array::itemSize(context, item_type);
    EXPECT_GT(size, 0);
}

TEST_F(ArrayTest, IsValidArrayItemType)
{
    vx_bool valid = Array::isValidArrayItemType(context, item_type);
    EXPECT_EQ(valid, vx_true_e);
}

TEST_F(ArrayTest, IsValidArray)
{
    vx_bool valid = Array::isValidArray(array);
    EXPECT_EQ(valid, vx_true_e);
}

TEST_F(ArrayTest, InitArrayMemory)
{
    array->initArrayMemory();
    EXPECT_NE(array->memory.ptrs, nullptr);
}

TEST_F(ArrayTest, InitVirtualArray)
{
    Reference::releaseReference((vx_reference*)&array, VX_TYPE_ARRAY, VX_EXTERNAL, nullptr);
    vx_enum new_item_type = VX_TYPE_INT16;
    vx_size new_capacity = 20;
    array = Array::createArray(context, new_item_type, new_capacity, vx_true_e, VX_TYPE_ARRAY);
    vx_bool result = array->initVirtualArray(new_item_type, new_capacity);
    EXPECT_EQ(result, vx_true_e);
    EXPECT_EQ(array->item_type, new_item_type);
    EXPECT_EQ(array->capacity, new_capacity);
}

TEST_F(ArrayTest, AccessArrayRange)
{
    // Add items to the array
    constexpr vx_size count = 5;
    const vx_uint8 items[count] = {1, 2, 3, 4, 5};
    vx_status add_status = vxAddArrayItems(array, count, items, sizeof(vx_uint8));
    EXPECT_EQ(add_status, VX_SUCCESS);

    vx_size start = 0;
    vx_size end = count;
    vx_size stride;
    void* ptr = nullptr;
    vx_enum usage = VX_READ_ONLY;

    vx_status status = array->accessArrayRange(start, end, &stride, &ptr, usage);
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_NE(ptr, nullptr);
}

TEST_F(ArrayTest, CommitArrayRange)
{
    // Add items to the array
    constexpr vx_size count = 5;
    const vx_uint8 items[count] = {1, 2, 3, 4, 5};
    vx_status add_status = vxAddArrayItems(array, count, items, sizeof(vx_uint8));
    EXPECT_EQ(add_status, VX_SUCCESS);

    vx_size start = 0;
    vx_size end = count;
    void *ptr = new vx_uint8[count];
    vx_status status = array->commitArrayRange(start, end, ptr);
    EXPECT_EQ(status, VX_SUCCESS);
    ::operator delete[](ptr);
}

TEST_F(ArrayTest, CopyArrayRange)
{
    // Add items to the array
    constexpr vx_size count = 5;
    const vx_uint8 items[count] = {1, 2, 3, 4, 5};
    vx_status add_status = vxAddArrayItems(array, count, items, sizeof(vx_uint8));
    EXPECT_EQ(add_status, VX_SUCCESS);

    vx_size start = 0;
    vx_size end = count;
    vx_size stride = sizeof(vx_uint8);
    void *ptr = new vx_uint8[count];
    vx_enum usage = VX_READ_ONLY;
    vx_enum mem_type = VX_MEMORY_TYPE_HOST;
    vx_status status = array->copyArrayRange(start, end, stride, ptr, usage, mem_type);
    EXPECT_EQ(status, VX_SUCCESS);
    ::operator delete[](ptr);
}

TEST_F(ArrayTest, MapArrayRange)
{
    vx_size start = 0;
    vx_size end = 5;
    vx_size stride;
    void *ptr;
    vx_enum usage = VX_READ_ONLY;

    // Add items to the array
    constexpr vx_size count = 5;
    vx_uint8 items[count] = {1, 2, 3, 4, 5};
    vx_status add_status = vxAddArrayItems(array, count, items, sizeof(vx_uint8));
    EXPECT_EQ(add_status, VX_SUCCESS);

    vx_map_id map_id;
    vx_status status = array->mapArrayRange(start, end, &map_id, &stride, &ptr, usage, VX_MEMORY_TYPE_HOST, 0);
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_NE(ptr, nullptr);

    // Unmap the array range
    status = array->unmapArrayRange(map_id);
    EXPECT_EQ(status, VX_SUCCESS);
}

TEST_F(ArrayTest, UnmapArrayRange)
{
    vx_size start = 0;
    vx_size end = 5;
    vx_size stride;
    void *ptr;
    vx_enum usage = VX_READ_ONLY;

    // Add items to the array
    constexpr vx_size count = 5;
    vx_uint8 items[count] = {1, 2, 3, 4, 5};
    vx_status add_status = vxAddArrayItems(array, count, items, sizeof(vx_uint8));
    EXPECT_EQ(add_status, VX_SUCCESS);

    vx_map_id map_id;
    vx_status status = array->mapArrayRange(start, end, &map_id, &stride, &ptr, usage, VX_MEMORY_TYPE_HOST, 0);
    EXPECT_EQ(status, VX_SUCCESS);
    EXPECT_NE(ptr, nullptr);

    // Unmap the array range
    status = array->unmapArrayRange(map_id);
    EXPECT_EQ(status, VX_SUCCESS);
}

TEST_F(ArrayTest, DestructArray)
{
    array->destruct();
    for (vx_uint32 p = 0; p < array->memory.nptrs; p++)
        EXPECT_EQ(array->memory.ptrs[p], nullptr);
    EXPECT_EQ(array->memory.allocated, vx_false_e);
}
