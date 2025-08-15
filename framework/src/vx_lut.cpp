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

using namespace corevx;
using Lut = Array;

vx_lut_t Lut::createLUT(vx_context context, vx_enum data_type, vx_size count)
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
                lut = (vx_lut_t)Error::getError(context, VX_ERROR_INVALID_PARAMETERS);
            }
            else
#endif
            {
                lut = (vx_lut_t)Array::createArray(context, VX_TYPE_UINT8, count, vx_false_e,
                                                   VX_TYPE_LUT);
                if (Error::getStatus((vx_reference)lut) == VX_SUCCESS && lut->type == VX_TYPE_LUT)
                {
                    lut->num_items = count;
                    lut->offset = 0;
                    Array::printArray(lut);
                }
            }
        }
        else if (data_type == VX_TYPE_INT16)
        {
            if (!(count <= 65536))
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid parameter to LUT\n");
                vxAddLogEntry(context, VX_ERROR_INVALID_PARAMETERS, "Invalid parameter to LUT\n");
                lut = (vx_lut_t)Error::getError(context, VX_ERROR_INVALID_PARAMETERS);
            }
            else
            {
                lut = (vx_lut_t)Array::createArray(context, VX_TYPE_INT16, count, vx_false_e,
                                                   VX_TYPE_LUT);
                if (Error::getStatus((vx_reference)lut) == VX_SUCCESS && lut->type == VX_TYPE_LUT)
                {
                    lut->num_items = count;
                    lut->offset = (vx_uint32)(count / 2);
                    Array::printArray(lut);
                }
            }
        }
#if !defined(OPENVX_STRICT_1_0)
        else if (data_type == VX_TYPE_UINT16)
        {
            lut = (vx_lut_t)Array::createArray(context, VX_TYPE_UINT16, count, vx_false_e,
                                               VX_TYPE_LUT);
            if (Error::getStatus((vx_reference)lut) == VX_SUCCESS && lut->type == VX_TYPE_LUT)
            {
                lut->num_items = count;
                lut->offset = 0;
                Array::printArray(lut);
            }
        }
#endif
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid data type\n");
            vxAddLogEntry(context, VX_ERROR_INVALID_TYPE, "Invalid data type\n");
            lut = (vx_lut_t)Error::getError(context, VX_ERROR_INVALID_TYPE);
        }
    }

    return lut;
}

/******************************************************************************/
/* PUBLIC INTERFACE                                                           */
/******************************************************************************/

VX_API_ENTRY vx_lut VX_API_CALL vxCreateLUT(vx_context context, vx_enum data_type, vx_size count)
{
    vx_lut_t lut = nullptr;

    lut = Lut::createLUT(context, data_type, count);

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
                *(vx_enum *)ptr = lut->itemType();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_COUNT:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = lut->numItems();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                *(vx_size *)ptr = lut->totalSize();
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_OFFSET:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = lut->offsetVal();
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

VX_API_ENTRY vx_status VX_API_CALL vxAccessLUT(vx_lut lut, void **ptr, vx_enum usage)
{
    vx_status status = VX_FAILURE;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
        status = lut->accessArrayRange(0, lut->num_items, nullptr, ptr, usage);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCommitLUT(vx_lut lut, const void *ptr)
{
    vx_status status = VX_FAILURE;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(lut), VX_TYPE_LUT) == vx_true_e)
    {
        status = lut->commitArrayRange(0, lut->num_items, ptr);
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
        status = lut->copyArrayRange(0, lut->num_items, stride, user_ptr, usage, user_mem_type);
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
        vx_size stride = lut->item_size;
        status =
            lut->mapArrayRange(0, lut->num_items, map_id, &stride, ptr, usage, mem_type, flags);
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
        status = lut->unmapArrayRange(map_id);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid object!\n");
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseLUT(vx_lut *l)
{
    vx_status status = VX_FAILURE;

    if (nullptr != l)
    {
        vx_lut lut = *l;
        if (vx_true_e == Reference::isValidReference(lut, VX_TYPE_LUT))
        {
            status =
                Reference::releaseReference((vx_reference *)l, VX_TYPE_LUT, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}