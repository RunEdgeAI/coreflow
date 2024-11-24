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
#include "vx_inlines.h"
#include "vx_internal.h"
#include "vx_remap.h"

/*****************************************************************************/
/* INTERNAL INTERFACE                                                        */
/*****************************************************************************/
Remap::Remap(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_REMAP, scope)
{

}

Remap::~Remap()
{
    destructRemap();
}

vx_bool Remap::isValidRemap(vx_remap remap)
{
    if (Reference::isValidReference(remap, VX_TYPE_REMAP) == vx_true_e)
    {
        return vx_true_e;
    }
    else
    {
        return vx_false_e;
    }
}

vx_status Remap::setCoordValue(vx_uint32 dst_x, vx_uint32 dst_y, vx_float32 src_x, vx_float32 src_y)
{
    vx_status status = VX_FAILURE;
    if ((Reference::isValidReference(this, VX_TYPE_REMAP) == vx_true_e) &&
         (ownAllocateMemory(context, &memory) == vx_true_e))
    {
        if ((dst_x < dst_width) &&
            (dst_y < dst_height))
        {
            vx_float32 *coords[] = {
                 (vx_float32*)ownFormatMemoryPtr(&memory, 0, dst_x, dst_y, 0),
                 (vx_float32*)ownFormatMemoryPtr(&memory, 1, dst_x, dst_y, 0),
            };
            *coords[0] = src_x;
            *coords[1] = src_y;
            // ownWroteToReference(remap);
            status = VX_SUCCESS;
            VX_PRINT(VX_ZONE_INFO, "SetCoordValue %ux%u to %f,%f\n", dst_x, dst_y, src_x, src_y);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid source or destintation values!\n");
            status = VX_ERROR_INVALID_VALUE;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

vx_status Remap::getCoordValue(vx_uint32 dst_x, vx_uint32 dst_y, vx_float32 *src_x, vx_float32 *src_y)
{
    vx_status status = VX_FAILURE;
    if (Reference::isValidReference(this, VX_TYPE_REMAP) == vx_true_e)
    {
        if ((dst_x < dst_width) &&
            (dst_y < dst_height))
        {
            vx_float32 *coords[] = {
                 (vx_float32*)ownFormatMemoryPtr(&memory, 0, dst_x, dst_y, 0),
                 (vx_float32*)ownFormatMemoryPtr(&memory, 1, dst_x, dst_y, 0),
            };
            *src_x = *coords[0];
            *src_y = *coords[1];
            read_count++;
            status = VX_SUCCESS;

            VX_PRINT(VX_ZONE_INFO, "GetCoordValue dst[%u,%u] to src[%f,%f]\n", dst_x, dst_y, src_x, src_y);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid source or destintation values!\n");
            status = VX_ERROR_INVALID_VALUE;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

void Remap::destructRemap()
{
    ownFreeMemory(context, &memory);
}

/*****************************************************************************/
/* PUBLIC INTERFACE                                                          */
/*****************************************************************************/

VX_API_ENTRY vx_remap VX_API_CALL vxCreateRemap(vx_context context,
                              vx_uint32 src_width, vx_uint32 src_height,
                              vx_uint32 dst_width, vx_uint32 dst_height)
{
    vx_remap remap = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if (src_width != 0 && src_height != 0 && dst_width != 0 && dst_height != 0)
        {
            remap = (vx_remap)Reference::createReference(context, VX_TYPE_REMAP, VX_EXTERNAL, context);
            if (vxGetStatus((vx_reference)remap) == VX_SUCCESS && remap->type == VX_TYPE_REMAP)
            {
                remap->src_width = src_width;
                remap->src_height = src_height;
                remap->dst_width = dst_width;
                remap->dst_height = dst_height;
                remap->memory.ndims = 3;
                remap->memory.nptrs = 1;
                remap->memory.dims[0][VX_DIM_C] = 2; // 2 "channels" of f32
                remap->memory.dims[0][VX_DIM_X] = dst_width;
                remap->memory.dims[0][VX_DIM_Y] = dst_height;
                remap->memory.strides[0][VX_DIM_C] = sizeof(vx_float32);
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to remap\n");
            vxAddLogEntry(context, VX_ERROR_INVALID_PARAMETERS, "Invalid parameters to remap\n");
            // remap = (vx_remap_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        }
    }

    return remap;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseRemap(vx_remap* r)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != r)
    {
        vx_reference ref = *r;
        if (vx_true_e == Reference::isValidReference(ref, VX_TYPE_REMAP))
        {
            status = Reference::releaseReference((vx_reference*)r, VX_TYPE_REMAP, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryRemap(vx_remap remap, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(remap, VX_TYPE_REMAP) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    switch (attribute)
    {
        case VX_REMAP_SOURCE_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = remap->src_width;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_SOURCE_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = remap->src_height;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_DESTINATION_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = remap->dst_width;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_DESTINATION_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = remap->dst_height;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        default:
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetRemapPoint(vx_remap remap, vx_uint32 dst_x, vx_uint32 dst_y,
                                 vx_float32 src_x, vx_float32 src_y)
{
    vx_status status = VX_FAILURE;
    if ((Reference::isValidReference(remap, VX_TYPE_REMAP) == vx_true_e) &&
         (ownAllocateMemory(remap->context, &remap->memory) == vx_true_e))
    {
        if ((dst_x < remap->dst_width) &&
            (dst_y < remap->dst_height))
        {
            vx_float32 *coords[] = {
                 (vx_float32*)ownFormatMemoryPtr(&remap->memory, 0, dst_x, dst_y, 0),
                 (vx_float32*)ownFormatMemoryPtr(&remap->memory, 1, dst_x, dst_y, 0),
            };
            *coords[0] = src_x;
            *coords[1] = src_y;
            // ownWroteToReference(remap);
            status = VX_SUCCESS;
            VX_PRINT(VX_ZONE_INFO, "SetRemapPoint %ux%u to %f,%f\n", dst_x, dst_y, src_x, src_y);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid source or destintation values!\n");
            status = VX_ERROR_INVALID_VALUE;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxGetRemapPoint(vx_remap remap, vx_uint32 dst_x, vx_uint32 dst_y,
                                 vx_float32 *src_x, vx_float32 *src_y)
{
    vx_status status = VX_FAILURE;
    if (Reference::isValidReference(remap, VX_TYPE_REMAP) == vx_true_e)
    {
        if ((dst_x < remap->dst_width) &&
            (dst_y < remap->dst_height))
        {
            vx_float32 *coords[] = {
                 (vx_float32*)ownFormatMemoryPtr(&remap->memory, 0, dst_x, dst_y, 0),
                 (vx_float32*)ownFormatMemoryPtr(&remap->memory, 1, dst_x, dst_y, 0),
            };
            *src_x = *coords[0];
            *src_y = *coords[1];
            remap->read_count++;
            status = VX_SUCCESS;

            VX_PRINT(VX_ZONE_INFO, "GetRemapPoint dst[%u,%u] to src[%f,%f]\n", dst_x, dst_y, src_x, src_y);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid source or destintation values!\n");
            status = VX_ERROR_INVALID_VALUE;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

VX_API_ENTRY vx_remap VX_API_CALL vxCreateVirtualRemap(vx_graph graph,
                              vx_uint32 src_width,
                              vx_uint32 src_height,
                              vx_uint32 dst_width,
                              vx_uint32 dst_height)
{
    vx_remap remap = nullptr;
    vx_reference gref = (vx_reference)graph;

    if (Reference::isValidReference(gref, VX_TYPE_GRAPH) == vx_true_e)
    {
        remap = vxCreateRemap(gref->context, src_width, src_height, dst_width, dst_height);
        if (vxGetStatus((vx_reference)remap) == VX_SUCCESS && remap->type == VX_TYPE_REMAP)
        {
            remap->scope = (vx_reference)graph;
            remap->is_virtual = vx_true_e;
        }
    }
    /* else, the graph is invalid, we can't get any context and then error object */
    return remap;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyRemapPatch(vx_remap remap,
                                                    const vx_rectangle_t *rect,
                                                    vx_size user_stride_y,
                                                    void * user_ptr,
                                                    vx_enum user_coordinate_type,
                                                    vx_enum usage,
                                                    vx_enum user_mem_type)
{
    vx_status status = VX_SUCCESS;
    vx_uint32 start_x = rect ? rect->start_x : 0u;
    vx_uint32 start_y = rect ? rect->start_y : 0u;
    vx_uint32 end_x = rect ? rect->end_x : 0u;
    vx_uint32 end_y = rect ? rect->end_y : 0u;
    vx_bool zero_area = ((((end_x - start_x) == 0) || ((end_y - start_y) == 0)) ? vx_true_e : vx_false_e);
    vx_size stride = user_stride_y / sizeof(vx_coordinates2df_t);

    /* bad parameters */
    if ( ((VX_READ_ONLY != usage) && (VX_WRITE_ONLY != usage)) ||
         (rect == nullptr) || (remap == nullptr) || (user_ptr == nullptr) )
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    /* more bad parameters */
    if ( VX_SUCCESS == status &&
        ((user_stride_y < sizeof(vx_coordinates2df_t)*(rect->end_x - rect->start_x)) ||
        (user_coordinate_type != VX_TYPE_COORDINATES2DF)))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    /* more bad parameters */
    if (VX_SUCCESS == status &&
        user_mem_type != VX_MEMORY_TYPE_HOST && user_mem_type != VX_MEMORY_TYPE_NONE)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    /* bad references */
    if (VX_SUCCESS == status &&
        Remap::isValidRemap(remap) == vx_false_e )
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    /* determine if virtual before checking for memory */
    if (VX_SUCCESS == status &&
        remap->is_virtual == vx_true_e)
    {
        if (remap->is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" remap. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual remap\n");
            status = VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual remap, this is ok. */
    }

    /* more bad parameters */
    if (VX_SUCCESS == status &&
        zero_area == vx_false_e &&
        ((0 >= remap->memory.nptrs) ||
         (start_x >= end_x) ||
         (start_y >= end_y)))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
#ifdef OPENVX_USE_OPENCL_INTEROP
        void * user_ptr_given = user_ptr;
        vx_enum user_mem_type_given = user_mem_type;
        if (user_mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
        {
            // get ptr from OpenCL buffer for HOST
            size_t size = 0;
            cl_mem opencl_buf = (cl_mem)user_ptr;
            cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyRemap: clGetMemObjectInfo(%p) => (%d)\n",
                opencl_buf, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            user_ptr = clEnqueueMapBuffer(remap->context->opencl_command_queue,
                opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
                0, nullptr, nullptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyRemap: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                opencl_buf, (int)size, user_ptr, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            user_mem_type = VX_MEMORY_TYPE_HOST;
        }
#endif

        if (usage == VX_READ_ONLY)
        {
            /* Copy from remap (READ) mode */
            vx_coordinates2df_t *ptr = (vx_coordinates2df_t*)user_ptr;
            vx_uint32 i;
            vx_uint32 j;
            for (i = start_y; i < end_y; i++)
            {
                for (j = start_x; j < end_x; j++)
                {
                    vx_coordinates2df_t *coord_ptr = &(ptr[i * stride + j]);
                    status = remap->getCoordValue(j, i, &coord_ptr->x, &coord_ptr->y);
                }
            }
        }
        else
        {
            /* Copy to remap (WRITE) mode */
            vx_coordinates2df_t *ptr = (vx_coordinates2df_t*)user_ptr;
            vx_uint32 i;
            vx_uint32 j;
            for (i = start_y; i < end_y; i++)
            {
                for (j = start_x; j < end_x; j++)
                {
                    vx_coordinates2df_t *coord_ptr = &(ptr[i * stride + j]);
                    status = remap->setCoordValue(j, i, coord_ptr->x, coord_ptr->y);
                }
            }
        }

#ifdef OPENVX_USE_OPENCL_INTEROP
        if (VX_SUCCESS == status &&
            user_mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
        {
            clEnqueueUnmapMemObject(remap->context->opencl_command_queue,
                (cl_mem)user_ptr_given, user_ptr, 0, nullptr, nullptr);
            clFinish(remap->context->opencl_command_queue);
        }
#endif
    }

    VX_PRINT(VX_ZONE_API, "returned %d\n", status);
    return status;
}


VX_API_ENTRY vx_status VX_API_CALL vxMapRemapPatch(vx_remap remap,
                                                   const vx_rectangle_t *rect,
                                                   vx_map_id *map_id,
                                                   vx_size *stride_y,
                                                   void **ptr,
                                                   vx_enum coordinate_type,
                                                   vx_enum usage,
                                                   vx_enum mem_type)
{
    vx_uint32 start_x = rect ? rect->start_x : 0u;
    vx_uint32 start_y = rect ? rect->start_y : 0u;
    vx_uint32 end_x   = rect ? rect->end_x : 0u;
    vx_uint32 end_y   = rect ? rect->end_y : 0u;
    vx_bool zero_area = ((((end_x - start_x) == 0) || ((end_y - start_y) == 0)) ? vx_true_e : vx_false_e);
    vx_status status = VX_SUCCESS;

    /* bad parameters */
    if ( (rect == nullptr) || (map_id == nullptr) || (remap == nullptr) || (ptr == nullptr) )
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    /* more bad parameters */
    if (VX_SUCCESS == status &&
        coordinate_type != VX_TYPE_COORDINATES2DF)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    /* bad references */
    if (VX_SUCCESS == status &&
        Remap::isValidRemap(remap) == vx_false_e)
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    /* determine if virtual before checking for memory */
    if (VX_SUCCESS == status &&
        remap->is_virtual == vx_true_e)
    {
        if (remap->is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" remap. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual remap\n");
            status = VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual remap, this is ok. */
    }

    /* more bad parameters */
    if (VX_SUCCESS == status &&
        zero_area == vx_false_e &&
        ((0 >= remap->memory.nptrs) ||
        (start_x >= end_x) ||
        (start_y >= end_y)))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_enum mem_type_requested = mem_type;
    if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    /* MAP mode */
    if (VX_SUCCESS == status)
    {
        vx_memory_map_extra extra;
        extra.image_data.plane_index = 0;
        extra.image_data.rect = *rect;

        vx_uint32 flags = 0;
        vx_uint8 *buf = nullptr;
        vx_size stride = (end_x - start_x);
        vx_size size = (stride * (end_y - start_y)) * sizeof(vx_coordinates2df_t);
        vx_size user_stride_y = stride * sizeof(vx_coordinates2df_t);

        if (remap->context->memoryMap((vx_reference)remap, size, usage, mem_type, flags, &extra, (void **)&buf, map_id) == vx_true_e)
        {
            if (VX_READ_ONLY == usage || VX_READ_AND_WRITE == usage)
            {

                if (ownSemWait(&remap->memory.locks[0]) == vx_true_e)
                {
                    *stride_y = user_stride_y;

                    vx_coordinates2df_t *buf_ptr = (vx_coordinates2df_t *)buf;
                    vx_uint32 i;
                    vx_uint32 j;
                    for (i = start_y; i < end_y; i++)
                    {
                        for (j = start_x; j < end_x; j++)
                        {
                            vx_coordinates2df_t *coord_ptr = &(buf_ptr[i * stride + j]);
                            status = remap->getCoordValue(j, i, &coord_ptr->x, &coord_ptr->y);
                        }
                    }

                    if (VX_SUCCESS == status)
                    {
                        *ptr = buf;
                        remap->incrementReference(VX_EXTERNAL);
                    }
                    ownSemPost(&remap->memory.locks[0]);
                }
                else
                {
                    status = VX_ERROR_NO_RESOURCES;
                }
            }
            else
            {
                /* write only mode */
                *stride_y = user_stride_y;
                *ptr = buf;
                remap->incrementReference(VX_EXTERNAL);
            }
        }
        else
        {
            status = VX_FAILURE;
        }

#ifdef OPENVX_USE_OPENCL_INTEROP
        if ((status == VX_SUCCESS) &&
            remap->context->opencl_context &&
            (mem_type_requested == VX_MEMORY_TYPE_OPENCL_BUFFER) &&
            (size > 0) && ptr && *ptr)
        {
            /* create OpenCL buffer using the host allocated pointer */
            cl_int cerr = 0;
            cl_mem opencl_buf = clCreateBuffer(remap->context->opencl_context,
                CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                size, *ptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxMapRemap: clCreateBuffer(%u) => %p (%d)\n",
                (vx_uint32)size, opencl_buf, cerr);
            if (cerr == CL_SUCCESS)
            {
                remap->context->memory_maps[*map_id].opencl_buf = opencl_buf;
                *ptr = opencl_buf;
            }
            else
            {
                status = VX_FAILURE;
            }
        }
#endif
    }

    VX_PRINT(VX_ZONE_API, "return %d\n", status);
    return status;
}


VX_API_ENTRY vx_status VX_API_CALL vxUnmapRemapPatch(vx_remap remap, vx_map_id map_id)
{
    vx_status status = VX_SUCCESS;

    /* bad references */
    if (Remap::isValidRemap(remap) == vx_false_e)
    {
        status = VX_ERROR_INVALID_REFERENCE;
        goto exit;
    }

    /* bad parameters */
    if (remap->context->findMemoryMap((vx_reference)remap, map_id) != vx_true_e)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        goto exit;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (remap->context->opencl_context &&
        remap->context->memory_maps[map_id].opencl_buf &&
        remap->context->memory_maps[map_id].ptr)
    {
        clEnqueueUnmapMemObject(remap->context->opencl_command_queue,
            remap->context->memory_maps[map_id].opencl_buf,
            remap->context->memory_maps[map_id].ptr, 0, nullptr, nullptr);
        clFinish(remap->context->opencl_command_queue);
        cl_int cerr = clReleaseMemObject(remap->context->memory_maps[map_id].opencl_buf);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxUnmapRemap: clReleaseMemObject(%p) => (%d)\n",
            remap->context->memory_maps[map_id].opencl_buf, cerr);
        remap->context->memory_maps[map_id].opencl_buf = nullptr;
    }
#endif

    if (VX_SUCCESS == status)
    {
        vx_context context = remap->context;
        vx_memory_map_t* map = &context->memory_maps[map_id];
        if (map->used && map->ref == (vx_reference)remap)
        {
            vx_rectangle_t rect = map->extra.image_data.rect;
            if (VX_WRITE_ONLY == map->usage || VX_READ_AND_WRITE == map->usage)
            {
                vx_size stride = (rect.end_x - rect.start_x);
                vx_coordinates2df_t *ptr = (vx_coordinates2df_t *)map->ptr;
                vx_uint32 i;
                vx_uint32 j;
                for (i = rect.start_y; i < rect.end_y; i++)
                {
                    for (j = rect.start_x; j < rect.end_x; j++)
                    {
                        vx_coordinates2df_t *coord_ptr = &(ptr[i * stride + j]);
                        status = remap->setCoordValue(j, i, coord_ptr->x, coord_ptr->y);
                        if(status != VX_SUCCESS)
                        {
                            goto exit;
                        }
                    }
                }

                context->memoryUnmap((vx_uint32)map_id);
                remap->decrementReference(VX_EXTERNAL);
                status = VX_SUCCESS;
            }
            else
            {
                /* rean only mode */
                remap->context->memoryUnmap((vx_uint32)map_id);
                remap->decrementReference(VX_EXTERNAL);
                status = VX_SUCCESS;
            }
        }
        else
        {
            status = VX_FAILURE;
            goto exit;
        }
    }
exit:
    VX_PRINT(VX_ZONE_API, "return %d\n", status);
    return status;
}
