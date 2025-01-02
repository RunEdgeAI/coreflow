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
#include <cinttypes>

#include "vx_internal.h"
#include "vx_type_pairs.h"

#if defined(OPENVX_USE_XML)

#if defined(__APPLE__)
typedef unsigned long ulong;
#endif

#include <libxml/tree.h>

static vx_bool vxIsMemberOf(vx_enum type, vx_enum types[], vx_size numtypes)
{
    vx_bool match = vx_false_e;
    vx_uint32 t;
    for (t = 0u; t < numtypes; t++) {
        if (type == types[t]) {
            match = vx_true_e;
            break;
        }
    }
    return match;
}

static vx_bool vxIsImgInVirtPyramid(vx_reference ref)
{
    vx_bool match = vx_false_e;

    if(ref->scope && (ref->scope->type == VX_TYPE_PYRAMID) && ref->scope->is_virtual) {
        match = vx_true_e;
    }

    return match;
}

static vx_bool vxIsDelayInDelay(vx_reference ref)
{
    vx_bool match = vx_false_e;

    if(ref->scope && (ref->type == VX_TYPE_DELAY) && (ref->scope->type == VX_TYPE_DELAY)) {
        match = vx_true_e;
    }

    return match;
}

static vx_char *vxFourCCString(vx_df_image format)
{
    static vx_char code[5];
#if defined(_LITTLE_ENDIAN_) || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32) || defined(WIN32)
    code[0] = (format >> 0)  & 0xFF;
    code[1] = (format >> 8)  & 0xFF;
    code[2] = (format >> 16) & 0xFF;
    code[3] = (format >> 24) & 0xFF;
#elif defined(_BIG_ENDIAN_) || (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    code[0] = (format >> 24) & 0xFF;
    code[1] = (format >> 16) & 0xFF;
    code[2] = (format >> 8)  & 0xFF;
    code[3] = (format >> 0)  & 0xFF;
#else
#error "Endian-ness must be defined!"
#endif
    code[4] = '\0';
    return code;
}

static vx_char refNameStr[VX_MAX_REFERENCE_NAME+10];

static void vxGetName(vx_reference ref)
{
    refNameStr[0] = 0;

    if (ref->name[0])
    {
        snprintf(refNameStr, sizeof(refNameStr), " name=\"%s\"", ref->name);
    }
}

static void vxComputeROIStartXY(vx_image parent, vx_image roi, vx_uint32 p, vx_uint32 *x, vx_uint32 *y)
{
    vx_uint32 offset = roi->memory.ptrs[p] - parent->memory.ptrs[p];
    *y = offset * roi->scale[p][VX_DIM_Y] / roi->memory.strides[p][VX_DIM_Y];
    *x = (offset - ((*y * roi->memory.strides[p][VX_DIM_Y]) / roi->scale[p][VX_DIM_Y]) )
                * roi->scale[p][VX_DIM_X] / roi->memory.strides[p][VX_DIM_X];
}

static vx_status vxExportToXMLROI(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id, vx_uint32 numrefs)
{
    vx_status status = VX_SUCCESS;
    vx_image image = (vx_image)refs[r];
    vx_char indent[10] = {0};
    vx_uint32 i, start_x, start_y;
    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    vxComputeROIStartXY(image->parent, image, 0, &start_x, &start_y);
    fprintf(fp, "%s<roi reference=\"%u\" start_x=\"%u\" start_y=\"%u\" end_x=\"%u\" end_y=\"%u\"%s",
                    indent, r, start_x, start_y, start_x+image->width, start_y+image->height, refNameStr);

    if (refs[r]->is_virtual == vx_true_e)
    {
        fprintf(fp, " />\n");
    } else {
        vx_uint32 r2 = 0;
        vx_uint32 roiFound = 0;

        /* List ROIs, if any */
        for (r2 = 0u; r2 < numrefs; r2++)
        {
            if ((vx_reference)image == refs[r2]->scope)
            {
                if(roiFound == 0) {
                    roiFound = 1;
                    fprintf(fp, ">\n");
                }
                vxGetName(refs[r2]);
                status |= vxExportToXMLROI(fp, refs, r2, id+1, numrefs);
            }
        }
        if(roiFound)
            fprintf(fp, "%s</roi>\n",indent);
        else
            fprintf(fp, " />\n");
    }
    return status;
}

