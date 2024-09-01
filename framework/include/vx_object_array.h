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

static vx_bool ownIsValidObjectArray(vx_object_array arr);

static vx_status ownInitObjectArrayInt(vx_object_array arr, vx_reference exemplar, vx_size num_items);

vx_object_array ownCreateObjectArrayInt(vx_reference scope, vx_reference exemplar, vx_size count, vx_bool is_virtual);

void ownDestructObjectArray(vx_reference ref);

vx_bool ownValidateObjectArray(vx_object_array objarr, vx_enum item_type, vx_size num_items);


/*! \brief The internal representation of a \ref vx_object_array
 * \ingroup group_int_object_array
 */
class ObjectArray : public Reference
{
public:
    ObjectArray(/* args */) = default;
    ~ObjectArray() = default;

    /*! \brief The reference table of array items */
    vx_reference items[VX_INT_MAX_REF];
    /*! \brief The number of items in the array */
    vx_size num_items;
    /*! \brief The item type of the array. */
    vx_enum item_type;
};

#endif /* VX_OBJECT_ARRAY_H */
