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
#include "vx_lut.h"

VX_API_ENTRY vx_lut VX_API_CALL vxCreateLUT(vx_context context, vx_enum data_type, vx_size count)
{
    vx_lut_t lut = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if (data_type == VX_TYPE_UINT8)
        {
#if defined(OPENVX_STRICT_1_0)
            if (count != 256)
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid parameter to LUT\n");
                vxAddLogEntry(context, VX_ERROR_INVALID_PARAMETERS, "Invalid parameter to LUT\n");
                // lut = (vx_lut_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
            }
            else
#endif
            {
                // lut = (vx_lut_t *)ownCreateArrayInt(context, VX_TYPE_UINT8, count, vx_false_e, VX_TYPE_LUT);
                if (vxGetStatus((vx_reference)lut) == VX_SUCCESS && lut->type == VX_TYPE_LUT)
                {
                    lut->num_items = count;
                    lut->offset = 0;
                    // ownPrintArray(lut);
                }
            }
        }
        else if (data_type == VX_TYPE_INT16)
        {
            if (!(count <= 65536))
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid parameter to LUT\n");
                vxAddLogEntry(context, VX_ERROR_INVALID_PARAMETERS, "Invalid parameter to LUT\n");
                // lut = (vx_lut_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
            }
            else
            {
                // lut = (vx_lut_t *)ownCreateArrayInt(context, VX_TYPE_INT16, count, vx_false_e, VX_TYPE_LUT);
                if (vxGetStatus((vx_reference)lut) == VX_SUCCESS && lut->type == VX_TYPE_LUT)
                {
                    lut->num_items = count;
                    lut->offset = (vx_uint32)(count/2);
                    // ownPrintArray(lut);
                }
            }
        }
#if !defined(OPENVX_STRICT_1_0)
        else if (data_type == VX_TYPE_UINT16)
        {
            // lut = (vx_lut_t *)ownCreateArrayInt(context, VX_TYPE_UINT16, count, vx_false_e, VX_TYPE_LUT);
            if (vxGetStatus((vx_reference)lut) == VX_SUCCESS && lut->type == VX_TYPE_LUT)
            {
                lut->num_items = count;
                lut->offset = 0;
                // ownPrintArray(lut);
            }
        }
#endif
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid data type\n");
            vxAddLogEntry(context, VX_ERROR_INVALID_TYPE, "Invalid data type\n");
            // lut = (vx_lut_t *)ownGetErrorObject(context, VX_ERROR_INVALID_TYPE);
        }
    }

    return (vx_lut)lut;
}

VX_API_ENTRY vx_lut VX_API_CALL vxCreateVirtualLUT(vx_graph graph, vx_enum data_type, vx_size count)
{
    vx_lut_t lut = nullptr;
    vx_reference gref = (vx_reference)graph;

    if (Reference::isValidReference(gref, VX_TYPE_GRAPH) == vx_true_e)
    {
        lut = (vx_lut_t)vxCreateLUT(gref->context, data_type, count);
        if (vxGetStatus((vx_reference)lut) == VX_SUCCESS && lut->type ==  VX_TYPE_LUT)
        {
           lut->scope = (vx_reference)graph;
           lut->is_virtual = vx_true_e;
        }
    }

    return (vx_lut)lut;
}


