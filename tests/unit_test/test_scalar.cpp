/**
 * @file test_scalar.cpp
 * @brief Test Internal Scalar Object
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

class ScalarTest : public ::testing::Test
{
protected:
    vx_context context;
    Scalar* scalar;

    void SetUp() override
    {
        context = vxCreateContext();
        scalar = new Scalar(context, nullptr);
    }

    void TearDown() override
    {
        delete scalar;
        vxReleaseContext(&context);
    }
};

TEST_F(ScalarTest, Constructor)
{
    ASSERT_NE(scalar, nullptr);
    EXPECT_EQ(scalar->type, VX_TYPE_SCALAR);
    EXPECT_EQ(scalar->data_addr, nullptr);
    EXPECT_EQ(scalar->data_len, 0);
}

TEST_F(ScalarTest, AllocateData)
{
    vx_uint32 test_value = 42;
    scalar->data_len = sizeof(test_value);
    scalar->data_addr = ::operator new(scalar->data_len);
    memcpy(scalar->data_addr, &test_value, scalar->data_len);

    EXPECT_NE(scalar->data_addr, nullptr);
    EXPECT_EQ(*(vx_uint32*)scalar->data_addr, test_value);
}

TEST_F(ScalarTest, DataTypeHandling)
{
    // Test with VX_TYPE_INT32
    vx_int32 int_value = -42;
    scalar->data_type = VX_TYPE_INT32;
    scalar->data_len = sizeof(int_value);
    scalar->data_addr = ::operator new(scalar->data_len);
    memcpy(scalar->data_addr, &int_value, scalar->data_len);

    EXPECT_EQ(*(vx_int32*)scalar->data_addr, int_value);
    scalar->destruct();

    // Test with VX_TYPE_FLOAT32
    vx_float32 float_value = 3.14f;
    scalar->data_type = VX_TYPE_FLOAT32;
    scalar->data_len = sizeof(float_value);
    scalar->data_addr = ::operator new(scalar->data_len);
    memcpy(scalar->data_addr, &float_value, scalar->data_len);

    EXPECT_FLOAT_EQ(*(vx_float32*)scalar->data_addr, float_value);
}

TEST_F(ScalarTest, DestructWithNoData)
{
    scalar->destruct();
    EXPECT_EQ(scalar->data_addr, nullptr);
    EXPECT_EQ(scalar->data_len, 0);
}

TEST_F(ScalarTest, DestructWithData)
{
    // Allocate data
    scalar->data_len = sizeof(vx_uint32);
    scalar->data_addr = ::operator new(scalar->data_len);
    EXPECT_NE(scalar->data_addr, nullptr);

    // Destruct and verify cleanup
    scalar->destruct();
    EXPECT_EQ(scalar->data_addr, nullptr);
    EXPECT_EQ(scalar->data_len, 0);
}