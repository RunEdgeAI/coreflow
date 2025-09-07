/**
 * @file test_image.cpp
 * @brief Test Internal Image Object
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

class ImageTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_image image;
    vx_uint32 width;
    vx_uint32 height;
    vx_df_image format;

    void SetUp() override
    {
        context = vxCreateContext();
        width = height = 128;
        format = VX_DF_IMAGE_RGB;
        image = Image::createImage(context, width, height, format, vx_false_e);
    }

    void TearDown() override
    {
        vxReleaseContext(&context);
    }
};

TEST_F(ImageTest, CreateImage)
{
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->width, width);
    EXPECT_EQ(image->height, height);
    EXPECT_EQ(image->format, format);
    EXPECT_EQ(image->is_virtual, vx_false_e);
}

TEST_F(ImageTest, AllocateImage)
{
    vx_bool valid = image->allocateImage();
    EXPECT_EQ(valid, vx_true_e);
}

TEST_F(ImageTest, IsValidImage)
{
    vx_bool valid = Image::isValidImage(image);
    EXPECT_EQ(valid, vx_true_e);
}

TEST_F(ImageTest, IsSupportedFourcc)
{
    vx_bool supported = Image::isSupportedFourcc(VX_DF_IMAGE_U8);
    EXPECT_EQ(supported, vx_true_e);
}

TEST_F(ImageTest, IsValidDimensions)
{
    vx_bool valid = Image::isValidDimensions(width, height, VX_DF_IMAGE_U8);
    EXPECT_EQ(valid, vx_true_e);
}

TEST_F(ImageTest, InitImage)
{
    image->initImage(width + 10, height + 10, VX_DF_IMAGE_U32);
    EXPECT_EQ(image->width, width + 10);
    EXPECT_EQ(image->height, height + 10);
    EXPECT_EQ(image->format, VX_DF_IMAGE_U32);
}

TEST_F(ImageTest, InitPlane)
{
    // Test single plane initialization
    const vx_uint32 index = 0;
    const vx_uint32 soc = sizeof(vx_uint8);
    const vx_uint32 channels = 1;

    image->initPlane(index, soc, channels, width, height);

    // Verify memory strides
    EXPECT_EQ(image->memory.strides[index][VX_DIM_C], soc);

    // Verify dimensions
    EXPECT_EQ(image->memory.dims[index][VX_DIM_C], channels);
    EXPECT_EQ(image->memory.dims[index][VX_DIM_X], width);
    EXPECT_EQ(image->memory.dims[index][VX_DIM_Y], height);
    EXPECT_EQ(image->memory.ndims, VX_DIM_MAX);

    // Verify scales
    EXPECT_EQ(image->scale[index][VX_DIM_C], 1);
    EXPECT_EQ(image->scale[index][VX_DIM_X], 1);
    EXPECT_EQ(image->scale[index][VX_DIM_Y], 1);

    // Verify bounds
    EXPECT_EQ(image->bounds[index][VX_DIM_C][VX_BOUND_START], 0);
    EXPECT_EQ(image->bounds[index][VX_DIM_C][VX_BOUND_END], channels);
    EXPECT_EQ(image->bounds[index][VX_DIM_X][VX_BOUND_START], 0);
    EXPECT_EQ(image->bounds[index][VX_DIM_X][VX_BOUND_END], width);
    EXPECT_EQ(image->bounds[index][VX_DIM_Y][VX_BOUND_START], 0);
    EXPECT_EQ(image->bounds[index][VX_DIM_Y][VX_BOUND_END], height);
}

TEST_F(ImageTest, SizeOfChannel)
{
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_U1), 0ul);
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_S16), sizeof(vx_uint16));
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_U16), sizeof(vx_uint16));
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_U32), sizeof(vx_uint32));
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_S32), sizeof(vx_uint32));
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_F32), sizeof(vx_uint32));
    EXPECT_EQ(Image::sizeOfChannel(VX_DF_IMAGE_U8), 1ul);
    EXPECT_EQ(Image::sizeOfChannel((vx_df_image)0), 0ul);
}

TEST_F(ImageTest, ComputePlaneRangeSize)
{
    // Test channel format
    EXPECT_EQ(Image::computePlaneRangeSize(image, width*height, 0), 0);

    // Test edge case - zero range
    EXPECT_EQ(Image::computePlaneRangeSize(image, 0, 0), 0);

    // Test invalid plane index
    EXPECT_EQ(Image::computePlaneRangeSize(image, 0, 5), 0);
}

TEST_F(ImageTest, ComputePatchRangeSize)
{
    vx_rectangle_t patch = {10, 10, 20, 20};  // 10x10 patch
    vx_imagepatch_addressing_t addr;
    addr.dim_y     = patch.end_y - patch.start_y;
    addr.stride_x  = image->memory.strides[0][VX_DIM_X];
    addr.stride_y  = image->memory.strides[0][VX_DIM_Y];
    addr.stride_x_bits = image->memory.stride_x_bits[0];
    addr.step_x    = image->scale[0][VX_DIM_X];
    addr.step_y    = image->scale[0][VX_DIM_Y];
    addr.scale_x   = VX_SCALE_UNITY / image->scale[0][VX_DIM_X];
    addr.scale_y   = VX_SCALE_UNITY / image->scale[0][VX_DIM_Y];

    // Test channel format
    EXPECT_EQ(Image::computePatchRangeSize(patch.end_x - patch.start_x, &addr), 0);

    // Test invalid rectangle
    EXPECT_EQ(Image::computePatchRangeSize(0, &addr), 0);

    // Test null addr
    EXPECT_EQ(Image::computePatchRangeSize(0, nullptr), 0);
}

TEST_F(ImageTest, ComputePlaneOffset)
{
    // Test plane offset
    EXPECT_EQ(Image::computePlaneOffset(image, width, height, 0), 0);

    // Test zero dimensions
    EXPECT_EQ(Image::computePlaneOffset(image, 0, 0, 0), 0);

    // Test invalid plane index
    EXPECT_EQ(Image::computePlaneOffset(image, width, height, 5), 0);
}

TEST_F(ImageTest, ComputePatchOffset)
{
    vx_rectangle_t patch = {10, 10, 20, 20};  // 10x10 patch
    vx_imagepatch_addressing_t addr;
    addr.dim_y     = patch.end_y - patch.start_y;
    addr.stride_x  = image->memory.strides[0][VX_DIM_X];
    addr.stride_y  = image->memory.strides[0][VX_DIM_Y];
    addr.stride_x_bits = image->memory.stride_x_bits[0];
    addr.step_x    = image->scale[0][VX_DIM_X];
    addr.step_y    = image->scale[0][VX_DIM_Y];
    addr.scale_x   = VX_SCALE_UNITY / image->scale[0][VX_DIM_X];
    addr.scale_y   = VX_SCALE_UNITY / image->scale[0][VX_DIM_Y];

    // Test channel format
    EXPECT_EQ(Image::computePatchOffset(patch.start_x, patch.start_y, &addr), 0);

    // Test invalid rectangle
    // end < start
    EXPECT_EQ(Image::computePatchOffset(patch.start_x + 20, patch.start_y - 10, &addr), 0);

    // Test null addr
    EXPECT_EQ(Image::computePatchOffset(0, 0, nullptr), 0);

    // Test zero width
    EXPECT_EQ(Image::computePatchOffset(0, 0, &addr), 0);

}

TEST_F(ImageTest, FreeImage)
{
    image->freeImage();
    for (vx_uint32 p = 0; p < image->memory.nptrs; p++)
        EXPECT_EQ(image->memory.ptrs[p], nullptr);
}

TEST_F(ImageTest, Destruct)
{
    image->destruct();
    EXPECT_EQ(image->memory.allocated, vx_false_e);
}