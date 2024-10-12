/*

 * Copyright (c) 2011-2017 The Khronos Group Inc.
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
#include "vx_context.h"

/*==============================================================================
 INTERNAL INTERFACE
 =============================================================================*/

static vx_size vxArrayItemSize(vx_context context, vx_enum item_type)
{
    vx_size res = Reference::sizeOfType(item_type);
    vx_uint32 i = 0;
    if (res == 0ul)
    {
        for (i = 0; i < VX_INT_MAX_USER_STRUCTS; ++i)
        {
            if (context->user_structs[i].type == item_type)
            {
                res = context->user_structs[i].size;
                break;
            }
        }
    }
    return res;
}

static vx_bool vxIsValidArrayItemType(vx_context context, vx_enum item_type)
{
    vx_bool res = vx_false_e;

    if (vxArrayItemSize(context, item_type) != 0)
    {
        res = vx_true_e;
    }

    return res;
}

static vx_bool vxIsValidArray(vx_array arr)
{
    vx_bool res = vx_false_e;

    if (arr != nullptr)
        res = Reference::isValidReference(reinterpret_cast<vx_reference>(arr), VX_TYPE_ARRAY);

    if (res == vx_true_e)
        res = vxIsValidArrayItemType(arr->context, arr->item_type);

    return res;
}

static void vxInitArrayMemory(vx_array arr)
{
    arr->memory.nptrs = 1;
    arr->memory.ndims = 2;

    arr->memory.dims[0][0] = (vx_uint32)arr->item_size;
    arr->memory.dims[0][1] = (vx_uint32)arr->capacity;
}

void vxPrintArray(vx_array array)
{
    VX_PRINT(VX_ZONE_INFO, "Array:%p has %zu elements of %04x type of %zu size each.\n", array, array->capacity, array->item_type, array->item_size);
}

/*==============================================================================
 PRIVATE INTERFACE
 =============================================================================*/
Array::Array(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_ARRAY, scope)
{

}

Array::Array(vx_context context, vx_enum type, vx_reference scope) : Reference(context, type, scope)
{

}

Array::~Array()
{
    destructArray();
}

vx_array Array::createArray(vx_context context, vx_enum item_type, vx_size capacity, vx_bool is_virtual, vx_enum type)
{
    vx_array arr = (vx_array)Reference::createReference(context, type, VX_EXTERNAL, context);
    if (vxGetStatus((vx_reference)arr) == VX_SUCCESS && arr->type == type)
    {
        arr->item_type = item_type;
        arr->item_size = vxArrayItemSize(context, item_type);
        arr->capacity = capacity;
        arr->is_virtual = is_virtual;
        vxInitArrayMemory(arr);
    }
    return arr;
}

void Array::destructArray()
{
    ownFreeMemory(context, &memory);
}

vx_bool Array::initVirtualArray(vx_enum item_type, vx_size capacity)
{
    vx_bool res = vx_false_e;
    if ((vxIsValidArrayItemType(context, item_type) == vx_true_e) &&
        ((this->capacity > 0 || capacity > 0) && (capacity <= this->capacity || this->capacity == 0)))
    {
        if ((this->item_type == VX_TYPE_INVALID) || (this->item_type == item_type))
        {
            this->item_type = item_type;
            item_size = vxArrayItemSize(context, item_type);

            if (this->capacity == 0)
                this->capacity = capacity;

            vxInitArrayMemory(this);
            res = vx_true_e;
        }
    }
    return res;
}

vx_bool Array::validateArray(vx_enum item_type, vx_size capacity)
{
    vx_bool res = vx_false_e;
    if ((vxIsValidArrayItemType(context, item_type)) &&
        (this->item_type == item_type))
    {
        /* if the required capacity is > 0 and the objects capacity is not sufficient */
        if ((capacity > 0) && (capacity > this->capacity))
            res = vx_false_e;
        else
            res = vx_true_e;
    }
    return res;
}

vx_bool Array::allocateArray()
{
    vx_bool res = vx_false_e;
    if (capacity > 0)
    {
        res = ownAllocateMemory(context, &memory);
    }
    return res;
}

