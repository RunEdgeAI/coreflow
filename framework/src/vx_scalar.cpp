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
#include "vx_scalar.h"

/*****************************************************************************/
/* INTERNAL INTERFACE                                                        */
/*****************************************************************************/
Scalar::Scalar(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_SCALAR, scope),
data_type(VX_TYPE_INVALID),
data(),
data_addr(nullptr),
data_len(0)
{

}

Scalar::~Scalar()
{
}

void* Scalar::allocateScalarMemory(vx_size size)
{
    if (data_addr == nullptr)
    {
        data_addr = new vx_char[size]();
    }

    return data_addr;
}

vx_status Scalar::scalarToHostMem(vx_scalar scalar, void* user_ptr)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Osal::semWait(&scalar->lock))
        return VX_ERROR_NO_RESOURCES;

    Scalar::printScalarValue(scalar);

    switch (scalar->data_type)
    {
    case VX_TYPE_CHAR:     *(vx_char*)user_ptr = scalar->data.chr; break;
    case VX_TYPE_INT8:     *(vx_int8*)user_ptr = scalar->data.s08; break;
    case VX_TYPE_UINT8:    *(vx_uint8*)user_ptr = scalar->data.u08; break;
    case VX_TYPE_INT16:    *(vx_int16*)user_ptr = scalar->data.s16; break;
    case VX_TYPE_UINT16:   *(vx_uint16*)user_ptr = scalar->data.u16; break;
    case VX_TYPE_INT32:    *(vx_int32*)user_ptr = scalar->data.s32; break;
    case VX_TYPE_UINT32:   *(vx_uint32*)user_ptr = scalar->data.u32; break;
    case VX_TYPE_INT64:    *(vx_int64*)user_ptr = scalar->data.s64; break;
    case VX_TYPE_UINT64:   *(vx_uint64*)user_ptr = scalar->data.u64; break;
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
    case VX_TYPE_FLOAT16:  *(vx_float16*)user_ptr = scalar->data.f16; break;
#endif
    case VX_TYPE_FLOAT32:  *(vx_float32*)user_ptr = scalar->data.f32; break;
    case VX_TYPE_FLOAT64:  *(vx_float64*)user_ptr = scalar->data.f64; break;
    case VX_TYPE_DF_IMAGE: *(vx_df_image*)user_ptr = scalar->data.fcc; break;
    case VX_TYPE_ENUM:     *(vx_enum*)user_ptr = scalar->data.enm; break;
    case VX_TYPE_SIZE:     *(vx_size*)user_ptr = scalar->data.size; break;
    case VX_TYPE_BOOL:     *(vx_bool*)user_ptr = scalar->data.boolean; break;

    default:
        VX_PRINT(VX_ZONE_ERROR, "some case is not covered in %s\n", __FUNCTION__);
        status = VX_ERROR_NOT_SUPPORTED;
        break;
    }

    if (vx_false_e == Osal::semPost(&scalar->lock))
        return VX_ERROR_NO_RESOURCES;

    // ownReadFromReference(&scalar);

    return status;
}

