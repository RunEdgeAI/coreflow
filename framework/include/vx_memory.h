/*
 * Copyright (c) 2012-2017 The Khronos Group Inc.
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

#ifndef VX_MEMORY_H
#define VX_MEMORY_H

#include "vx_internal.h"

/*! \file
 * \brief Defines an API for memory operations.
 *
 * \defgroup group_int_memory Internal Memory API.
 * \ingroup group_internal
 * \brief The Internal Memory API.
 */

/*! \brief The internal representation of a \ref vx_memory_t
 * \ingroup group_int_memory
 */
class Memory
{
public:
    /*! \brief Frees a memory block.
     * \ingroup group_int_memory
     * \param [in] context The reference to the overall context.
     * \param [in] memory The memory block.
     * \return vx_true_e if successful.
     */
    static vx_bool freeMemory(vx_context context, vx_memory_t *memory);

    /*! \brief Allocates a memory block.
     * \ingroup group_int_memory
     * \param [in] context The reference to the overall context.
     * \param [in] memory The memory block.
     * \return vx_true_e if successful.
     */
    static vx_bool allocateMemory(vx_context context, vx_memory_t *memory);

    /*! \brief Print info of memory block.
     * \ingroup group_int_memory
     * \param [in] mem The memory block.
     */
    static void printMemory(vx_memory_t *mem);

    /*! \brief Compute size of memory block.
     * \ingroup group_int_memory
     * \param [in] memory The memory block.
     * \param [in] p The plane index.
     * \return Size of memory block.
     */
    static vx_size computeMemorySize(vx_memory_t *memory, vx_uint32 p);

    /*! \brief Get pointer to memory block.
     * \ingroup group_int_memory
     * \param memory The memory block.
     * \param c The channel index.
     * \param x The stride in x-dim.
     * \param y The stride in y-dim.
     * \param p The plane index.
     * \return Pointer to memory block.
     */
    static void* formatMemoryPtr(vx_memory_t *memory,
                                            vx_uint32 c,
                                            vx_uint32 x,
                                            vx_uint32 y,
                                            vx_uint32 p);
};

#endif /* VX_MEMORY_H */
