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

/*
*
* Copyright (c) 2018 Texas Instruments Incorporated
*
*/
#include <cstring>

#include "vx_internal.h"

#if defined(OPENVX_USE_USER_DATA_OBJECT)

/*****************************************************************************/
/* INTERNAL INTERFACE                                                        */
/*****************************************************************************/
UserDataObject::UserDataObject(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_USER_DATA_OBJECT, scope),
memory(),
size(0),
type_name()
{
}

UserDataObject::~UserDataObject()
{
    ownFreeMemory(context, &memory);
}

vx_bool UserDataObject::allocateUserDataObject()
{
    vx_bool res = vx_false_e;
    if (size > 0)
    {
        res = ownAllocateMemory(context, &memory);
    }
    return res;
}

/*****************************************************************************/
/* PUBLIC INTERFACE                                                          */
/*****************************************************************************/
VX_API_ENTRY vx_user_data_object VX_API_CALL vxCreateUserDataObject(
    vx_context context,
    const vx_char *type_name,
    vx_size size,
    const void *ptr)
{
    vx_user_data_object user_data_object = nullptr;
    vx_status status = (vx_status)VX_SUCCESS;

    if(Context::isValidContext(context) == (vx_bool)vx_true_e)
    {
        if (size < 1U)
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid size for the user data object.\n");
            user_data_object = (vx_user_data_object)vxGetErrorObject((vx_context)context, (vx_status)VX_ERROR_INVALID_PARAMETERS);
        }

        if( nullptr == user_data_object )
        {
            user_data_object = (vx_user_data_object)Reference::createReference(context, VX_TYPE_USER_DATA_OBJECT, VX_EXTERNAL, context);

            if ((vxGetStatus((vx_reference)user_data_object) == (vx_status)VX_SUCCESS) &&
                (user_data_object->type == VX_TYPE_USER_DATA_OBJECT))
            {
                user_data_object->size = size;

                /* Initialize string with zeros, which safely fills with nullptr terminators */
                user_data_object->type_name[0] = (char)0;

                if (type_name != nullptr)
                {
                    strncpy(user_data_object->type_name, type_name, VX_MAX_REFERENCE_NAME);
                }

                user_data_object->memory.nptrs = 1;
                user_data_object->memory.ndims = 1;
                user_data_object->memory.dims[0][0] = (vx_uint32)size;

                if (nullptr != ptr)
                {
                    status = vxCopyUserDataObject(user_data_object, 0, size, (void*)ptr, (vx_enum)VX_WRITE_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST);
                }
                else
                {
                    status = VX_BOOL_TO_STATUS(user_data_object->allocateUserDataObject());

                    if (status == (vx_status)VX_SUCCESS)
                    {
                        vx_uint8 *start_ptr = (vx_uint8 *)&user_data_object->memory.ptrs[0][0];
                        memset(start_ptr, 0, size);
                    }
                }
            }

            if(status != (vx_status)VX_SUCCESS)
            {
                user_data_object = nullptr;
                VX_PRINT(VX_ZONE_ERROR, "Failed to create user data object with error: %d\n", status);
                user_data_object = (vx_user_data_object)vxGetErrorObject(
                    context, (vx_status)VX_ERROR_INVALID_PARAMETERS);
            }
        }
    }

    return (user_data_object);
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseUserDataObject(vx_user_data_object *user_data_object)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != user_data_object)
    {
        vx_user_data_object obj = *user_data_object;
        if (vx_true_e == Reference::isValidReference(obj, VX_TYPE_USER_DATA_OBJECT))
        {
            status = Reference::releaseReference((vx_reference*)user_data_object, VX_TYPE_USER_DATA_OBJECT, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryUserDataObject (
    vx_user_data_object user_data_object, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(user_data_object), VX_TYPE_USER_DATA_OBJECT) == (vx_bool)vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR,"vxQueryUserDataObject failed\n");
        VX_PRINT(VX_ZONE_ERROR,"Reference is invalid or object descriptor is nullptr\n");
        status = (vx_status)VX_ERROR_INVALID_REFERENCE;
    }
    else
    {
        switch (attribute)
        {
            case (vx_enum)VX_USER_DATA_OBJECT_NAME:
                if ((ptr != nullptr) && ((vx_enum)size >= VX_MAX_REFERENCE_NAME))
                {
                    strncpy(reinterpret_cast<char*>(ptr), user_data_object->type_name, VX_MAX_REFERENCE_NAME);
                }
                else
                {
                    VX_PRINT(VX_ZONE_ERROR, "VX_USER_DATA_OBJECT_NAME failed\n");
                    status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case (vx_enum)VX_USER_DATA_OBJECT_SIZE:
                if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3U))
                {
                    *(vx_size *)ptr = user_data_object->size;
                }
                else
                {
                    VX_PRINT(VX_ZONE_ERROR, "VX_USER_DATA_OBJECT_SIZE failed\n");
                    status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            default:
                VX_PRINT(VX_ZONE_ERROR,"query user data object option not supported\n");
                status = (vx_status)VX_ERROR_NOT_SUPPORTED;
                break;
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyUserDataObject(vx_user_data_object user_data_object,
        vx_size offset, vx_size size, void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = (vx_status)VX_SUCCESS;
    vx_uint8 *start_ptr;

    if ((Reference::isValidReference(reinterpret_cast<vx_reference>(user_data_object), VX_TYPE_USER_DATA_OBJECT) == (vx_bool)vx_false_e))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid user data object reference\n");
        status = (vx_status)VX_ERROR_INVALID_REFERENCE;
    }
    else
    {
        if ((vx_enum)VX_MEMORY_TYPE_HOST != user_mem_type)
        {
            VX_PRINT(VX_ZONE_ERROR, "User mem type is not equal to VX_MEMORY_TYPE_HOST\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }

        /* Memory still not allocated */
        if (((vx_enum)VX_READ_ONLY == usage) &&
            ((uint64_t)(uintptr_t)nullptr == (uintptr_t)user_data_object->memory.ptrs[0]))
        {
            VX_PRINT(VX_ZONE_ERROR, "Memory is not allocated\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }

        if (nullptr == user_ptr)
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid nullptr pointer\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }

        if ((size < 1U) || ((offset + size) > user_data_object->size))
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid offset or size parameter\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if ((vx_status)VX_SUCCESS == status)
    {
        if (user_data_object->allocateUserDataObject() == vx_false_e)
        {
            return VX_ERROR_NO_MEMORY;
        }
    }

    if (status == (vx_status)VX_SUCCESS)
    {
        /* Get the offset to the free memory */
        start_ptr = (vx_uint8 *)&user_data_object->memory.ptrs[0][offset];

        /* Copy from internal object to user memory */
        if ((vx_enum)VX_READ_ONLY == usage)
        {
            memcpy(user_ptr, start_ptr, size);
        }
        else /* Copy from user memory to internal object */
        {
            memcpy(start_ptr, user_ptr, size);
        }
    }

    return (status);
}

VX_API_ENTRY vx_status VX_API_CALL vxMapUserDataObject(
    vx_user_data_object user_data_object,
    vx_size offset,
    vx_size size,
    vx_map_id *map_id,
    void **ptr,
    vx_enum usage,
    vx_enum mem_type,
    vx_uint32 flags)
{
    vx_status status = (vx_status)VX_SUCCESS;
    if ((Reference::isValidReference(reinterpret_cast<vx_reference>(user_data_object), VX_TYPE_USER_DATA_OBJECT) == (vx_bool)vx_false_e))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid user data object reference\n");
        status = (vx_status)VX_ERROR_INVALID_REFERENCE;
    }

    if(status == (vx_status)VX_SUCCESS)
    {
        if (ptr == nullptr)
        {
            VX_PRINT(VX_ZONE_ERROR, "User pointer is nullptr\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }
        if (map_id == nullptr)
        {
            VX_PRINT(VX_ZONE_ERROR, "Map ID is nullptr\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if(status == (vx_status)VX_SUCCESS)
    {

        if (((offset + size) > user_data_object->size))
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid offset or size parameter\n");
            status = (vx_status)VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if ((vx_status)VX_SUCCESS == status)
    {
        if (user_data_object->allocateUserDataObject() == vx_false_e)
        {
            status = VX_ERROR_NO_MEMORY;
        }
    }

    if ((vx_status)VX_SUCCESS == status)
    {
        vx_memory_map_extra extra;
        extra.array_data.start = offset;
        extra.array_data.end = offset + size;
        vx_uint8 *buf = nullptr;
        if (user_data_object->context->memoryMap((vx_reference)user_data_object, size, usage, mem_type, flags, &extra, (void **)&buf, map_id) == vx_true_e)
        {
            if (VX_READ_ONLY == usage || VX_READ_AND_WRITE == usage)
            {
                if (Osal::semWait(&user_data_object->memory.locks[0]) == vx_true_e)
                {
                    vx_uint8 *pSrc = (vx_uint8 *)&user_data_object->memory.ptrs[0][offset];
                    vx_uint8 *pDst = (vx_uint8 *)buf;
                    memcpy(pDst, pSrc, size);

                    *ptr = buf;
                    user_data_object->incrementReference(VX_EXTERNAL);
                    Osal::semPost(&user_data_object->memory.locks[0]);

                    status = VX_SUCCESS;
                }
                else
                {
                    status = VX_ERROR_NO_RESOURCES;
                }
            }
            else
            {
                /* write only mode */
                *ptr = buf;
                user_data_object->incrementReference(VX_EXTERNAL);
                status = VX_SUCCESS;
            }
        }
        else
        {
            status = VX_FAILURE;
        }
    }

    return (status);
}

VX_API_ENTRY vx_status VX_API_CALL vxUnmapUserDataObject(vx_user_data_object user_data_object, vx_map_id map_id)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if ((Reference::isValidReference(reinterpret_cast<vx_reference>(user_data_object), VX_TYPE_USER_DATA_OBJECT) == (vx_bool)vx_false_e))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid user data object reference\n");
        status = (vx_status)VX_ERROR_INVALID_REFERENCE;
    }

    if(status == (vx_status)VX_SUCCESS)
    {
        vx_context context = user_data_object->context;
        vx_memory_map_t* map = &context->memory_maps[map_id];
        if (map->used && map->ref == (vx_reference)user_data_object)
        {
            vx_size start = map->extra.array_data.start;
            vx_size end = map->extra.array_data.end;
            if (VX_WRITE_ONLY == map->usage || VX_READ_AND_WRITE == map->usage)
            {
                if (Osal::semWait(&user_data_object->memory.locks[0]) == vx_true_e)
                {
                    vx_uint32 offset = (vx_uint32)start;
                    vx_uint8 *pSrc = (vx_uint8 *)map->ptr;
                    vx_uint8 *pDst = (vx_uint8 *)&user_data_object->memory.ptrs[0][offset];
                    vx_size size = (end - start);
                    memcpy(pDst, pSrc, size);

                    context->memoryUnmap((vx_uint32)map_id);
                    user_data_object->decrementReference(VX_EXTERNAL);
                    Osal::semPost(&user_data_object->memory.locks[0]);
                    status = VX_SUCCESS;
                }
                else
                {
                    // Log the error if semaphore wait fails
                    VX_PRINT(VX_ZONE_ERROR, "Failed to acquire semaphore lock\n");
                    status = VX_ERROR_NO_RESOURCES;
                }
            }
            else
            {
                /* read only mode */
                user_data_object->context->memoryUnmap((vx_uint32)map_id);
                user_data_object->decrementReference(VX_EXTERNAL);
                status = VX_SUCCESS;
            }
        }
        else
        {
            status = VX_FAILURE;
        }
    }

    return status;
}

#endif /* defined(OPENVX_USE_USER_DATA_OBJECT) */