vx_status Array::accessArrayRange(vx_size start, vx_size end, vx_size *pStride, void **ptr, vx_enum usage)
{
    vx_status status = VX_FAILURE;

    /* bad parameters */
    if ((usage < VX_READ_ONLY) || (VX_READ_AND_WRITE < usage) ||
        (ptr == nullptr) ||
        (start >= end) || (end > num_items))
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    /* determine if virtual before checking for memory */
    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" array. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual array\n");
            return VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual array, this is ok. */
    }

    /* verify has not run or will not run yet. this allows this API to "touch"
     * the array to create it.
     */
    if (allocateArray() == vx_false_e)
    {
        return VX_ERROR_NO_MEMORY;
    }

    /* POSSIBILITIES:
     * 1.) !*ptr && RO == COPY-ON-READ (make ptr=alloc)
     * 2.) !*ptr && WO == MAP
     * 3.) !*ptr && RW == MAP
     * 4.)  *ptr && RO||RW == COPY (UNLESS MAP)
     */

    /* MAP mode */
    if (*ptr == nullptr)
    {
        if ((usage == VX_WRITE_ONLY) || (usage == VX_READ_AND_WRITE))
        {
            /*-- MAP --*/
            status = VX_ERROR_NO_RESOURCES;

            /* lock the memory */
            if(ownSemWait(&memory.locks[0]) == vx_true_e)
            {
                vx_size offset = start * item_size;

                *ptr = &memory.ptrs[0][offset];

                if (usage != VX_WRITE_ONLY)
                {
                    // ownReadFromReference(&base);
                }
                incrementReference(VX_EXTERNAL);

                status = VX_SUCCESS;
            }
        }
        else
        {
            /*-- COPY-ON-READ --*/
            vx_size size = ((end - start) * item_size);
            vx_uint32 a = 0u;

            vx_size *stride_save = (vx_size*)calloc(1, sizeof(vx_size));
            *stride_save = item_size;

            if (context->addAccessor(size, usage, *ptr, (vx_reference)this, &a, stride_save) == vx_true_e)
            {
                vx_size offset;
                *ptr = context->accessors[a].ptr;

                offset = start * item_size;

                memcpy(*ptr, &memory.ptrs[0][offset], size);

                // ownReadFromReference(&base);
                incrementReference(VX_EXTERNAL);

                status = VX_SUCCESS;
            }
            else
            {
                status = VX_ERROR_NO_MEMORY;
                vxAddLogEntry((vx_reference)this, status, "Failed to allocate memory for COPY-ON-READ! Size=" VX_FMT_SIZE "\n", size);
            }
        }
        if ((status == VX_SUCCESS) && (pStride != nullptr))
        {
            *pStride = item_size;
        }
    }

    /* COPY mode */
    else
    {
        vx_size size = ((end - start) * item_size);
        vx_uint32 a = 0u;

        vx_size *stride_save = (vx_size*)calloc(1, sizeof(vx_size));
        if (pStride == nullptr) {
            *stride_save = item_size;
            pStride = stride_save;
        }
        else {
            *stride_save = *pStride;
        }

        if (context->addAccessor(size, usage, *ptr, this, &a, stride_save) == vx_true_e)
        {
            *ptr = context->accessors[a].ptr;

            status = VX_SUCCESS;

            if ((usage == VX_WRITE_ONLY) || (usage == VX_READ_AND_WRITE))
            {
                if (ownSemWait(&memory.locks[0]) == vx_false_e)
                {
                    status = VX_ERROR_NO_RESOURCES;
                }
            }

            if (status == VX_SUCCESS)
            {
                if (usage != VX_WRITE_ONLY)
                {
                    int i;
                    vx_uint8 *pSrc, *pDest;

                    for (i = (int)start, pDest = (vx_uint8*)*ptr, pSrc = &memory.ptrs[0][start * item_size];
                         i < (int)end;
                         i++, pDest += *pStride, pSrc += item_size)
                    {
                        memcpy(pDest, pSrc, item_size);
                    }

                    // ownReadFromReference(&base);
                }

                incrementReference(VX_EXTERNAL);
            }
        }
        else
        {
            status = VX_ERROR_NO_MEMORY;
            vxAddLogEntry((vx_reference)this, status, "Failed to allocate memory for COPY-ON-READ! Size=" VX_FMT_SIZE "\n", size);
        }
    }

    return status;
}

