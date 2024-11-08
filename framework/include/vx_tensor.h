/*
 * Copyright (c) 2016-2017 The Khronos Group Inc.
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

#ifndef VX_TENSOR_H
#define VX_TENSOR_H

#include <VX/vx.h>
#include "vx_internal.h"

/*!
 * \file
 * \brief The internal image implementation
 * \defgroup group_int_tensor Internal Tensor API
 * \ingroup group_internal
 * \brief The internal Tensor API.
 */

/*! \brief The internal representation of a tensor.
 * \ingroup group_int_tensor
 */
class Tensor : public Reference
{
public:
    /**
     * @brief Construct a new Tensor object
     *
     * @param context       The context associated with this obj
     * @param reference     The parent ref of this obj
     * @ingroup group_int_tensor
     */
    Tensor(vx_context context, vx_reference reference);

    /**
     * @brief Destroy the Tensor object
     * @ingroup group_int_tensor
     */
    ~Tensor();

    /*! \brief Used to validate the vx_tensor types.
     * \param [in] tensor The vx_tensor to validate.
     * \ingroup group_int_tensor
     */
    static vx_bool isValidTensor(vx_tensor tensor);

    /**
     * @brief Allocate tensor memory
     *
     * @return void*    pointer to allocated memory
     * @ingroup group_int_tensor
     */
    void* allocateTensorMemory();

    /*! \brief Used to initialize the tensor data structure with the correct
     * \param [in] dimensions           arr of dimensions of tensor.
     * \param [in] number_of_dimensions number of tensor dimensions.
     * \param [in] data_type            data type of tensor.
     * \param [in] fixed_point_position fixed point position of tensor.
     * \ingroup group_int_tensor
     */
    void initTensor(const vx_size* dimensions, vx_size number_of_dimensions, vx_enum data_type, vx_int8 fixed_point_position);

    /**
     * @brief Function to destroy tensor obj
     * @ingroup group_int_tensor
     */
    void destructTensor();

    /*! \brief The memory layout definition */
    void *addr;
    /*! \brief Number of dimensions */
    vx_uint32 number_of_dimensions;
    /*! \brief Size of all dimensions */
    vx_size dimensions[VX_MAX_TENSOR_DIMENSIONS];
    /*! \brief Stride of all dimensions */
    vx_size stride[VX_MAX_TENSOR_DIMENSIONS];
    /*! \brief Type of data element */
    vx_enum data_type;
    /*! \brief Fixed point position */
    vx_int8 fixed_point_position;
    /*! \brief Array of subtensors. */
    vx_tensor subtensors[VX_INT_MAX_REF];
    /*! \brief A pointer to a parent md data object. */
    vx_tensor parent;
    /*! \brief Array of subimages. */
    vx_image  subimages[VX_INT_MAX_REF];
};

#endif