vx_status Scalar::hostMemToScalar(vx_scalar scalar, void* user_ptr)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Osal::semWait(&scalar->lock))
        return VX_ERROR_NO_RESOURCES;

    switch (scalar->data_type)
    {
    case VX_TYPE_CHAR:     scalar->data.chr = *(vx_char*)user_ptr; break;
    case VX_TYPE_INT8:     scalar->data.s08 = *(vx_int8*)user_ptr; break;
    case VX_TYPE_UINT8:    scalar->data.u08 = *(vx_uint8*)user_ptr; break;
    case VX_TYPE_INT16:    scalar->data.s16 = *(vx_int16*)user_ptr; break;
    case VX_TYPE_UINT16:   scalar->data.u16 = *(vx_uint16*)user_ptr; break;
    case VX_TYPE_INT32:    scalar->data.s32 = *(vx_int32*)user_ptr; break;
    case VX_TYPE_UINT32:   scalar->data.u32 = *(vx_uint32*)user_ptr; break;
    case VX_TYPE_INT64:    scalar->data.s64 = *(vx_int64*)user_ptr; break;
    case VX_TYPE_UINT64:   scalar->data.u64 = *(vx_uint64*)user_ptr; break;
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
    case VX_TYPE_FLOAT16:  scalar->data.f16 = *(vx_float16*)user_ptr; break;
#endif
    case VX_TYPE_FLOAT32:  scalar->data.f32 = *(vx_float32*)user_ptr; break;
    case VX_TYPE_FLOAT64:  scalar->data.f64 = *(vx_float64*)user_ptr; break;
    case VX_TYPE_DF_IMAGE: scalar->data.fcc = *(vx_df_image*)user_ptr; break;
    case VX_TYPE_ENUM:     scalar->data.enm = *(vx_enum*)user_ptr; break;
    case VX_TYPE_SIZE:     scalar->data.size = *(vx_size*)user_ptr; break;
    case VX_TYPE_BOOL:     scalar->data.boolean = *(vx_bool*)user_ptr; break;

    default:
        VX_PRINT(VX_ZONE_ERROR, "some case is not covered in %s\n", __FUNCTION__);
        status = VX_ERROR_NOT_SUPPORTED;
        break;
    }

    Scalar::printScalarValue(scalar);

    if (vx_false_e == Osal::semPost(&scalar->lock))
        return VX_ERROR_NO_RESOURCES;

    // ownWroteToReference(&scalar);

    return status;
}