vx_status Array::commitArrayRange(vx_size start, vx_size end, const void *ptr)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    vx_bool external = vx_true_e; /* assume that it was an allocated buffer */

    if ((ptr == nullptr) ||
        (start > end) || (end > num_items))
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    /* determine if virtual before checking for memory */
    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" array. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual array\n");
            return VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual array, this is ok. */
    }

    /* VARIABLES:
     * 1.) ZERO_AREA
     * 2.) CONSTANT - independant
     * 3.) INTERNAL - independant of area
     * 4.) EXTERNAL - dependant on area (do nothing on zero, determine on non-zero)
     * 5.) !INTERNAL && !EXTERNAL == MAPPED
     */

    {
        /* check to see if the range is zero area */
        vx_bool zero_area = (end == 0) ? vx_true_e : vx_false_e;
        vx_uint32 index = UINT32_MAX; /*  out of bounds, if given to remove, won't do anything */
        vx_bool internal = context->findAccessor(ptr, &index);

        if (zero_area == vx_false_e)
        {
            /* this could be a write-back */
            if (internal == vx_true_e && context->accessors[index].usage == VX_READ_ONLY)
            {
                /* this is a buffer that we allocated on behalf of the user and now they are done. Do nothing else*/
                context->removeAccessor(index);
            }
            else
            {
                vx_uint8 *beg_ptr = memory.ptrs[0];
                vx_uint8 *end_ptr = &beg_ptr[item_size * num_items];

                if ((beg_ptr <= (vx_uint8 *)ptr) && ((vx_uint8 *)ptr < end_ptr))
                {
                    /* the pointer in contained in the array, so it was mapped, thus
                     * there's nothing else to do. */
                    external = vx_false_e;
                }

                if (external == vx_true_e || internal == vx_true_e)
                {
                    /* the pointer was not mapped, copy. */
                    vx_size offset = start * item_size;
                    vx_size len = (end - start) * item_size;

                    if (internal == vx_true_e)
                    {
                        vx_size stride = *(vx_size *)context->accessors[index].extra_data;

                        if (stride == item_size) {
                            memcpy(&beg_ptr[offset], ptr, len);
                        }
                        else {
                            int i;
                            const vx_uint8 *pSrc; vx_uint8 *pDest;

                            for (i = (int)start, pSrc = (const vx_uint8*)ptr, pDest= &beg_ptr[offset];
                                 i < (int)end;
                                 i++, pSrc += stride, pDest += item_size)
                            {
                                memcpy(pDest, pSrc, item_size);
                            }
                        }

                        /* a write only or read/write copy */
                        context->removeAccessor(index);
                    }
                    else {
                        memcpy(&beg_ptr[offset], ptr, len);
                    }
                }

                // ownWroteToReference(&base);
            }

            ownSemPost(&memory.locks[0]);

            status = VX_SUCCESS;
        }
        else
        {
            /* could be RO|WO|RW where they decided not to commit anything. */
            if (internal == vx_true_e) /* RO */
            {
                context->removeAccessor(index);
            }
            else /* RW|WO */
            {
                ownSemPost(&memory.locks[0]);
            }

            status = VX_SUCCESS;
        }

        decrementReference(VX_EXTERNAL);
    }

    return status;
}

vx_status Array::copyArrayRange(vx_size start, vx_size end, vx_size stride, void *ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_FAILURE;
    (void)mem_type;

    /* bad parameters */
    if (((usage != VX_READ_ONLY) && (VX_WRITE_ONLY != usage)) ||
         (ptr == nullptr) || (stride < item_size) ||
         (start >= end) || (end > num_items))
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    /* determine if virtual before checking for memory */
    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" array. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual array\n");
            return VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual array, this is ok. */
    }

    /* verify has not run or will not run yet. this allows this API to "touch"
     * the array to create it.
     */
    if (allocateArray() == vx_false_e)
    {
        return VX_ERROR_NO_MEMORY;
    }

    vx_size offset = start * item_size;
    if (usage == VX_READ_ONLY)
    {
        VX_PRINT(VX_ZONE_ARRAY, "CopyArrayRange from " VX_FMT_REF " to ptr %p from %u to %u\n", this, ptr, start, end);

        vx_uint8 *pSrc = (vx_uint8 *)&memory.ptrs[0][offset];
        vx_uint8 *pDst = (vx_uint8 *)ptr;
        if (stride == item_size)
        {
            vx_size size = (end - start) * item_size;
            memcpy(pDst, pSrc, size);
        }
        else
        {
            /* The source is not compact, we need to copy per element */
            for (vx_size i = start; i < end; i++)
            {
                memcpy(pDst, pSrc, item_size);
                pDst += stride;
                pSrc += item_size;
            }
        }

        // ownReadFromReference(&base);
        status = VX_SUCCESS;
    }
    else
    {
        VX_PRINT(VX_ZONE_ARRAY, "CopyArrayRange from ptr %p to " VX_FMT_REF " from %u to %u\n", this, ptr, start, end);

        if (ownSemWait(&memory.locks[0]) == vx_true_e)
        {
            vx_uint8 *pSrc = (vx_uint8 *)ptr;
            vx_uint8 *pDst = (vx_uint8 *)&memory.ptrs[0][offset];
            if (stride == item_size)
            {
                vx_size size = (end - start) * item_size;
                memcpy(pDst, pSrc, size);
            }
            else
            {
                /* The source is not compact, we need to copy per element */
                for (vx_size i = start; i < end; i++)
                {
                    memcpy(pDst, pSrc, item_size);
                    pDst += item_size;
                    pSrc += stride;
                }
            }

            // ownWroteToReference(&base);
            ownSemPost(&memory.locks[0]);
            status = VX_SUCCESS;
        }
        else
        {
            status = VX_ERROR_NO_RESOURCES;
        }
    }

    return status;
}

