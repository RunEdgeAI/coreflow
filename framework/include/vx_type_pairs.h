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

#ifndef VX_TYPE_PAIRS_H
#define VX_TYPE_PAIRS_H

#include <VX/vx.h>

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal type pairs implementation
 *
 * \defgroup group_int_type_pairs Internal Type Pairs Helper API
 * \ingroup group_internal
 * \brief The Internal Type Pairs Helper API.
 */
namespace corevx {

/*! \brief The enum string structure
 * \ingroup group_int_type_pairs
 */
struct vx_enum_string_t
{
    /*! \brief The data type enumeration */
    vx_enum type;
    /*! \brief A character string to hold the name of the data type enum */
    vx_char name[64];
    /*! \brief Value of how many nibbles the data type uses */
    uintmax_t nibbles;
};

/*! \brief The type pairs array
 * \ingroup group_int_type_pairs
 */
const vx_enum_string_t type_pairs[] = {
    {VX_STRINGERIZE(VX_TYPE_INVALID), 0},
    /* scalar objects */
    {VX_STRINGERIZE(VX_TYPE_CHAR), sizeof(vx_char) * 2},
    {VX_STRINGERIZE(VX_TYPE_UINT8), sizeof(vx_uint8) * 2},
    {VX_STRINGERIZE(VX_TYPE_UINT16), sizeof(vx_uint16) * 2},
    {VX_STRINGERIZE(VX_TYPE_UINT32), sizeof(vx_uint32) * 2},
    {VX_STRINGERIZE(VX_TYPE_UINT64), sizeof(vx_uint64) * 2},
    {VX_STRINGERIZE(VX_TYPE_INT8), sizeof(vx_int8) * 2},
    {VX_STRINGERIZE(VX_TYPE_INT16), sizeof(vx_int16) * 2},
    {VX_STRINGERIZE(VX_TYPE_INT32), sizeof(vx_int32) * 2},
    {VX_STRINGERIZE(VX_TYPE_INT64), sizeof(vx_int64) * 2},
    {VX_STRINGERIZE(VX_TYPE_FLOAT32), sizeof(vx_float32) * 2},
    {VX_STRINGERIZE(VX_TYPE_FLOAT64), sizeof(vx_float64) * 2},
    {VX_STRINGERIZE(VX_TYPE_SIZE), sizeof(vx_size) * 2},
    {VX_STRINGERIZE(VX_TYPE_DF_IMAGE), sizeof(vx_df_image) * 2},
    {VX_STRINGERIZE(VX_TYPE_BOOL), sizeof(vx_bool) * 2},
    {VX_STRINGERIZE(VX_TYPE_ENUM), sizeof(vx_enum) * 2},
    /* struct objects */
    {VX_STRINGERIZE(VX_TYPE_COORDINATES2D), sizeof(vx_coordinates2d_t) * 2},
    {VX_STRINGERIZE(VX_TYPE_COORDINATES3D), sizeof(vx_coordinates3d_t) * 2},
    {VX_STRINGERIZE(VX_TYPE_RECTANGLE), sizeof(vx_rectangle_t) * 2},
    {VX_STRINGERIZE(VX_TYPE_KEYPOINT), sizeof(vx_keypoint_t) * 2},
    /* data objects */
    {VX_STRINGERIZE(VX_TYPE_ARRAY), sizeof(Array)},
    {VX_STRINGERIZE(VX_TYPE_DISTRIBUTION), sizeof(Distribution)},
    {VX_STRINGERIZE(VX_TYPE_LUT), sizeof(Lut)},
    {VX_STRINGERIZE(VX_TYPE_IMAGE), sizeof(Image)},
    {VX_STRINGERIZE(VX_TYPE_CONVOLUTION), sizeof(Convolution)},
    {VX_STRINGERIZE(VX_TYPE_THRESHOLD), sizeof(Threshold)},
    {VX_STRINGERIZE(VX_TYPE_TENSOR), sizeof(Tensor)},
    {VX_STRINGERIZE(VX_TYPE_MATRIX), sizeof(Matrix)},
    {VX_STRINGERIZE(VX_TYPE_OBJECT_ARRAY), sizeof(ObjectArray)},
    {VX_STRINGERIZE(VX_TYPE_SCALAR), sizeof(Scalar)},
    {VX_STRINGERIZE(VX_TYPE_PYRAMID), sizeof(Pyramid)},
    {VX_STRINGERIZE(VX_TYPE_REMAP), sizeof(Remap)},
#ifdef OPENVX_KHR_XML
    {VX_STRINGERIZE(VX_TYPE_IMPORT), sizeof(Import)},
#endif
    /* framework objects */
    {VX_STRINGERIZE(VX_TYPE_CONTEXT), sizeof(Context)},
    {VX_STRINGERIZE(VX_TYPE_GRAPH), sizeof(Graph)},
    {VX_STRINGERIZE(VX_TYPE_NODE), sizeof(Node)},
    {VX_STRINGERIZE(VX_TYPE_KERNEL), sizeof(Kernel)},
    {VX_STRINGERIZE(VX_TYPE_TARGET), sizeof(Target)},
    {VX_STRINGERIZE(VX_TYPE_REFERENCE), sizeof(Reference)},
    {VX_STRINGERIZE(VX_TYPE_PARAMETER), sizeof(Parameter)},
};

class TypePairs
{
public:
#if defined(EXPERIMENTAL_USE_DOT) || defined(OPENVX_USE_XML)
    /*! \brief Fetch index of string in type pairs list
     * \ingroup group_int_type_pairs
     */
    static vx_int32 stringFromType(vx_enum type);

#endif /* defined(EXPERIMENTAL_USE_DOT) || defined(OPENVX_USE_XML) */

#if defined(OPENVX_USE_XML)
    /*! \brief Get enum of type given string
     * \ingroup group_int_type_pairs
     */
    static vx_status typeFromString(char *string, vx_enum *type);

    /*! \brief Get meta size of type given enum
     * \ingroup group_int_type_pairs
     */
    static vx_size metaSizeOfType(vx_enum type);
#endif /* defined (OPENVX_USE_XML) */
};
} // namespace corevx

#endif /* VX_TYPE_PAIRS_H */
