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
#include <VX/vx.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_khr_pipelining.h>

#include "vx_internal.h"

#ifdef OPENVX_USE_PIPELINING

VX_API_ENTRY vx_status VX_API_CALL vxEnableEvents(vx_context context)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Context::isValidContext(context))
    {
        VX_PRINT(VX_ZONE_ERROR, "context is invalid\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        context->event_queue.status(vx_true_e);
        status = VX_SUCCESS;
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxDisableEvents(vx_context context)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Context::isValidContext(context))
    {
        VX_PRINT(VX_ZONE_ERROR, "context is invalid\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        status = context->event_queue.status(vx_false_e);
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSendUserEvent(vx_context context, vx_uint32 id, void *parameter)
{
    vx_status status = VX_SUCCESS;

    if (Context::isValidContext(context) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "context is invalid\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        if (!context->event_queue.isEnabled())
        {
            VX_PRINT(VX_ZONE_ERROR, "events are disabled\n");
            status = VX_FAILURE;
        }
    }

    if (VX_SUCCESS == status)
    {
        vx_event_info_t event_info;
        event_info.user_event.user_event_parameter = parameter;
        status = context->event_queue.push(VX_EVENT_USER, id, &event_info);
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxWaitEvent(
                    vx_context context, vx_event_t *event,
                    vx_bool do_not_block)
{
    vx_status status = VX_SUCCESS;

    if (Context::isValidContext(context) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR,"context is invalid\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        if (!context->event_queue.isEnabled())
        {
            VX_PRINT(VX_ZONE_ERROR, "events are disabled\n");
            status = VX_FAILURE;
        }
    }

    if (VX_SUCCESS == status)
    {
        /* Call general wait function */
        status = context->event_queue.wait(event, do_not_block);
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxRegisterEvent(
    vx_reference ref, vx_event_type_e type, vx_uint32 param, vx_uint32 app_value)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Reference::isValidReference(ref))
    {
        VX_PRINT(VX_ZONE_ERROR, "ref is invalid\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        if (!ref->context->event_queue.isEnabled())
        {
            VX_PRINT(VX_ZONE_ERROR, "events are disabled\n");
            status = VX_FAILURE;
        }
    }

    if (VX_SUCCESS == status)
    {
        if (ref->type != VX_TYPE_GRAPH &&
            ref->type != VX_TYPE_NODE &&
            ref->type != VX_TYPE_PARAMETER)
        {
            VX_PRINT(VX_ZONE_ERROR, "ref is not a graph or node\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        // Only some event types are allowed per spec
        switch (type)
        {
            case VX_EVENT_GRAPH_PARAMETER_CONSUMED:
            case VX_EVENT_GRAPH_COMPLETED:
            case VX_EVENT_NODE_COMPLETED:
            case VX_EVENT_NODE_ERROR:
            case VX_EVENT_USER:
                break;
            default:
                status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        status = ref->context->event_queue.registerEvent(ref, type, param, app_value);
    }

    return status;
}

#endif /* OPENVX_USE EVENTS */
