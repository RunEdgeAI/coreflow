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

#include "vx_reference.h"

/*!
 * \file
 * \brief The internal array implementation
 *
 * \defgroup group_int_array Internal Array API
 * \ingroup group_internal
 * \brief The Internal Array API.
 */

/*! \brief The internal representation of a \ref vx_array
 * \ingroup group_int_array
 */
class Array : public Reference
{
public:
    /**
     * @brief Construct a new Array object
     *
     * @param context   The context associated with this object
     * @param scope     The parent reference of this object
     * @ingroup group_int_array
     */
    Array(vx_context context, vx_reference scope);

    /**
     * @brief Construct a new Array object
     *
     * @param context   The context associated with this object
     * @param type      The type of this array (arr or lut)
     * @param scope     The parent reference of this object
     * @ingroup group_int_array
     */
    Array(vx_context context, vx_enum type, vx_reference scope);

    /**
     * @brief Destroy the Array object
     * @ingroup group_int_array
     */
    ~Array();

    /**
     * @brief Create a Array object
     *
     * @param context       context object
     * @param item_type     array object type
     * @param capacity      capacity of array
     * @param is_virtual    is array virtual
     * @param type          array or lut
     * @return vx_array
     * @ingroup group_int_array
     */
    static vx_array createArray(vx_context context, vx_enum item_type, vx_size capacity, vx_bool is_virtual, vx_enum type);

    /**
     * @brief Validate array object
     *
     * @param item_type     array object type
     * @param capacity      capacity of array
     * @return vx_bool
     * @ingroup group_int_array
     */
    vx_bool validateArray(vx_enum item_type, vx_size capacity);

    /**
     * @brief Allocate memory for array object
     *
     * @return vx_bool
     * @ingroup group_int_array
     */
    vx_bool allocateArray();

    /**
     * @brief Get item size of type
     *
     * @param context     global context
     * @param item_type   item type
     * @return vx_size
     * @ingroup group_int_array
     */
    static vx_size itemSize(vx_context context, vx_enum item_type);

    /**
     * @brief Validate array item type
     *
     * @param context    global context
     * @param item_type  item type
     * @return vx_bool
     * @ingroup group_int_array
     */
    static vx_bool isValidArrayItemType(vx_context context, vx_enum item_type);

    /**
     * @brief Validate array object
     *
     * @param arr       array object to check
     * @return vx_bool
     * @ingroup group_int_array
     */
    static vx_bool isValidArray(vx_array arr);

    /**
     * @brief Initialize internal array memory
     *
     * @ingroup group_int_array
     */
    void initArrayMemory();

    /**
     * @brief Initialize virutal array object
     *
     * @param item_type     array object type
     * @param capacity      capacity of array
     * @return vx_bool
     * @ingroup group_int_array
     */
    vx_bool initVirtualArray(vx_enum item_type, vx_size capacity);

    /**
     * @brief Add items to array
     *
     * @param count     number of items to add
     * @param ptr       pointer to data
     * @param stride    size of stride
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_array
     */
    vx_status addItems(vx_size count, const void *ptr, vx_size stride);

    /**
     * @brief Truncate array to new number of items
     *
     * @param new_num_items   new number of items
     * @return vx_status  VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_array
     */
    vx_status truncate(vx_size new_num_items);

    /**
     * @brief Get item type of the array
     *
     * @return vx_enum
     * @ingroup group_int_array
     */
    vx_enum itemType() const;

    /**
     * @brief Get number of items in array
     *
     * @return vx_size
     * @ingroup group_int_array
     */
    vx_size numItems() const;

    /**
     * @brief Get capacity of array
     *
     * @return vx_size
     * @ingroup group_int_array
     */
    vx_size totalCapacity() const;

    /**
     * @brief Get item size in array
     *
     * @return vx_size  Item size in bytes
     * @ingroup group_int_array
     */
    vx_size itemSize() const;

    /**
     * @brief Get total size of array
     *
     * @return vx_size   Total size in bytes
     * @ingroup group_int_array
     */
    vx_size totalSize() const;

    /**
     * @brief Get offset value
     *
     * @return vx_uint32 Offset value
     * @ingroup group_int_array
     */
    vx_uint32 offsetVal() const;

    /**
     * @brief Access array range in object
     *
     * @param start     start index
     * @param end       end index
     * @param pStride   pointer of stride
     * @param ptr       pointer to data
     * @param usage     ro | rw | wo
     * @return vx_status
     * @ingroup group_int_array
     */
    vx_status accessArrayRange(vx_size start, vx_size end, vx_size *pStride, void **ptr, vx_enum usage);

    /**
     * @brief Commit array range
     *
     * @param start     start index
     * @param end       end index
     * @param ptr       pointer to data
     * @return vx_status
     * @ingroup group_int_array
     */
    vx_status commitArrayRange(vx_size start, vx_size end, const void *ptr);

    /**
     * @brief Copy array range
     *
     * @param start     start index
     * @param end       end index
     * @param stride    size of stride
     * @param ptr       pointer to data
     * @param usage     ro | rw | wo
     * @param mem_type  host | device
     * @return vx_status
     * @ingroup group_int_array
     */
    vx_status copyArrayRange(vx_size start, vx_size end, vx_size stride, void *ptr, vx_enum usage, vx_enum mem_type);

    /**
     * @brief Map array range
     *
     * @param start     start index
     * @param end       end index
     * @param map_id    memory map id
     * @param stride    size of stride
     * @param ptr       pointer to data
     * @param usage     ro | rw | wo
     * @param mem_type  host | device
     * @param flags     additional flags
     * @return vx_status
     * @ingroup group_int_array
     */
    vx_status mapArrayRange(vx_size start, vx_size end, vx_map_id *map_id, vx_size *stride,
                                void **ptr, vx_enum usage, vx_enum mem_type, vx_uint32 flags);

    /**
     * @brief Unmap array range
     *
     * @param map_id        memory map id
     * @return vx_status
     * @ingroup group_int_array
     */
    vx_status unmapArrayRange(vx_map_id map_id);

    /**
     * @brief Destruct function for array object
     * @ingroup group_int_array
     */
    void destruct() override final;

    /**
     * @brief Print array object
     *
     * @param array
     * @ingroup group_int_array
     */
    static void printArray(vx_array array);

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

#endif /* VX_ARRAY_H */
