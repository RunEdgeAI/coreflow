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

#ifndef VX_INLINES_H
#define VX_INLINES_H

#include "vx_internal.h"

static VX_INLINE void *ownFormatMemoryPtr(vx_memory_t *memory,
                                          vx_uint32 c,
                                          vx_uint32 x,
                                          vx_uint32 y,
                                          vx_uint32 p)
{
    intmax_t offset = (memory->strides[p][VX_DIM_Y] * y) +
                      (memory->strides[p][VX_DIM_X] * x) +
                      (memory->strides[p][VX_DIM_C] * c);
    void *ptr = (void *)&memory->ptrs[p][offset];
    //ownPrintMemory(memory);
    //VX_PRINT(VX_ZONE_INFO, "&(%p[%zu]) = %p\n", memory->ptrs[p], offset, ptr);
    return ptr;
}

static VX_INLINE int isodd(size_t a)
{
    return (int)(a & 1);
}

static VX_INLINE vx_bool vxIsOdd(vx_uint32 a)
{
    if (a & 0x1)
        return vx_true_e;
    else
        return vx_false_e;
}

static VX_INLINE vx_bool vxIsPowerOfTwo(vx_uint32 a)
{
    if (a == 0)
        return vx_false_e;
    else if ((a & ((a) - 1)) == 0)
        return vx_true_e;
    else
        return vx_false_e;
}

static VX_INLINE vx_size strncount(const vx_char string[], vx_size size, vx_char c)
{
    vx_size i = 0ul, count = 0ul;
    while (string[i] != '\0' && i < size)
        if (string[i++] == c)
            count++;
    return count;
}

static VX_INLINE vx_size strnindex(const vx_char *str, vx_char c, vx_size limit)
{
    vx_size index = 0;
    while (index < limit && *str != c)
    {
        if(!*str)
        {
            index = limit;
            break;
        }
        str++;
        index++;
    }
    return index;
}

static VX_INLINE int validFormat(vx_enum data_type, vx_uint8 fixed_point_pos)
{
        return
#ifdef EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
            data_type == VX_TYPE_FLOAT16 ||
#endif /* EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT */
#ifdef OPENVX_CONFORMANCE_NNEF_IMPORT
            data_type == VX_TYPE_FLOAT32 || data_type == VX_TYPE_INT32 || data_type == VX_TYPE_BOOL ||
#endif /* OPENVX_CONFORMANCE_NNEF_IMPORT */
            (data_type == VX_TYPE_INT16 && fixed_point_pos == Q78_FIXED_POINT_POSITION) ||
            (data_type == VX_TYPE_INT8 && fixed_point_pos == 0) ||
            (data_type == VX_TYPE_UINT8 && fixed_point_pos == 0);
}

#endif /* VX_INLINES_H */
