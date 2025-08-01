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
#include "vx_threshold.h"

/*****************************************************************************/
/* INTERNAL INTERFACE                                                        */
/*****************************************************************************/
Threshold::Threshold(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_THRESHOLD, scope),
thresh_type(),
data_type(),
value(),
lower(),
upper(),
true_value(),
false_value(),
input_format(),
output_format()
{

}

Threshold::~Threshold()
{

}

void Threshold::setBinaryValue(vx_int32 value)
{
    this->value.S32 = value;
}

void Threshold::setBinaryValueUnion(vx_pixel_value_t value)
{
    this->value = value;
}

void Threshold::setLowerBound(vx_int32 lower)
{
    this->lower.S32 = lower;
}

void Threshold::setLowerBoundUnion(vx_pixel_value_t lower)
{
    this->lower = lower;
}

void Threshold::setUpperBound(vx_int32 upper)
{
    this->upper.S32 = upper;
}

void Threshold::setUpperBoundUnion(vx_pixel_value_t upper)
{
    this->upper = upper;
}

void Threshold::setTrueValue(vx_int32 true_value)
{
    this->true_value.S32 = true_value;
}

void Threshold::setTrueValueUnion(vx_pixel_value_t true_value)
{
    this->true_value = true_value;
}

void Threshold::setFalseValue(vx_int32 false_value)
{
    this->false_value.S32 = false_value;
}

void Threshold::setFalseValueUnion(vx_pixel_value_t false_value)
{
    this->false_value = false_value;
}

vx_int32 Threshold::binaryValue() const
{
    return value.S32;
}

vx_pixel_value_t Threshold::binaryValueUnion() const
{
    return value;
}

vx_int32 Threshold::lowerBound() const
{
    return lower.S32;
}

vx_pixel_value_t Threshold::lowerBoundUnion() const
{
    return lower;
}

vx_int32 Threshold::upperBound() const
{
    return upper.S32;
}

vx_pixel_value_t Threshold::upperBoundUnion() const
{
    return upper;
}

vx_int32 Threshold::trueValue() const
{
    return true_value.S32;
}

vx_pixel_value_t Threshold::trueValueUnion() const
{
    return true_value;
}

vx_int32 Threshold::falseValue() const
{
    return true_value.S32;
}

vx_pixel_value_t Threshold::falseValueUnion() const
{
    return true_value;
}

vx_enum Threshold::dataType() const
{
    return data_type;
}

vx_enum Threshold::threshType() const
{
    return thresh_type;
}

vx_df_image Threshold::inputFormat() const
{
    return input_format;
}

vx_df_image Threshold::outputFormat() const
{
    return output_format;
}

vx_bool Threshold::isValidThresholdType(vx_enum thresh_type)
{
    vx_bool ret = vx_false_e;
    if ((thresh_type == VX_THRESHOLD_TYPE_BINARY) ||
        (thresh_type == VX_THRESHOLD_TYPE_RANGE))
    {
        ret = vx_true_e;
    }
    return ret;
}

vx_bool Threshold::isValidThresholdDataType(vx_enum data_type)
{
    vx_bool ret = vx_false_e;
    if (data_type == VX_TYPE_BOOL ||
        data_type == VX_TYPE_INT8 ||
        data_type == VX_TYPE_UINT8 ||
        data_type == VX_TYPE_INT16 ||
        data_type == VX_TYPE_UINT16 ||
        data_type == VX_TYPE_INT32 ||
        data_type == VX_TYPE_UINT32)
    {
        ret = vx_true_e;
    }
    return ret;
}

