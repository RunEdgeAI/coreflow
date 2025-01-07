/**
 * @file test_import.cpp
 * @brief Test Internal Import Object
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <gtest/gtest.h>
#include <VX/vx.h>

#include "vx_internal.h"

class ImportTest : public ::testing::Test
{
protected:
    vx_context context;
    Import* import;

    void SetUp() override
    {
        context = vxCreateContext();
        import = Import::createImportInt(context, VX_IMPORT_TYPE_XML, 1u);
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(ImportTest, CreateImport)
{
    ASSERT_NE(import, nullptr);
    EXPECT_EQ(import->type, VX_TYPE_IMPORT);
    EXPECT_EQ(import->import_type, VX_IMPORT_TYPE_XML);
    EXPECT_EQ(import->count, 1);
}

TEST_F(ImportTest, DestructImport)
{
    // Simulate adding references
    import->refs = new vx_reference[import->count];
    for (vx_uint32 i = 0; i < import->count; ++i)
    {
        import->refs[i] = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8);
        import->refs[i]->incrementReference(VX_INTERNAL);
    }

    import->destruct();
    EXPECT_EQ(import->refs, nullptr);
}