vx_status Array::mapArrayRange(vx_size start, vx_size end, vx_map_id *map_id, vx_size *stride,
                             void **ptr, vx_enum usage, vx_enum mem_type, vx_uint32 flags)
{
    vx_status status = VX_FAILURE;

    /* bad parameters */
    if ((usage < VX_READ_ONLY) || (VX_READ_AND_WRITE < usage) ||
        (ptr == nullptr) || (stride == nullptr) ||
        (start >= end) || (end > num_items))
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    /* determine if virtual before checking for memory */
    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" array. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual array\n");
            return VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual array, this is ok. */
    }

    /* verify has not run or will not run yet. this allows this API to "touch"
     * the array to create it.
     */
    if (allocateArray() == vx_false_e)
    {
        return VX_ERROR_NO_MEMORY;
    }

    VX_PRINT(VX_ZONE_ARRAY, "MapArrayRange from " VX_FMT_REF " to ptr %p from %u to %u\n", this, *ptr, start, end);

    vx_memory_map_extra extra;
    extra.array_data.start = start;
    extra.array_data.end = end;
    vx_uint8 *buf = nullptr;
    vx_size size = (end - start) * item_size;
    if (context->memoryMap((vx_reference)this, size, usage, mem_type, flags, &extra, (void **)&buf, map_id) == vx_true_e)
    {
        if (VX_READ_ONLY == usage || VX_READ_AND_WRITE == usage)
        {
            if (ownSemWait(&memory.locks[0]) == vx_true_e)
            {
                *stride = item_size;

                vx_uint32 offset = (vx_uint32)(start * item_size);
                vx_uint8 *pSrc = (vx_uint8 *)&memory.ptrs[0][offset];
                vx_uint8 *pDst = (vx_uint8 *)buf;
                memcpy(pDst, pSrc, size);

                *ptr = buf;
                incrementReference(VX_EXTERNAL);
                ownSemPost(&memory.locks[0]);

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
            *stride = item_size;
            *ptr = buf;
            incrementReference(VX_EXTERNAL);
            status = VX_SUCCESS;
        }
    }
    else
    {
        status = VX_FAILURE;
    }

    return status;
}

vx_status Array::unmapArrayRange(vx_map_id map_id)
{
    vx_status status = VX_FAILURE;

    /* determine if virtual before checking for memory */
    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            /* User tried to access a "virtual" array. */
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual array\n");
            return VX_ERROR_OPTIMIZED_AWAY;
        }
        /* framework trying to access a virtual array, this is ok. */
    }

    /* bad parameters */
    if (context->findMemoryMap((vx_reference)this, map_id) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters to unmap array range\n");
        return VX_ERROR_INVALID_PARAMETERS;
    }

    VX_PRINT(VX_ZONE_ARRAY, "UnmapArrayRange from " VX_FMT_REF "\n", this);

    vx_context context = context;
    vx_memory_map_t* map = &context->memory_maps[map_id];
    if (map->used && map->ref == (vx_reference)this)
    {
        vx_size start = map->extra.array_data.start;
        vx_size end = map->extra.array_data.end;
        if (VX_WRITE_ONLY == map->usage || VX_READ_AND_WRITE == map->usage)
        {
            if (ownSemWait(&memory.locks[0]) == vx_true_e)
            {
                vx_uint32 offset = (vx_uint32)(start * item_size);
                vx_uint8 *pSrc = (vx_uint8 *)map->ptr;
                vx_uint8 *pDst = (vx_uint8 *)&memory.ptrs[0][offset];
                vx_size size = (end - start) * item_size;
                memcpy(pDst, pSrc, size);

                context->memoryUnmap((vx_uint32)map_id);
                decrementReference(VX_EXTERNAL);
                ownSemPost(&memory.locks[0]);
                status = VX_SUCCESS;
            }
            else
            {
                status = VX_ERROR_NO_RESOURCES;
            }
        }
        else
        {
            /* read only mode */
            context->memoryUnmap((vx_uint32)map_id);
            decrementReference(VX_EXTERNAL);
            status = VX_SUCCESS;
        }
    }
    else
    {
        status = VX_FAILURE;
    }

    return status;
}

