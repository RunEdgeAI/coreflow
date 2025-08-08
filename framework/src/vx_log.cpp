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
#include "vx_log.h"

using namespace corevx;

/******************************************************************************/
/* INTERNAL INTERFACE                                                         */
/******************************************************************************/

void Logger::registerLogCallback(vx_context context, vx_log_callback_f callback, vx_bool reentrant)
{
    if (Context::isValidContext(context) == vx_true_e)
    {
        Osal::semWait(&context->lock);
        if ((context->log_callback == nullptr) && (callback != nullptr))
        {
            context->log_enabled = vx_true_e;
            if (reentrant == vx_false_e)
            {
                Osal::createSem(&context->log_lock, 1);
            }
            context->log_reentrant = reentrant;
        }
        if ((context->log_callback != nullptr) && (callback == nullptr))
        {
            if (context->log_reentrant == vx_false_e)
            {
                Osal::destroySem(&context->log_lock);
            }
            context->log_enabled = vx_false_e;
        }
        if ((context->log_callback != nullptr) && (callback != nullptr) &&
            (context->log_callback != callback))
        {
            if (context->log_reentrant == vx_false_e)
            {
                Osal::destroySem(&context->log_lock);
            }
            if (reentrant == vx_false_e)
            {
                Osal::createSem(&context->log_lock, 1);
            }
            context->log_reentrant = reentrant;
        }
        context->log_callback = callback;
        Osal::semPost(&context->lock);
    }
}

void Logger::addLogEntry(vx_reference ref, vx_status status, const char *message, va_list ap)
{
    vx_context context = nullptr;
    vx_char string[VX_MAX_LOG_MESSAGE_LEN];

    if (Reference::isValidReference(ref) == vx_false_e)
    {
        if (Context::isValidContext((vx_context)ref) == vx_false_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid reference!\n");
            return;
        }
    }

    if (status == VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid status code!\n");
        return;
    }

    if (message == nullptr)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid message!\n");
        return;
    }

    if (ref->type == VX_TYPE_CONTEXT)
    {
        context = (vx_context)ref;
    }
    else
    {
        context = ref->context;
    }

    vsnprintf(string, VX_MAX_LOG_MESSAGE_LEN, message, ap);
    string[VX_MAX_LOG_MESSAGE_LEN - 1] = 0; /* for MSVC which is not C99 compliant */

    if (context->log_callback == nullptr)
    {
        VX_PRINT(VX_ZONE_ERROR, "No callback is registered\n");
        VX_PRINT(VX_ZONE_LOG, "%s", string);
        return;
    }

    if (context->log_enabled == vx_false_e)
    {
        return;
    }

    if (context->log_reentrant == vx_false_e)
    {
        Osal::semWait(&context->log_lock);
    }
    context->log_callback(context, ref, status, string);
    if (context->log_reentrant == vx_false_e)
    {
        Osal::semPost(&context->log_lock);
    }
    return;
}

/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/

VX_API_ENTRY void VX_API_CALL vxRegisterLogCallback(vx_context context, vx_log_callback_f callback,
                                                    vx_bool reentrant)
{
    Logger::registerLogCallback(context, callback, reentrant);
}

VX_API_ENTRY void VX_API_CALL vxAddLogEntry(vx_reference ref, vx_status status, const char *message,
                                            ...)
{
    va_list ap;
    va_start(ap, message);
    Logger::addLogEntry(ref, status, message, ap);
    va_end(ap);
}