static vx_status vxExportToXMLImage(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id, vx_uint32 numrefs)
{
    vx_status status = VX_SUCCESS;
    vx_image image = (vx_image)refs[r];
    vx_char indent[10] = {0};
    vx_uint32 i;
    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<image reference=\"%u\" width=\"%u\" height=\"%u\" format=\"%s\"%s",
                    indent, r, image->width, image->height, vxFourCCString(image->format), refNameStr);

    if (refs[r]->is_virtual == vx_true_e)
    {
        fprintf(fp, " />\n");
    } else {
        vx_uint32 r2 = 0;
        fprintf(fp, ">\n");

        /* List ROIs, if any */
        for (r2 = 0u; r2 < numrefs; r2++)
        {
            if ((vx_reference)image == refs[r2]->scope)
            {
                vxGetName(refs[r2]);
                status |= vxExportToXMLROI(fp, refs, r2, id+1, numrefs);
            }
        }

        if (vx_true_e == image->constant)
        {
            vx_uint32 p;
            vx_rectangle_t rect;
            vx_imagepatch_addressing_t addr;
            vxGetValidRegionImage((vx_image)image, &rect);
            fprintf(fp, "%s\t<uniform>\n", indent);
            for (p = 0u; p < image->planes; p++)
            {
                void *base = nullptr;
                status |= vxAccessImagePatch((vx_image)image, &rect, p, &addr, &base, VX_READ_ONLY);

                if (image->format == VX_DF_IMAGE_U8)
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<uint8>%hhu</uint8>\n",
                            indent, *ptr);
                }
                else if (image->format == VX_DF_IMAGE_S16)
                {
                    vx_int16 *ptr = (vx_int16*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<int16>%hd</int16>\n",
                            indent, *ptr);
                }
                else if (image->format == VX_DF_IMAGE_U16)
                {
                    vx_uint16 *ptr = (vx_uint16*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<uint16>%hu</uint16>\n",
                            indent, *ptr);
                }
                else if (image->format == VX_DF_IMAGE_S32)
                {
                    vx_int32 *ptr = (vx_int32*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<int32>%d</int32>\n",
                            indent, *ptr);
                }
                else if (image->format == VX_DF_IMAGE_U32)
                {
                    vx_uint32 *ptr = (vx_uint32*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<uint32>%u</uint32>\n",
                            indent, *ptr);
                }
                else if (image->format == VX_DF_IMAGE_RGB)
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<rgb>#%02x%02x%02x</rgb>\n",
                            indent, ptr[0], ptr[1], ptr[2]);
                }
                else if (image->format == VX_DF_IMAGE_RGBX)
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<rgba>#%02x%02x%02x%02x</rgba>\n",
                            indent, ptr[0], ptr[1], ptr[2], ptr[3]);
                }
                else if (image->format == VX_DF_IMAGE_UYVY )
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<yuv>%hhu %hhu %hhu </yuv>\n",
                            indent, ptr[1], ptr[0], ptr[2]);
                }
                else if (image->format == VX_DF_IMAGE_YUYV )
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    fprintf(fp, "%s\t\t<yuv>%hhu %hhu %hhu </yuv>\n",
                            indent, ptr[0], ptr[1], ptr[3]);
                }
                else if (image->format == VX_DF_IMAGE_YUV4 ||
                          image->format == VX_DF_IMAGE_IYUV)
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    if(p == 0) {
                        fprintf(fp, "%s\t\t<yuv>", indent);
                    }

                    fprintf(fp, "%hhu ", ptr[0]);

                    if(p == 2) {
                        fprintf(fp, "</yuv>\n");
                    }
                }
                else if ((image->format == VX_DF_IMAGE_NV12) ||
                          (image->format == VX_DF_IMAGE_NV21))
                {
                    vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, 0, 0, &addr);
                    if(p == 0)
                    {
                        fprintf(fp, "%s\t\t<yuv>%hhu ", indent, ptr[0]);
                    }
                    else
                    {
                        if(image->format == VX_DF_IMAGE_NV12)
                        {
                            fprintf(fp, "%hhu %hhu </yuv>\n", ptr[0], ptr[1]);
                        }
                        else
                        {
                            fprintf(fp, "%hhu %hhu </yuv>\n", ptr[1], ptr[0]);
                        }
                    }
                }
                status |= vxCommitImagePatch((vx_image)image, nullptr, 0, &addr, base);
            }
            fprintf(fp, "%s\t</uniform>\n", indent);
        }
        else if (image->write_count > 0)
        {
            vx_uint32 p, x, y;
            vx_rectangle_t rect;
            vx_imagepatch_addressing_t addr;
            vxGetValidRegionImage((vx_image)image, &rect);
            for (p = 0u; p < image->planes; p++)
            {
                void *base = nullptr;
                fprintf(fp, "%s\t<rectangle plane=\"%u\">\n", indent, p);
                fprintf(fp, "%s\t\t<start_x>%u</start_x>\n", indent, rect.start_x);
                fprintf(fp, "%s\t\t<start_y>%u</start_y>\n", indent, rect.start_y);
                fprintf(fp, "%s\t\t<end_x>%u</end_x>\n", indent, rect.end_x);
                fprintf(fp, "%s\t\t<end_y>%u</end_y>\n", indent, rect.end_y);
                fprintf(fp, "%s\t\t<pixels>\n", indent);
                status |= vxAccessImagePatch((vx_image)image, &rect, p, &addr, &base, VX_READ_ONLY);
                for (y = 0u; y < addr.dim_y; y+=addr.step_y)
                {
                    for (x = 0u; x < addr.dim_x; x+=addr.step_x)
                    {
                        if ((image->format == VX_DF_IMAGE_U8) ||
                            (image->format == VX_DF_IMAGE_YUV4) ||
                            (image->format == VX_DF_IMAGE_IYUV))
                        {
                            vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<uint8 x=\"%u\" y=\"%u\">%hhu</uint8>\n",
                                    indent, x, y, *ptr);
                        }
                        else if (image->format == VX_DF_IMAGE_S16)
                        {
                            vx_int16 *ptr = (vx_int16*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<int16 x=\"%u\" y=\"%u\">%hd</int16>\n",
                                    indent, x, y, *ptr);
                        }
                        else if (image->format == VX_DF_IMAGE_U16)
                        {
                            vx_uint16 *ptr = (vx_uint16*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<uint16 x=\"%u\" y=\"%u\">%hu</uint16>\n",
                                    indent, x, y, *ptr);
                        }
                        else if (image->format == VX_DF_IMAGE_S32)
                        {
                            vx_int32 *ptr = (vx_int32*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<int32 x=\"%u\" y=\"%u\">%d</int32>\n",
                                    indent, x, y, *ptr);
                        }
                        else if (image->format == VX_DF_IMAGE_U32)
                        {
                            vx_uint32 *ptr = (vx_uint32*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<uint32 x=\"%u\" y=\"%u\">%u</uint32>\n",
                                    indent, x, y, *ptr);
                        }
                        else if (image->format == VX_DF_IMAGE_RGB)
                        {
                            vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<rgb x=\"%u\" y=\"%u\">#%02x%02x%02x</rgb>\n",
                                    indent, x, y, ptr[0], ptr[1], ptr[2]);
                        }
                        else if (image->format == VX_DF_IMAGE_RGBX)
                        {
                            vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<rgba x=\"%u\" y=\"%u\">#%02x%02x%02x%02x</rgba>\n",
                                    indent, x, y, ptr[0], ptr[1], ptr[2], ptr[3]);
                        }
                        else if (image->format == VX_DF_IMAGE_UYVY ||
                                  image->format == VX_DF_IMAGE_YUYV)
                        {
                            vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                            fprintf(fp, "%s\t\t\t<yuv x=\"%u\" y=\"%u\">%hhu %hhu</yuv>\n",
                                    indent, x, y, ptr[0], ptr[1]);
                        }
                        else if ((image->format == VX_DF_IMAGE_NV12) ||
                                  (image->format == VX_DF_IMAGE_NV21))
                        {
                            if (p == 0)
                            {
                                vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                                fprintf(fp, "%s\t\t\t<uint8 x=\"%u\" y=\"%u\">%hhu</uint8>\n",
                                        indent, x, y, *ptr);
                            }
                            else
                            {
                                vx_uint8 *ptr = (vx_uint8*)vxFormatImagePatchAddress2d(base, x, y, &addr);
                                fprintf(fp, "%s\t\t\t<yuv x=\"%u\" y=\"%u\">%hhu %hhu</yuv>\n",
                                        indent, x, y, ptr[0], ptr[1]);
                            }
                        }
                    }
                }
                status |= vxCommitImagePatch((vx_image)image, nullptr, p, &addr, base);
                fprintf(fp, "%s\t\t</pixels>\n", indent);
                fprintf(fp, "%s\t</rectangle>\n", indent);
            }
        }
        fprintf(fp, "%s</image>\n",indent);
    }
    return status;
}

