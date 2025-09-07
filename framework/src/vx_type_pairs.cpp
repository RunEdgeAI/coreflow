/*
 * Copyright (c) 2012-2017 The Khronos Group Inc. *
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

#include "vx_type_pairs.h"

using namespace coreflow;

#if defined(EXPERIMENTAL_USE_DOT) || defined(OPENVX_USE_XML)

vx_int32 TypePairs::stringFromType(vx_enum type)
{
    vx_uint32 i = 0u;
    for (i = 0u; i < dimof(type_pairs); i++)
    {
        if (type == type_pairs[i].type)
        {
            return i;
        }
    }
    return -1;
}

#endif /* defined(EXPERIMENTAL_USE_DOT) || defined(OPENVX_USE_XML) */

#if defined(OPENVX_USE_XML)

vx_status TypePairs::typeFromString(char *string, vx_enum *type)
{
    vx_status status = VX_ERROR_INVALID_TYPE;
    vx_uint32 i = 0u;

    for (i = 0u; i < dimof(type_pairs); i++)
    {
        if (strncmp(string, type_pairs[i].name, sizeof(type_pairs[i].name)) == 0)
        {
            *type = type_pairs[i].type;
            status = VX_SUCCESS;
            break;
        }
    }
    return status;
}

vx_size TypePairs::metaSizeOfType(vx_enum type)
{
    vx_size size = 0ul;
    vx_uint32 i = 0u;
    for (i = 0u; i < dimof(type_pairs); i++)
    {
        if (type_pairs[i].type == type)
        {
            size = type_pairs[i].nibbles / 2;
            break;
        }
    }
    return size;
}

#endif /* defined (OPENVX_USE_XML) */
