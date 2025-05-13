/*
 * Copyright (c) 2014-2017 The Khronos Group Inc.
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
#include "vx_meta_format.h"

/******************************************************************************/
/* INTERNAL INTERFACE                                                         */
/******************************************************************************/
MetaFormat::MetaFormat(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_META_FORMAT, scope),
size(0),
type(),
dim(),
set_valid_rectangle_callback(nullptr)
{

}

vx_bool MetaFormat::isValidMetaFormat(vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool isValid = vx_false_e;

    if ((Reference::isValidReference(meta1, VX_TYPE_META_FORMAT) == vx_true_e) &&
        (Reference::isValidReference(meta2, VX_TYPE_META_FORMAT) == vx_true_e))
    {
        isValid = vx_true_e;
    }

    if (isValid == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Meta format is invalid!\n");
    }

    return isValid;
}

vx_bool MetaFormat::isMetaFormatEqual(
    vx_meta_format meta1, vx_meta_format meta2, vx_enum ref_type)
{
    vx_bool isEqual = vx_false_e;

    if (vx_true_e == MetaFormat::isValidMetaFormat(meta1, meta2))
    {
        switch (ref_type)
        {
            case VX_TYPE_IMAGE:
                isEqual = MetaFormat::isMetaFormatImageEqual(meta1, meta2);
                break;
            case VX_TYPE_ARRAY:
                isEqual = MetaFormat::isMetaFormatArrayEqual(meta1, meta2);
                break;
            case VX_TYPE_SCALAR:
                isEqual = MetaFormat::isMetaFormatScalarEqual(meta1, meta2);
                break;
            case VX_TYPE_PYRAMID:
                isEqual = MetaFormat::isMetaFormatPyramidEqual(meta1, meta2);
                break;
            case VX_TYPE_MATRIX:
                isEqual = MetaFormat::isMetaFormatMatrixEqual(meta1, meta2);
                break;
            case VX_TYPE_DISTRIBUTION:
                isEqual = MetaFormat::isMetaFormatDistributionEqual(meta1, meta2);
                break;
            case VX_TYPE_CONVOLUTION:
                isEqual = MetaFormat::isMetaFormatConvolutionEqual(meta1, meta2);
                break;
            case VX_TYPE_THRESHOLD:
                isEqual = MetaFormat::isMetaFormatThresholdEqual(meta1, meta2);
                break;
            case VX_TYPE_REMAP:
                isEqual = MetaFormat::isMetaFormatRemapEqual(meta1, meta2);
                break;
            case VX_TYPE_LUT:
                isEqual = MetaFormat::isMetaFormatLutEqual(meta1, meta2);
                break;
            case VX_TYPE_OBJECT_ARRAY:
                isEqual = MetaFormat::isMetaFormatObjectArrayEqual(meta1, meta2);
                break;
            case VX_TYPE_TENSOR:
                isEqual = MetaFormat::isMetaFormatTensorEqual(meta1, meta2);
                break;
            case VX_TYPE_USER_DATA_OBJECT:
                isEqual = MetaFormat::isMetaFormatUserDataObjectEqual(meta1, meta2);
                break;
            default:
                VX_PRINT(VX_ZONE_ERROR, "invalid ref_type attribute\n");
                break;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Meta format is invalid!\n");
    }

    return isEqual;
}

vx_bool MetaFormat::isMetaFormatImageEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.image.width  == meta2->dim.image.width) &&
             (meta1->dim.image.height == meta2->dim.image.height) &&
             (meta1->dim.image.format == meta2->dim.image.format) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Image object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatArrayEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.array.item_type == meta2->dim.array.item_type) &&
             (meta1->dim.array.capacity  == meta2->dim.array.capacity)  // &&
            //  (meta1->dim.array.item_size == meta2->dim.array.item_size)
            )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Array object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatScalarEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( meta1->dim.scalar.type == meta2->dim.scalar.type )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Scalar object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatPyramidEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.pyramid.width  == meta2->dim.pyramid.width) &&
             (meta1->dim.pyramid.height == meta2->dim.pyramid.height) &&
             (meta1->dim.pyramid.format == meta2->dim.pyramid.format) &&
             (meta1->dim.pyramid.levels == meta2->dim.pyramid.levels) &&
             (meta1->dim.pyramid.scale  == meta2->dim.pyramid.scale) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Pyramid object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatMatrixEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.matrix.type     == meta2->dim.matrix.type) &&
             (meta1->dim.matrix.rows     == meta2->dim.matrix.rows) &&
             (meta1->dim.matrix.cols     == meta2->dim.matrix.cols) // &&
            //  (meta1->dim.matrix.size     == meta2->dim.matrix.size) &&
            //  (meta1->dim.matrix.pattern  == meta2->dim.matrix.pattern) &&
            //  (meta1->dim.matrix.origin.x == meta2->dim.matrix.origin.x) &&
            //  (meta1->dim.matrix.origin.y == meta2->dim.matrix.origin.y)
            )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Matrix object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatDistributionEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.distribution.bins   == meta2->dim.distribution.bins) &&
             (meta1->dim.distribution.offset == meta2->dim.distribution.offset) &&
             (meta1->dim.distribution.range  == meta2->dim.distribution.range) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Distribution object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatConvolutionEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
    //     if ( (meta1->dim.convolution.rows  == meta2->dim.convolution.rows) &&
    //          (meta1->dim.convolution.cols  == meta2->dim.convolution.cols) &&
    //          (meta1->dim.convolution.scale == meta2->dim.convolution.scale) &&
    //          (meta1->dim.convolution.size  == meta2->dim.convolution.size) )
    //     {
    //         is_equal = vx_true_e;
    //     }
    //     else
    //     {
    //         VX_PRINT(VX_ZONE_INFO, "Convolution object meta data are not equivalent!\n");
    //     }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatRemapEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.remap.src_width  == meta2->dim.remap.src_width) &&
             (meta1->dim.remap.src_height == meta2->dim.remap.src_height) &&
             (meta1->dim.remap.dst_width  == meta2->dim.remap.dst_width) &&
             (meta1->dim.remap.dst_height == meta2->dim.remap.dst_height) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Remap object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatThresholdEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( meta1->dim.threshold.type  == meta2->dim.threshold.type )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Threshold object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatObjectArrayEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.object_array.item_type == meta2->dim.object_array.item_type) &&
             (meta1->dim.object_array.num_items == meta2->dim.object_array.num_items) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Object Array object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatLutEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.lut.type  == meta2->dim.lut.type) &&
             (meta1->dim.lut.count == meta2->dim.lut.count) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "LUT object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatTensorEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;
    vx_uint32 i;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.tensor.number_of_dimensions                 == meta2->dim.tensor.number_of_dimensions) &&
             (meta1->dim.tensor.data_type                            == meta2->dim.tensor.data_type) &&
             (meta1->dim.tensor.fixed_point_position                 == meta2->dim.tensor.fixed_point_position) // &&
            //  (meta1->dim.tensor.scaling_divisor                      == meta2->dim.tensor.scaling_divisor) &&
            //  (meta1->dim.tensor.scaling_divisor_fixed_point_position == meta2->dim.tensor.scaling_divisor_fixed_point_position)
            )
        {
            for (i = 0; i < meta1->dim.tensor.number_of_dimensions; i++)
            {
                if ((meta1->dim.tensor.dimensions[i] != meta2->dim.tensor.dimensions[i]) // ||
                    // (meta1->dim.tensor.strides[i]    != meta2->dim.tensor.strides[i])
                    )
                {
                    break;
                }
            }
            if (i == meta1->dim.tensor.number_of_dimensions)
            {
                is_equal = vx_true_e;
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "Tensor object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

vx_bool MetaFormat::isMetaFormatUserDataObjectEqual(
    vx_meta_format meta1, vx_meta_format meta2)
{
    vx_bool is_equal = vx_false_e;

    if (MetaFormat::isValidMetaFormat(meta1, meta2) == vx_true_e )
    {
        if ( (meta1->dim.user_data_object.size == meta2->dim.user_data_object.size) &&
             ( 0 == strncmp(meta1->dim.user_data_object.type_name, meta2->dim.user_data_object.type_name, VX_MAX_REFERENCE_NAME) ) )
        {
            is_equal = vx_true_e;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "User data object meta data are not equivalent!\n");
        }
    }

    return is_equal;
}

/******************************************************************************/
/* PUBLIC INTERFACE                                                           */
/******************************************************************************/

vx_meta_format vxCreateMetaFormat(vx_context context)
{
    vx_meta_format meta = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        meta = (vx_meta_format)Reference::createReference(context, VX_TYPE_META_FORMAT, VX_EXTERNAL, context);
        if (vxGetStatus((vx_reference)meta) == VX_SUCCESS)
        {
            meta->size = sizeof(vx_meta_format);
            meta->type = VX_TYPE_INVALID;
        }
    }

    return meta;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryMetaFormatAttribute(vx_meta_format meta, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(meta), VX_TYPE_META_FORMAT) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    if (VX_TYPE(attribute) != (vx_uint32)meta->type &&
        attribute != VX_VALID_RECT_CALLBACK)
    {
        return VX_ERROR_INVALID_TYPE;
    }

    switch(attribute)
    {
        /**********************************************************************/
        case VX_IMAGE_FORMAT:
            if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3)) {
                *(vx_df_image *)ptr = meta->dim.image.format;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_IMAGE_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.image.height;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_IMAGE_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.image.width;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_ARRAY_CAPACITY:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.array.capacity;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_ARRAY_ITEMTYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                *(vx_enum *)ptr = meta->dim.array.item_type;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_PYRAMID_FORMAT:
            if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3)) {
                *(vx_df_image *)ptr = meta->dim.pyramid.format;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.pyramid.height;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.pyramid.width;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_LEVELS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.pyramid.levels;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_SCALE:
            if (VX_CHECK_PARAM(ptr, size, vx_float32, 0x3)) {
                *(vx_float32 *)ptr = meta->dim.pyramid.scale;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_SCALAR_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                *(vx_enum *)ptr = meta->dim.scalar.type;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_MATRIX_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                *(vx_enum *)ptr = meta->dim.matrix.type;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_MATRIX_ROWS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.matrix.rows;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_MATRIX_COLUMNS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.matrix.cols;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_DISTRIBUTION_BINS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.distribution.bins;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_RANGE:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.distribution.range;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_OFFSET:
            if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3)) {
                *(vx_int32 *)ptr = meta->dim.distribution.offset;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_REMAP_SOURCE_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.remap.src_width;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_SOURCE_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.remap.src_height;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_DESTINATION_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.remap.dst_width;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_DESTINATION_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                *(vx_uint32 *)ptr = meta->dim.remap.dst_height;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_LUT_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                *(vx_enum *)ptr = meta->dim.lut.type;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_COUNT:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.lut.count;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_THRESHOLD_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                *(vx_enum *)ptr = meta->dim.threshold.type;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_VALID_RECT_CALLBACK:
            if (VX_CHECK_PARAM(ptr, size, vx_kernel_image_valid_rectangle_f, 0x0)) {
                *(vx_kernel_image_valid_rectangle_f*)ptr = meta->set_valid_rectangle_callback;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_OBJECT_ARRAY_ITEMTYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                *(vx_enum *)ptr = meta->dim.object_array.item_type;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_OBJECT_ARRAY_NUMITEMS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_enum *)ptr = meta->dim.object_array.num_items;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_TENSOR_NUMBER_OF_DIMS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                *(vx_size *)ptr = meta->dim.tensor.number_of_dimensions;
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_DIMS:
            if (size <= (sizeof(vx_size)*VX_MAX_TENSOR_DIMENSIONS) && ((vx_size)ptr & 0x3) == 0)
            {
                memcpy(ptr, meta->dim.tensor.dimensions, size);
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_DATA_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
            {
                *(vx_enum *)ptr = meta->dim.tensor.data_type;
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_FIXED_POINT_POSITION:
            if (VX_CHECK_PARAM(ptr, size, vx_int8, 0x0)) //??
            {
                *(vx_int8 *)ptr = meta->dim.tensor.fixed_point_position;
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        default:
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetMetaFormatAttribute(vx_meta_format meta, vx_enum attribute, const void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(meta), VX_TYPE_META_FORMAT) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_TYPE(attribute) != (vx_uint32)meta->type &&
        attribute != VX_VALID_RECT_CALLBACK)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid attribute provided in %s\n", __func__);
        return VX_ERROR_INVALID_TYPE;
    }

    switch(attribute)
    {
        /**********************************************************************/
        case VX_IMAGE_FORMAT:
            if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3)) {
                meta->dim.image.format = *(vx_df_image *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_IMAGE_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.image.height = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_IMAGE_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.image.width = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_ARRAY_CAPACITY:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.array.capacity = *(vx_size *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_ARRAY_ITEMTYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                meta->dim.array.item_type = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_PYRAMID_FORMAT:
            if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3)) {
                meta->dim.pyramid.format = *(vx_df_image *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.pyramid.height = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.pyramid.width = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_LEVELS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.pyramid.levels = *(vx_size *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_PYRAMID_SCALE:
            if (VX_CHECK_PARAM(ptr, size, vx_float32, 0x3)) {
                meta->dim.pyramid.scale = *(vx_float32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_SCALAR_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                meta->dim.scalar.type = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_MATRIX_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                meta->dim.matrix.type = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_MATRIX_ROWS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.matrix.rows = *(vx_size *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_MATRIX_COLUMNS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.matrix.cols = *(vx_size *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_DISTRIBUTION_BINS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.distribution.bins = *(vx_size *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_RANGE:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.distribution.range = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_DISTRIBUTION_OFFSET:
            if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3)) {
                meta->dim.distribution.offset = *(vx_int32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_REMAP_SOURCE_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.remap.src_width = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_SOURCE_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.remap.src_height = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_DESTINATION_WIDTH:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.remap.dst_width = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REMAP_DESTINATION_HEIGHT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3)) {
                meta->dim.remap.dst_height = *(vx_uint32 *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_LUT_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                meta->dim.lut.type = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_LUT_COUNT:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.lut.count = *(vx_size *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_THRESHOLD_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                meta->dim.threshold.type = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_VALID_RECT_CALLBACK:
            if (VX_CHECK_PARAM(ptr, size, vx_kernel_image_valid_rectangle_f, 0x0)) {
                meta->set_valid_rectangle_callback = *(vx_kernel_image_valid_rectangle_f*)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_OBJECT_ARRAY_ITEMTYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3)) {
                meta->dim.object_array.item_type = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_OBJECT_ARRAY_NUMITEMS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.object_array.num_items = *(vx_enum *)ptr;
            } else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
        case VX_TENSOR_NUMBER_OF_DIMS:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3)) {
                meta->dim.tensor.number_of_dimensions = *(vx_size *)ptr;
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_DIMS:
            if (size <= (sizeof(vx_size)*VX_MAX_TENSOR_DIMENSIONS) && ((vx_size)ptr & 0x3) == 0)
            {
                memcpy(meta->dim.tensor.dimensions, ptr, size);
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_DATA_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
            {
                meta->dim.tensor.data_type = *(vx_enum *)ptr;
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_TENSOR_FIXED_POINT_POSITION:
            if (VX_CHECK_PARAM(ptr, size, vx_int8, 0x0)) //??
            {
                meta->dim.tensor.fixed_point_position = *(vx_int8 *)ptr;
            }
            else {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        /**********************************************************************/
#if defined(OPENVX_USE_USER_DATA_OBJECT)
        case VX_USER_DATA_OBJECT_NAME:
            if ((ptr != nullptr) && (size <= VX_MAX_REFERENCE_NAME))
            {
                strncpy(meta->dim.user_data_object.type_name, (const char*)ptr, VX_MAX_REFERENCE_NAME);
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "vxSetMetaFormatAttribute: VX_USER_DATA_OBJECT_NAME error\n");
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_USER_DATA_OBJECT_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3U))
            {
                meta->dim.user_data_object.size = *(vx_size *)ptr;
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR,"vxSetMetaFormatAttribute: VX_USER_DATA_OBJECT_SIZE error\n");
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
#endif /* defined(OPENVX_USE_USER_DATA_OBJECT) */
        default:
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }

    VX_PRINT(VX_ZONE_API, "%s returned %d\n", __func__, status);
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetMetaFormatFromReference(vx_meta_format meta, vx_reference examplar)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(meta), VX_TYPE_META_FORMAT) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(examplar) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    switch (examplar->type)
    {
    case VX_TYPE_TENSOR:
    {
        vx_tensor tensor = (vx_tensor)examplar;
        meta->type = VX_TYPE_TENSOR;
        meta->dim.tensor.data_type = tensor->data_type;
        meta->dim.tensor.fixed_point_position = tensor->fixed_point_position;
        meta->dim.tensor.number_of_dimensions = tensor->number_of_dimensions;
        memcpy (meta->dim.tensor.dimensions, tensor->dimensions, sizeof(tensor->dimensions));
        break;
    }
    case VX_TYPE_IMAGE:
    {
        vx_image image = (vx_image)examplar;
        meta->type = VX_TYPE_IMAGE;
        meta->dim.image.width = image->width;
        meta->dim.image.height = image->height;
        meta->dim.image.format = image->format;
        break;
    }
    case VX_TYPE_ARRAY:
    {
        vx_array array = (vx_array)examplar;
        meta->type = VX_TYPE_ARRAY;
        meta->dim.array.item_type = array->item_type;
        meta->dim.array.capacity = array->capacity;
        break;
    }
    case VX_TYPE_PYRAMID:
    {
        vx_pyramid pyramid = (vx_pyramid)examplar;
        meta->type = VX_TYPE_PYRAMID;
        meta->dim.pyramid.width = pyramid->width;
        meta->dim.pyramid.height = pyramid->height;
        meta->dim.pyramid.format = pyramid->format;
        meta->dim.pyramid.levels = pyramid->numLevels;
        meta->dim.pyramid.scale = pyramid->scale;
        break;
    }
    case VX_TYPE_SCALAR:
    {
        vx_scalar scalar = (vx_scalar)examplar;
        meta->type = VX_TYPE_SCALAR;
        meta->dim.scalar.type = scalar->data_type;
        break;
    }
    case VX_TYPE_MATRIX:
    {
        vx_matrix matrix = (vx_matrix)examplar;
        meta->type = VX_TYPE_MATRIX;
        meta->dim.matrix.type = matrix->data_type;
        meta->dim.matrix.cols = matrix->columns;
        meta->dim.matrix.rows = matrix->rows;
        break;
    }
    case VX_TYPE_DISTRIBUTION:
    {
        vx_distribution distribution = (vx_distribution)examplar;
        meta->type = VX_TYPE_DISTRIBUTION;
        meta->dim.distribution.bins = distribution->memory.dims[0][VX_DIM_X];
        meta->dim.distribution.offset = distribution->offset_x;
        meta->dim.distribution.range = distribution->range_x;
        break;
    }
    case VX_TYPE_REMAP:
    {
        vx_remap remap = (vx_remap)examplar;
        meta->type = VX_TYPE_REMAP;
        meta->dim.remap.src_width = remap->src_width;
        meta->dim.remap.src_height = remap->src_height;
        meta->dim.remap.dst_width = remap->dst_width;
        meta->dim.remap.dst_height = remap->dst_height;
        break;
    }
    case VX_TYPE_LUT:
    {
        vx_lut_t lut = (vx_lut_t)examplar;
        meta->type = VX_TYPE_LUT;
        meta->dim.lut.type = lut->item_type;
        meta->dim.lut.count = lut->num_items;
        break;
    }
    case VX_TYPE_THRESHOLD:
    {
        vx_threshold threshold = (vx_threshold)examplar;
        meta->type = VX_TYPE_THRESHOLD;
        meta->dim.threshold.type = threshold->thresh_type;
        break;
    }
#if defined(OPENVX_USE_USER_DATA_OBJECT)
    case VX_TYPE_USER_DATA_OBJECT:
    {
        vx_user_data_object user_data_object = (vx_user_data_object)examplar;
        meta->type = VX_TYPE_USER_DATA_OBJECT;
        meta->dim.user_data_object.size = user_data_object->size;
        meta->dim.user_data_object.type_name[0] = 0;
        strncpy(meta->dim.user_data_object.type_name, user_data_object->type_name, VX_MAX_REFERENCE_NAME);
        break;
    }
#endif /* defined(OPENVX_USE_USER_DATA_OBJECT) */
    case VX_TYPE_OBJECT_ARRAY:
    {
        vx_object_array objarray = (vx_object_array)examplar;
        vx_reference item = objarray->items[0];
        meta->type = VX_TYPE_OBJECT_ARRAY;
        meta->dim.object_array.item_type = objarray->item_type;
        meta->dim.object_array.num_items = objarray->num_items;

        switch (objarray->item_type)
        {
        case VX_TYPE_IMAGE:
        {
            vx_image image = (vx_image)item;
            meta->dim.image.width = image->width;
            meta->dim.image.height = image->height;
            meta->dim.image.format = image->format;
            break;
        }
        case VX_TYPE_ARRAY:
        {
            vx_array array = (vx_array)item;
            meta->dim.array.item_type = array->item_type;
            meta->dim.array.capacity = array->capacity;
            break;
        }
        case VX_TYPE_PYRAMID:
        {
            vx_pyramid pyramid = (vx_pyramid)item;
            meta->dim.pyramid.width = pyramid->width;
            meta->dim.pyramid.height = pyramid->height;
            meta->dim.pyramid.format = pyramid->format;
            meta->dim.pyramid.levels = pyramid->numLevels;
            meta->dim.pyramid.scale = pyramid->scale;
            break;
        }
        case VX_TYPE_SCALAR:
        {
            vx_scalar scalar = (vx_scalar)item;
            meta->dim.scalar.type = scalar->data_type;
            break;
        }
        case VX_TYPE_MATRIX:
        {
            vx_matrix matrix = (vx_matrix)item;
            meta->dim.matrix.type = matrix->data_type;
            meta->dim.matrix.cols = matrix->columns;
            meta->dim.matrix.rows = matrix->rows;
            break;
        }
        case VX_TYPE_DISTRIBUTION:
        {
            vx_distribution distribution = (vx_distribution)item;
            meta->dim.distribution.bins = distribution->memory.dims[0][VX_DIM_X];
            meta->dim.distribution.offset = distribution->offset_x;
            meta->dim.distribution.range = distribution->range_x;
            break;
        }
        case VX_TYPE_REMAP:
        {
            vx_remap remap = (vx_remap)item;
            meta->dim.remap.src_width = remap->src_width;
            meta->dim.remap.src_height = remap->src_height;
            meta->dim.remap.dst_width = remap->dst_width;
            meta->dim.remap.dst_height = remap->dst_height;
            break;
        }
        case VX_TYPE_LUT:
        {
            vx_lut_t lut = (vx_lut_t)item;
            meta->dim.lut.type = lut->item_type;
            meta->dim.lut.count = lut->num_items;
            break;
        }
        case VX_TYPE_THRESHOLD:
        {
            vx_threshold threshold = (vx_threshold)item;
            meta->dim.threshold.type = threshold->thresh_type;
            break;
        }
#if defined(OPENVX_USE_USER_DATA_OBJECT)
        case VX_TYPE_USER_DATA_OBJECT:
        {
            vx_user_data_object user_data_object = (vx_user_data_object)item;
            meta->dim.user_data_object.size = user_data_object->size;
            meta->dim.user_data_object.type_name[0] = 0;
            strncpy(meta->dim.user_data_object.type_name, user_data_object->type_name, VX_MAX_REFERENCE_NAME);
            break;
        }
#endif /* defined(OPENVX_USE_USER_DATA_OBJECT) */
        default:
            status = VX_ERROR_INVALID_REFERENCE;
            break;
        }

        break;
    }
    default:
        status = VX_ERROR_INVALID_REFERENCE;
        break;
    }

    return status;
}

vx_status vxReleaseMetaFormat(vx_meta_format* pmeta)
{
    vx_status status = VX_FAILURE;

    if (pmeta != nullptr)
    {
        vx_meta_format this_meta_format = *pmeta;
        if (Reference::isValidReference((vx_reference)this_meta_format, VX_TYPE_META_FORMAT) == vx_true_e)
        {
            status = Reference::releaseReference((vx_reference*)pmeta, VX_TYPE_META_FORMAT, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}