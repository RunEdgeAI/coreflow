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

#ifndef VX_PYRAMID_H
#define VX_PYRAMID_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal pyramid implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_pyramid Internal Pyramid API
 * \ingroup group_internal
 * \brief The Internal Pyramid API.
 */

/*! \brief A pyramid object. Contains a set of scaled images.
 * \ingroup group_int_pyramid
 */
class Pyramid : public Reference
{
public:
    /**
     * @brief Construct a new Pyramid object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_pyramid
     */
    Pyramid(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Pyramid object
     * @ingroup group_int_pyramid
     */
    ~Pyramid();

    /*! \brief Initializes the internals of a pyramid structure
     * \ingroup group_int_pyramid
     */
    vx_status initPyramid(vx_size levels,
                          vx_float32 scale,
                          vx_uint32 width,
                          vx_uint32 height,
                          vx_df_image format);

    /*! \brief Destroys a pyrmid object.
     * \ingroup group_int_pyramid
     */
    void destruct() override final;

    /*! \brief Number of levels in the pyramid */
    vx_size numLevels;
    /*! \brief Array of images */
    vx_image *levels;
    /*! \brief Scaling factor between levels of the pyramid. */
    vx_float32 scale;
    /*! \brief Level 0 width */
    vx_uint32 width;
    /*! \brief Level 0 height */
    vx_uint32 height;
    /*! \brief Format for all levels */
    vx_df_image format;
};

#endif /* VX_PYRAMID_H */
