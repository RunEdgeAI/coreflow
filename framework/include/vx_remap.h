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

#ifndef VX_REMAP_H
#define VX_REMAP_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The Internal Remap API
 * \defgroup group_int_remap Internal Remap API
 * \ingroup group_internal
 * \brief The Internal Remap API
 */

/*! \brief A remap is a 2D image of float32 pairs.
 * \ingroup group_int_remap
 */
class Remap : public Reference
{
public:
    /**
     * @brief Construct a new Remap object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_remap
     */
    Remap(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Remap object
     * @ingroup group_int_remap
     */
    ~Remap();

    /**
     * @brief Validate remap object
     *
     * @param remap
     * @return vx_bool
     * @ingroup group_int_remap
     */
    static vx_bool isValidRemap(vx_remap remap);

    /**
     * @brief Set the Coord Value object
     *
     * @param dst_x     destination x coord
     * @param dst_y     destination y coord
     * @param src_x     source x coord
     * @param src_y     source y coord
     * @return vx_status
     * @ingroup group_int_remap
     */
    vx_status setCoordValue(vx_uint32 dst_x, vx_uint32 dst_y, vx_float32 src_x, vx_float32 src_y);

    /**
     * @brief Get the Coord Value object
     *
     * @param dst_x     destination x coord
     * @param dst_y     destination y coord
     * @param src_x     source x coord
     * @param src_y     source y coord
     * @return vx_status
     * @ingroup group_int_remap
     */
    vx_status getCoordValue(vx_uint32 dst_x, vx_uint32 dst_y, vx_float32 *src_x, vx_float32 *src_y);

    /**
     * @brief Function to destroy a remap object
     * @ingroup group_int_remap
     */
    void destruct() override final;

    /*! \brief The memory layout */
    vx_memory_t memory;
    /*! \brief Input Width */
    vx_uint32 src_width;
    /*! \brief Input Height */
    vx_uint32 src_height;
    /*! \brief Output Width */
    vx_uint32 dst_width;
    /*! \brief Output Height */
    vx_uint32 dst_height;
};

#endif /* VX_REMAP_H */
