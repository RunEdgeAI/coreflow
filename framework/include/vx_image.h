/*
 * Copyright (c) 2012-2017 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VX_IMAGE_H
#define VX_IMAGE_H

#include <VX/vx.h>

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal image implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_image Internal Image API
 * \ingroup group_internal
 * \brief The internal Image API.
 */

/*! \brief The internal representation of a \ref vx_image
 * \ingroup group_int_image
 */
class Image : public Reference
{
public:
    /**
     * @brief Construct a new Image object
     *
     * @param context   The context associated with this obj
     * @param ref       The parent ref of this obj
     * @ingroup group_int_image
     */
    Image(vx_context context, vx_reference ref);

    /**
     * @brief Destroy the Image object
     * @ingroup group_int_image
     */
    ~Image();

    /*! \brief Used to validate the vx_image types.
     * \param [in] img The vx_image to validate.
     * \ingroup group_int_image
     */
    static vx_bool isValidImage(vx_image image);

    /**
     * @brief Create a Image object
     *
     * @param[in] context       context to associate image to
     * @param[in] width         width of image
     * @param[in] height        height of image
     * @param[in] color         format / color of image
     * @param[in] is_virtual    indicate of object is virtual or not
     * @return vx_image
     * @ingroup group_int_image
     */
    static vx_image createImage(vx_context context,
                                   vx_uint32 width,
                                   vx_uint32 height,
                                   vx_df_image color,
                                   vx_bool is_virtual);

    /*! \brief Used to initialize a single plane in an image object.
     * \param [in] index The index to the plane.
     * \param [in] bpp The unit size (size of pixel).
     * \param [in] width The width in pixels.
     * \param [in] height The height in pixels.
     * \ingroup group_int_image
     */
    void initPlane(vx_uint32 index,
                   vx_uint32 soc,
                   vx_uint32 channels,
                   vx_uint32 width,
                   vx_uint32 height);

    /*! \brief Used to initialize the image meta-data structure with the correct
     * values per the df_image code.
     * \param [in,out] image The image object.
     * \param [in] width Width in pixels
     * \param [in] height Height in pixels
     * \param [in] color VX_DF_IMAGE color space.
     * \ingroup group_int_image
     */
    void initImage(vx_uint32 width, vx_uint32 height, vx_df_image color);

    /*! \brief Used to allocate an image object.
     * \param [in,out] image The image object.
     * \ingroup group_int_image
     */
    vx_bool allocateImage();

    /*! \brief Determines which VX_DF_IMAGE are supported in the sample implementation.
     * \param [in] code The df_image code to test.
     * \retval vx_bool
     * \retval vx_true_e The code is supported.
     * \retval vx_false_e The code is not supported.
     * \ingroup group_int_image
     */
    static vx_bool isSupportedFourcc(vx_df_image code);

    /**
     * @brief Validate image dimensions
     *
     * @param width     width
     * @param height    height
     * @param color     color
     * @return vx_bool
     * @ingroup group_int_image
     */
    static vx_bool isValidDimensions(vx_uint32 width, vx_uint32 height, vx_df_image color);

    /**
     * @brief Compute patch offset
     *
     * @param x     x coord
     * @param y     y coord
     * @param addr  image patch addressing
     * @return vx_uint32
     * @ingroup group_int_image
     */
    static vx_uint32 computePatchOffset(vx_uint32 x, vx_uint32 y, const vx_imagepatch_addressing_t* addr);

    /**
     * @brief Compute plane offset
     *
     * @param image     image
     * @param x         x coord
     * @param y         y coord
     * @param p         plane index
     * @return vx_uint32
     * @ingroup group_int_image
     */
    static vx_uint32 computePlaneOffset(vx_image image, vx_uint32 x, vx_uint32 y, vx_uint32 p);

    /**
     * @brief Compute patch range size
     *
     * @param range     range
     * @param addr      image patch addressing
     * @return vx_uint32
     * @ingroup group_int_image
     */
    static vx_uint32 computePatchRangeSize(vx_uint32 range, const vx_imagepatch_addressing_t* addr);

    /**
     * @brief Compute plane range size
     *
     * @param image     image
     * @param range     range
     * @param p         plane index
     * @return vx_uint32
     * @ingroup group_int_image
     */
    static vx_uint32 computePlaneRangeSize(vx_image image, vx_uint32 range, vx_uint32 p);

    /**
     * @brief Size of color channel
     *
     * @param color     color
     * @return vx_size
     * @ingroup group_int_image
     */
    static vx_size sizeOfChannel(vx_df_image color);

    /*! \brief Prints the values of the images.
     * \ingroup group_int_image
     */
    static void printImage(vx_image image);

    /*! \brief Used to free an image object.
     * Only the data is freed, not the
     * meta-data structure.
     * \ingroup group_int_image
     */
    void freeImage();

    /*! \brief Destroys an image
     * \ingroup group_int_image
     */
    void destructImage();

    /*! \brief The memory layout definition */
    vx_memory_t    memory;
    /*! \brief Width of the Image in Pixels */
    vx_uint32      width;
    /*! \brief Height of the Image in Pixels */
    vx_uint32      height;
    /*! \brief Format of the Image in VX_DF_IMAGE codes */
    vx_df_image    format;
    /*! \brief The number of active planes */
    vx_uint32      planes;
    /*! \brief The constants space (BT601 or BT709) */
    vx_enum        space;
    /*! \brief The desired color range */
    vx_enum        range;
    /*! \brief The sub-channel scaling for each plane */
    vx_uint32      scale[VX_PLANE_MAX][VX_DIM_MAX];
    /*! \brief The per-plane, per-dimension bounds (start, end). */
    vx_uint32      bounds[VX_PLANE_MAX][VX_DIM_MAX][VX_BOUND_MAX];
    /*! \brief A pointer to a parent image object. */
    vx_image       parent;
    /*! \brief The array of ROIs from this image */
    vx_image       subimages[VX_INT_MAX_REF];
    /*! \brief Indicates if the image is constant. */
    vx_bool        constant;
    /*! \brief The valid region */
    vx_rectangle_t region;
    /*! \brief The memory type */
    vx_enum        memory_type;
#if defined(EXPERIMENTAL_USE_OPENCL)
    /*! \brief This describes the type of OpenCL Image that maps to this image (if applicable). */
    cl_image_format cl_format;
#endif
};

#endif /* VX_IMAGE_H */