/*==============================================================================
 PUBLIC INTERFACE
 =============================================================================*/

VX_API_ENTRY vx_array VX_API_CALL vxCreateArray(vx_context context, vx_enum item_type, vx_size capacity)
{
    vx_array arr = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if ( (vxIsValidArrayItemType(context, item_type) == vx_true_e) &&
             (capacity > 0))
        {
            arr = (vx_array)Array::createArray(context, item_type, capacity, vx_false_e, VX_TYPE_ARRAY);

            if (arr == nullptr)
            {
                // arr = (vx_array)ownGetErrorObject(context, VX_ERROR_NO_MEMORY);
            }
        }
        else
        {
            // arr = (vx_array)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        }
    }

    return arr;
}

VX_API_ENTRY vx_array VX_API_CALL vxCreateVirtualArray(vx_graph graph, vx_enum item_type, vx_size capacity)
{
    vx_array arr = nullptr;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e)
    {
        if (((vxIsValidArrayItemType(graph->context, item_type) == vx_true_e) || item_type == VX_TYPE_INVALID))
        {
            arr = (vx_array)Array::createArray(graph->context, item_type, capacity, vx_true_e, VX_TYPE_ARRAY);

            if (arr && arr->type == VX_TYPE_ARRAY)
            {
                arr->scope = (vx_reference)graph;
            }
            else
            {
                // arr = (vx_array)ownGetErrorObject(graph->base.context, VX_ERROR_NO_MEMORY);
            }
        }
        else
        {
            // arr = (vx_array)ownGetErrorObject(graph->base.context, VX_ERROR_INVALID_PARAMETERS);
        }
    }

    return arr;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseArray(vx_array *a)
{
    vx_status status = VX_FAILURE;

    /* nullptr means standard destructor */
    if (nullptr != a)
    {
        vx_array arr = *(a);
        if (Reference::isValidReference(arr, VX_TYPE_ARRAY) == vx_true_e)
        {
            status = arr->releaseReference(VX_TYPE_ARRAY, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryArray(vx_array arr, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;
    if (vxIsValidArray(arr) == vx_true_e)
    {
        status = VX_SUCCESS;
        switch (attribute)
        {
            case VX_ARRAY_ITEMTYPE:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                {
                    *(vx_enum *)ptr = arr->item_type;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;

            case VX_ARRAY_NUMITEMS:
                if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
                {
                    *(vx_size *)ptr = arr->num_items;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;

            case VX_ARRAY_CAPACITY:
                if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
                {
                    *(vx_size *)ptr = arr->capacity;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;

            case VX_ARRAY_ITEMSIZE:
                if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
                {
                    *(vx_size *)ptr = arr->item_size;
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
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxAddArrayItems(vx_array arr, vx_size count, const void *ptr, vx_size stride)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;
    if (vxIsValidArray(arr) == vx_true_e)
    {
        status = VX_ERROR_NO_MEMORY;

        if(arr->allocateArray() == vx_true_e)
        {
            status = VX_ERROR_INVALID_PARAMETERS;

            if ((count > 0) && (ptr != nullptr) && (stride >= arr->item_size))
            {
                status = VX_FAILURE;

                if (arr->num_items + count <= arr->capacity)
                {
                    vx_size offset = arr->num_items * arr->item_size;
                    vx_uint8 *dst_ptr = &arr->memory.ptrs[0][offset];

                    vx_size i;
                    for (i = 0; i < count; ++i)
                    {
                        vx_uint8 *tmp = (vx_uint8 *)ptr;
                        memcpy(&dst_ptr[i * arr->item_size], &tmp[i * stride], arr->item_size);
                    }

                    arr->num_items += count;
                    // ownWroteToReference(&arr->base);

                    status = VX_SUCCESS;
                }
            }
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxTruncateArray(vx_array arr, vx_size new_num_items)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;
    if (vxIsValidArray(arr) == vx_true_e)
    {
        status = VX_ERROR_INVALID_PARAMETERS;

        if (new_num_items <= arr->num_items)
        {
            arr->num_items = new_num_items;
            // ownWroteToReference(&arr->base);

            status = VX_SUCCESS;
        }
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxAccessArrayRange(vx_array arr, vx_size start, vx_size end, vx_size *stride, void **ptr, vx_enum usage)
{
    vx_status status = VX_FAILURE;
    /* bad references */
    if (vxIsValidArray(arr) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid array!\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

    /* bad parameters */
    if (stride == nullptr)
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    status = arr->accessArrayRange(start, end, stride, ptr, usage);

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCommitArrayRange(vx_array arr, vx_size start, vx_size end, const void *ptr)
{
    if (vxIsValidArray(arr) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }
    return arr->commitArrayRange(start, end, ptr);
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyArrayRange(vx_array arr, vx_size start, vx_size end, vx_size stride,
                                                    void *ptr, vx_enum usage, vx_enum mem_type)
{
    vx_status status = VX_FAILURE;
    /* bad references */
    if (vxIsValidArray(arr) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid array!\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    void * ptr_given = ptr;
    vx_enum mem_type_given = mem_type;
    if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        /* get ptr from OpenCL buffer for HOST */
        size_t size = 0;
        cl_mem opencl_buf = (cl_mem)ptr;
        cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyArrayRange: clGetMemObjectInfo(%p) => (%d)\n",
            opencl_buf, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        ptr = clEnqueueMapBuffer(context->opencl_command_queue,
            opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
            0, nullptr, nullptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyArrayRange: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
            opencl_buf, (int)size, ptr, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    status = arr->copyArrayRange(start, end, stride, ptr, usage, mem_type);

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        clEnqueueUnmapMemObject(context->opencl_command_queue,
            (cl_mem)ptr_given, ptr, 0, nullptr, nullptr);
        clFinish(context->opencl_command_queue);
    }
#endif

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxMapArrayRange(vx_array arr, vx_size start, vx_size end, vx_map_id *map_id, vx_size *stride,
                                                   void **ptr, vx_enum usage, vx_enum mem_type, vx_uint32 flags)
{
    vx_status status = VX_FAILURE;
    /* bad references */
    if (vxIsValidArray(arr) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid array!\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_enum mem_type_requested = mem_type;
    if (mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    status = arr->mapArrayRange(start, end, map_id, stride, ptr, usage, mem_type, flags);

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_size size = (end - start) * *stride;
    if ((status == VX_SUCCESS) && context->opencl_context &&
        (mem_type_requested == VX_MEMORY_TYPE_OPENCL_BUFFER) &&
        (size > 0) && ptr && *ptr)
    {
        /* create OpenCL buffer using the host allocated pointer */
        cl_int cerr = 0;
        cl_mem opencl_buf = clCreateBuffer(context->opencl_context,
            CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
            size, *ptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxMapArrayRange: clCreateBuffer(%u) => %p (%d)\n",
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

VX_API_ENTRY vx_status VX_API_CALL vxUnmapArrayRange(vx_array arr, vx_map_id map_id)
{
    vx_status status = VX_FAILURE;
    /* bad references */
    if (vxIsValidArray(arr) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid array!\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (context->opencl_context &&
        context->memory_maps[map_id].opencl_buf &&
        context->memory_maps[map_id].ptr)
    {
        clEnqueueUnmapMemObject(context->opencl_command_queue,
            context->memory_maps[map_id].opencl_buf,
            context->memory_maps[map_id].ptr, 0, nullptr, nullptr);
        clFinish(context->opencl_command_queue);
        cl_int cerr = clReleaseMemObject(context->memory_maps[map_id].opencl_buf);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxUnmapArrayRange: clReleaseMemObject(%p) => (%d)\n",
            context->memory_maps[map_id].opencl_buf, cerr);
        context->memory_maps[map_id].opencl_buf = nullptr;
    }
#endif

    status = arr->unmapArrayRange(map_id);

    return status;
}
