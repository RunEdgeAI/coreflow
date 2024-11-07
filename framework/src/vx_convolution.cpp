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
#include "vx_convolution.h"

Convolution::Convolution(vx_context context, vx_reference scope) : Matrix(context, VX_TYPE_CONVOLUTION, scope)
{

}

Convolution::~Convolution()
{
    destructConvolution();
}

void Convolution::destructConvolution()
{
    ownFreeMemory(context, &memory);
}

static VX_INLINE int isodd(size_t a)
{
    return (int)(a & 1);
}

static vx_bool vxIsPowerOfTwo(vx_uint32 a)
{
    if (a == 0)
        return vx_false_e;
    else if ((a & ((a) - 1)) == 0)
        return vx_true_e;
    else
        return vx_false_e;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseConvolution(vx_convolution* convolution)
{
    vx_status status = VX_FAILURE;

    if (nullptr != convolution)
    {
        vx_convolution conv = *convolution;
        if (vx_true_e == Reference::isValidReference(conv, VX_TYPE_CONVOLUTION))
        {
            status = Reference::releaseReference((vx_reference*)convolution, VX_TYPE_CONVOLUTION, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_convolution VX_API_CALL vxCreateConvolution(vx_context context, vx_size columns, vx_size rows)
{
    vx_convolution convolution = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if (isodd(columns) && columns >= 3 && isodd(rows) && rows >= 3)
        {
            convolution = (vx_convolution)Reference::createReference(context, VX_TYPE_CONVOLUTION, VX_EXTERNAL, context);
            if (vxGetStatus((vx_reference)convolution) == VX_SUCCESS && convolution->type == VX_TYPE_CONVOLUTION)
            {
                convolution->data_type = VX_TYPE_INT16;
                convolution->columns = columns;
                convolution->rows = rows;
                convolution->memory.ndims = 2;
                convolution->memory.nptrs = 1;
                convolution->memory.dims[0][0] = sizeof(vx_int16);
                convolution->memory.dims[0][1] = (vx_uint32)(columns*rows);
                convolution->scale = 1;
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to create convolution, invalid dimensions\n");
            vxAddLogEntry((vx_reference)context, VX_ERROR_INVALID_DIMENSION, "Invalid dimensions to convolution\n");
            // convolution = (vx_convolution )ownGetErrorObject((vx_context )context, VX_ERROR_INVALID_DIMENSION);
        }
    }
    return convolution;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryConvolution(vx_convolution convolution, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(convolution, VX_TYPE_CONVOLUTION) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }
    switch (attribute)
    {
        case VX_CONVOLUTION_ROWS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = convolution->rows;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_CONVOLUTION_COLUMNS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = convolution->columns;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_CONVOLUTION_SCALE:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = convolution->scale;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_CONVOLUTION_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = convolution->columns * convolution->rows * sizeof(vx_int16);
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

VX_API_ENTRY vx_status VX_API_CALL vxSetConvolutionAttribute(vx_convolution convolution, vx_enum attribute, const void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(convolution, VX_TYPE_CONVOLUTION) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }
    switch (attribute)
    {
        case VX_CONVOLUTION_SCALE:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                vx_uint32 scale = *(vx_uint32 *)ptr;
                if (vxIsPowerOfTwo(scale) == vx_true_e)
                {
                    VX_PRINT(VX_ZONE_INFO, "Convolution Scale assigned to %u\n", scale);
                    convolution->scale = scale;
                }
                else
                {
                    status = VX_ERROR_INVALID_VALUE;
                }
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        default:
            status = VX_ERROR_INVALID_PARAMETERS;
            break;
    }
    if (status != VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to set attribute on convolution! (%d)\n", status);
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxReadConvolutionCoefficients(vx_convolution convolution, vx_int16 *array)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;
    if ((Reference::isValidReference(convolution, VX_TYPE_CONVOLUTION) == vx_true_e) &&
        (ownAllocateMemory(convolution->context, &convolution->memory) == vx_true_e))
    {
        ownSemWait(&convolution->lock);
        if (array)
        {
            vx_size size = convolution->memory.strides[0][1] *
                           convolution->memory.dims[0][1];
            memcpy(array, convolution->memory.ptrs[0], size);
        }
        ownSemPost(&convolution->lock);
        // ownReadFromReference(&convolution);
        status = VX_SUCCESS;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxWriteConvolutionCoefficients(vx_convolution convolution, const vx_int16 *array)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;
    if ((Reference::isValidReference(convolution, VX_TYPE_CONVOLUTION) == vx_true_e) &&
        (ownAllocateMemory(convolution->context, &convolution->memory) == vx_true_e))
    {
        ownSemWait(&convolution->lock);
        if (array)
        {
            vx_size size = convolution->memory.strides[0][1] *
                           convolution->memory.dims[0][1];

            memcpy(convolution->memory.ptrs[0], array, size);
        }
        ownSemPost(&convolution->lock);
        // ownWroteToReference(&convolution);
        status = VX_SUCCESS;
    }
    return status;
}

vx_status VX_API_CALL vxCopyConvolutionCoefficients(vx_convolution convolution, void *ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;
    (void)mem_type;

    if (Reference::isValidReference(convolution, VX_TYPE_CONVOLUTION) == vx_true_e)
    {
        if (ownAllocateMemory(convolution->context, &convolution->memory) == vx_true_e)
        {
#ifdef OPENVX_USE_OPENCL_INTEROP
            void * ptr_given = ptr;
            vx_enum mem_type_given = mem_type;
            if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
            {
                // get ptr from OpenCL buffer for HOST
                size_t size = 0;
                cl_mem opencl_buf = (cl_mem)ptr;
                cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
                VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyConvolutionCoefficients: clGetMemObjectInfo(%p) => (%d)\n",
                    opencl_buf, cerr);
                if (cerr != CL_SUCCESS)
                {
                    return VX_ERROR_INVALID_PARAMETERS;
                }
                ptr = clEnqueueMapBuffer(convolution->context->opencl_command_queue,
                    opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
                    0, nullptr, nullptr, &cerr);
                VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyConvolutionCoefficients: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                    opencl_buf, (int)size, ptr, cerr);
                if (cerr != CL_SUCCESS)
                {
                    return VX_ERROR_INVALID_PARAMETERS;
                }
                mem_type = VX_MEMORY_TYPE_HOST;
            }
#endif

            if (usage == VX_READ_ONLY)
            {
                ownSemWait(&convolution->lock);
                if (ptr)
                {
                    vx_size size = convolution->memory.strides[0][1] *
                                   convolution->memory.dims[0][1];
                    memcpy(ptr, convolution->memory.ptrs[0], size);
                }
                ownSemPost(&convolution->lock);
                // ownReadFromReference(&convolution);
                status = VX_SUCCESS;
            }
            else if (usage == VX_WRITE_ONLY)
            {
                ownSemWait(&convolution->lock);
                if (ptr)
                {
                    vx_size size = convolution->memory.strides[0][1] *
                                   convolution->memory.dims[0][1];

                    memcpy(convolution->memory.ptrs[0], ptr, size);
                }
                ownSemPost(&convolution->lock);
                // ownWroteToReference(&convolution);
                status = VX_SUCCESS;
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Wrong parameters for convolution\n");
                status = VX_ERROR_INVALID_PARAMETERS;
            }

#ifdef OPENVX_USE_OPENCL_INTEROP
            if (mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
            {
                clEnqueueUnmapMemObject(convolution->context->opencl_command_queue,
                    (cl_mem)ptr_given, ptr, 0, nullptr, nullptr);
                clFinish(convolution->context->opencl_command_queue);
            }
#endif
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to allocate convolution\n");
            status = VX_ERROR_NO_MEMORY;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid reference for convolution\n");
    }
    return status;
}

VX_API_ENTRY vx_convolution VX_API_CALL vxCreateVirtualConvolution(vx_graph graph, vx_size columns, vx_size rows)
{
    vx_convolution convolution = nullptr;

    if (Reference::isValidReference(graph, VX_TYPE_GRAPH) == vx_true_e)
    {
        convolution = vxCreateConvolution(graph->context, columns, rows);
        if (vxGetStatus((vx_reference)convolution) == VX_SUCCESS && convolution->type == VX_TYPE_CONVOLUTION)
        {
            convolution->scope = (vx_reference)graph;
            convolution->is_virtual = vx_true_e;
        }
    }
    /* else, the graph is invalid, we can't get any context and then error object */
    return convolution;
}