void vxDestructLUT(vx_reference ref)
{
    vx_lut_t lut = (vx_lut_t)ref;
    // ownDestructArray((vx_reference)lut);
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseLUT(vx_lut *l)
{
    // return ownReleaseReferenceInt((vx_reference *)l, VX_TYPE_LUT, VX_EXTERNAL, nullptr);
    return VX_ERROR_NOT_IMPLEMENTED;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryLUT(vx_lut lut, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    switch (attribute)
    {
        case VX_LUT_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
            {
                *(vx_enum *)ptr = lut->item_type;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_COUNT:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = lut->num_items;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = lut->num_items * lut->item_size;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_OFFSET:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = lut->offset;
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

VX_API_ENTRY vx_status VX_API_CALL vxAccessLUT(vx_lut l, void **ptr, vx_enum usage)
{
    vx_status status = VX_FAILURE;
    vx_lut_t *lut = (vx_lut_t *)l;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
        // status = ownAccessArrayRangeInt((vx_array_t *)l, 0, lut->num_items, nullptr, ptr, usage);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCommitLUT(vx_lut l, const void *ptr)
{
    vx_status status = VX_FAILURE;
    vx_lut_t *lut = (vx_lut_t *)l;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
        // status = ownCommitArrayRangeInt((vx_array_t *)l, 0, lut->num_items, ptr);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyLUT(vx_lut lut, void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_FAILURE;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
        vx_size stride = lut->item_size;
#ifdef OPENVX_USE_OPENCL_INTEROP
        void * user_ptr_given = user_ptr;
        vx_enum user_mem_type_given = user_mem_type;
        if (user_mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
        {
            // get ptr from OpenCL buffer for HOST
            size_t size = 0;
            cl_mem opencl_buf = (cl_mem)user_ptr;
            cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyLUT: clGetMemObjectInfo(%p) => (%d)\n",
                opencl_buf, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            user_ptr = clEnqueueMapBuffer(lut->context->opencl_command_queue,
                opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
                0, nullptr, nullptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyLUT: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                opencl_buf, (int)size, user_ptr, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            user_mem_type = VX_MEMORY_TYPE_HOST;
        }
#endif

        // status = ownCopyArrayRangeInt((vx_array)l, 0, lut->num_items, stride, user_ptr, usage, user_mem_type);

#ifdef OPENVX_USE_OPENCL_INTEROP
        if (user_mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
        {
            clEnqueueUnmapMemObject(lut->context->opencl_command_queue,
                (cl_mem)user_ptr_given, user_ptr, 0, nullptr, nullptr);
            clFinish(lut->context->opencl_command_queue);
        }
#endif

    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxMapLUT(vx_lut lut, vx_map_id *map_id, void **ptr, vx_enum usage, vx_enum mem_type, vx_bitfield flags)
{
    vx_status status = VX_FAILURE;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
#ifdef OPENVX_USE_OPENCL_INTEROP
         vx_enum mem_type_requested = mem_type;
         if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
         {
             mem_type = VX_MEMORY_TYPE_HOST;
         }
#endif

        vx_size stride = lut->item_size;
        // status = ownMapArrayRangeInt((vx_array_t *)lut, 0, lut->num_items, map_id, &stride, ptr, usage, mem_type, flags);

#ifdef OPENVX_USE_OPENCL_INTEROP
        vx_size size = lut->num_items * stride;
        if ((status == VX_SUCCESS) && lut->context->opencl_context &&
            (mem_type_requested == VX_MEMORY_TYPE_OPENCL_BUFFER) &&
            (size > 0) && ptr && *ptr)
        {
            /* create OpenCL buffer using the host allocated pointer */
            cl_int cerr = 0;
            cl_mem opencl_buf = clCreateBuffer(lut->context->opencl_context,
                CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                size, *ptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxMapLUT: clCreateBuffer(%u) => %p (%d)\n",
                (vx_uint32)size, opencl_buf, cerr);
            if (cerr == CL_SUCCESS)
            {
                lut->context->memory_maps[*map_id].opencl_buf = opencl_buf;
                *ptr = opencl_buf;
            }
            else
            {
                status = VX_FAILURE;
            }
        }
#endif
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxUnmapLUT(vx_lut lut, vx_map_id map_id)
{
    vx_status status = VX_FAILURE;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
#ifdef OPENVX_USE_OPENCL_INTEROP
        if (lut->context->opencl_context &&
            lut->context->memory_maps[map_id].opencl_buf &&
            lut->context->memory_maps[map_id].ptr)
        {
            clEnqueueUnmapMemObject(lut->context->opencl_command_queue,
                lut->context->memory_maps[map_id].opencl_buf,
                lut->context->memory_maps[map_id].ptr, 0, nullptr, nullptr);
            clFinish(lut->context->opencl_command_queue);
            cl_int cerr = clReleaseMemObject(lut->context->memory_maps[map_id].opencl_buf);
            VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxUnmapLUT: clReleaseMemObject(%p) => (%d)\n",
                lut->context->memory_maps[map_id].opencl_buf, cerr);
            lut->context->memory_maps[map_id].opencl_buf = nullptr;
        }
#endif

        // status = ownUnmapArrayRangeInt((vx_array_t *)l, map_id);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

