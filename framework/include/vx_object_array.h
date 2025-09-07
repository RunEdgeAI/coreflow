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

/*!
 * \file
 * \brief The internal object array implementation
 *
 * \defgroup group_int_object_array Internal Object Array API
 * \ingroup group_internal
 * \brief The Internal Object Array API.
 */

/*! \brief The internal representation of a \ref vx_object_array
 * \ingroup group_int_object_array
 */
namespace coreflow {

class ObjectArray : public Reference
{
public:
    /**
     * @brief Construct a new Object Array object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_object_array
     */
    ObjectArray(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Object Array object
     * @ingroup group_int_object_array
     */
    ~ObjectArray();

    /**
     * @brief Create a Object Array object
     *
     * @param context   The context associated with this obj
     * @param type      The type of the object array
     * @return vx_object_array The object array object
     * @ingroup group_int_object_array
     */
    static vx_object_array createObjectArray(vx_context context, vx_enum type);

    /**
     * @brief Create a Object Array object
     *
     * @param scope             Parent ref of obj
     * @param exemplar          ref to create arry of
     * @param count             depth of obj arr
     * @param is_virtual        indicate if obj should be virtual
     * @return vx_object_array  Object array created
     * @ingroup group_int_object_array
     */
    static vx_object_array createObjectArray(
        vx_reference scope, vx_reference exemplar, vx_size count, vx_bool is_virtual = vx_false_e);

    /**
     * @brief Initialize object array
     *
     * @param exemplar      Ref of object array
     * @param num_items     num of items in array
     * @return vx_status    VX_SUCCESS if successful
     * @ingroup group_int_object_array
     */
    vx_status initObjectArray(vx_reference exemplar, vx_size num_items);

    /**
     * @brief   Validate object array
     *
     * @param arr       Ref of object array
     * @return vx_bool  vx_true_e if valid | otherwise vx_false_e
     * @ingroup group_int_object_array
     */
    static vx_bool isValidObjectArray(vx_object_array arr);

    /**
     * @brief   Validate object array
     *
     * @param objarr        Ref of object array
     * @param item_type     type of data in array
     * @param num_items     num of items in array
     * @return vx_bool      vx_true_e if valid | otherwise vx_false_e
     * @ingroup group_int_object_array
     */
    static vx_bool isValidObjectArray(vx_object_array objarr, vx_enum item_type, vx_size num_items);

    /**
     * @brief Get the item at the specified index
     *
     * @param index The index of the item to get
     * @return vx_reference The reference to the item
     * @ingroup group_int_object_array
     */
    vx_reference getItem(vx_size index) const;

    /**
     * @brief Set the item at the specified index
     *
     * @param index The index of the item to set
     * @param ref   The reference to set at the index
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_object_array
     */
    vx_status setItem(vx_size index, vx_reference ref);

    /**
     * @brief Get the item type of the object array
     *
     * @return vx_enum The item type of the object array
     * @ingroup group_int_object_array
     */
    vx_enum itemType() const;

    /**
     * @brief Get the number of items in the object array
     *
     * @return vx_size The number of items in the object array
     * @ingroup group_int_object_array
     */
    vx_size numItems() const;

    /**
     * @brief Function to destroy the object array
     * @ingroup group_int_object_array
     */
    void destruct() override final;

    /*! \brief The reference table of array items */
    vx_reference items[VX_INT_MAX_REF];
    /*! \brief The number of items in the array */
    vx_size num_items;
    /*! \brief The item type of the array. */
    vx_enum item_type;
};

} // namespace coreflow

#endif /* VX_OBJECT_ARRAY_H */