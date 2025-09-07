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
namespace coreflow {

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
     * @brief Create a remap object
     *
     * @param context   The context associated with this obj
     * @param src_width The width of the source image
     * @param src_height The height of the source image
     * @param dst_width The width of the destination image
     * @param dst_height The height of the destination image
     * @return vx_remap The remap object
     * @ingroup group_int_remap
     */
    static vx_remap createRemap(vx_context context, vx_uint32 src_width, vx_uint32 src_height, vx_uint32 dst_width, vx_uint32 dst_height);

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
     * @brief Get the source width
     *
     * @return vx_uint32 The source width
     * @ingroup group_int_remap
     */
    vx_uint32 srcWidth() const;

    /**
     * @brief Get the source height
     *
     * @return vx_uint32 The source height
     * @ingroup group_int_remap
     */
    vx_uint32 srcHeight() const;

    /**
     * @brief Get the destination width
     *
     * @return vx_uint32 The destination width
     * @ingroup group_int_remap
     */
    vx_uint32 dstWidth() const;

    /**
     * @brief Get the destination height
     *
     * @return vx_uint32 The destination height
     * @ingroup group_int_remap
     */
    vx_uint32 dstHeight() const;

    /**
     * @brief Set the Remap Point
     *
     * @param dst_x     destination x coord
     * @param dst_y     destination y coord
     * @param src_x     source x coord
     * @param src_y     source y coord
     * @return vx_status
     * @ingroup group_int_remap
     */
    vx_status setRemapPoint(vx_uint32 dst_x, vx_uint32 dst_y, vx_float32 src_x, vx_float32 src_y);

    /**
     * @brief Get the Remap Point
     *
     * @param dst_x     destination x coord
     * @param dst_y     destination y coord
     * @param src_x     source x coord
     * @param src_y     source y coord
     * @return vx_status
     * @ingroup group_int_remap
     */
    vx_status getRemapPoint(vx_uint32 dst_x, vx_uint32 dst_y, vx_float32 *src_x, vx_float32 *src_y);

    /**
     * @brief Copy a patch of remap data to/from user memory
     *
     * @param rect                  The rectangle to copy
     * @param user_stride_y         The stride in bytes for the user memory
     * @param user_ptr              The pointer to the user memory
     * @param user_coordinate_type  The type of coordinates in the user memory
     * @param usage                 The usage of the memory (read/write)
     * @param user_mem_type         The type of memory (host, opencl, etc.)
     * @return vx_status            VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_remap
     */
    vx_status copyPatch(const vx_rectangle_t *rect, vx_size user_stride_y, void *user_ptr,
                            vx_enum user_coordinate_type, vx_enum usage, vx_enum user_mem_type);

    /**
     * @brief Map a patch of remap data for reading or writing
     * @param rect                  The rectangle to map
     * @param map_id                The ID of the map
     * @param stride_y              The stride in bytes for the mapped memory
     * @param ptr                   The pointer to the mapped memory
     * @param coordinate_type       The type of coordinates in the mapped memory
     * @param usage                 The usage of the memory (read/write)
     * @param mem_type              The type of memory (host, opencl, etc.)
     * @return vx_status            VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_remap
     */
    vx_status mapPatch(const vx_rectangle_t *rect, vx_map_id *map_id, vx_size *stride_y,
                            void **ptr, vx_enum coordinate_type, vx_enum usage, vx_enum mem_type);

    /**
     * @brief Unmap a previously mapped patch of remap data
     * @param map_id                The ID of the map to unmap
     * @return vx_status            VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_remap
     */
    vx_status unmapPatch(vx_uint32 map_id);

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

} // namespace coreflow

#endif /* VX_REMAP_H */