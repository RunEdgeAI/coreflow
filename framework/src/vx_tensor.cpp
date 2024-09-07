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

#include "VX/vx_khr_nn.h"

#include "vx_internal.h"
#include "vx_tensor.h"


/*==============================================================================
Tensor INTERNAL HELPER FUNCTIONS
=============================================================================*/
Tensor::Tensor(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_TENSOR, scope)
{

}

Tensor::~Tensor()
{
    destructTensor();
}

vx_bool Tensor::isValidTensor(vx_tensor tensor)
{
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(tensor), VX_TYPE_TENSOR) == vx_true_e)
    {
        return vx_true_e;
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid ITensor!\n");
        return vx_false_e;
    }
}

void* Tensor::allocateTensorMemory()
{
    vx_size total_size = Reference::sizeOfType(this->data_type);

    if (this->addr == nullptr)
    {
        for (vx_uint32 i = 0; i < this->number_of_dimensions; i++)
        {
            total_size *= this->dimensions[i];
        }

        this->addr = calloc(total_size,1);
    }

    return this->addr;
}

void Tensor::initTensor(const vx_size* dimensions, vx_size number_of_dimensions, vx_enum data_type, vx_int8 fixed_point_position)
{
    this->data_type = data_type;
    this->fixed_point_position = fixed_point_position;
    this->number_of_dimensions = (vx_uint32) number_of_dimensions;

    this->dimensions[0] = dimensions[0];
    this->stride[0] = Reference::sizeOfType(this->data_type);
    for (vx_uint32 i = 1; i < number_of_dimensions; i++)
    {
        this->dimensions[i] = dimensions[i];
        this->stride[i] = this->stride[i - 1] * this->dimensions[i - 1];
    }
    for (vx_uint32 i = (vx_uint32)number_of_dimensions; i < VX_MAX_TENSOR_DIMENSIONS; i++)
    {
        this->dimensions[i] = 0;
        this->stride[i] = 0;
    }
}

void Tensor::destructTensor()
{
    /* if it's not imported and does not have a parent, free it */
    if (parent == nullptr && addr)
    {
        free (addr);
        addr = nullptr;
    }
    else if (parent)
    {
        parent->releaseReference(VX_TYPE_TENSOR, VX_INTERNAL, nullptr);
    }
}

/* @TODO: move this somewhere, there's like 3 copies of this already */
static VX_INLINE int validFormat(vx_enum data_type, vx_uint8 fixed_point_pos)
{
        return
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
            data_type == VX_TYPE_FLOAT16 ||
#endif
#ifdef OPENVX_CONFORMANCE_NNEF_IMPORT
            data_type == VX_TYPE_FLOAT32 || data_type == VX_TYPE_INT32 || data_type == VX_TYPE_BOOL ||
#endif
            (data_type == VX_TYPE_INT16 && fixed_point_pos == Q78_FIXED_POINT_POSITION) ||
            (data_type == VX_TYPE_INT8 && fixed_point_pos == 0) ||
            (data_type == VX_TYPE_UINT8 && fixed_point_pos == 0);
}

/*==============================================================================
   Tensor API FUNCTIONS
=============================================================================*/
VX_API_ENTRY vx_tensor VX_API_CALL vxCreateTensor(
        vx_context context,
        vx_size number_of_dims,
        const vx_size * dims,
        vx_enum data_type,
        vx_int8 fixed_point_position)
{
    vx_tensor tensor = nullptr;

    //TODO: add unlikely() for err checks

    if (Context::isValidContext(context) == vx_false_e)
        //TODO: check, do we not need to set the err here
        return tensor;

    if (number_of_dims < 1)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid dimensions for the tensor.\n");
        // return (vx_tensor)ownGetErrorObject((vx_context)context, VX_ERROR_INVALID_DIMENSION);
    }

    if (!validFormat(data_type, fixed_point_position))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid format for the tensor.\n");
        // return (vx_tensor)ownGetErrorObject((vx_context)context, VX_ERROR_INVALID_TYPE);
    }

    tensor = (vx_tensor)Reference::createReference(context, VX_TYPE_TENSOR, VX_EXTERNAL, context);
    if (vxGetStatus((vx_reference)tensor) != VX_SUCCESS || tensor->type != VX_TYPE_TENSOR)
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to create reference for tensor.\n");
        //TODO: check, do we not need to set the err here
        return tensor;
    }
    tensor->initTensor(dims, number_of_dims, data_type, fixed_point_position);
                  tensor->parent = nullptr;
                  tensor->scope = (vx_reference)context;

    return tensor;
}