static vx_status vxExportToXMLPyramid(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id, vx_uint32 numrefs)
{
    vx_status status = VX_SUCCESS;
    vx_pyramid pyr = (vx_pyramid)refs[r];
    vx_uint32 level = 0;
    vx_char indent[10] = {0};
    vx_uint32 i;
    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<pyramid reference=\"%u\" width=\"%u\" height=\"%u\" format=\"%s\" scale=\"%f\" levels=\"" VX_FMT_SIZE "\"%s",
                    indent, r, pyr->width, pyr->height, vxFourCCString(pyr->format), pyr->scale, pyr->numLevels, refNameStr);

    if (refs[r]->is_virtual == vx_true_e)
    {
        fprintf(fp, " />\n");
    } else {
        fprintf(fp, ">\n");

        for (level = 0u; level < pyr->numLevels; level++)
        {
            vx_uint32 r2 = 0;
            for (r2 = 0u; r2 < numrefs; r2++)
            {
                if (refs[r2] == (vx_reference)pyr->levels[level])
                {
                    vxGetName(refs[r2]);
                    status |= vxExportToXMLImage(fp, refs, r2, id+1, numrefs);
                }
            }
        }
        fprintf(fp, "%s</pyramid>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLArray(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_array array = (vx_array)refs[r];
    vx_int32 j = TypePairs::stringFromType(array->item_type);
    vx_char indent[10] = {0};
    vx_uint32 i;
    vx_bool skipDataWrite = vx_false_e;
    vx_bool isUserType = vx_false_e;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<array reference=\"%u\" capacity=\"" VX_FMT_SIZE "\" elemType=",
                    indent, r, array->capacity);

    if(j == -1) { /* Type was not found, check if it is a user type */
        if(array->item_type >= VX_TYPE_USER_STRUCT_START &&
           array->item_type < VX_TYPE_USER_STRUCT_START+VX_INT_MAX_USER_STRUCTS) {
            fprintf(fp, "\"USER_STRUCT_%d\"", array->item_type-VX_TYPE_USER_STRUCT_START);
            isUserType = vx_true_e;
        } else {
            /* Since type is not valid, set flag to not write out data */
            skipDataWrite = vx_true_e;
            fprintf(fp, "\"%s\"", type_pairs[0].name); /* INVALID type */
        }
    } else {
        fprintf(fp, "\"%s\"", type_pairs[j].name);
    }

    if (refs[r]->is_virtual == vx_true_e || skipDataWrite == vx_true_e)
    {
        fprintf(fp, "%s />\n", refNameStr);
    } else {
        fprintf(fp, "%s>\n", refNameStr);

        if (refs[r]->write_count > 0)
        {
            switch (array->item_type)
            {
                case VX_TYPE_CHAR:
                {
                    vx_char *ptr = (vx_char *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<char>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        /* character array is a string in xsd, not space-separated as lists are */
                        fprintf(fp, "%c",  ptr[j]);
                    }
                    fprintf(fp, "</char>\n");
                    break;
                }
                case VX_TYPE_INT8:
                {
                    vx_int8 *ptr = (vx_int8 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<int8>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%hhd ",  ptr[j]);
                    }
                    fprintf(fp, "</int8>\n");
                    break;
                }
                case VX_TYPE_INT16:
                {
                    vx_int16 *ptr = (vx_int16 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<int16>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%hd ",  ptr[j]);
                    }
                    fprintf(fp, "</int16>\n");
                    break;
                }
                case VX_TYPE_INT32:
                {
                    vx_int32 *ptr = (vx_int32 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<int32>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%d ",  ptr[j]);
                    }
                    fprintf(fp, "</int32>\n");
                    break;
                }
                case VX_TYPE_INT64:
                {
                    vx_int64 *ptr = (vx_int64 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<int64>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%" PRId64 " ",  ptr[j]);
                    }
                    fprintf(fp, "</int64>\n");
                    break;
                }
                case VX_TYPE_UINT8:
                {
                    vx_uint8 *ptr = (vx_uint8 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<uint8>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%hhu ",  ptr[j]);
                    }
                    fprintf(fp, "</uint8>\n");
                    break;
                }
                case VX_TYPE_UINT16:
                {
                    vx_uint16 *ptr = (vx_uint16 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<uint16>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%hu ",  ptr[j]);
                    }
                    fprintf(fp, "</uint16>\n");
                    break;
                }
                case VX_TYPE_UINT32:
                {
                    vx_uint32 *ptr = (vx_uint32 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<uint32>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%u ",  ptr[j]);
                    }
                    fprintf(fp, "</uint32>\n");
                    break;
                }
                case VX_TYPE_UINT64:
                {
                    vx_uint64 *ptr = (vx_uint64 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<uint64>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%" PRIu64 " ",  ptr[j]);
                    }
                    fprintf(fp, "</uint64>\n");
                    break;
                }
                case VX_TYPE_FLOAT32:
                {
                    vx_float32 *ptr = (vx_float32 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<float32>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%f ",  ptr[j]);
                    }
                    fprintf(fp, "</float32>\n");
                    break;
                }
                case VX_TYPE_FLOAT64:
                {
                    vx_float64 *ptr = (vx_float64 *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<float64>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%lf ",  ptr[j]);
                    }
                    fprintf(fp, "</float64>\n");
                    break;
                }
                case VX_TYPE_ENUM:
                {
                    vx_enum *ptr = (vx_enum *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<enum>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%d ",  ptr[j]);
                    }
                    fprintf(fp, "</enum>\n");
                    break;
                }
                case VX_TYPE_BOOL:
                {
                    vx_bool *ptr = (vx_bool *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<bool>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%s ",  (ptr[j]?"true":"false"));
                    }
                    fprintf(fp, "</bool>\n");
                    break;
                }
                case VX_TYPE_DF_IMAGE:
                {
                    vx_df_image *ptr = (vx_df_image *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<df_image>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%s ",  vxFourCCString(ptr[j]));
                    }
                    fprintf(fp, "</df_image>\n");
                    break;
                }
                case VX_TYPE_SIZE:
                {
                    vx_size *ptr = (vx_size *)array->memory.ptrs[0];
                    fprintf(fp, "%s\t<size>", indent);
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%zu ",  ptr[j]);
                    }
                    fprintf(fp, "</size>\n");
                    break;
                }
                case VX_TYPE_RECTANGLE:
                {
                    vx_rectangle_t *rect = (vx_rectangle_t *)array->memory.ptrs[0];
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%s\t<rectangle>\n", indent);
                        fprintf(fp, "%s\t\t<start_x>%u</start_x>\n", indent, rect[j].start_x);
                        fprintf(fp, "%s\t\t<start_y>%u</start_y>\n", indent, rect[j].start_y);
                        fprintf(fp, "%s\t\t<end_x>%u</end_x>\n", indent, rect[j].end_x);
                        fprintf(fp, "%s\t\t<end_y>%u</end_y>\n", indent, rect[j].end_y);
                        fprintf(fp, "%s\t</rectangle>\n", indent);
                    }
                    break;
                }
                case VX_TYPE_KEYPOINT:
                {
                    vx_keypoint_t *key = (vx_keypoint_t *)array->memory.ptrs[0];
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%s\t<keypoint>\n", indent);
                        fprintf(fp, "%s\t\t<x>%u</x>\n", indent, key[j].x);
                        fprintf(fp, "%s\t\t<y>%u</y>\n", indent, key[j].y);
                        fprintf(fp, "%s\t\t<strength>%f</strength>\n", indent, key[j].strength);
                        fprintf(fp, "%s\t\t<scale>%f</scale>\n", indent, key[j].scale);
                        fprintf(fp, "%s\t\t<orientation>%f</orientation>\n", indent, key[j].orientation);
                        fprintf(fp, "%s\t\t<tracking_status>%u</tracking_status>\n", indent, key[j].tracking_status);
                        fprintf(fp, "%s\t\t<error>%f</error>\n", indent, key[j].error);
                        fprintf(fp, "%s\t</keypoint>\n", indent);
                    }
                    break;
                }
                case VX_TYPE_COORDINATES2D:
                {
                    vx_coordinates2d_t *cord2d = (vx_coordinates2d_t *)array->memory.ptrs[0];
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%s\t<coordinates2d>\n", indent);
                        fprintf(fp, "%s\t\t<x>%u</x>\n", indent, cord2d[j].x);
                        fprintf(fp, "%s\t\t<y>%u</y>\n", indent, cord2d[j].y);
                        fprintf(fp, "%s\t</coordinates2d>\n", indent);
                    }
                    break;
                }
                case VX_TYPE_COORDINATES3D:
                {
                    vx_coordinates3d_t *cord3d = (vx_coordinates3d_t *)array->memory.ptrs[0];
                    for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                        fprintf(fp, "%s\t<coordinates3d>\n", indent);
                        fprintf(fp, "%s\t\t<x>%u</x>\n", indent, cord3d[j].x);
                        fprintf(fp, "%s\t\t<y>%u</y>\n", indent, cord3d[j].y);
                        fprintf(fp, "%s\t\t<z>%u</z>\n", indent, cord3d[j].z);
                        fprintf(fp, "%s\t</coordinates3d>\n", indent);
                    }
                    break;
                }
                default:
                    if(isUserType) {
                        vx_uint8 *ptr = (vx_uint8 *)array->memory.ptrs[0];
                        for (j = 0; j < static_cast<vx_int32>(array->num_items); j++) {
                            fprintf(fp, "%s\t<user>", indent);
                            for (i = 0; i < array->item_size; i++) {
                                fprintf(fp, "%hhu ", ptr[j*array->item_size+i]);
                            }
                            fprintf(fp, "</user>\n");
                        }
                    } else {
                        status = VX_FAILURE;
                    }
                    break;
            }
        }
        fprintf(fp, "%s</array>\n", indent);
    }
    return status;
}


