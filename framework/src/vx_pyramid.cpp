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
#include "vx_pyramid.h"

Pyramid::Pyramid(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_PYRAMID, scope)
{

}

Pyramid::~Pyramid()
{
    destructPyramid();
}

void Pyramid::destructPyramid()
{
    vx_uint32 i = 0;
    for (i = 0; i < numLevels; i++)
    {
        if (levels[i])
        {
            Reference::releaseReference((vx_reference*)&levels[i], VX_TYPE_IMAGE, VX_INTERNAL, nullptr);
        }
    }
    free(levels);
    levels = nullptr;
}

vx_status Pyramid::initPyramid(vx_size numLevels,
                               vx_float32 scale,
                               vx_uint32 width,
                               vx_uint32 height,
                               vx_df_image format)
{
    const vx_float32 c_orbscale[4] = {
        0.5f, VX_SCALE_PYRAMID_ORB, VX_SCALE_PYRAMID_ORB * VX_SCALE_PYRAMID_ORB, VX_SCALE_PYRAMID_ORB * VX_SCALE_PYRAMID_ORB * VX_SCALE_PYRAMID_ORB
    };
    vx_status status = VX_SUCCESS;

    /* very first init will come in here */
    if (levels == nullptr)
    {
        this->numLevels = numLevels;
        this->scale = scale;
        this->levels = (vx_image *)calloc(numLevels, sizeof(vx_image));
    }

    /* these could be "virtual" values or hard values */
    this->width = width;
    this->height = height;
    this->format = format;

    if (levels)
    {
        if (width != 0 && height != 0 && format != VX_DF_IMAGE_VIRT)
        {
            vx_int32 i;
            vx_uint32 w = width;
            vx_uint32 h = height;
            vx_uint32 ref_w = width;
            vx_uint32 ref_h = height;

            for (i = 0; i < (vx_int32)numLevels; i++)
            {
                vx_context c = (vx_context)context;
                if (levels[i] == 0)
                {
                    levels[i] = vxCreateImage(c, w, h, format);

                    /* increment the internal counter on the image, not the external one */
                    levels[i]->incrementReference(VX_INTERNAL);
                    levels[i]->decrementReference(VX_EXTERNAL);

                    /* remember that the scope of the image is the pyramid */
                    levels[i]->scope = (vx_reference)this;

                    if (VX_SCALE_PYRAMID_ORB == scale)
                    {
                        vx_float32 orb_scale = c_orbscale[(i + 1) % 4];
                        w = (vx_uint32)ceilf((vx_float32)ref_w * orb_scale);
                        h = (vx_uint32)ceilf((vx_float32)ref_h * orb_scale);
                        if (0 == ((i + 1) % 4))
                        {
                            ref_w = w;
                            ref_h = h;
                        }
                    }
                    else
                    {
                        w = (vx_uint32)ceilf((vx_float32)w * scale);
                        h = (vx_uint32)ceilf((vx_float32)h * scale);
                    }
                }
            }
        }
        else
        {
            /* virtual images, but in a pyramid we really need to know the
             * level 0 value. Dimensionless images don't work after validation
             * time.
             */
        }
    }
    else
    {
        status = VX_ERROR_NO_MEMORY;
    }
    return status;
}

static vx_pyramid vxCreatePyramidInt(vx_context context,
                                      vx_size levels,
                                      vx_float32 scale,
                                      vx_uint32 width,
                                      vx_uint32 height,
                                      vx_df_image format,
                                      vx_bool is_virtual)
{
    vx_pyramid pyramid = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
    {
        /* Context is invalid, we can't get any error object,
         * we then simply return nullptr as it is handled by vxGetStatus
         */
        return nullptr;
    }

    if ((scale != VX_SCALE_PYRAMID_HALF) &&
        (scale != VX_SCALE_PYRAMID_ORB))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid scale %lf for pyramid!\n",scale);
        vxAddLogEntry((vx_reference)context, VX_ERROR_INVALID_PARAMETERS, "Invalid scale %lf for pyramid!\n",scale);
        // pyramid = (vx_pyramid_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
    }
    else if (levels == 0 || levels > 8)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid number of levels for pyramid!\n", levels);
        vxAddLogEntry((vx_reference)context, VX_ERROR_INVALID_PARAMETERS, "Invalid number of levels for pyramid!\n", levels);
        // pyramid = (vx_pyramid_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
    }
    else
    {
        pyramid = (vx_pyramid)Reference::createReference(context, VX_TYPE_PYRAMID, VX_EXTERNAL, context);
        if (vxGetStatus((vx_reference)pyramid) == VX_SUCCESS && pyramid->type == VX_TYPE_PYRAMID)
        {
            vx_status status;
            pyramid->is_virtual = is_virtual;
            status = pyramid->initPyramid(levels, scale, width, height, format);
            if (status != VX_SUCCESS)
            {
                vxAddLogEntry((vx_reference)pyramid, status, "Failed to initialize pyramid\n");
                vxReleasePyramid((vx_pyramid *)&pyramid);
                // pyramid = (vx_pyramid_t *)ownGetErrorObject(context, status);
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to allocate memory\n");
            vxAddLogEntry((vx_reference)context, VX_ERROR_NO_MEMORY, "Failed to allocate memory\n");
            // pyramid = (vx_pyramid_t *)ownGetErrorObject(context, VX_ERROR_NO_MEMORY);
        }
    }

    return pyramid;
}

