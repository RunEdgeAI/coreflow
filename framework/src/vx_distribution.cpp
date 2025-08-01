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

/******************************************************************************/
/* INTERNAL INTERFACE                                                         */
/******************************************************************************/
Distribution::Distribution(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_DISTRIBUTION, scope),
memory(),
range_x(0),
range_y(0),
offset_x(0),
offset_y(0)
{
}

vx_size Distribution::dims() const
{
    return (vx_size)(memory.ndims - 1);
}

vx_uint32 Distribution::range() const
{
    return (vx_uint32)(range_x);
}

vx_size Distribution::bins() const
{
    return (vx_size)memory.dims[0][VX_DIM_X];
}

vx_uint32 Distribution::window() const
{
    vx_size nbins = (vx_size)memory.dims[0][VX_DIM_X];
    vx_uint32 range = (vx_uint32)(range_x);
    vx_uint32 window = (vx_uint32)(range / nbins);
    if (window * nbins == range)
        return window;
    else
        return 0;
}

vx_int32 Distribution::offset() const
{
    return offset_x;
}

vx_uint32 Distribution::size() const
{
    return memory.strides[0][VX_DIM_C] * memory.dims[0][VX_DIM_X];
}

vx_status Distribution::access(void **ptr, vx_enum usage)
{
    vx_status status = VX_ERROR_NO_MEMORY;
    (void)usage;

    if (Memory::allocateMemory(context, &memory) == vx_true_e)
    {
        if (ptr != nullptr)
        {
            Osal::semWait(&lock);
            {
                vx_size size = Memory::computeMemorySize(&memory, 0);
                Memory::printMemory(&memory);
                if (*ptr == nullptr)
                {
                    *ptr = memory.ptrs[0];
                }
                else if (*ptr != nullptr)
                {
                    memcpy(*ptr, memory.ptrs[0], size);
                }
            }
            Osal::semPost(&lock);
            // ownReadFromReference(&base);
        }
        incrementReference(VX_EXTERNAL);
        status = VX_SUCCESS;
    }

    return status;
}

vx_status Distribution::commit(const void *ptr)
{
    vx_status status = VX_ERROR_NO_MEMORY;

    if (Memory::allocateMemory(context, &memory) == vx_true_e)
    {
        if (ptr != nullptr)
        {
            Osal::semWait(&lock);
            {
                if (ptr != memory.ptrs[0])
                {
                    vx_size size = Memory::computeMemorySize(&memory, 0);
                    memcpy(memory.ptrs[0], ptr, size);
                    VX_PRINT(VX_ZONE_INFO,
                             "Copied distribution from %p to %p for " VX_FMT_SIZE " bytes\n", ptr,
                             memory.ptrs[0], size);
                }
            }
            Osal::semPost(&lock);
            // ownWroteToReference(&base);
        }
        decrementReference(VX_EXTERNAL);
        status = VX_SUCCESS;
    }

    return status;
}

vx_status Distribution::copy(void *user_ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_SUCCESS;
    vx_size size = 0;

    /* bad references */
    if (Memory::allocateMemory(context, &memory) != vx_true_e)
    {
        status = VX_ERROR_NO_MEMORY;
        VX_PRINT(VX_ZONE_ERROR, "Distribution object not allocated!\n");
        return status;
    }

    /* bad parameters */
    if (((usage != VX_READ_ONLY) && (usage != VX_WRITE_ONLY)) || (user_ptr == nullptr) ||
        (mem_type != VX_MEMORY_TYPE_HOST))
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to copy distribution\n");
        return status;
    }

    /* copy data */
    size = Memory::computeMemorySize(&memory, 0);
    Memory::printMemory(&memory);

#ifdef OPENVX_USE_OPENCL_INTEROP
    void *user_ptr_given = user_ptr;
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
        user_ptr =
            clEnqueueMapBuffer(context->opencl_command_queue, opencl_buf, CL_TRUE,
                               CL_MAP_READ | CL_MAP_WRITE, 0, size, 0, nullptr, nullptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT,
                 "OPENCL: vxCopyDistribution: clEnqueueMapBuffer(%p,%d) => %p (%d)\n", opencl_buf,
                 (int)size, user_ptr, cerr);
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
            if (Osal::semWait(&lock) == vx_true_e)
            {
                memcpy(user_ptr, memory.ptrs[0], size);
                Osal::semPost(&lock);

                // ownReadFromReference(&base);
                status = VX_SUCCESS;
            }
            break;
        case VX_WRITE_ONLY:
            if (Osal::semWait(&lock) == vx_true_e)
            {
                memcpy(memory.ptrs[0], user_ptr, size);
                Osal::semPost(&lock);

                // ownWroteToReference(&base);
                status = VX_SUCCESS;
            }
            break;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)user_ptr_given, user_ptr, 0,
                                nullptr, nullptr);
        clFinish(context->opencl_command_queue);
    }
#endif

    return status;
}

