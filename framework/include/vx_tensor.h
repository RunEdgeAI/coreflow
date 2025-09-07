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
namespace coreflow {

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

    /**
     * @brief Create a tensor object
     *
     * @param context   The context associated with this obj
     * @param number_of_dims The number of dimensions of the tensor
     * @param dims The dimensions of the tensor
     * @param data_type The data type of the tensor
     * @param fixed_point_position The fixed point position of the tensor
     * @return vx_tensor The tensor object
     * @ingroup group_int_tensor
     */
    static vx_tensor createTensor(
        vx_context context, vx_size number_of_dims, const vx_size *dims, vx_enum data_type, vx_int8 fixed_point_position);

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
     * @brief  Check tensor sizes
     *
     * @param dimensions           dimensions
     * @param view_start           view start
     * @param view_end             view end
     * @param number_of_dimensions number of dimensions
     * @return vx_int32
     * @ingroup group_int_tensor
     */
    static vx_int32 checkSizes(vx_size* dimensions, const vx_size * view_start, const vx_size * view_end, vx_size number_of_dimensions);

    /**
     * @brief Compute patch size
     *
     * @param view_start           view start
     * @param view_end             view end
     * @param number_of_dimensions number of dimensions
     * @return vx_size
     * @ingroup group_int_tensor
     */
    static vx_size computePatchSize(const vx_size * view_start, const vx_size * view_end, vx_size number_of_dimensions);

    /**
     * @brief Compute positions from index
     *
     * @param index                 index
     * @param start                 view start
     * @param end                   view end
     * @param tensor_stride         tensor stride
     * @param patch_stride          patch stride
     * @param number_of_dimensions  number of dimensions
     * @param tensor_pos            tensor position
     * @param patch_pos             patch position
     * @ingroup group_int_tensor
     */
    static void computePositionsFromIndex(vx_size index, const vx_size * start, const vx_size * end,
        const vx_size * tensor_stride, const vx_size * patch_stride,  vx_size number_of_dimensions,
        vx_size * tensor_pos, vx_size * patch_pos);

    /**
     * @brief Get the dimensions of the tensor
     * @return const vx_size* The dimensions of the tensor
     * @ingroup group_int_tensor
     */
    const vx_size *dims() const;

    /**
     * @brief Get the number of dimensions in the tensor
     * @return vx_size The number of dimensions
     * @ingroup group_int_tensor
     */
    vx_size numDims() const;

    /**
     * @brief Get the data type of the tensor
     * @return vx_enum The data type of the tensor
     * @ingroup group_int_tensor
     */
    vx_enum dataType() const;

    /**
     * @brief Get the fixed point position of the tensor
     * @return vx_int8 The fixed point position of the tensor
     * @ingroup group_int_tensor
     */
    vx_int8 fixedPointPosition() const;

    /**
     * @brief Get the strides of the tensor
     * @return const vx_size* The strides of the tensor
     * @ingroup group_int_tensor
     */
    const vx_size *strides() const;

    /**
     * @brief Get the size of the tensor in bytes
     * @return vx_size The size of the tensor in bytes
     * @ingroup group_int_tensor
     */
    vx_size size() const;

    /**
     * @brief Copy a patch of the tensor to/from user memory
     *
     * @param number_of_dimensions The number of dimensions in the patch
     * @param view_start           The start indices of the patch
     * @param view_end             The end indices of the patch
     * @param user_stride          The stride in user memory
     * @param user_ptr             The pointer to user memory
     * @param usage                The usage of the memory (read/write)
     * @param user_memory_type     The type of user memory (host, opencl, etc.)
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_tensor
     */
    vx_status copyPatch(vx_size number_of_dimensions, const vx_size *view_start,
                        const vx_size *view_end, const vx_size *user_stride, void *user_ptr,
                        vx_enum usage, vx_enum user_memory_type);

    /**
     * @brief Map a patch of the tensor to user memory
     * @param number_of_dimensions The number of dimensions in the patch
     * @param view_start           The start indices of the patch
     * @param view_end             The end indices of the patch
     * @param map_id               The map ID for the patch
     * @param stride               The stride in the patch
     * @param ptr                  The pointer to the mapped memory
     * @param usage                The usage of the memory (read/write)
     * @param mem_type             The type of memory (host, device, etc.)
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_tensor
     */
    vx_status mapPatch(vx_size number_of_dimensions, const vx_size *view_start,
                       const vx_size *view_end, vx_map_id *map_id, vx_size *stride, void **ptr,
                       vx_enum usage, vx_enum mem_type);

    /**
     * @brief Unmap a patch of the tensor
     * @param map_id The map ID for the patch
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_tensor
     */
    vx_status unmapPatch(vx_map_id map_id);

    /**
     * @brief Function to destroy tensor obj
     * @ingroup group_int_tensor
     */
    void destruct() override final;

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

} // namespace coreflow

#endif /* VX_TENSOR_H */