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
#include "vx_error.h"

using namespace coreflow;
/******************************************************************************/
/* INTERNAL INTERFACE                                                         */
/******************************************************************************/

Error::Error(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_ERROR, scope),
status(VX_SUCCESS)
{

}

Error::~Error()
{
    vx_error ref = this;
    if (internal_count)
        releaseError(&ref);
}

void Error::releaseError(vx_error* error)
{
    Reference::releaseReference((vx_reference *)error, VX_TYPE_ERROR, VX_INTERNAL, nullptr);
}

vx_error Error::allocateError(vx_context context, vx_status status)
{
    vx_error error = (vx_error)Reference::createReference(context, VX_TYPE_ERROR, VX_INTERNAL, context);
    if (error)
    {
        error->status = status;
    }
    return error;
}

vx_bool Error::createConstErrors(vx_context context)
{
    vx_bool ret = vx_true_e;
    vx_enum e = 0;
    /* create an error object for each status enumeration */
    for (e = VX_STATUS_MIN; (e < VX_SUCCESS) && (ret == vx_true_e); e++)
    {
        if (allocateError(context, e) == nullptr)
            ret = vx_false_e;
    }
    return ret;
}

vx_error Error::getError(vx_context context, vx_status status)
{
    vx_error error = nullptr;
    vx_size i = 0ul;
    Osal::semWait(&context->lock);
    for (i = 0ul; i < context->num_references; i++)
    {
        if (context->reftable[i] == nullptr) continue;

        if (context->reftable[i]->type == VX_TYPE_ERROR)
        {
            error = (vx_error)context->reftable[i];
            if (error->status == status)
            {
                break;
            }
            error = nullptr;
        }
    }
    Osal::semPost(&context->lock);
    return error;
}

vx_status Error::getStatus(vx_reference ref)
{
    if (ref == nullptr)
    {
        /*! \internal probably ran out of handles or memory */
        VX_PRINT(VX_ZONE_ERROR, "Nullptr reference\n");
        return VX_ERROR_NO_RESOURCES;
    }
    else if (Reference::isValidReference(ref) == vx_true_e)
    {
        if (ref->type == VX_TYPE_ERROR)
        {
            vx_error error = (vx_error)ref;
            VX_PRINT(VX_ZONE_INFO, "NOT SUCCESS\n");
            return error->status;
        }
        else
        {
            VX_PRINT(VX_ZONE_INFO, "SUCCESS\n");
            return VX_SUCCESS;
        }
    }
    else if (Context::isValidContext((vx_context)ref) == vx_true_e)
    {
        VX_PRINT(VX_ZONE_INFO, "SUCCESS\n");
        return VX_SUCCESS;
    }

    VX_PRINT(VX_ZONE_ERROR, "returning fail\n");
    return VX_FAILURE;
}

/******************************************************************************/
/* PUBLIC INTERFACE                                                           */
/******************************************************************************/

VX_API_ENTRY vx_error vxGetErrorObject(vx_context context, vx_status status)
{
    return Error::getError(context, status);
}

VX_API_ENTRY vx_status VX_API_CALL vxGetStatus(vx_reference ref)
{
    return Error::getStatus(ref);
}
