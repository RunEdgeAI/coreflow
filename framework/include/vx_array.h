/*
 * Copyright (c) 2011-2017 The Khronos Group Inc.
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

#ifndef VX_ARRAY_H
#define VX_ARRAY_H

#include <VX/vx.h>

#include "vx_reference.h"

/*! \brief The internal representation of a \ref vx_array
 * \ingroup group_int_array
 */
class Array : public Reference
{
public:
    /**
     * @brief Construct a new Array object
     *
     * @param context
     * @param scope
     */
    Array(vx_context context, vx_reference scope);

    /**
     * @brief Construct a new Array object
     *
     * @param context
     * @param type
     * @param scope
     */
    Array(vx_context context, vx_enum type, vx_reference scope);

    /**
     * @brief Destroy the Array object
     *
     */
    ~Array();

    static vx_array createArray(vx_context context, vx_enum item_type, vx_size capacity, vx_bool is_virtual, vx_enum type);

    vx_bool validateArray(vx_enum item_type, vx_size capacity);

    vx_bool allocateArray();

    vx_bool initVirtualArray(vx_enum item_type, vx_size capacity);

    vx_status accessArrayRange(vx_size start, vx_size end, vx_size *pStride, void **ptr, vx_enum usage);

    vx_status commitArrayRange(vx_size start, vx_size end, const void *ptr);

    vx_status copyArrayRange(vx_size start, vx_size end, vx_size stride, void *ptr, vx_enum usage, vx_enum mem_type);

    vx_status mapArrayRange(vx_size start, vx_size end, vx_map_id *map_id, vx_size *stride,
                                void **ptr, vx_enum usage, vx_enum mem_type, vx_uint32 flags);

    vx_status unmapArrayRange(vx_map_id map_id);

    void destructArray();

    /*! \brief The memory layout definition */
    vx_memory_t memory;
    /*! \brief The item type of the array. */
    vx_enum item_type;
    /*! \brief The size of array item in bytes */
    vx_size item_size;
    /*! \brief The number of items in the array */
    vx_size num_items;
    /*! \brief The array capacity */
    vx_size capacity;
    /*! \brief Offset attribute value. Used internally by LUT implementation */
    vx_uint32 offset;
};

void vxPrintArray(vx_array array);

#endif /* VX_ARRAY_H */