vx_status Scalar::copy(void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_SUCCESS;

    if (nullptr == user_ptr || VX_MEMORY_TYPE_HOST != user_mem_type)
        return VX_ERROR_INVALID_PARAMETERS;

#ifdef OPENVX_USE_OPENCL_INTEROP
    void *user_ptr_given = user_ptr;
    vx_enum user_mem_type_given = user_mem_type;
    if (user_mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        // get ptr from OpenCL buffer for HOST
        size_t size = 0;
        cl_mem opencl_buf = (cl_mem)user_ptr;
        cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyScalar: clGetMemObjectInfo(%p) => (%d)\n",
                 opencl_buf, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        user_ptr =
            clEnqueueMapBuffer(context->opencl_command_queue, opencl_buf, CL_TRUE,
                               CL_MAP_READ | CL_MAP_WRITE, 0, size, 0, nullptr, nullptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyScalar: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                 opencl_buf, (int)size, user_ptr, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        user_mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    switch (usage)
    {
        case VX_READ_ONLY:
            status = Scalar::scalarToHostMem(this, user_ptr);
            break;
        case VX_WRITE_ONLY:
            status = Scalar::hostMemToScalar(this, user_ptr);
            break;

        default:
            status = VX_ERROR_INVALID_PARAMETERS;
            break;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (user_mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)user_ptr_given, user_ptr, 0,
                                nullptr, nullptr);
        clFinish(context->opencl_command_queue);
    }
#endif

    return status;
}

vx_status Scalar::copy(vx_size size, void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_SUCCESS;
    vx_size min_size = 0;

    if (nullptr == user_ptr || VX_MEMORY_TYPE_HOST != user_mem_type)
        return VX_ERROR_INVALID_PARAMETERS;

    switch (usage)
    {
        case VX_READ_ONLY:
            if (data_addr != nullptr && data_len != 0)
            {
                min_size = data_len > size ? size : data_len;
                memcpy(user_ptr, data_addr, min_size);
            }
            else
            {
                status = VX_ERROR_NO_RESOURCES;
            }
            break;
        case VX_WRITE_ONLY:
            if (data_addr == nullptr)
            {
                if (nullptr == allocateScalarMemory(size))
                {
                    status = VX_ERROR_NO_MEMORY;
                }
                else
                {
                    data_len = size;
                    memcpy(data_addr, user_ptr, size);
                }
            }
            else
            {
                if (data_len < size)
                {
                    void *tmp_addr = data_addr;
                    data_addr = nullptr;
                    if (nullptr == allocateScalarMemory(size))
                    {
                        data_addr = tmp_addr;
                        status = VX_ERROR_NO_MEMORY;
                    }
                    else
                    {
                        free(tmp_addr);
                        data_len = size;
                        memcpy(data_addr, user_ptr, size);
                    }
                }
                else
                {
                    data_len = size;
                    memcpy(data_addr, user_ptr, size);
                }
            }
            break;

        default:
            status = VX_ERROR_INVALID_PARAMETERS;
            break;
    }

    return status;
}

vx_status Scalar::readValue(void* ptr)
{
    vx_status status = VX_SUCCESS;

    if (ptr == nullptr) return VX_ERROR_INVALID_PARAMETERS;

    Osal::semWait(&lock);
    Scalar::printScalarValue(this);
    switch (data_type)
    {
        case VX_TYPE_CHAR:
            *(vx_char *)ptr = data.chr;
            break;
        case VX_TYPE_INT8:
            *(vx_int8 *)ptr = data.s08;
            break;
        case VX_TYPE_UINT8:
            *(vx_uint8 *)ptr = data.u08;
            break;
        case VX_TYPE_INT16:
            *(vx_int16 *)ptr = data.s16;
            break;
        case VX_TYPE_UINT16:
            *(vx_uint16 *)ptr = data.u16;
            break;
        case VX_TYPE_INT32:
            *(vx_int32 *)ptr = data.s32;
            break;
        case VX_TYPE_UINT32:
            *(vx_uint32 *)ptr = data.u32;
            break;
        case VX_TYPE_INT64:
            *(vx_int64 *)ptr = data.s64;
            break;
        case VX_TYPE_UINT64:
            *(vx_uint64 *)ptr = data.u64;
            break;
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
        case VX_TYPE_FLOAT16:
            *(vx_float16 *)ptr = data.f16;
            break;
#endif
        case VX_TYPE_FLOAT32:
            *(vx_float32 *)ptr = data.f32;
            break;
        case VX_TYPE_FLOAT64:
            *(vx_float64 *)ptr = data.f64;
            break;
        case VX_TYPE_DF_IMAGE:
            *(vx_df_image *)ptr = data.fcc;
            break;
        case VX_TYPE_ENUM:
            *(vx_enum *)ptr = data.enm;
            break;
        case VX_TYPE_SIZE:
            *(vx_size *)ptr = data.size;
            break;
        case VX_TYPE_BOOL:
            *(vx_bool *)ptr = data.boolean;
            break;
        default:
            VX_PRINT(VX_ZONE_ERROR, "some case is not covered in %s\n", __FUNCTION__);
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }
    Osal::semPost(&lock);
    // ownReadFromReference(&base);

    return status;
}

vx_status Scalar::writeValue(const void *ptr)
{
    vx_status status = VX_SUCCESS;
    if (ptr == nullptr) return VX_ERROR_INVALID_PARAMETERS;

    Osal::semWait(&lock);
    switch (data_type)
    {
        case VX_TYPE_CHAR:
            data.chr = *(vx_char *)ptr;
            break;
        case VX_TYPE_INT8:
            data.s08 = *(vx_int8 *)ptr;
            break;
        case VX_TYPE_UINT8:
            data.u08 = *(vx_uint8 *)ptr;
            break;
        case VX_TYPE_INT16:
            data.s16 = *(vx_int16 *)ptr;
            break;
        case VX_TYPE_UINT16:
            data.u16 = *(vx_uint16 *)ptr;
            break;
        case VX_TYPE_INT32:
            data.s32 = *(vx_int32 *)ptr;
            break;
        case VX_TYPE_UINT32:
            data.u32 = *(vx_uint32 *)ptr;
            break;
        case VX_TYPE_INT64:
            data.s64 = *(vx_int64 *)ptr;
            break;
        case VX_TYPE_UINT64:
            data.u64 = *(vx_uint64 *)ptr;
            break;
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
        case VX_TYPE_FLOAT16:
            data.f16 = *(vx_float16 *)ptr;
            break;
#endif
        case VX_TYPE_FLOAT32:
            data.f32 = *(vx_float32 *)ptr;
            break;
        case VX_TYPE_FLOAT64:
            data.f64 = *(vx_float64 *)ptr;
            break;
        case VX_TYPE_DF_IMAGE:
            data.fcc = *(vx_df_image *)ptr;
            break;
        case VX_TYPE_ENUM:
            data.enm = *(vx_enum *)ptr;
            break;
        case VX_TYPE_SIZE:
            data.size = *(vx_size *)ptr;
            break;
        case VX_TYPE_BOOL:
            data.boolean = *(vx_bool *)ptr;
            break;
        default:
            VX_PRINT(VX_ZONE_ERROR, "some case is not covered in %s\n", __FUNCTION__);
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }
    Scalar::printScalarValue(this);
    Osal::semPost(&lock);
    // ownWroteToReference(&base);

    return status;
}

vx_enum Scalar::dataType() const
{
    return data_type;
}

void Scalar::printScalarValue(vx_scalar scalar)
{
    switch (scalar->data_type)
    {
        case VX_TYPE_CHAR:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %c\n", scalar, scalar->data.chr);
            break;
        case VX_TYPE_INT8:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %d\n", scalar, scalar->data.s08);
            break;
        case VX_TYPE_UINT8:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %u\n", scalar, scalar->data.u08);
            break;
        case VX_TYPE_INT16:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %hd\n", scalar, scalar->data.s16);
            break;
        case VX_TYPE_UINT16:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %hu\n", scalar, scalar->data.u16);
            break;
        case VX_TYPE_INT32:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %d\n", scalar, scalar->data.s32);
            break;
        case VX_TYPE_UINT32:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %u\n", scalar, scalar->data.u32);
            break;
        case VX_TYPE_INT64:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %ld\n", scalar, scalar->data.s64);
            break;
        case VX_TYPE_UINT64:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %lu\n", scalar, scalar->data.u64);
            break;
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
        case VX_TYPE_FLOAT16:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %f\n", scalar, scalar->data.f16);
            break;
#endif
        case VX_TYPE_FLOAT32:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %lf\n", scalar, scalar->data.f32);
            break;
        case VX_TYPE_FLOAT64:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %lf\n", scalar, scalar->data.f64);
            break;
        case VX_TYPE_DF_IMAGE:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %08x\n", scalar, scalar->data.fcc);
            break;
        case VX_TYPE_ENUM:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %d\n", scalar, scalar->data.enm);
            break;
        case VX_TYPE_SIZE:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %zu\n", scalar, scalar->data.size);
            break;
        case VX_TYPE_BOOL:
            VX_PRINT(VX_ZONE_SCALAR, "scalar " VX_FMT_REF " = %s\n", scalar, (scalar->data.boolean == vx_true_e?"TRUE":"FALSE"));
            break;
        default:
            VX_PRINT(VX_ZONE_ERROR, "Case %08x is not covered!\n", scalar->data_type);
            DEBUG_BREAK();
            break;
    }

    return;
} /* printScalarValue() */

void Scalar::destruct()
{
    if (data_addr)
    {
        ::operator delete(data_addr);
        data_addr = nullptr;
        data_len = 0;
    }
}

/******************************************************************************/
/* PUBLIC API                                                                 */
/******************************************************************************/

VX_API_ENTRY vx_scalar VX_API_CALL vxCreateScalar(vx_context context, vx_enum data_type, const void* ptr)
{
    vx_scalar scalar = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
        return nullptr;

    if (!VX_TYPE_IS_SCALAR(data_type))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid type to scalar\n");
        vxAddLogEntry(reinterpret_cast<vx_reference>(context), VX_ERROR_INVALID_TYPE, "Invalid type to scalar\n");
        scalar = (vx_scalar)vxGetErrorObject(context, VX_ERROR_INVALID_TYPE);
    }
    else
    {
        scalar = (vx_scalar)Reference::createReference(context, VX_TYPE_SCALAR, VX_EXTERNAL, context);
        if (vxGetStatus((vx_reference)scalar) == VX_SUCCESS && scalar->type == VX_TYPE_SCALAR)
        {
            scalar->data_type = data_type;
            vxCopyScalar(scalar, (void*)ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        }
    }

    return (vx_scalar)scalar;
} /* vxCreateScalar() */

VX_API_ENTRY vx_scalar VX_API_CALL vxCreateScalarWithSize(vx_context context, vx_enum data_type, const void *ptr, vx_size size)
{
    vx_scalar scalar = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
        return nullptr;

    if (!VX_TYPE_IS_SCALAR_WITH_SIZE(data_type))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid type to scalar\n");
        vxAddLogEntry(reinterpret_cast<vx_reference>(context), VX_ERROR_INVALID_TYPE, "Invalid type to scalar\n");
        scalar = (vx_scalar)vxGetErrorObject(context, VX_ERROR_INVALID_TYPE);
    }
    else
    {
        scalar = (vx_scalar)Reference::createReference(context, VX_TYPE_SCALAR, VX_EXTERNAL, context);
        if (vxGetStatus((vx_reference)scalar) == VX_SUCCESS && scalar->type == VX_TYPE_SCALAR)
        {
            scalar->data_type = data_type;
            vxCopyScalarWithSize(scalar, size, (void*)ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        }
    }

    return (vx_scalar)scalar;
}

VX_API_ENTRY vx_scalar VX_API_CALL vxCreateVirtualScalar(vx_graph graph, vx_enum data_type)
{
    vx_scalar scalar = nullptr;
    vx_reference ref = (vx_reference)graph;

    if (Reference::isValidReference(ref, VX_TYPE_GRAPH) != vx_true_e)
        return nullptr;

    if (!VX_TYPE_IS_SCALAR_WITH_SIZE(data_type))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid type to scalar\n");
        vxAddLogEntry(ref, VX_ERROR_INVALID_TYPE, "Invalid type to scalar\n");
        scalar = (vx_scalar)vxGetErrorObject(ref->context, VX_ERROR_INVALID_TYPE);
    }
    else
    {
        scalar = (vx_scalar)Reference::createReference(ref->context, VX_TYPE_SCALAR, VX_EXTERNAL, ref->context);
        if (vxGetStatus((vx_reference)scalar) == VX_SUCCESS && scalar->type == VX_TYPE_SCALAR)
        {
            scalar->is_virtual = vx_true_e;
            scalar->scope = (vx_reference)graph;
            scalar->data_type = data_type;
        }
    }

    return (vx_scalar)scalar;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryScalar(vx_scalar scalar, vx_enum attribute, void* ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(scalar), VX_TYPE_SCALAR) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    switch (attribute)
    {
        case VX_SCALAR_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
            {
                *(vx_enum*)ptr = scalar->dataType();
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
} /* vxQueryScalar() */

VX_API_ENTRY vx_status VX_API_CALL vxCopyScalar(vx_scalar scalar, void* user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    if (vx_false_e == Reference::isValidReference(reinterpret_cast<vx_reference>(scalar), VX_TYPE_SCALAR))
        return VX_ERROR_INVALID_REFERENCE;

    return scalar->copy(user_ptr, usage, user_mem_type);;
} /* vxCopyScalar() */

VX_API_ENTRY vx_status VX_API_CALL vxCopyScalarWithSize(vx_scalar scalar, vx_size size, void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    if (vx_false_e == Reference::isValidReference(reinterpret_cast<vx_reference>(scalar), VX_TYPE_SCALAR))
        return VX_ERROR_INVALID_REFERENCE;

    return scalar->copy(size, user_ptr, usage, user_mem_type);
}

VX_API_ENTRY vx_status VX_API_CALL vxReadScalarValue(vx_scalar scalar, void *ptr)
{
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(scalar), VX_TYPE_SCALAR) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    return scalar->readValue(ptr);;
}

VX_API_ENTRY vx_status VX_API_CALL vxWriteScalarValue(vx_scalar scalar, const void *ptr)
{
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(scalar), VX_TYPE_SCALAR) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    return scalar->writeValue(ptr);;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseScalar(vx_scalar *s)
{
    vx_status status = VX_FAILURE;

    if (nullptr != s)
    {
        vx_scalar scalar = *s;
        if (vx_true_e == Reference::isValidReference(scalar, VX_TYPE_SCALAR))
        {
            status = Reference::releaseReference((vx_reference *)s, VX_TYPE_SCALAR, VX_EXTERNAL,
                                                 nullptr);
        }
    }

    return status;
} /* vxReleaseScalar() */