static vx_status vxExportToXMLLut(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_lut lut = (vx_lut)refs[r];
    vx_int32 j = TypePairs::stringFromType(lut->item_type);
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<lut reference=\"%u\" count=\"" VX_FMT_SIZE "\" elemType=\"%s\"%s",
                    indent, r, lut->num_items, type_pairs[j].name, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual LUT not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        if (refs[r]->write_count > 0)
        {
            for (j = 0; j < static_cast<vx_int32>(lut->num_items); j++)
            {
                vx_uint8 *ptr = (vx_uint8 *)lut->memory.ptrs[0];
                fprintf(fp, "%s\t<uint8 index=\"%u\">%hhu</uint8>\n",
                        indent, j, ptr[j]);
            }
        }
        fprintf(fp, "%s</lut>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLMatrix(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_matrix mat = (vx_matrix)refs[r];
    vx_size ci,ri;
    vx_int32 j = TypePairs::stringFromType(mat->data_type);
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<matrix reference=\"%u\" elemType=\"%s\" rows=\"%zu\" columns=\"%zu\"%s",
                    indent, r, type_pairs[j].name, mat->rows, mat->columns, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual Matrix not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        if (refs[r]->write_count > 0)
        {
            for (ri = 0ul; ri < mat->rows; ri++)
            {
                for (ci = 0ul; ci < mat->columns; ci++)
                {
                    if (mat->data_type == VX_TYPE_INT32)
                    {
                        vx_int32 *ptr = (vx_int32 *)mat->memory.ptrs[0];
                        vx_int32 value = ptr[ri * mat->columns + ci];
                        fprintf(fp, "%s\t<int32 row=\"%zu\" column=\"%zu\">%d</int32>\n",
                                indent, ri, ci, value);
                    }
                    else if (mat->data_type == VX_TYPE_FLOAT32)
                    {
                        vx_float32 *ptr = (vx_float32 *)mat->memory.ptrs[0];
                        vx_float32 value = ptr[ri * mat->columns + ci];
                        fprintf(fp, "%s\t<float32 row=\"%zu\" column=\"%zu\">%f</float32>\n",
                                indent, ri, ci, value);
                    }
                }
            }
        }
        fprintf(fp, "%s</matrix>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLConvolution(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_convolution conv = (vx_convolution)refs[r];
    vx_size ci,ri;
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<convolution reference=\"%u\" rows=\"%zu\" columns=\"%zu\" scale=\"%u\"%s",
                 indent, r, conv->rows, conv->columns, conv->scale, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual Convolution not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        if (refs[r]->write_count > 0)
        {
            for (ri = 0ul; ri < conv->rows; ri++)
            {
                for (ci = 0ul; ci < conv->columns; ci++)
                {
                    if (conv->data_type == VX_TYPE_INT16)
                    {
                        vx_int16 *ptr = (vx_int16 *)conv->memory.ptrs[0];
                        vx_int16 value = ptr[ri * conv->columns + ci];
                        fprintf(fp, "%s\t<int16 row=\"%zu\" column=\"%zu\">%hd</int16>\n",
                                indent, ri, ci, value);
                    }
                }
            }
        }
        fprintf(fp, "%s</convolution>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLDistribution(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_distribution dist = (vx_distribution)refs[r];
    vx_uint32 b;
    vx_uint32 bins = (vx_uint32)dist->memory.dims[0][VX_DIM_X];
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<distribution reference=\"%u\" bins=\"%u\" offset=\"%u\" range=\"%u\"%s",
            indent, r, bins, dist->offset_x, dist->range_x, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual Distribution not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        if (refs[r]->write_count > 0)
        {
            for (b = 0; b < bins; b++)
            {
                vx_int32 *ptr = (vx_int32*)Memory::formatMemoryPtr(&dist->memory, 0, b, 0, 0);
                fprintf(fp, "%s\t<frequency bin=\"%u\">%d</frequency>\n", indent, b, *ptr);
            }
        }
        fprintf(fp, "%s</distribution>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLRemap(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_remap remap = (vx_remap)refs[r];
    vx_uint32 x, y;
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<remap reference=\"%u\" src_width=\"%u\" src_height=\"%u\" dst_width=\"%u\" dst_height=\"%u\"%s",
            indent, r, remap->src_width, remap->src_height, remap->dst_width, remap->dst_height, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual Remap not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        if (refs[r]->write_count > 0)
        {
            for (y = 0u; y < remap->dst_height; y++)
            {
                for (x = 0u; x < remap->dst_width; x++)
                {
                    vx_float32 *coords[] = {
                         (vx_float32 *)Memory::formatMemoryPtr(&remap->memory, 0, x, y, 0),
                         (vx_float32 *)Memory::formatMemoryPtr(&remap->memory, 1, x, y, 0),
                    };
                    vx_float32 src_x = *coords[0];
                    vx_float32 src_y = *coords[1];
                    fprintf(fp, "%s\t<point src_x=\"%lf\" src_y=\"%lf\" dst_x=\"%u\" dst_y=\"%u\" />\n",
                            indent, src_x, src_y, x, y);
                }
            }
        }
        fprintf(fp, "%s</remap>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLThreshold(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_threshold thresh = (vx_threshold)refs[r];
    vx_int32 j = TypePairs::stringFromType(thresh->data_type);
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<threshold reference=\"%u\" elemType=\"%s\"%s",
                 indent, r, type_pairs[j].name, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual Threshold not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        if (thresh->thresh_type == VX_THRESHOLD_TYPE_RANGE)
        {
            fprintf(fp, "%s\t<range ", indent);
            switch(thresh->data_type)
            {
                case VX_TYPE_UINT8:
                    fprintf(fp, "lower=\"%u\" upper=\"%u\"", thresh->lower.U8, thresh->upper.U8);
                    break;
                case VX_TYPE_INT16:
                    fprintf(fp, "lower=\"%d\" upper=\"%d\"", thresh->lower.S16, thresh->upper.S16);
                    break;
                case VX_TYPE_UINT16:
                    fprintf(fp, "lower=\"%u\" upper=\"%u\"", thresh->lower.U16, thresh->upper.U16);
                    break;
                case VX_TYPE_INT32:
                    fprintf(fp, "lower=\"%d\" upper=\"%d\"", thresh->lower.S32, thresh->upper.S32);
                    break;
                case VX_TYPE_UINT32:
                    fprintf(fp, "lower=\"%u\" upper=\"%u\"", thresh->lower.U32, thresh->upper.U32);
                    break;
                default:
                    // Handle unexpected types or add more cases as needed
                    fprintf(fp, "lower=\"ERROR\" upper=\"ERROR\"");
                    status = VX_ERROR_INVALID_TYPE;
            }
            fprintf(fp, " />\n");
        }
        else if (thresh->thresh_type == VX_THRESHOLD_TYPE_BINARY)
        {
            fprintf(fp, "%s\t<binary>", indent);
            switch(thresh->data_type)
            {
                case VX_TYPE_UINT8:
                    fprintf(fp, "%u", thresh->value.U8);
                    break;
                case VX_TYPE_INT16:
                    fprintf(fp, "%d", thresh->value.S16);
                    break;
                case VX_TYPE_UINT16:
                    fprintf(fp, "%u", thresh->value.U16);
                    break;
                case VX_TYPE_INT32:
                    fprintf(fp, "%d", thresh->value.S32);
                    break;
                case VX_TYPE_UINT32:
                    fprintf(fp, "%u", thresh->value.U32);
                    break;
                default:
                    fprintf(fp, "ERROR");
                    status = VX_ERROR_INVALID_TYPE;
            }
            fprintf(fp, "</binary>\n");
        }
        fprintf(fp, "%s</threshold>\n", indent);
    }
    return status;
}

static vx_status vxExportToXMLScalar(FILE* fp, vx_reference refs[], vx_uint32 r, vx_uint32 id)
{
    vx_status status = VX_SUCCESS;
    vx_scalar scalar = (vx_scalar)refs[r];
    vx_char indent[10] = {0};
    vx_uint32 i;

    for (i = 0; i < (dimof(indent)-1) && i < id; i++)
        indent[i] = '\t';
    indent[i] = '\0';

    fprintf(fp, "%s<scalar reference=\"%u\" elemType=\"%s\"%s",
            indent, r, type_pairs[TypePairs::stringFromType(scalar->data_type)].name, refNameStr);

    if (refs[r]->is_virtual == vx_true_e) /* is not virtual in 1.0, but check anyway */
    {
        fprintf(fp, " />\n");
        fprintf(fp, "%s<!-- Virtual Scalar not supported in OpenVX 1.0 spec -->\n", indent);
    } else {
        fprintf(fp, ">\n");

        switch (scalar->data_type)
        {
            case VX_TYPE_CHAR:
                fprintf(fp, "%s\t<char>%c</char>\n", indent, scalar->data.chr);
                break;
            case VX_TYPE_INT8:
                fprintf(fp, "%s\t<int8>%hhd</int8>\n", indent, scalar->data.s08);
                break;
            case VX_TYPE_INT16:
                fprintf(fp, "%s\t<int16>%hd</int16>\n", indent, scalar->data.s16);
                break;
            case VX_TYPE_INT32:
                fprintf(fp, "%s\t<int32>%d</int32>\n", indent, scalar->data.s32);
                break;
            case VX_TYPE_INT64:
                fprintf(fp, "%s\t<int64>%" PRId64 "</int64>\n", indent, scalar->data.s64);
                break;
            case VX_TYPE_UINT8:
                fprintf(fp, "%s\t<uint8>%hhu</uint8>\n", indent, scalar->data.u08);
                break;
            case VX_TYPE_UINT16:
                fprintf(fp, "%s\t<uint16>%hu</uint16>\n", indent, scalar->data.u16);
                break;
            case VX_TYPE_UINT32:
                fprintf(fp, "%s\t<uint32>%u</uint32>\n", indent, scalar->data.u32);
                break;
            case VX_TYPE_UINT64:
                fprintf(fp, "%s\t<uint64>%" PRIu64 "</uint64>\n", indent, scalar->data.u64);
                break;
            case VX_TYPE_FLOAT32:
                fprintf(fp, "%s\t<float32>%f</float32>\n", indent, scalar->data.f32);
                break;
            case VX_TYPE_FLOAT64:
                fprintf(fp, "%s\t<float64>%lf</float64>\n", indent, scalar->data.f64);
                break;
            case VX_TYPE_ENUM:
                fprintf(fp, "%s\t<enum>%d</enum>\n", indent, scalar->data.enm);
                break;
            case VX_TYPE_BOOL:
                fprintf(fp, "%s\t<bool>%s</bool>\n", indent, (scalar->data.boolean?"true":"false"));
                break;
            case VX_TYPE_DF_IMAGE:
                fprintf(fp, "%s\t<df_image>%s</df_image>\n", indent, vxFourCCString(scalar->data.fcc));
                break;
            case VX_TYPE_SIZE:
                fprintf(fp, "%s\t<size>%zu</size>\n", indent, scalar->data.size);
                break;
            default:
                break;
        }
        fprintf(fp, "%s</scalar>\n", indent);
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxExportToXML(vx_context context, vx_char xmlfile[])
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    FILE *fp = nullptr;
    vx_uint32 r, r1, numrefs = 0u;
    /* these types don't get exported */
    vx_enum skipTypes[] = {
            VX_TYPE_ERROR,
            VX_TYPE_KERNEL,
            VX_TYPE_TARGET,
            VX_TYPE_PARAMETER,
            VX_TYPE_CONTEXT,
            VX_TYPE_IMPORT,
    };
    std::unique_ptr<vx_reference[]> refs = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
        return VX_ERROR_INVALID_REFERENCE;

    /* count the number of objects to export */
    for (r = 0u, r1 = 0u; r1 < context->num_references; r++)
    {
        if (context->reftable[r]) {
            r1++;
            if(vxIsMemberOf(context->reftable[r]->type, skipTypes, dimof(skipTypes)) == vx_true_e ||
               vxIsImgInVirtPyramid(context->reftable[r]) == vx_true_e  ||
               vxIsDelayInDelay(context->reftable[r]) == vx_true_e)
                continue;
            numrefs++;
        }
    }

    /* check the number */
    if (numrefs == 0 || numrefs > VX_INT_MAX_REF)
        return VX_ERROR_NOT_SUPPORTED;

    /* create the temp renamer */
    refs = std::make_unique<vx_reference[]>(numrefs);
    if (!refs)
        return VX_ERROR_NO_MEMORY;
    else
    {
        vx_uint32 r2;
        /* populate the refs */
        for (r = 0u, r1 = 0u, r2 = 0u; r1 < context->num_references; r2++)
        {
            if (context->reftable[r2]) {
                r1++;
                if (vxIsMemberOf(context->reftable[r2]->type, skipTypes, dimof(skipTypes)) == vx_true_e ||
                    vxIsImgInVirtPyramid(context->reftable[r2]) == vx_true_e ||
                    vxIsDelayInDelay(context->reftable[r2]) == vx_true_e)
                    continue;
                refs[r++] = context->reftable[r2];
            }
            /* "r" or the index in this list is the reference "index" */
        }
    }

    /* open the XML output */
    fp = fopen(xmlfile, "w+");
    if (fp)
    {
        status = VX_SUCCESS;
        fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                    "<openvx xmlns:xsi=\"https://www.w3.org/TR/xmlschema-1\"\n"
                    "        xmlns=\"https://www.khronos.org/registry/vx/schema\"\n"
                    "        xsi:schemaLocation=\"https://registry.khronos.org/OpenVX/schema/openvx-1-1.xsd\"\n"
                    "        references=\"%u\">\n", numrefs);

        for (r = 0u; r < context->num_modules; r++)
        {
            fprintf(fp, "\t<library>%s</library>\n", context->modules[r].name);
        }

        for (r = 0u; r < VX_INT_MAX_USER_STRUCTS; r++)
        {
            if (context->user_structs[r].type != VX_TYPE_INVALID)
            {
                fprintf(fp, "\t<struct size=\"%d\">USER_STRUCT_%d</struct>\n",
                        (vx_int32)context->user_structs[r].size, r);
            }
        }

        for (r = 0u; r < numrefs; r++)
        {
            if (refs[r] == nullptr) continue;
            vxGetName(refs[r]);
            if (refs[r]->type == VX_TYPE_GRAPH)
            {
                vx_uint32 n, p, r2, r3;
                vx_graph graph = (vx_graph)refs[r];
                /* do nodes and virtuals in here. */
                fprintf(fp, "\t<graph reference=\"%u\"%s>\n", r, refNameStr);
                for (n = 0u; n < graph->numNodes; n++)
                {
                    for (r2 = 0u; r2 < numrefs; r2++)
                    {
                        if (refs[r2] == (vx_reference)graph->nodes[n])
                        {
                            vxGetName(refs[r2]);
                            fprintf(fp, "\t\t<node reference=\"%u\"%s", r2, refNameStr);
                            if(graph->nodes[n]->attributes.borders.mode != VX_BORDER_MODE_UNDEFINED)
                            {
                                const char *bordermodeStrings[] = {"CONSTANT", "REPLICATE"};
                                const char *bordermode;
                                if(graph->nodes[n]->attributes.borders.mode == VX_BORDER_MODE_CONSTANT)
                                {
                                    bordermode = bordermodeStrings[0];
                                }
                                else
                                {
                                    bordermode = bordermodeStrings[1];
                                }

                                fprintf(fp, " bordermode=\"%s\"", bordermode);
                            }
                            if(graph->nodes[n]->is_replicated == vx_true_e)
                            {
                                fprintf(fp, " is_replicated=\"true\"");
                            }
                            fprintf(fp, ">\n\t\t\t<kernel>%s</kernel>\n", graph->nodes[n]->kernel->name);
                            for (p = 0u; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
                            {
                                for (r3 = 0u; r3 < numrefs; r3++)
                                {
                                    if (refs[r3] == graph->nodes[n]->parameters[p])
                                    {
                                        fprintf(fp, "\t\t\t<parameter index=\"%u\" reference=\"%u\"", p, r3);
                                        if(graph->nodes[n]->is_replicated == vx_true_e)
                                        {
                                            if(graph->nodes[n]->replicated_flags[p] == vx_true_e)
                                            {
                                                fprintf(fp, " replicate_flag=\"true\"");
                                            }
                                            else
                                            {
                                                fprintf(fp, " replicate_flag=\"false\"");
                                            }
                                        }
                                        fprintf(fp, " />\n");
                                        break;
                                    }
                                }
                            }
                            if(graph->nodes[n]->attributes.borders.mode == VX_BORDER_MODE_CONSTANT)
                            {
                                fprintf(fp, "\t\t\t<borderconst>#%08x</borderconst>\n", graph->nodes[n]->attributes.borders.constant_value.U32);
                            }
                            fprintf(fp, "\t\t</node>\n");
                            break;
                        }
                    }
                }
                /* print the graph parameters */
                for (p = 0u; p < graph->numParams; p++)
                {
                    for (r2 = 0u; r2 < numrefs; r2++)
                    {
                        if (refs[r2] == (vx_reference)graph->parameters[p].node)
                        {
                            fprintf(fp, "\t\t<parameter index=\"%u\" node=\"%u\" parameter=\"%u\" />\n",
                                    p, r2, graph->parameters[p].index);
                        }
                    }
                }
                /* scan for "virtual" objects in the context and if they match this scope, print them */
                for (r2 = 0u; r2 < numrefs; r2++)
                {
                    if (refs[r2]->scope == refs[r])
                    {
                        vxGetName(refs[r2]);
                        if (refs[r2]->type == VX_TYPE_IMAGE)
                        {
                            status |= vxExportToXMLImage(fp, refs.get(), r2, 2, numrefs);
                        }
                        else if (refs[r2]->type == VX_TYPE_ARRAY)
                        {
                            status |= vxExportToXMLArray(fp, refs.get(), r2, 2);
                        }
                        else if (refs[r2]->type == VX_TYPE_PYRAMID)
                        {
                            status |= vxExportToXMLPyramid(fp, refs.get(), r2, 2, numrefs);
                        }
                    }
                }
                fprintf(fp, "\t</graph>\n");
            }
            else if (refs[r]->type == VX_TYPE_NODE)
            {
                /* skip, no nodes should exist outside a graph */
            }
            else if (refs[r]->type == VX_TYPE_IMAGE)
            {
                if(refs[r]->scope->type != VX_TYPE_PYRAMID &&
                   refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_IMAGE &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLImage(fp, refs.get(), r, 1, numrefs);
                }
            }
            else if (refs[r]->type == VX_TYPE_PYRAMID)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLPyramid(fp, refs.get(), r, 1, numrefs);
                }
            }
            else if (refs[r]->type == VX_TYPE_ARRAY)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                   status |= vxExportToXMLArray(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_LUT)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLLut(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_MATRIX)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLMatrix(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_CONVOLUTION)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLConvolution(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_DISTRIBUTION)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLDistribution(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_REMAP)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLRemap(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_THRESHOLD)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLThreshold(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_SCALAR)
            {
                if(refs[r]->scope->type != VX_TYPE_GRAPH &&
                   refs[r]->scope->type != VX_TYPE_DELAY &&
                   refs[r]->scope->type != VX_TYPE_OBJECT_ARRAY)
                {
                    status |= vxExportToXMLScalar(fp, refs.get(), r, 1);
                }
            }
            else if (refs[r]->type == VX_TYPE_DELAY ||
                      refs[r]->type == VX_TYPE_OBJECT_ARRAY)
            {
                vx_uint32 totalCount, count = 0;
                vx_reference *internalRefs;
                char objectName[16];

                if(refs[r]->type == VX_TYPE_DELAY)
                {
                    vx_delay delay = (vx_delay)refs[r];
                    totalCount = delay->count;
                    internalRefs = (vx_reference *)delay->refs;
                    snprintf(objectName, sizeof(objectName), "delay");
                }
                else
                {
                    vx_object_array objArray = (vx_object_array)refs[r];
                    totalCount = objArray->num_items;
                    internalRefs = (vx_reference *)objArray->items;
                    snprintf(objectName, sizeof(objectName), "object_array");
                }

                fprintf(fp, "\t<%s reference=\"%u\" count=\"%u\"%s>\n", objectName, r, totalCount, refNameStr);

                for (count = 0u; count < totalCount; count++)
                {
                    vx_uint32 r2 = 0;
                    for (r2 = 0u; r2 < numrefs; r2++)
                    {
                        if (refs[r2] == internalRefs[count])
                        {
                            vxGetName(refs[r2]);
                            switch (refs[r2]->type) {
                                case VX_TYPE_IMAGE:
                                    status |= vxExportToXMLImage(fp, refs.get(), r2, 2, numrefs);
                                    break;
                                case VX_TYPE_ARRAY:
                                    status |= vxExportToXMLArray(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_MATRIX:
                                    status |= vxExportToXMLMatrix(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_CONVOLUTION:
                                    status |= vxExportToXMLConvolution(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_DISTRIBUTION:
                                    status |= vxExportToXMLDistribution(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_REMAP:
                                    status |= vxExportToXMLRemap(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_LUT:
                                    status |= vxExportToXMLLut(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_PYRAMID:
                                    status |= vxExportToXMLPyramid(fp, refs.get(), r2, 2, numrefs);
                                    break;
                                case VX_TYPE_THRESHOLD:
                                    status |= vxExportToXMLThreshold(fp, refs.get(), r2, 2);
                                    break;
                                case VX_TYPE_SCALAR:
                                    status |= vxExportToXMLScalar(fp, refs.get(), r2, 2);
                                    break;
                                default:
                                    fprintf(fp, "<unsupported %s object=\"%x\" />\n", objectName, refs[r]->type);
                                    status |= VX_ERROR_INVALID_PARAMETERS;
                                    break;
                            }
                        }
                    }
                }
                fprintf(fp, "\t</%s>\n", objectName);
            }
            else
            {
                fprintf(fp, "<unknown object=\"%x\" />\n", refs[r]->type);
                status = VX_ERROR_NOT_IMPLEMENTED;
            }
        }
        fprintf(fp,"</openvx>\n");
        fclose(fp);
    }
    return status;
}

#endif /* defined(OPENVX_USE_XML) */
