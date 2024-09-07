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

#ifndef VX_OBJECT_ARRAY_H
#define VX_OBJECT_ARRAY_H

#include <VX/vx.h>

/*! \brief The internal representation of a \ref vx_object_array
 * \ingroup group_int_object_array
 */
class ObjectArray : public Reference
{
public:
    /**
     * @brief Construct a new Object Array object
     *
     * @param context
     * @param scope
     */
    ObjectArray(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Object Array object
     *
     */
    ~ObjectArray();

    static vx_object_array createObjectArray(vx_reference scope, vx_reference exemplar, vx_size count, vx_bool is_virtual);

    vx_status initObjectArray(vx_reference exemplar, vx_size num_items);

    static vx_bool isValidObjectArray(vx_object_array arr);

    static vx_bool isValidObjectArray(vx_object_array objarr, vx_enum item_type, vx_size num_items);

    void destructObjectArray();

    /*! \brief The reference table of array items */
    vx_reference items[VX_INT_MAX_REF];
    /*! \brief The number of items in the array */
    vx_size num_items;
    /*! \brief The item type of the array. */
    vx_enum item_type;
};

#endif /* VX_OBJECT_ARRAY_H */