VX_API_ENTRY vx_tensor VX_API_CALL vxCreateTensorFromHandle(vx_context context, vx_size number_of_dims, const vx_size *dims,
    vx_enum data_type, vx_int8 fixed_point_position,
    const vx_size *stride, void * ptr, vx_enum memory_type)
{
    vx_tensor tensor = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
    {
        /* @TODO: check, do we not need to set the err here */
        return tensor;
    }

    if (number_of_dims < 1)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid dimensions for the tensor.\n");
        // return (vx_tensor)ownGetErrorObject((vx_context)context, VX_ERROR_INVALID_DIMENSION);
    }

    if (!validFormat(data_type, fixed_point_position))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid format for the tensor.\n");
        // return (vx_tensor)ownGetErrorObject((vx_context)context, VX_ERROR_INVALID_TYPE);
    }

    // if (ownIsValidImport(memory_type) == vx_false_e)
    {
        // return (vx_tensor)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
    }

    if (stride[0] != Reference::sizeOfType(data_type))
    {
        // return (vx_tensor)ownGetErrorObject(context, VX_ERROR_INVALID_VALUE);
    }

    tensor = (vx_tensor)Reference::createReference(context, VX_TYPE_TENSOR, VX_EXTERNAL, context);
    if (vxGetStatus((vx_reference)tensor) != VX_SUCCESS || tensor->type != VX_TYPE_TENSOR)
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to create reference for tensor.\n");
        //TODO: check, do we not need to set the err here
        return tensor;
    }

    tensor->data_type = data_type;
    tensor->fixed_point_position = fixed_point_position;
    tensor->number_of_dimensions = (vx_uint32) number_of_dims;

    memcpy(tensor->dimensions, dims, number_of_dims * sizeof(vx_size));
    for (vx_uint32 i = 0; i < (vx_uint32)number_of_dims; i++)
    {
        tensor->stride[i] = stride[i];
    }

    for (vx_uint32 i = (vx_uint32)number_of_dims; i < VX_MAX_TENSOR_DIMENSIONS; i++)
    {
        tensor->dimensions[i] = 0;
        tensor->stride[i] = 0;
    }

    tensor->addr = ptr;
    tensor->parent = nullptr;
    tensor->scope = (vx_reference)context;

    return tensor;
}