vx_bool Threshold::isValidThresholdFormat(vx_df_image format)
{
    vx_bool ret = vx_false_e;
    if (format == VX_DF_IMAGE_U1 || format == VX_DF_IMAGE_U8 || format == VX_DF_IMAGE_S16 ||
        format == VX_DF_IMAGE_U16 || format == VX_DF_IMAGE_S32 || format == VX_DF_IMAGE_U32 ||
        format == VX_DF_IMAGE_RGB || format == VX_DF_IMAGE_RGBX || format == VX_DF_IMAGE_NV12 ||
        format == VX_DF_IMAGE_NV21 || format == VX_DF_IMAGE_UYVY || format == VX_DF_IMAGE_YUYV ||
        format == VX_DF_IMAGE_IYUV || format == VX_DF_IMAGE_YUV4)
    {
        ret = vx_true_e;
    }
    return ret;
}

vx_status Threshold::copyOutput(vx_pixel_value_t *true_value_ptr, vx_pixel_value_t *false_value_ptr,
                                vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_SUCCESS;

    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual threshold\n");
            status = VX_ERROR_OPTIMIZED_AWAY;
            return status;
        }
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_pixel_value_t *true_value_ptr_given = true_value_ptr;
    vx_pixel_value_t *false_value_ptr_given = false_value_ptr;
    vx_enum user_mem_type_given = user_mem_type;
    if (user_mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        // get ptr from OpenCL buffer for HOST
        if (true_value_ptr)
        {
            size_t size = 0;
            cl_mem opencl_buf = (cl_mem)true_value_ptr;
            cl_int cerr =
                clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdOutput: clGetMemObjectInfo(%p) => (%d)\n", opencl_buf,
                     cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            true_value_ptr = (vx_pixel_value_t *)clEnqueueMapBuffer(
                context->opencl_command_queue, opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                size, 0, nullptr, nullptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdOutput: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                     opencl_buf, (int)size, true_value_ptr, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
        }
        if (false_value_ptr)
        {
            size_t size = 0;
            cl_mem opencl_buf = (cl_mem)false_value_ptr;
            cl_int cerr =
                clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdOutput: clGetMemObjectInfo(%p) => (%d)\n", opencl_buf,
                     cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            false_value_ptr = (vx_pixel_value_t *)clEnqueueMapBuffer(
                context->opencl_command_queue, opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                size, 0, nullptr, nullptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdOutput: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                     opencl_buf, (int)size, false_value_ptr, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
        }
        user_mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    if (VX_MEMORY_TYPE_HOST == user_mem_type)
    {
        if (usage == VX_READ_ONLY)
        {
            Osal::semWait(&lock);
            vx_size size = sizeof(vx_pixel_value_t);
            if (true_value_ptr)
            {
                memcpy(true_value_ptr, &true_value, size);
            }
            if (false_value_ptr)
            {
                memcpy(false_value_ptr, &false_value, size);
            }
            Osal::semPost(&lock);
            // ownReadFromReference(&threshold);
            status = VX_SUCCESS;
        }
        else if (usage == VX_WRITE_ONLY)
        {
            Osal::semWait(&lock);
            vx_size size = sizeof(vx_pixel_value_t);
            if (true_value_ptr)
            {
                memcpy(&true_value, true_value_ptr, size);
            }
            if (false_value_ptr)
            {
                memcpy(&false_value, false_value_ptr, size);
            }
            Osal::semPost(&lock);
            // ownWroteToReference(&threshold);
            status = VX_SUCCESS;
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Wrong parameters for threshold\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to allocate threshold\n");
        status = VX_ERROR_NO_MEMORY;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (user_mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        if (true_value_ptr_given)
        {
            clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)true_value_ptr_given,
                                    true_value_ptr, 0, nullptr, nullptr);
            clFinish(context->opencl_command_queue);
        }
        if (false_value_ptr_given)
        {
            clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)false_value_ptr_given,
                                    false_value_ptr, 0, nullptr, nullptr);
            clFinish(context->opencl_command_queue);
        }
    }
#endif

    return status;
}

