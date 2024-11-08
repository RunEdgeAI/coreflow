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

#include "vx_internal.h"
#include "vx_distribution.h"

Distribution::Distribution(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_DISTRIBUTION, scope),
memory(),
range_x(0),
range_y(0),
offset_x(0),
offset_y(0)
{
}

Distribution::~Distribution()
{
    destructDistribution();
}

void Distribution::destructDistribution()
{
    ownFreeMemory(context, &memory);
}

VX_API_ENTRY vx_distribution VX_API_CALL vxCreateDistribution(vx_context context, vx_size numBins, vx_int32 offset, vx_uint32 range)
{
    vx_distribution distribution = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if ((numBins != 0) && (range != 0))
        {
            distribution = (vx_distribution)Reference::createReference(context, VX_TYPE_DISTRIBUTION, VX_EXTERNAL, context);
            if ( vxGetStatus((vx_reference)distribution) == VX_SUCCESS &&
                 distribution->type == VX_TYPE_DISTRIBUTION)
            {
                distribution->memory.ndims = 2;
                distribution->memory.nptrs = 1;
                distribution->memory.strides[0][VX_DIM_C] = sizeof(vx_int32);
                distribution->memory.dims[0][VX_DIM_C] = 1;
                distribution->memory.dims[0][VX_DIM_X] = (vx_uint32)numBins;
                distribution->memory.dims[0][VX_DIM_Y] = 1;
                distribution->range_x = (vx_uint32)range;
                distribution->range_y = 1;
                distribution->offset_x = offset;
                distribution->offset_y = 0;
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to distribution\n");
            vxAddLogEntry(context, VX_ERROR_INVALID_PARAMETERS, "Invalid parameters to distribution\n");
            // distribution = (vx_distribution)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        }
    }

    return distribution;
}

VX_API_ENTRY vx_distribution VX_API_CALL vxCreateVirtualDistribution( vx_graph graph, vx_size numBins, vx_int32 offset, vx_uint32 range)
{
    vx_distribution distribution = nullptr;
    vx_reference gref = (vx_reference)graph;

    if (Reference::isValidReference(gref, VX_TYPE_GRAPH) == vx_true_e)
    {
        distribution = vxCreateDistribution(gref->context, numBins, offset, range);
        if (vxGetStatus((vx_reference)distribution) == VX_SUCCESS && distribution->type ==  VX_TYPE_DISTRIBUTION)
        {
           distribution->scope = (vx_reference)graph;
           distribution->is_virtual = vx_true_e;
        }
    }
    return distribution;
}

vx_status vxReleaseDistributionInt(vx_distribution* distribution)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != distribution)
    {
        vx_reference ref = *distribution;
        if (vx_true_e == Reference::isValidReference(ref, VX_TYPE_DISTRIBUTION))
        {
            status = Reference::releaseReference((vx_reference*)distribution, VX_TYPE_DISTRIBUTION, VX_INTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseDistribution(vx_distribution *d)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != d)
    {
        vx_reference ref = *d;
        if (vx_true_e == Reference::isValidReference(ref, VX_TYPE_DISTRIBUTION))
        {
            status = Reference::releaseReference((vx_reference*)d, VX_TYPE_DISTRIBUTION, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryDistribution(vx_distribution distribution, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    switch (attribute)
    {
        case VX_DISTRIBUTION_DIMENSIONS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size*)ptr = (vx_size)(distribution->memory.ndims - 1);
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_RANGE:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32*)ptr = (vx_uint32)(distribution->range_x);
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_BINS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size*)ptr = (vx_size)distribution->memory.dims[0][VX_DIM_X];
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_WINDOW:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                vx_size nbins = (vx_size)distribution->memory.dims[0][VX_DIM_X];
                vx_uint32 range = (vx_uint32)(distribution->range_x);
                vx_uint32 window = (vx_uint32)(range / nbins);
                if (window*nbins == range)
                    *(vx_uint32*)ptr = window;
                else
                    *(vx_uint32*)ptr = 0;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_OFFSET:
            if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3))
            {
                *(vx_int32*)ptr = distribution->offset_x;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size*)ptr = distribution->memory.strides[0][VX_DIM_C] *
                                    distribution->memory.dims[0][VX_DIM_X];
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

VX_API_ENTRY vx_status VX_API_CALL vxAccessDistribution(vx_distribution distribution, void **ptr, vx_enum usage)
{
    vx_status status = VX_FAILURE;
    (void)usage;

    if ((Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) == vx_true_e) &&
        (ownAllocateMemory(distribution->context, &distribution->memory) == vx_true_e))
    {
        if (ptr != nullptr)
        {
            ownSemWait(&distribution->lock);
            {
                vx_size size = ownComputeMemorySize(&distribution->memory, 0);
                ownPrintMemory(&distribution->memory);
                if (*ptr == nullptr)
                {
                    *ptr = distribution->memory.ptrs[0];
                }
                else if (*ptr != nullptr)
                {
                    memcpy(*ptr, distribution->memory.ptrs[0], size);
                }
            }
            ownSemPost(&distribution->lock);
            // ownReadFromReference(&distribution->base);
        }
        distribution->incrementReference(VX_EXTERNAL);
        status = VX_SUCCESS;
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCommitDistribution(vx_distribution distribution, const void *ptr)
{
    vx_status status = VX_FAILURE;
    if ((Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) == vx_true_e) &&
        (ownAllocateMemory(distribution->context, &distribution->memory) == vx_true_e))
    {
        if (ptr != nullptr)
        {
            ownSemWait(&distribution->lock);
            {
                if (ptr != distribution->memory.ptrs[0])
                {
                    vx_size size = ownComputeMemorySize(&distribution->memory, 0);
                    memcpy(distribution->memory.ptrs[0], ptr, size);
                    VX_PRINT(VX_ZONE_INFO, "Copied distribution from %p to %p for " VX_FMT_SIZE " bytes\n", ptr, distribution->memory.ptrs[0], size);
                }
            }
            ownSemPost(&distribution->lock);
            // ownWroteToReference(&distribution->base);
        }
        distribution->decrementReference(VX_EXTERNAL);
        status = VX_SUCCESS;
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyDistribution(vx_distribution distribution, void *user_ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_FAILURE;
    vx_size size = 0;

    /* bad references */
    if ((Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e) ||
        (ownAllocateMemory(distribution->context, &distribution->memory) != vx_true_e))
    {
        status = VX_ERROR_INVALID_REFERENCE;
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    /* bad parameters */
    if (((usage != VX_READ_ONLY) && (usage != VX_WRITE_ONLY)) ||
        (user_ptr == nullptr) || (mem_type != VX_MEMORY_TYPE_HOST))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to copy distribution\n");
        return status;
    }

    /* copy data */
    size = ownComputeMemorySize(&distribution->memory, 0);
    ownPrintMemory(&distribution->memory);

#ifdef OPENVX_USE_OPENCL_INTEROP
    void * user_ptr_given = user_ptr;
    vx_enum mem_type_given = mem_type;
    if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        // get ptr from OpenCL buffer for HOST
        size_t size = 0;
        cl_mem opencl_buf = (cl_mem)user_ptr;
        cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyDistribution: clGetMemObjectInfo(%p) => (%d)\n",
            opencl_buf, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        user_ptr = clEnqueueMapBuffer(distribution->context->opencl_command_queue,
            opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
            0, nullptr, nullptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyDistribution: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
            opencl_buf, (int)size, user_ptr, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    switch (usage)
    {
    case VX_READ_ONLY:
        if (ownSemWait(&distribution->lock) == vx_true_e)
        {
            memcpy(user_ptr, distribution->memory.ptrs[0], size);
            ownSemPost(&distribution->lock);

            // ownReadFromReference(&distribution->base);
            status = VX_SUCCESS;
        }
        break;
    case VX_WRITE_ONLY:
        if (ownSemWait(&distribution->lock) == vx_true_e)
        {
            memcpy(distribution->memory.ptrs[0], user_ptr, size);
            ownSemPost(&distribution->lock);

            // ownWroteToReference(&distribution->base);
            status = VX_SUCCESS;
        }
        break;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        clEnqueueUnmapMemObject(distribution->context->opencl_command_queue,
            (cl_mem)user_ptr_given, user_ptr, 0, nullptr, nullptr);
        clFinish(distribution->context->opencl_command_queue);
    }
#endif

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxMapDistribution(vx_distribution distribution, vx_map_id *map_id, void **ptr, vx_enum usage, vx_enum mem_type, vx_bitfield flags)
{
    vx_status status = VX_FAILURE;
    vx_size size = 0;

    /* bad references */
    if ((Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e) ||
        (ownAllocateMemory(distribution->context, &distribution->memory) != vx_true_e))
    {
        status = VX_ERROR_INVALID_REFERENCE;
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_enum mem_type_requested = mem_type;
    if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    /* bad parameters */
    if (((usage != VX_READ_ONLY) && (usage != VX_READ_AND_WRITE) && (usage != VX_WRITE_ONLY)) ||
        (mem_type != VX_MEMORY_TYPE_HOST))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to map distribution\n");
        return status;
    }

    /* map data */
    size = ownComputeMemorySize(&distribution->memory, 0);
    ownPrintMemory(&distribution->memory);

    if (distribution->context->memoryMap((vx_reference)distribution, size, usage, mem_type, flags, nullptr, ptr, map_id) == vx_true_e)
    {
        switch (usage)
        {
        case VX_READ_ONLY:
        case VX_READ_AND_WRITE:
            if (ownSemWait(&distribution->lock) == vx_true_e)
            {
                memcpy(*ptr, distribution->memory.ptrs[0], size);
                ownSemPost(&distribution->lock);

                // ownReadFromReference(&distribution->base);
                status = VX_SUCCESS;
            }
            break;
        case VX_WRITE_ONLY:
            status = VX_SUCCESS;
            break;
        }

        if (status == VX_SUCCESS)
            distribution->incrementReference(VX_EXTERNAL);
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if ((status == VX_SUCCESS) && distribution->context->opencl_context &&
        (mem_type_requested == VX_MEMORY_TYPE_OPENCL_BUFFER) &&
        (size > 0) && ptr && *ptr)
    {
        /* create OpenCL buffer using the host allocated pointer */
        cl_int cerr = 0;
        cl_mem opencl_buf = clCreateBuffer(distribution->context->opencl_context,
            CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
            size, *ptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxMapDistribution: clCreateBuffer(%u) => %p (%d)\n",
            (vx_uint32)size, opencl_buf, cerr);
        if (cerr == CL_SUCCESS)
        {
            distribution->context->memory_maps[*map_id].opencl_buf = opencl_buf;
            *ptr = opencl_buf;
        }
        else
        {
            status = VX_FAILURE;
        }
    }
#endif

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxUnmapDistribution(vx_distribution distribution, vx_map_id map_id)
{
    vx_status status = VX_FAILURE;
    vx_size size = 0;

    /* bad references */
    if ((Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e) ||
        (ownAllocateMemory(distribution->context, &distribution->memory) != vx_true_e))
    {
        status = VX_ERROR_INVALID_REFERENCE;
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    /* bad parameters */
    if (distribution->context->findMemoryMap((vx_reference)distribution, map_id) != vx_true_e)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to unmap distribution\n");
        return status;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (distribution->context->opencl_context &&
        distribution->context->memory_maps[map_id].opencl_buf &&
        distribution->context->memory_maps[map_id].ptr)
    {
        clEnqueueUnmapMemObject(distribution->context->opencl_command_queue,
            distribution->context->memory_maps[map_id].opencl_buf,
            distribution->context->memory_maps[map_id].ptr, 0, nullptr, nullptr);
        clFinish(distribution->context->opencl_command_queue);
        cl_int cerr = clReleaseMemObject(distribution->context->memory_maps[map_id].opencl_buf);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxUnmapDistribution: clReleaseMemObject(%p) => (%d)\n",
            distribution->context->memory_maps[map_id].opencl_buf, cerr);
        distribution->context->memory_maps[map_id].opencl_buf = nullptr;
    }
#endif

    /* unmap data */
    size = ownComputeMemorySize(&distribution->memory, 0);
    ownPrintMemory(&distribution->memory);

    {
        vx_uint32 id = (vx_uint32)map_id;
        vx_memory_map_t* map = &distribution->context->memory_maps[id];

        switch (map->usage)
        {
        case VX_READ_ONLY:
            status = VX_SUCCESS;
            break;
        case VX_READ_AND_WRITE:
        case VX_WRITE_ONLY:
            if (ownSemWait(&distribution->lock) == vx_true_e)
            {
                memcpy(distribution->memory.ptrs[0], map->ptr, size);
                ownSemPost(&distribution->lock);

                // ownWroteToReference(&distribution->base);
                status = VX_SUCCESS;
            }
            break;
        }

        distribution->context->memoryUnmap((vx_uint32)map_id);

        /* regardless of the current status, if we're here, so previous call to vxMapDistribution()
         * was successful and thus ref was locked once by a call to ownIncrementReference() */
        distribution->decrementReference(VX_EXTERNAL);
    }

    return status;
}
