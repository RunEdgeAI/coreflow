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
    Image(/* args */) = default;
    ~Image() = default;

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