vx_status Distribution::map(vx_map_id *map_id, void **ptr, vx_enum usage, vx_enum mem_type,
                            vx_bitfield flags)
{
    vx_status status = VX_SUCCESS;
    vx_size size = 0;

    /* bad references */
    if (Memory::allocateMemory(context, &memory) != vx_true_e)
    {
        status = VX_ERROR_NO_MEMORY;
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
    size = Memory::computeMemorySize(&memory, 0);
    Memory::printMemory(&memory);

    if (context->memoryMap((vx_reference)this, size, usage, mem_type, flags, nullptr, ptr,
                           map_id) == vx_true_e)
    {
        switch (usage)
        {
            case VX_READ_ONLY:
            case VX_READ_AND_WRITE:
                if (Osal::semWait(&lock) == vx_true_e)
                {
                    memcpy(*ptr, memory.ptrs[0], size);
                    Osal::semPost(&lock);

                    // ownReadFromReference(&base);
                    status = VX_SUCCESS;
                }
                break;
            case VX_WRITE_ONLY:
                status = VX_SUCCESS;
                break;
        }

        if (status == VX_SUCCESS) incrementReference(VX_EXTERNAL);
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if ((status == VX_SUCCESS) && context->opencl_context &&
        (mem_type_requested == VX_MEMORY_TYPE_OPENCL_BUFFER) && (size > 0) && ptr && *ptr)
    {
        /* create OpenCL buffer using the host allocated pointer */
        cl_int cerr = 0;
        cl_mem opencl_buf = clCreateBuffer(
            context->opencl_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size, *ptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxMapDistribution: clCreateBuffer(%u) => %p (%d)\n",
                 (vx_uint32)size, opencl_buf, cerr);
        if (cerr == CL_SUCCESS)
        {
            context->memory_maps[*map_id].opencl_buf = opencl_buf;
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

vx_status Distribution::unmap(vx_map_id map_id)
{
    vx_status status = VX_SUCCESS;
    vx_size size = 0;

    if (Memory::allocateMemory(context, &memory) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to allocate memory for distribution\n");
        return VX_ERROR_NO_MEMORY;
    }

    /* bad parameters */
    if (context->findMemoryMap((vx_reference)this, map_id) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to unmap distribution\n");
        return VX_ERROR_INVALID_PARAMETERS;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (context->opencl_context && context->memory_maps[map_id].opencl_buf &&
        context->memory_maps[map_id].ptr)
    {
        clEnqueueUnmapMemObject(context->opencl_command_queue,
                                context->memory_maps[map_id].opencl_buf,
                                context->memory_maps[map_id].ptr, 0, nullptr, nullptr);
        clFinish(context->opencl_command_queue);
        cl_int cerr = clReleaseMemObject(context->memory_maps[map_id].opencl_buf);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxUnmapDistribution: clReleaseMemObject(%p) => (%d)\n",
                 context->memory_maps[map_id].opencl_buf, cerr);
        context->memory_maps[map_id].opencl_buf = nullptr;
    }
#endif

    /* unmap data */
    size = Memory::computeMemorySize(&memory, 0);
    Memory::printMemory(&memory);

    {
        vx_uint32 id = (vx_uint32)map_id;
        vx_memory_map_t *map = &context->memory_maps[id];

        switch (map->usage)
        {
            case VX_READ_ONLY:
                status = VX_SUCCESS;
                break;
            case VX_READ_AND_WRITE:
            case VX_WRITE_ONLY:
                if (Osal::semWait(&lock) == vx_true_e)
                {
                    memcpy(memory.ptrs[0], map->ptr, size);
                    Osal::semPost(&lock);

                    // ownWroteToReference(&base);
                    status = VX_SUCCESS;
                }
                break;
        }

        context->memoryUnmap((vx_uint32)map_id);

        /* regardless of the current status, if we're here, so previous call to vxMapDistribution()
         * was successful and thus ref was locked once by a call to ownIncrementReference() */
        decrementReference(VX_EXTERNAL);
    }

    return status;
}

Distribution::~Distribution()
{
}

void Distribution::destruct()
{
    Memory::freeMemory(context, &memory);
}

/******************************************************************************/
/* PUBLIC  INTERFACE                                                          */
/******************************************************************************/
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
            distribution = (vx_distribution)vxGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
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
                *(vx_size*)ptr = distribution->dims();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_RANGE:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32*)ptr = distribution->range();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_BINS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size*)ptr = distribution->bins();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_WINDOW:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32*)ptr = distribution->window();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_OFFSET:
            if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3))
            {
                *(vx_int32*)ptr = distribution->offset();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size*)ptr = distribution->size();
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
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    return distribution->access(ptr, usage);
}

VX_API_ENTRY vx_status VX_API_CALL vxCommitDistribution(vx_distribution distribution, const void *ptr)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    return distribution->commit(ptr);
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyDistribution(vx_distribution distribution, void *user_ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    return distribution->copy(user_ptr, usage, mem_type);
}

VX_API_ENTRY vx_status VX_API_CALL vxMapDistribution(vx_distribution distribution, vx_map_id *map_id, void **ptr, vx_enum usage, vx_enum mem_type, vx_bitfield flags)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    return distribution->map(map_id, ptr, usage, mem_type, flags);
}

VX_API_ENTRY vx_status VX_API_CALL vxUnmapDistribution(vx_distribution distribution, vx_map_id map_id)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(distribution, VX_TYPE_DISTRIBUTION) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid distribution object!\n");
        return status;
    }

    return distribution->unmap(map_id);
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