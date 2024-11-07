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
#include "vx_internal.h"
#include "vx_import.h"

Import::Import(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_IMPORT, scope),
import_type(),
count(0),
refs(nullptr)
{

}

Import::~Import()
{
#if defined(OPENVX_USE_XML) || defined(OPENVX_USE_IX)
    destructImport();
#endif /* defined(OPENVX_USE_XML) || defined(OPENVX_USE_IX) */
}

#if defined(OPENVX_USE_XML) || defined(OPENVX_USE_IX)

vx_import Import::createImportInt(vx_context context,
                                  vx_enum type,
                                  vx_uint32 count)
{
    vx_import import = nullptr;

    if (Context::isValidContext(context) == vx_false_e)
        return 0;

    import = (vx_import)Reference::createReference(context, VX_TYPE_IMPORT, VX_EXTERNAL, context);
    if (import && import->type == VX_TYPE_IMPORT)
    {
        import->refs = (vx_reference *)calloc(count, sizeof(vx_reference));
        import->import_type = type;
        import->count = count;
        VX_PRINT(VX_ZONE_INFO, "Creating Import of %u objects of type %x!\n", count, type);
    }
    return import;
}

void Import::destructImport()
{
    vx_uint32 i = 0;
    for (i = 0; i < count; i++)
    {
        if (refs && refs[i])
        {
            Reference::releaseReference((vx_reference*)&refs[i], refs[i]->type, VX_INTERNAL, nullptr);
        }
    }
    if (refs)
    {
        free(refs);
    }
}

#endif /* defined(OPENVX_USE_XML) || defined(OPENVX_USE_IX) */

/******************************************************************************/
/* PUBLIC API */
/******************************************************************************/
#if defined(OPENVX_USE_XML)
VX_API_ENTRY vx_reference VX_API_CALL vxGetImportReferenceByIndex(vx_import import, vx_uint32 index)
{
    vx_reference ref = nullptr;
    if (import && import->type == VX_TYPE_IMPORT)
    {
        if (index < import->count)
        {
            ref = (vx_reference)import->refs[index];
            ref->incrementReference(VX_EXTERNAL);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Incorrect index value\n");
            vxAddLogEntry(import->context, VX_ERROR_INVALID_PARAMETERS, "Incorrect index value\n");
            // ref = (vx_reference_t *)ownGetErrorObject(import->base.context, VX_ERROR_INVALID_PARAMETERS);
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid import reference!\n");
    }
    return ref;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryImport(vx_import import, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference((vx_reference)import, VX_TYPE_IMPORT) == vx_true_e)
    {
        switch (attribute)
        {
            case VX_IMPORT_ATTRIBUTE_COUNT:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                {
                    *(vx_uint32 *)ptr = import->count;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_IMPORT_ATTRIBUTE_TYPE:
                if ((size <= VX_MAX_TARGET_NAME) && (ptr != nullptr))
                {
                    *(vx_uint32 *)ptr = import->import_type;
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
        status = VX_ERROR_INVALID_REFERENCE;
    return status;
}
#endif /* defined(OPENVX_USE_XML) */

#if defined(OPENVX_USE_IX) || defined(OPENVX_USE_XML)

VX_API_ENTRY vx_reference VX_API_CALL vxGetImportReferenceByName(vx_import import, const vx_char *name)
{
    vx_reference ref = nullptr;
    if (import && import->type == VX_TYPE_IMPORT)
    {
        vx_uint32 index = 0;
        for (index = 0; index < import->count; index++)
        {
            if (strncmp(name, import->refs[index]->name, VX_MAX_REFERENCE_NAME) == 0)
            {
                ref = (vx_reference)import->refs[index];
                ref->incrementReference(VX_EXTERNAL);
                break;
            }
        }
    }
    return ref;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseImport(vx_import* import)
{
    vx_status status = VX_FAILURE;

    if (import != nullptr)
    {
        vx_import this_import = *import;
        if (Reference::isValidReference(this_import, VX_TYPE_IMPORT) == vx_true_e)
        {
            status = Reference::releaseReference((vx_reference*)import, VX_TYPE_IMPORT, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

#endif /* defined(OPENVX_USE_IX) || defined(OPENVX_USE_XML) */