vx_status Threshold::copyRange(vx_pixel_value_t *lower_value_ptr, vx_pixel_value_t *upper_value_ptr,
                               vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_SUCCESS;

    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual threshold\n");
            status = VX_ERROR_OPTIMIZED_AWAY;
            return status;
        }
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_pixel_value_t *lower_value_ptr_given = lower_value_ptr;
    vx_pixel_value_t *upper_value_ptr_given = upper_value_ptr;
    vx_enum user_mem_type_given = user_mem_type;
    if (user_mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        // get ptr from OpenCL buffer for HOST
        if (lower_value_ptr)
        {
            size_t size = 0;
            cl_mem opencl_buf = (cl_mem)lower_value_ptr;
            cl_int cerr =
                clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdRange: clGetMemObjectInfo(%p) => (%d)\n", opencl_buf,
                     cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            lower_value_ptr = (vx_pixel_value_t *)clEnqueueMapBuffer(
                context->opencl_command_queue, opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                size, 0, nullptr, nullptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdRange: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                     opencl_buf, (int)size, lower_value_ptr, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
        }
        if (upper_value_ptr)
        {
            size_t size = 0;
            cl_mem opencl_buf = (cl_mem)upper_value_ptr;
            cl_int cerr =
                clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdRange: clGetMemObjectInfo(%p) => (%d)\n", opencl_buf,
                     cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
            upper_value_ptr = (vx_pixel_value_t *)clEnqueueMapBuffer(
                context->opencl_command_queue, opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                size, 0, nullptr, nullptr, &cerr);
            VX_PRINT(VX_ZONE_CONTEXT,
                     "OPENCL: vxCopyThresholdRange: clEnqueueMapBuffer(%p,%d) => %p (%d)\n",
                     opencl_buf, (int)size, upper_value_ptr, cerr);
            if (cerr != CL_SUCCESS)
            {
                return VX_ERROR_INVALID_PARAMETERS;
            }
        }
        user_mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    if (VX_MEMORY_TYPE_HOST == user_mem_type)
    {
        if (usage == VX_READ_ONLY)
        {
            Osal::semWait(&lock);
            vx_size size = sizeof(vx_pixel_value_t);
            if (lower_value_ptr)
            {
                memcpy(lower_value_ptr, &lower, size);
            }
            if (upper_value_ptr)
            {
                memcpy(upper_value_ptr, &upper, size);
            }
            Osal::semPost(&lock);
            // ownReadFromReference(&threshold);
            status = VX_SUCCESS;
        }
        else if (usage == VX_WRITE_ONLY)
        {
            Osal::semWait(&lock);
            vx_size size = sizeof(vx_pixel_value_t);
            if (lower_value_ptr)
            {
                memcpy(&lower, lower_value_ptr, size);
            }
            if (upper_value_ptr)
            {
                memcpy(&upper, upper_value_ptr, size);
            }
            Osal::semPost(&lock);
            // ownWroteToReference(&threshold);
            status = VX_SUCCESS;
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Wrong parameters for threshold\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to allocate threshold\n");
        status = VX_ERROR_NO_MEMORY;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (user_mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        if (lower_value_ptr_given)
        {
            clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)lower_value_ptr_given,
                                    lower_value_ptr, 0, nullptr, nullptr);
            clFinish(context->opencl_command_queue);
        }
        if (upper_value_ptr_given)
        {
            clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)upper_value_ptr_given,
                                    upper_value_ptr, 0, nullptr, nullptr);
            clFinish(context->opencl_command_queue);
        }
    }
#endif

    return status;
}

