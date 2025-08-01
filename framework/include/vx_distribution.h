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

#ifndef VX_DISTRIBUTION_H
#define VX_DISTRIBUTION_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal distribution implementation
 *
 * \defgroup group_int_distribution Internal Distribution API
 * \ingroup group_internal
 * \brief The Internal Distribution API.
 */

/*! \brief A Distribution.
 * \ingroup group_int_distribution
 */
class Distribution : public Reference
{
public:
    /**
     * @brief Construct a new Distribution object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_distribution
     */
    Distribution(vx_context context, vx_reference scope);

    /**
     * @brief Get the number of dimensions in the distribution
     *
     * @return vx_size The number of dimensions
     * @ingroup group_int_distribution
     */
    vx_size dims() const;

    /**
     * @brief Get the range of the distribution
     *
     * @return vx_uint32 The range value
     * @ingroup group_int_distribution
     */
    vx_uint32 range() const;

    /**
     * @brief Get the number of bins in the distribution
     *
     * @return vx_size The number of bins
     * @ingroup group_int_distribution
     */
    vx_size bins() const;

    /**
     * @brief Get the window size of the distribution
     *
     * @return vx_uint32 The window size
     * @ingroup group_int_distribution
     */
    vx_uint32 window() const;

    /**
     * @brief Get the offset of the distribution
     *
     * @return vx_int32 The offset value
     * @ingroup group_int_distribution
     */
    vx_int32 offset() const;

    /**
     * @brief Get the size of the distribution in bytes
     *
     * @return vx_size The size in bytes
     * @ingroup group_int_distribution
     */
    vx_uint32 size() const;

    /**
     * @brief Access the distribution data
     *
     * @param ptr   The pointer to the data to access
     * @param usage The usage of the memory (read/write)
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_distribution
     */
    vx_status access(void **ptr, vx_enum usage);

    /**
     * @brief Commit the distribution with the given data
     *
     * @param ptr   The pointer to the data to commit
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_distribution
     */
    vx_status commit(const void *ptr);

    /**
     * @brief Copy the distribution data to/from user memory
     *
     * @param user_ptr The pointer to the user memory
     * @param usage    The usage of the memory (read/write)
     * @param mem_type The type of memory (host, opencl, etc.)
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_distribution
     */
    vx_status copy(void *user_ptr, vx_enum usage, vx_enum mem_type);

    /**
     * @brief Map the distribution to a memory map
     *
     * @param map_id    The map id to use
     * @param ptr       The pointer to the mapped memory
     * @param usage     The usage of the memory (read/write)
     * @param mem_type  The type of memory (host, opencl, etc.)
     * @param flags     Additional flags for mapping
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_distribution
     */
    vx_status map(vx_map_id *map_id, void **ptr, vx_enum usage, vx_enum mem_type,
                  vx_bitfield flags);

    /**
     * @brief Unmap the distribution from the memory map
     *
     * @param map_id    The map id to unmap
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_distribution
     */
    vx_status unmap(vx_map_id map_id);

    /**
     * @brief Destroy the Distribution object
     * @ingroup group_int_distribution
     */
    ~Distribution();

    /*! \brief Destroys a distribution.
     * \ingroup group_int_distribution
     */
    void destruct() override final;

    /*! \brief Memory layout */
    vx_memory_t memory;
    /*! \brief The total number of the values in the active X dimension of the distribution. */
    vx_uint32 range_x;
    /*! \brief The total number of the values in the active Y dimension of the distribution. */
    vx_uint32 range_y;
    /*! \brief The number of inactive elements from zero in the X dimension */
    vx_int32 offset_x;
    /*! \brief The number of inactive elements from zero in the Y dimension */
    vx_int32 offset_y;
};

#endif /* VX_DISTRIBUTION_H */