VX_API_ENTRY vx_status VX_API_CALL vxSwapTensorHandle(vx_tensor tensor, void* new_ptr, void** prev_ptr)
{
    vx_status status = VX_SUCCESS;

    if (Tensor::isValidTensor(tensor) == vx_true_e)
    {
        if (new_ptr == nullptr)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        if (tensor->data_type != VX_MEMORY_TYPE_NONE)
        {
            vx_uint32 i;

            if (prev_ptr != nullptr && tensor->parent != nullptr)
            {
                /* The tensor was already being accessed */
                return VX_FAILURE;
            }

            if (prev_ptr != nullptr && tensor->parent == nullptr)
            {
                /* return previous tensor handles */
                *prev_ptr = tensor->addr;
            }

            for (i = 0; i < VX_INT_MAX_REF; i++)
            {
                if (tensor->subtensors[i] != nullptr)
                {
                    if (new_ptr == nullptr)
                        status = vxSwapTensorHandle(tensor->subtensors[i], nullptr, nullptr);
                    else
                    {
                        status = vxSwapTensorHandle(tensor->subtensors[i], new_ptr, nullptr);
                    }
                    break;
                }
            }

            /* reclaim previous and set new handles for this tensor */
            if (new_ptr == nullptr)
            {
                tensor->addr = nullptr;
            }
            else
            {
                tensor->addr = new_ptr;
            }

        }
        else
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    return status;
}

VX_API_ENTRY vx_object_array VX_API_CALL vxCreateImageObjectArrayFromTensor(vx_tensor tensor, const vx_rectangle_t *rect, vx_size array_size, vx_size stride, vx_df_image image_format)
{
    (void)array_size;
    (void)stride;

	vx_object_array images = (vx_object_array)Reference::createReference(tensor->context, VX_TYPE_OBJECT_ARRAY, VX_EXTERNAL, (vx_reference)tensor->context);

    if ((rect->end_x <= rect->start_x) ||
        (rect->end_y <= rect->start_y))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid rectangle!\n");
        goto exit;
    }

    if (Tensor::isValidTensor(tensor) == vx_true_e)
    {
        /* perhaps the parent hasn't been allocated yet? */
        if (tensor->allocateTensorMemory() != nullptr)
        {
            if ((tensor->data_type == VX_TYPE_INT16 && image_format == VX_DF_IMAGE_S16) ||
                (tensor->data_type == VX_TYPE_UINT8 && image_format == VX_DF_IMAGE_U8))
            {
                if (tensor->number_of_dimensions == 3)
                {
                    if ((rect->end_y <= tensor->dimensions[1]) && (rect->end_x <= tensor->dimensions[0]))
                    {
                        vx_uint32 image_byte_size = (vx_uint32)(tensor->dimensions[1] * tensor->dimensions[0] * Reference::sizeOfType(tensor->data_type));
                        vx_imagepatch_addressing_t addr;
                        addr.dim_x = rect->end_x - rect->start_x;
                        addr.dim_y = rect->end_y - rect->start_y;
                        addr.scale_x = VX_SCALE_UNITY;
                        addr.scale_y = VX_SCALE_UNITY;
                        addr.step_x = 1;
                        addr.step_y = addr.dim_x;
                        addr.stride_x = (vx_uint32) Reference::sizeOfType(tensor->data_type);
                        addr.stride_y = (vx_uint32)(tensor->dimensions[0] * Reference::sizeOfType(tensor->data_type));
                        vx_image exemplar = vxCreateImage(tensor->context, addr.dim_x, addr.dim_y, image_format);
						if (vxGetStatus((vx_reference)images) == VX_SUCCESS && images->type == VX_TYPE_OBJECT_ARRAY)
                        {
							int i;
							images->scope = (vx_reference)tensor->context;
							images->is_virtual = vx_false_e;
							images->num_items = 0;
							images->item_type = VX_TYPE_IMAGE;
                            for (i = 0; i < (int)tensor->dimensions[2]; i++)
                            {
								vx_image subimage = vxCreateImage(tensor->context, addr.dim_x, addr.dim_y, image_format);
								if (subimage)
                                {
									vx_uint32 p = 0;
									subimage->parent = (vx_image)tensor;
									subimage->scope = (vx_reference)tensor;
									subimage->memory.allocated = vx_true_e;
									subimage->memory.ptrs[0] = (vx_uint8*)(tensor->addr) + rect->start_y*addr.stride_y + rect->start_x*addr.stride_x + i*image_byte_size;
									subimage->memory.strides[0][1] = addr.stride_y;
									subimage->memory.strides[0][2] = image_byte_size;
									for (p = 0; p < VX_INT_MAX_REF; p++)
									{
										if (tensor->subimages[p] == nullptr)
                                        {
											tensor->subimages[p] = subimage;
											break;
                                        }
                                    }
									images->items[i] = (vx_reference)subimage;
									images->num_items++;
									subimage->scope = (vx_reference)images;
                                }
                            }
                        }
                        else
                        {
							VX_PRINT(VX_ZONE_ERROR, "Child tensor failed to allocate!\n");
                        }

                    }
                    else
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Rectangle exceeds tensor dimensions!\n");
                        // images = (vx_object_array)ownGetErrorObject(tensor->context, VX_ERROR_INVALID_DIMENSION);
                    }
                }
                else
                {
                    VX_PRINT(VX_ZONE_ERROR, "Failed to create image array from a non-3d tensor!\n");
                    // images = (vx_object_array)ownGetErrorObject(tensor->context, VX_ERROR_INVALID_DIMENSION);
                }
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Failed to allocate tensor memory!\n");
                // images = (vx_object_array)ownGetErrorObject(tensor->context, VX_ERROR_NO_MEMORY);
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Requested image format does not match tensor type!\n");
            // images = (vx_object_array)ownGetErrorObject(tensor->context, VX_ERROR_INVALID_FORMAT);
        }
    }

exit:
    return images;
}

