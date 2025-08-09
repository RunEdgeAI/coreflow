/**
 * @file test_userdataobject.cpp
 * @brief Test Internal UserDataObject obj
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

#ifdef OPENVX_USE_USER_DATA_OBJECT

class UserDataObjectTest : public ::testing::Test
{
protected:
    vx_context context;
    UserDataObject* udata;

    void SetUp() override
    {
        context = vxCreateContext();
        udata = new UserDataObject(context, nullptr);
    }

    void TearDown() override
    {
        delete udata;
        vxReleaseContext(&context);
    }
};

TEST_F(UserDataObjectTest, CreateUserDataObject)
{
    ASSERT_NE(udata, nullptr);
    EXPECT_EQ(udata->type, VX_TYPE_USER_DATA_OBJECT);
    EXPECT_EQ(udata->size, 0);
    for (vx_uint32 p = 0; p < VX_PLANE_MAX; p++)
        EXPECT_EQ(udata->memory.ptrs[p], nullptr);
    EXPECT_STREQ(udata->type_name, "");
}

TEST_F(UserDataObjectTest, AllocateUserDataObjectZeroSize)
{
    udata->size = 0;
    vx_bool result = udata->allocateUserDataObject();
    EXPECT_EQ(result, vx_false_e);
    EXPECT_EQ(udata->memory.allocated, vx_false_e);
}

TEST_F(UserDataObjectTest, AllocateUserDataObjectValidSize)
{
    udata->size = 128;
    vx_bool result = udata->allocateUserDataObject();
    EXPECT_EQ(result, vx_true_e);
    EXPECT_NE(udata->memory.ptrs, nullptr);
}

TEST_F(UserDataObjectTest, UserDataObjectDestructor)
{
    udata->size = 128;
    udata->memory.nptrs = 1;
    udata->memory.ndims = 1;
    udata->memory.dims[0][0] = udata->size;
    udata->allocateUserDataObject();
    EXPECT_NE(udata->memory.ptrs, nullptr);

    // Destroy the object
    udata->~UserDataObject();
    for (vx_uint32 p = 0; p < udata->memory.nptrs; p++)
        EXPECT_EQ(udata->memory.ptrs[p], nullptr);
}

#endif /* OPENVX_USE_USER_DATA_OBJECT */