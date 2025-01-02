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

/*!
 * \file
 * \brief A set of internal utility functions.
 *
 * \defgroup group_int_inlines Internal Utility Functions
 * \ingroup group_internal
 * \brief Static internal utility functions
 */

/**
 * @brief Is the number odd?
 * @ingroup group_int_inlines
 * @param a The number to check
 * @return vx_true_e if odd, else vx_false_e
 */
static VX_INLINE vx_bool vxIsOdd(vx_uint32 a)
{
    if (a & 0x1)
        return vx_true_e;
    else
        return vx_false_e;
}

/**
 * @brief Is the number a power of two?
 * @ingroup group_int_inlines
 * @param a The number to check
 * @return vx_true_e if even, else vx_false_e
 */
static VX_INLINE vx_bool vxIsPowerOfTwo(vx_uint32 a)
{
    if (a == 0)
        return vx_false_e;
    else if ((a & ((a) - 1)) == 0)
        return vx_true_e;
    else
        return vx_false_e;
}

/**
 * @brief Count the number of occurrences of a character in a string.
 * @ingroup group_int_inlines
 * @param string The string to search
 * @param size   The size of the string
 * @param c      The character to count
 * @return       The number of occurrences of the character in the string.
 */
static VX_INLINE vx_size strncount(const vx_char string[], vx_size size, vx_char c)
{
    vx_size i = 0ul, count = 0ul;
    while (string[i] != '\0' && i < size)
        if (string[i++] == c)
            count++;
    return count;
}

/**
 * @brief Find the index of a character in a string.
 * @ingroup group_int_inlines
 * @param str    The string to search
 * @param c      The character to find
 * @param limit  The maximum number of characters to search
 * @return       The index of the character in the string.
 */
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

/**
 * @brief Is valid format?
 * @ingroup group_int_inlines
 * @param data_type         data type
 * @param fixed_point_pos   fixed point position
 * @return                  vx_true_e if valid, else vx_false_e
 */
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

/*! \brief Prints the name of an object type.
 * \ingroup group_int_inlines
 */
static VX_INLINE const char* vxGetObjectTypeName(vx_enum type)
{
    const char *name = "";

    switch(type)
    {
        case VX_TYPE_CONTEXT:
            name = "CONTEXT"; break;
        case VX_TYPE_GRAPH:
            name = "GRAPH"; break;
        case VX_TYPE_NODE:
            name = "NODE"; break;
        case VX_TYPE_KERNEL:
            name = "KERNEL"; break;
        case VX_TYPE_TARGET:
            name = "TARGET"; break;
        case VX_TYPE_PARAMETER:
            name = "PARAMETER"; break;
        case VX_TYPE_DELAY:
            name = "DELAY"; break;
        case VX_TYPE_LUT:
            name = "LUT"; break;
        case VX_TYPE_DISTRIBUTION:
            name = "DISTRIBUTION"; break;
        case VX_TYPE_PYRAMID:
            name = "PYRAMID"; break;
        case VX_TYPE_THRESHOLD:
            name = "THRESHOLD"; break;
        case VX_TYPE_MATRIX:
            name = "MATRIX"; break;
        case VX_TYPE_CONVOLUTION:
            name = "CONVOLUTION"; break;
        case VX_TYPE_SCALAR:
            name = "SCALAR"; break;
        case VX_TYPE_ARRAY:
            name = "ARRAY"; break;
        case VX_TYPE_IMAGE:
            name = "IMAGE"; break;
        case VX_TYPE_REMAP:
            name = "REMAP"; break;
        case VX_TYPE_ERROR:
            name = "<ERROR OBJECT>"; break;
        case VX_TYPE_META_FORMAT:
            name = "META_FORMAT"; break;
        case VX_TYPE_OBJECT_ARRAY:
            name = "OBJECT_ARRAY"; break;
        case VX_TYPE_TENSOR:
            name = "TENSOR"; break;
        default:
            name = "<UNKNOWN TYPE>";
    }

    return name;
}

#endif /* VX_INLINES_H */