VX_API_ENTRY vx_tensor VX_API_CALL vxCreateTensorFromView(vx_tensor tensor, vx_size number_of_dimensions, const vx_size * view_start, const vx_size * view_end)
{
    vx_tensor subtensor = nullptr;
    (void)number_of_dimensions;

    if ((Tensor::isValidTensor(tensor) == vx_true_e) && (nullptr != view_start) && (nullptr != view_end))
    {
        /* perhaps the parent hasn't been allocated yet? */
        if (tensor->allocateTensorMemory() != nullptr)
        {
            subtensor = (vx_tensor)Reference::createReference(tensor->context, VX_TYPE_TENSOR, VX_EXTERNAL, tensor->context);
            if (subtensor)
            {
				vx_uint32 p = 0;
                /* refer to our parent md data and internally refcount it */
                subtensor->parent = tensor;
                subtensor->scope = (vx_reference)tensor;
				for (p = 0; p < VX_INT_MAX_REF; p++)
				{
					if (tensor->subtensors[p] == nullptr)
					{
						tensor->subtensors[p] = subtensor;
						break;
					}
				}
                tensor->incrementReference(VX_INTERNAL);
                /* duplicate the metadata */
                subtensor->data_type = tensor->data_type;
                subtensor->fixed_point_position = tensor->fixed_point_position;
                subtensor->number_of_dimensions = tensor->number_of_dimensions;
                subtensor->addr = tensor->addr;
                for (vx_uint32 i = 0; i < subtensor->number_of_dimensions; i++)
                {
                    subtensor->stride[i] = tensor->stride[i];
                    subtensor->dimensions[i] = view_end[i] - view_start[i];
                    //((vx_uint8*)(subtensor->addr)) += view->start[i] * tensor->stride[i];
                    subtensor->addr = (vx_uint8*)(subtensor->addr) + view_start[i] * tensor->stride[i];
                }
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Child tensor failed to allocate!\n");
            }
        }
        else
        {
            // subtensor = (vx_tensor)ownGetErrorObject(tensor->context, VX_ERROR_NO_MEMORY);
            VX_PRINT(VX_ZONE_ERROR, "Parent tensor failed to allocate!\n");
        }
    }
    return subtensor;
}