VX_API_ENTRY vx_pyramid VX_API_CALL vxCreateVirtualPyramid(vx_graph graph,
                                                           vx_size levels,
                                                           vx_float32 scale,
                                                           vx_uint32 width,
                                                           vx_uint32 height,
                                                           vx_df_image format)
{
    vx_pyramid pyramid = nullptr;

    if (Reference::isValidReference(graph, VX_TYPE_GRAPH) == vx_true_e)
    {
        pyramid = vxCreatePyramidInt(graph->context, levels, scale,
                                     width, height, format,
                                     vx_true_e);
        if ( vxGetStatus((vx_reference)pyramid) == VX_SUCCESS &&
             pyramid->type == VX_TYPE_PYRAMID)
        {
            pyramid->scope = (vx_reference)graph;
        }
    }
    /* else, the graph is invalid, we can't get any context and then error object */

    return pyramid;
}

VX_API_ENTRY vx_pyramid VX_API_CALL vxCreatePyramid(vx_context context, vx_size levels, vx_float32 scale, vx_uint32 width, vx_uint32 height, vx_df_image format)
{
    vx_pyramid pyr = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        if ((width == 0) || (height == 0) || (format == VX_DF_IMAGE_VIRT))
        {
            // pyr = (vx_pyramid)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        }
        else
        {
            pyr = (vx_pyramid)vxCreatePyramidInt(context,
                                                 levels, scale, width, height, format,
                                                 vx_false_e);
        }
    }

    return pyr;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryPyramid(vx_pyramid pyramid, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(pyramid, VX_TYPE_PYRAMID) == vx_true_e)
    {
        switch (attribute)
        {
            case VX_PYRAMID_LEVELS:
                if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
                {
                    *(vx_size *)ptr = pyramid->numLevels;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_PYRAMID_SCALE:
                if (VX_CHECK_PARAM(ptr, size, vx_float32, 0x3))
                {
                    *(vx_float32 *)ptr = pyramid->scale;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_PYRAMID_WIDTH:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                {
                    *(vx_uint32 *)ptr = pyramid->width;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_PYRAMID_HEIGHT:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                {
                    *(vx_uint32 *)ptr = pyramid->height;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_PYRAMID_FORMAT:
                if (VX_CHECK_PARAM(ptr, size, vx_df_image, 0x3))
                {
                    *(vx_df_image *)ptr = pyramid->format;
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

VX_API_ENTRY vx_image VX_API_CALL vxGetPyramidLevel(vx_pyramid pyramid, vx_uint32 index)
{
    vx_image image = 0;
    if (Reference::isValidReference(pyramid, VX_TYPE_PYRAMID) == vx_true_e)
    {
        if (index < pyramid->numLevels)
        {
            image = pyramid->levels[index];
            image->incrementReference(VX_EXTERNAL);
        }
        else
        {
            vxAddLogEntry(pyramid, VX_ERROR_INVALID_PARAMETERS, "Failed to get pyramid level %d\n", index);
            // image = (vx_image_t *)ownGetErrorObject(pyramid->context, VX_ERROR_INVALID_PARAMETERS);
        }
    }
    return image;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleasePyramid(vx_pyramid* pyr)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != pyr)
    {
        vx_reference ref = *pyr;
        if (vx_true_e == Reference::isValidReference(ref, VX_TYPE_PYRAMID))
        {
            status = Reference::releaseReference((vx_reference*)pyr, VX_TYPE_PYRAMID, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}