vx_status Threshold::copyValue(vx_pixel_value_t *value_ptr, vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_SUCCESS;

    if (is_virtual == vx_true_e)
    {
        if (is_accessible == vx_false_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Can not access a virtual threshold\n");
            status = VX_ERROR_OPTIMIZED_AWAY;
            return status;
        }
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    vx_pixel_value_t *value_ptr_given = value_ptr;
    vx_enum user_mem_type_given = user_mem_type;
    if (user_mem_type == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        // get ptr from OpenCL buffer for HOST
        size_t size = 0;
        cl_mem opencl_buf = (cl_mem)value_ptr;
        cl_int cerr = clGetMemObjectInfo(opencl_buf, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
        VX_PRINT(VX_ZONE_CONTEXT, "OPENCL: vxCopyThresholdValue: clGetMemObjectInfo(%p) => (%d)\n",
                 opencl_buf, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        value_ptr = (vx_pixel_value_t *)clEnqueueMapBuffer(
            context->opencl_command_queue, opencl_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size,
            0, nullptr, nullptr, &cerr);
        VX_PRINT(VX_ZONE_CONTEXT,
                 "OPENCL: vxCopyThresholdValue: clEnqueueMapBuffer(%p,%d) => %p (%d)\n", opencl_buf,
                 (int)size, value_ptr, cerr);
        if (cerr != CL_SUCCESS)
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }
        user_mem_type = VX_MEMORY_TYPE_HOST;
    }
#endif

    if (VX_MEMORY_TYPE_HOST == user_mem_type)
    {
        if (usage == VX_READ_ONLY)
        {
            Osal::semWait(&lock);
            vx_size size = sizeof(vx_pixel_value_t);
            if (value_ptr)
            {
                memcpy(value_ptr, &value, size);
            }
            Osal::semPost(&lock);
            // ownReadFromReference(&threshold);
            status = VX_SUCCESS;
        }
        else if (usage == VX_WRITE_ONLY)
        {
            Osal::semWait(&lock);
            vx_size size = sizeof(vx_pixel_value_t);
            if (value_ptr)
            {
                memcpy(&value, value_ptr, size);
            }
            Osal::semPost(&lock);
            // ownWroteToReference(&threshold);
            status = VX_SUCCESS;
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Wrong parameters for threshold\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to allocate threshold\n");
        status = VX_ERROR_NO_MEMORY;
    }

#ifdef OPENVX_USE_OPENCL_INTEROP
    if (user_mem_type_given == VX_MEMORY_TYPE_OPENCL_BUFFER)
    {
        clEnqueueUnmapMemObject(context->opencl_command_queue, (cl_mem)value_ptr_given, value_ptr,
                                0, nullptr, nullptr);
        clFinish(context->opencl_command_queue);
    }
#endif

    return status;
}

/*****************************************************************************/
/* PUBLIC INTERFACE                                                          */
/*****************************************************************************/

VX_API_ENTRY vx_threshold VX_API_CALL vxCreateThreshold(vx_context context, vx_enum thresh_type,
                                                        vx_enum data_type)
{
    vx_threshold threshold = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if (Threshold::isValidThresholdDataType(data_type) == vx_true_e)
        {
            if (Threshold::isValidThresholdType(thresh_type) == vx_true_e)
            {
                threshold = (vx_threshold)Reference::createReference(context, VX_TYPE_THRESHOLD,
                                                                     VX_EXTERNAL, context);
                if (vxGetStatus((vx_reference)threshold) == VX_SUCCESS &&
                    threshold->type == VX_TYPE_THRESHOLD)
                {
                    threshold->thresh_type = thresh_type;
                    threshold->data_type = data_type;
                    switch (data_type)
                    {
                        case VX_TYPE_BOOL:
                            threshold->true_value.U1 = VX_U1_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.U1 = VX_U1_THRESHOLD_FALSE_VALUE;
                            break;
                        case VX_TYPE_INT8:
                            threshold->true_value.U8 = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.U8 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                            break;
                        case VX_TYPE_UINT8:
                            threshold->true_value.U8 = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.U8 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                            break;
                        case VX_TYPE_UINT16:
                            threshold->true_value.U16 = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.U16 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                            break;
                        case VX_TYPE_INT16:
                            threshold->true_value.S16 = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.S16 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                            break;
                        case VX_TYPE_INT32:
                            threshold->true_value.S32 = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.S32 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                            break;
                        case VX_TYPE_UINT32:
                            threshold->true_value.U32 = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                            threshold->false_value.U32 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                            break;
                        default:
                            break;
                    }
                }
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid threshold type\n");
                vxAddLogEntry(context, VX_ERROR_INVALID_TYPE, "Invalid threshold type\n");
                threshold = (vx_threshold)vxGetErrorObject(context, VX_ERROR_INVALID_TYPE);
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid data type\n");
            vxAddLogEntry(context, VX_ERROR_INVALID_TYPE, "Invalid data type\n");
            threshold = (vx_threshold)vxGetErrorObject(context, VX_ERROR_INVALID_TYPE);
        }
    }

    return threshold;
}

VX_API_ENTRY vx_threshold VX_API_CALL vxCreateThresholdForImage(vx_context context,
                                                                vx_enum thresh_type,
                                                                vx_df_image input_format,
                                                                vx_df_image output_format)
{
    vx_threshold threshold = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid context\n");
        threshold = nullptr;
    }

    if (Threshold::isValidThresholdType(thresh_type) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid threshold type\n");
        vxAddLogEntry(context, VX_ERROR_INVALID_TYPE, "Invalid threshold type\n");
        threshold = (vx_threshold)vxGetErrorObject(context, VX_ERROR_INVALID_TYPE);
    }

    if ((Threshold::isValidThresholdFormat(input_format) == vx_false_e) ||
        (Threshold::isValidThresholdFormat(output_format) == vx_false_e))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid input or output format\n");
        vxAddLogEntry(context, VX_ERROR_INVALID_TYPE, "Invalid input or output format\n");
        threshold = (vx_threshold)vxGetErrorObject(context, VX_ERROR_INVALID_TYPE);
    }


    threshold = (vx_threshold)Reference::createReference(context, VX_TYPE_THRESHOLD, VX_EXTERNAL, context);
    if (vxGetStatus((vx_reference)threshold) == VX_SUCCESS && threshold->type == VX_TYPE_THRESHOLD)
    {
        threshold->thresh_type = thresh_type;
        threshold->input_format = input_format;
        threshold->output_format = output_format;
        switch (output_format)
        {
            case VX_DF_IMAGE_RGB:
            {
                threshold->data_type = VX_TYPE_DF_IMAGE;
                threshold->true_value.RGB[0] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.RGB[1] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.RGB[2] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->false_value.RGB[0] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.RGB[1] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.RGB[2] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_RGBX:
            {
                threshold->data_type = VX_TYPE_DF_IMAGE;
                threshold->true_value.RGBX[0] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.RGBX[1] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.RGBX[2] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.RGBX[3] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->false_value.RGBX[0] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.RGBX[1] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.RGBX[2] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.RGBX[3] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_NV12:
            case VX_DF_IMAGE_NV21:
            case VX_DF_IMAGE_UYVY:
            case VX_DF_IMAGE_YUYV:
            case VX_DF_IMAGE_IYUV:
            case VX_DF_IMAGE_YUV4:
            {
                threshold->data_type = VX_TYPE_DF_IMAGE;
                threshold->true_value.YUV[0] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.YUV[1] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->true_value.YUV[2] = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->false_value.YUV[0] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.YUV[1] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                threshold->false_value.YUV[2] = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_U1:
            {
                threshold->data_type = VX_TYPE_BOOL;
                threshold->true_value.U1  = VX_U1_THRESHOLD_TRUE_VALUE;
                threshold->false_value.U1 = VX_U1_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_U8:
            {
                threshold->data_type = VX_TYPE_UINT8;
                threshold->true_value.U8  = VX_DEFAULT_THRESHOLD_TRUE_VALUE;
                threshold->false_value.U8 = VX_DEFAULT_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_S16:
            {
                threshold->data_type = VX_TYPE_INT16;
                threshold->true_value.S16  = VX_S16_THRESHOLD_TRUE_VALUE;
                threshold->false_value.S16 = VX_S16_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_U16:
            {
                threshold->data_type = VX_TYPE_UINT16;
                threshold->true_value.U16  = VX_U16_THRESHOLD_TRUE_VALUE;
                threshold->false_value.U16 = VX_U16_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_S32:
            {
                threshold->data_type = VX_TYPE_INT32;
                threshold->true_value.S32  = VX_S32_THRESHOLD_TRUE_VALUE;
                threshold->false_value.S32 = VX_S32_THRESHOLD_FALSE_VALUE;
                break;
            }
            case VX_DF_IMAGE_U32:
            {
                threshold->data_type = VX_TYPE_UINT32;
                threshold->true_value.U32  = VX_U32_THRESHOLD_TRUE_VALUE;
                threshold->false_value.U32 = VX_U32_THRESHOLD_FALSE_VALUE;
                break;
            }
            default:
            {
                threshold->data_type = VX_TYPE_INVALID;
                break;
            }
        }
    }
    return threshold;
}

VX_API_ENTRY vx_threshold VX_API_CALL vxCreateVirtualThresholdForImage(vx_graph graph,
                                                                       vx_enum thresh_type,
                                                                       vx_df_image input_format,
                                                                       vx_df_image output_format)
{
    vx_threshold threshold = nullptr;
    vx_reference gref = (vx_reference)graph;
    if (Reference::isValidReference(gref, VX_TYPE_GRAPH) == vx_true_e)
    {
        threshold = vxCreateThresholdForImage(gref->context, thresh_type, input_format, output_format);
        if (vxGetStatus((vx_reference)threshold) == VX_SUCCESS && threshold->type == VX_TYPE_THRESHOLD)
        {
            threshold->scope = (vx_reference)graph;
            threshold->is_virtual = vx_true_e;
        }
        else
        {
            threshold = (vx_threshold)vxGetErrorObject(graph->context, VX_ERROR_INVALID_PARAMETERS);
        }
    }
    return threshold;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetThresholdAttribute(vx_threshold threshold,
                                                           vx_enum attribute, const void *ptr,
                                                           vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(threshold, VX_TYPE_THRESHOLD) == vx_true_e)
    {
        switch (attribute)
        {
            case VX_THRESHOLD_THRESHOLD_VALUE:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3) &&
                    (threshold->thresh_type == VX_THRESHOLD_TYPE_BINARY))
                {
                    threshold->setBinaryValue(*(vx_int32 *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3) &&
                         (threshold->thresh_type == VX_THRESHOLD_TYPE_BINARY))
                {
                    threshold->setBinaryValueUnion(*(vx_pixel_value_t *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_THRESHOLD_LOWER:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3) &&
                    (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    threshold->setLowerBound(*(vx_int32 *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3) &&
                         (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    threshold->setLowerBoundUnion(*(vx_pixel_value_t *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_THRESHOLD_UPPER:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3) &&
                    (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    threshold->setUpperBound(*(vx_int32 *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3) &&
                         (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    threshold->setUpperBoundUnion(*(vx_pixel_value_t *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_TRUE_VALUE:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3))
                {
                    threshold->setTrueValue(*(vx_int32 *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3))
                {
                    threshold->setTrueValueUnion(*(vx_pixel_value_t *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_FALSE_VALUE:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3))
                {
                    threshold->setFalseValue(*(vx_int32 *)ptr);
                    // ownWroteToReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3))
                {
                    threshold->setFalseValueUnion(*(vx_pixel_value_t *)ptr);
                    // ownWroteToReference(&threshold);
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
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }
    VX_PRINT(VX_ZONE_API, "return %d\n", status);
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryThreshold(vx_threshold threshold, vx_enum attribute,
                                                    void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(threshold, VX_TYPE_THRESHOLD) == vx_true_e)
    {
        switch (attribute)
        {
            case VX_THRESHOLD_THRESHOLD_VALUE:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3) &&
                    (threshold->thresh_type == VX_THRESHOLD_TYPE_BINARY))
                {
                    *(vx_int32 *)ptr = threshold->binaryValue();
                    // ownReadFromReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3) &&
                         (threshold->thresh_type == VX_THRESHOLD_TYPE_BINARY))
                {
                    *(vx_pixel_value_t *)ptr = threshold->binaryValueUnion();
                    // ownReadFromReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_THRESHOLD_LOWER:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3) &&
                    (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    *(vx_int32 *)ptr = threshold->lowerBound();
                    // ownReadFromReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3) &&
                         (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    *(vx_pixel_value_t *)ptr = threshold->lowerBoundUnion();
                    // ownReadFromReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_THRESHOLD_UPPER:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3) &&
                    (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    *(vx_int32 *)ptr = threshold->upperBound();
                    // ownReadFromReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3) &&
                         (threshold->thresh_type == VX_THRESHOLD_TYPE_RANGE))
                {
                    *(vx_pixel_value_t *)ptr = threshold->upperBoundUnion();
                    // ownReadFromReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_TRUE_VALUE:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3))
                {
                    *(vx_int32 *)ptr = threshold->trueValue();
                    // ownReadFromReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3))
                {
                    *(vx_pixel_value_t *)ptr = threshold->trueValueUnion();
                    // ownReadFromReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_FALSE_VALUE:
                if (VX_CHECK_PARAM(ptr, size, vx_int32, 0x3))
                {
                    *(vx_int32 *)ptr = threshold->falseValue();
                    // ownReadFromReference(&threshold);
                }
                else if (VX_CHECK_PARAM(ptr, size, vx_pixel_value_t, 0x3))
                {
                    *(vx_pixel_value_t *)ptr = threshold->falseValueUnion();
                    // ownReadFromReference(&threshold);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_DATA_TYPE:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                {
                    *(vx_enum *)ptr = threshold->dataType();
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_TYPE:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                {
                    *(vx_enum *)ptr = threshold->threshType();
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_INPUT_FORMAT:
                if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3))
                {
                    *(vx_df_image *)ptr = threshold->inputFormat();
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_THRESHOLD_OUTPUT_FORMAT:
                if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3))
                {
                    *(vx_df_image *)ptr = threshold->outputFormat();
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
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }
    VX_PRINT(VX_ZONE_API, "return %d\n", status);
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyThresholdOutput(vx_threshold threshold,
                                                         vx_pixel_value_t *true_value_ptr,
                                                         vx_pixel_value_t *false_value_ptr,
                                                         vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(threshold, VX_TYPE_THRESHOLD) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid reference for threshold\n");
        status = VX_ERROR_INVALID_REFERENCE;
        return status;
    }

    return threshold->copyOutput(true_value_ptr, false_value_ptr, usage, user_mem_type);
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyThresholdRange(vx_threshold threshold,
                                                        vx_pixel_value_t *lower_value_ptr,
                                                        vx_pixel_value_t *upper_value_ptr,
                                                        vx_enum usage, vx_enum user_mem_type)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(threshold, VX_TYPE_THRESHOLD) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid reference for threshold\n");
        status = VX_ERROR_INVALID_REFERENCE;
        return status;
    }

    return threshold->copyRange(lower_value_ptr, upper_value_ptr, usage, user_mem_type);
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyThresholdValue(vx_threshold threshold,
                                                        vx_pixel_value_t *value_ptr, vx_enum usage,
                                                        vx_enum user_mem_type)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(threshold, VX_TYPE_THRESHOLD) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid reference for threshold\n");
        status = VX_ERROR_INVALID_REFERENCE;
        return status;
    }

    return threshold->copyValue(value_ptr, usage, user_mem_type);
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseThreshold(vx_threshold *t)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != t)
    {
        vx_threshold ref = *t;
        if (vx_true_e == Reference::isValidReference(ref, VX_TYPE_THRESHOLD))
        {
            status = Reference::releaseReference((vx_reference *)t, VX_TYPE_THRESHOLD, VX_EXTERNAL,
                                                 nullptr);
        }
    }

    return status;
}