VX_API_ENTRY vx_tensor VX_API_CALL vxCreateVirtualTensor(
        vx_graph graph,
        vx_size number_of_dims,
        const vx_size *dims,
        vx_enum data_type,
        vx_int8 fixed_point_position)
{
    vx_tensor tensor = nullptr;
    vx_reference gref = (vx_reference)graph;

    if (Reference::isValidReference(gref, VX_TYPE_GRAPH) == vx_true_e)
    {
        if (data_type == VX_TYPE_INVALID || validFormat(data_type, fixed_point_position))
        {
            tensor = (vx_tensor)Reference::createReference(gref->context, VX_TYPE_TENSOR, VX_EXTERNAL, gref->context);
            if (vxGetStatus((vx_reference)tensor) == VX_SUCCESS  && tensor->type == VX_TYPE_TENSOR)
            {
                tensor->initTensor(dims, number_of_dims, data_type, fixed_point_position);
                tensor->parent = nullptr;
                // tensor->scope = (vx_reference)context;
                tensor->is_virtual = vx_true_e;
				tensor->scope = (vx_reference)graph;
            } else
            {
                VX_PRINT(VX_ZONE_ERROR, "Failed to create reference for tensor.\n");
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid tensor format.\n");
            // tensor = (vx_tensor)ownGetErrorObject(graph->context, VX_ERROR_INVALID_FORMAT);
        }
    }

    return tensor;
}




VX_API_ENTRY vx_status VX_API_CALL vxReleaseTensor(vx_tensor* tensor)
{
    vx_status status = VX_FAILURE;

    if (nullptr != tensor)
    {
        vx_tensor this_tensor = *tensor;
        if (vx_true_e == Reference::isValidReference(this_tensor, VX_TYPE_TENSOR))
        {
            status = this_tensor->releaseReference(VX_TYPE_TENSOR, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}


VX_API_ENTRY vx_status VX_API_CALL vxQueryTensor(vx_tensor tensor, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Tensor::isValidTensor(tensor) == vx_true_e)
    {
        switch (attribute)
        {
        case VX_TENSOR_NUMBER_OF_DIMS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = tensor->number_of_dimensions;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_DIMS:
            if (size >= (sizeof(vx_size)*tensor->number_of_dimensions) && ((vx_size)ptr & 0x3) == 0)
            {
                memcpy(ptr, tensor->dimensions, sizeof(vx_size)*tensor->number_of_dimensions);
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_DATA_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
            {
                *(vx_enum *)ptr = tensor->data_type;
            }
            break;
        case VX_TENSOR_FIXED_POINT_POSITION:
            if (VX_CHECK_PARAM(ptr, size, vx_int8, 0x0)) //??
            {
                *(vx_int8 *)ptr = tensor->fixed_point_position;
            }
            break;
        default:
            status = VX_ERROR_NOT_SUPPORTED;
            break;
        }
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }
    VX_PRINT(VX_ZONE_API, "%s returned %d\n", __FUNCTION__, status);
    return status;
}

int CheckSizes(vx_size* dimensions, const vx_size * view_start, const vx_size * view_end, vx_size number_of_dimensions)
{
    for (vx_size i = 0; i < number_of_dimensions; i++)
    {
        if (view_end[i] <= view_start[i] ||
                view_end[i] > dimensions[i])
            return -1;
    }
    return 0;

}

vx_size ComputePatchSize(const vx_size * view_start, const vx_size * view_end, vx_size number_of_dimensions)
{
    vx_size total_size = 1;
    for (vx_size i = 0; i < number_of_dimensions; i++)
    {
        total_size *= view_end[i] - view_start[i];
    }
    return total_size;
}


void ComputePositionsFromIndex(vx_size index, const vx_size * start, const vx_size * end,
        const vx_size * tensor_stride, const vx_size * patch_stride,  vx_size number_of_dimensions,
        vx_size * tensor_pos, vx_size * patch_pos)
{
    *tensor_pos = 0;
    *patch_pos = 0;
    vx_size index_leftover = index;
    int divisor = 1;
    for (vx_size i = 0; i < number_of_dimensions; i++)
    {
        divisor = (vx_uint32)(end[i] - start[i]);
        vx_size curr_dim_index = index_leftover%divisor;
        *tensor_pos += tensor_stride[i] * (curr_dim_index + start[i]);
        *patch_pos += patch_stride[i] * curr_dim_index ;
        index_leftover = index_leftover /divisor;
    }

}

VX_API_ENTRY vx_status VX_API_CALL vxMapTensorPatch(vx_tensor tensor, vx_size number_of_dims,
    const vx_size * view_start, const vx_size * view_end,
    vx_map_id * map_id, vx_size * stride, void ** ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_FAILURE;
    vx_uint8 *buf = nullptr;
    vx_size size;
    vx_memory_map_extra extra;

    /* bad parameters */
    if ((view_start == nullptr) || (view_end == nullptr) || (map_id == nullptr) || (ptr == nullptr) ||
        (stride == nullptr))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        goto exit;
    }

    /* bad references */
    if (Tensor::isValidTensor(tensor) == vx_false_e)
    {
        status = VX_ERROR_INVALID_REFERENCE;
        goto exit;
    }

    /* determine if virtual before checking for memory */
    if (tensor->is_virtual == vx_true_e)
    {
        if (tensor->is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" tensor. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual tensor\n");
            status = VX_ERROR_OPTIMIZED_AWAY;
            goto exit;
        }
    }
    if (tensor->addr == nullptr)
    {
        if (usage != VX_WRITE_ONLY || tensor->allocateTensorMemory() == nullptr)
        {
            VX_PRINT(VX_ZONE_ERROR, "Tensor memory was allocated failed!\n");
            status = VX_ERROR_NO_MEMORY;
            goto exit;
        }
    }

    if (tensor->number_of_dimensions < number_of_dims || number_of_dims == 0)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid number of patch dimensions\n");
        status = VX_ERROR_INVALID_PARAMETERS;
        goto exit;

    }
    if (CheckSizes(tensor->dimensions, view_start, view_end, number_of_dims) != 0)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid view\n");
        status = VX_ERROR_INVALID_PARAMETERS;
        goto exit;

    }

    stride[0] = Reference::sizeOfType(tensor->data_type);
    for (vx_uint32 i = 1; i < number_of_dims; i++)
    {
        stride[i] = stride[i - 1] * (view_end[i] - view_start[i]);
    }
    //vx_map_id * map_id, vx_size * stride, void ** ptr
    size = ComputePatchSize(view_start, view_end, number_of_dims);

    memcpy(extra.tensor_data.start, view_start, sizeof(vx_size) * number_of_dims);
    memcpy(extra.tensor_data.end, view_end, sizeof(vx_size) * number_of_dims);
    memcpy(extra.tensor_data.stride, stride, sizeof(vx_size) * number_of_dims);
    extra.tensor_data.number_of_dims = number_of_dims;

    if (VX_MEMORY_TYPE_HOST  == mem_type &&
        vx_true_e == tensor->context->memoryMap((vx_reference)tensor, 0, usage, mem_type, 0, (void *)&extra, (void **)&buf, map_id))
    {
        *ptr = tensor->addr;

        tensor->incrementReference(VX_EXTERNAL);

        status = VX_SUCCESS;
    }
    else if (vx_true_e == tensor->context->memoryMap((vx_reference)tensor, size, usage, mem_type, 0, (void *)&extra, (void **)&buf, map_id))
    {
        vx_uint8* user_curr_ptr = buf;
        vx_uint8* tensor_ptr = (vx_uint8*)tensor->addr;
        if (usage == VX_READ_ONLY || usage == VX_READ_AND_WRITE)
        {
            for (vx_size i = 0; i < size; i++)
            {
                vx_size patch_pos = 0;
                vx_size tensor_pos = 0;
                ComputePositionsFromIndex(i,view_start, view_end, tensor->stride, stride, number_of_dims, &tensor_pos, &patch_pos);
                memcpy (user_curr_ptr + patch_pos, tensor_ptr + tensor_pos, tensor->stride[0]);
            }

            // ownReadFromReference(&tensor->base);
        }
        *ptr = buf;
        tensor->incrementReference(VX_EXTERNAL);

        status = VX_SUCCESS;
    }

exit:
    VX_PRINT(VX_ZONE_API, "returned %d\n", status);
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxUnmapTensorPatch(vx_tensor tensor, vx_map_id map_id)
{
    vx_status status = VX_FAILURE;

    /* bad references */
    if (Tensor::isValidTensor(tensor) == vx_false_e)
    {
        status = VX_ERROR_INVALID_REFERENCE;
        goto exit;
    }

    /* bad parameters */
    if (tensor->context->findMemoryMap((vx_reference)tensor, map_id) != vx_true_e)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to unmap image patch\n");
        return status;
    }

    {
        vx_context       context = tensor->context;
        vx_memory_map_t* map = &context->memory_maps[map_id];

        if (map->used && map->ref == (vx_reference)tensor)
        {
            /* commit changes for write access */
            if ((VX_WRITE_ONLY == map->usage || VX_READ_AND_WRITE == map->usage) && nullptr != map->ptr)
            {
                if (vx_true_e == ownSemWait(&tensor->lock))
                {
                    vx_uint32 size = ComputePatchSize(map->extra.tensor_data.start,
                                                      map->extra.tensor_data.end,
                                                      map->extra.tensor_data.number_of_dims);
                    vx_uint8* pSrc = (vx_uint8*)map->ptr;
                    vx_uint8* pDst = (vx_uint8*)tensor->addr;

                    for (vx_size i = 0; i < size; i++)
                    {
                        vx_size patch_pos = 0;
                        vx_size tensor_pos = 0;
                        ComputePositionsFromIndex(i,map->extra.tensor_data.start,
                                                  map->extra.tensor_data.end,
                                                  tensor->stride,
                                                  map->extra.tensor_data.stride,
                                                  map->extra.tensor_data.number_of_dims,
                                                  &tensor_pos,
                                                  &patch_pos);
                        memcpy (pDst + patch_pos, pSrc + tensor_pos, tensor->stride[0]);
                    }

                    ownSemPost(&tensor->lock);
                }
                else
                {
                    status = VX_FAILURE;
                    VX_PRINT(VX_ZONE_ERROR, "Can't lock memory plane for unmapping\n");
                    goto exit;
                }
            }

            /* freeing mapping buffer */
            context->memoryUnmap((vx_uint32)map_id);

            tensor->decrementReference(VX_EXTERNAL);

            status = VX_SUCCESS;
        }
        else
            status = VX_FAILURE;
    }

exit:
    VX_PRINT(VX_ZONE_API, "return %d\n", status);

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyTensorPatch(vx_tensor tensor, vx_size number_of_dimensions, const vx_size * view_start, const vx_size * view_end,
        const vx_size * user_stride, void * user_ptr, vx_enum usage, vx_enum user_memory_type)
{
    vx_status status = VX_FAILURE;
    (void)user_memory_type;

    /* bad parameters */
    if ((view_start == nullptr) || (view_end == nullptr) || (user_stride == nullptr) || (user_ptr == nullptr))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    /* bad references */
    if (VX_SUCCESS == status && Tensor::isValidTensor(tensor) == vx_false_e)
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    /* determine if virtual before checking for memory */
    if (VX_SUCCESS == status && tensor->is_virtual == vx_true_e && tensor->is_accessible == vx_false_e)
    {
        /* User tried to access a "virtual" tensor. */
        VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual tensor\n");
        status = VX_ERROR_OPTIMIZED_AWAY;
    }

    if (VX_SUCCESS == status && tensor->addr == nullptr)
    {
        if (usage != VX_WRITE_ONLY || tensor->allocateTensorMemory() == nullptr)
        {
            VX_PRINT(VX_ZONE_ERROR, "Tensor memory was not allocated!\n");
            status = VX_ERROR_NOT_ALLOCATED;
        }
    }

    if (VX_SUCCESS == status && tensor->number_of_dimensions < number_of_dimensions || number_of_dimensions ==0)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid number of patch dimensions\n");
        status = VX_ERROR_INVALID_PARAMETERS;

    }
    if (VX_SUCCESS == status && CheckSizes(tensor->dimensions, view_start, view_end, number_of_dimensions) != 0)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid view\n");
        status = VX_ERROR_INVALID_PARAMETERS;

    }

    if (VX_SUCCESS == status)
    {
#ifdef OPENVX_USE_OPENCL_INTEROP
    void * user_ptr_given = user_ptr;
    vx_enum user_memory_type_given = user_memory_type;
    if (user_memory_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        /* get user_ptr from OpenCL buffer for HOST */
        size_t size = 0;
        cl_mem opencl_buf = (cl_mem)user_ptr;
        cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyTensorPatch: clGetMemObjectInfo(%p) => (%d)\n",
            opencl_buf, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        user_ptr = clEnqueueMapBuffer(tensor->context->opencl_command_queue,
            opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
            0, nullptr, nullptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyTensorPatch: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
            opencl_buf, (int)size, user_ptr, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        user_memory_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    //element_size = Reference::sizeOfType(tensor->data_type);
    vx_uint8* user_curr_ptr = (vx_uint8*)user_ptr;
    vx_uint8* tensor_ptr = (vx_uint8*)tensor->addr;
    vx_size patch_size = ComputePatchSize (view_start, view_end, number_of_dimensions);
    for (vx_size i = 0; i < patch_size; i++) {
        vx_size patch_pos = 0;
        vx_size tensor_pos = 0;
        ComputePositionsFromIndex(i,view_start, view_end, tensor->stride, user_stride, number_of_dimensions, &tensor_pos, &patch_pos);
        if (usage == VX_READ_ONLY)
            memcpy (user_curr_ptr + patch_pos, tensor_ptr + tensor_pos, tensor->stride[0]);
        else
            memcpy (tensor_ptr + tensor_pos, user_curr_ptr + patch_pos, tensor->stride[0]);


    }
    status = VX_SUCCESS;

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (user_memory_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        clEnqueueUnmapMemObject(tensor->context->opencl_command_queue,
            (cl_mem)user_ptr_given, user_ptr, 0, nullptr, nullptr);
        clFinish(tensor->context->opencl_command_queue);
    }
#endif
    }
exit:
    VX_PRINT(VX_ZONE_API, "returned %d\n", status);
    return status;
}
